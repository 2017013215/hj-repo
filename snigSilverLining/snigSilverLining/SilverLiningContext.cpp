#include <SilverLining.h> // SilverLining SDK
#include "SilverLiningContext.h"
#include "SilverLiningNode.h"
#include <osg/Light>
#include <osgDB/FileNameUtils>
#include <osgEarth/SpatialReference>
#include <cstdlib>

#define LC "[SilverLiningContext] "

/**
 * Adapter that converts the return value of osgEarth::SilverLining::Callback::getMilliseconds()
 * into a usable value for SilverLining using the SilverLining MillisecondTimer callback.
 */
class MillisecondTimerAdapter : public ::SilverLining::MillisecondTimer
{
public:
    MillisecondTimerAdapter(ByySilverLiningContext* context) :
    _context(context),
    _defaultTimer(new ::SilverLining::MillisecondTimer)
    {
    }

    virtual ~MillisecondTimerAdapter()
    {
        delete _defaultTimer;
    }

    virtual unsigned long SILVERLINING_API GetMilliseconds() const
    {
        osg::ref_ptr<ByySilverLiningContext> context;
        unsigned long milliseconds = 0;
        if (_context.lock(context))
        {
            osg::ref_ptr<SilverLiningCallback> callback = context->getCallback();
            if (callback.valid())
                milliseconds = callback->getMilliseconds();
        }

        // As per documentation, use the default SilverLining timer instead of returning 0
        if (milliseconds != 0)
            return milliseconds;
        return _defaultTimer->GetMilliseconds();
    }

private:
    osg::observer_ptr<ByySilverLiningContext> _context;
    ::SilverLining::MillisecondTimer* _defaultTimer;
};

ByySilverLiningContext::ByySilverLiningContext(const ByySilverLiningOptions& options) :
_options              ( options ),
_initAttempted        ( false ),
_initFailed           ( false ),
_maxAmbientLightingAlt( -1.0 ),
_atmosphere           ( 0L ),
_minAmbient           ( 0,0,0,0 )
{
    // Create the millisecond timer that we'll use to control time
    _msTimer = new MillisecondTimerAdapter(this);

    // Create a SL atmosphere (the main SL object).
    _atmosphere = new ::SilverLining::Atmosphere(
        options.user()->c_str(),
        options.licenseCode()->c_str() );

	_atmosphere->EnableLensFlare( true );
	_atmosphere->SetSkyModel(HOSEK_WILKIE);
//    _atmosphereWrapper = new Atmosphere((uintptr_t)_atmosphere);
}

ByySilverLiningContext::~ByySilverLiningContext()
{
  //  delete _atmosphereWrapper;
    delete _atmosphere;
    delete _msTimer;

    OE_INFO << LC << "Destroyed\n";
}

void
ByySilverLiningContext::setCallback(SilverLiningCallback* cb)
{
    _callback = cb;
}

void
ByySilverLiningContext::setLight(osg::Light* light)
{
    _light = light;
}

void
ByySilverLiningContext::setSRS(const osgEarth::SpatialReference* srs)
{
    _srs = srs;
}

void
ByySilverLiningContext::setMinimumAmbient(const osg::Vec4f& value)
{
    _minAmbient = value;
}

void
ByySilverLiningContext::initialize(osg::RenderInfo& renderInfo)
{
    if ( !_initAttempted && !_initFailed )
    {
        // lock/double-check:
        osgEarth::Threading::ScopedMutexLock excl(_initMutex);
        if ( !_initAttempted && !_initFailed )
        {
            _initAttempted = true;

            // constant random seed ensures consistent clouds across windows
            // TODO: replace this with something else since this is global! -gw
            ::srand(1234);

            std::string resourcePath = _options.resourcePath().get();
            if (resourcePath.empty() && ::getenv("SILVERLINING_PATH"))
            {
                resourcePath = osgDB::concatPaths(::getenv("SILVERLINING_PATH"), "Resources");
            }

            int result = _atmosphere->Initialize(
                ::SilverLining::Atmosphere::OPENGL,
                resourcePath.c_str(),
                true,
                0 );

            if ( result != ::SilverLining::Atmosphere::E_NOERROR )
            {
                _initFailed = true;
                OE_WARN << LC << "SilverLining failed to initialize: " << result << std::endl;
            }
            else
            {
                OE_INFO << LC << "SilverLining initialized OK!" << std::endl;

                // Defaults for a projected terrain. ECEF terrain vectors are set
                // in updateLocation().
                _atmosphere->SetUpVector( 0.0, 0.0, 1.0 );
                _atmosphere->SetRightVector( 1.0, 0.0, 0.0 );
				_atmosphere->SetSkyModel(HOSEK_WILKIE);
				_atmosphere->EnableLensFlare(true);

                // Configure the timer used for animations
                _atmosphere->GetConditions()->SetMillisecondTimer(_msTimer);
				_atmosphere->GetConditions()->SetPresetConditions(::SilverLining::AtmosphericConditions::FAIR,*_atmosphere);

#if 0 // todo: review this
                _maxAmbientLightingAlt =
                    _atmosphere->GetConfigOptionDouble("atmosphere-height");
#endif
                if ( _options.drawClouds() == true )
                {
                    OE_INFO << LC << "Initializing clouds\n";
                    setupClouds();
                }

                // user callback for initialization
                if (_callback.valid())
                {
                    _callback->onInitialize( _atmosphere );
                }
            }
        }
    }
}

void
ByySilverLiningContext::setupClouds()
{
    ::SilverLining::CloudLayer* clouds = ::SilverLining::CloudLayerFactory::Create( ::CUMULUS_CONGESTUS );
    clouds->SetIsInfinite( true );
    clouds->SetFadeTowardEdges(true);
    clouds->SetBaseAltitude( 2000 );
    clouds->SetThickness( 200 );
    clouds->SetBaseLength( 100000 );
    clouds->SetBaseWidth( 100000 );
    clouds->SetDensity( 0.6 );
    clouds->SetAlpha( 0.8 );

    clouds->SeedClouds( *_atmosphere );
    clouds->GenerateShadowMaps( false );

    clouds->SetLayerPosition(0, 0);

    _atmosphere->GetConditions()->AddCloudLayer( clouds );
}

void
ByySilverLiningContext::updateLight()
{
    if ( !ready() || !_light.valid() || !_srs.valid() )
        return;

    float ra, ga, ba, rd, gd, bd, x, y, z;

    // Clamp the camera's altitude while fetching the colors so the
    // lighting's ambient component doesn't fade to black at high altitude.
    ::SilverLining::Location savedLoc = _atmosphere->GetConditions()->GetLocation();
    ::SilverLining::Location clampedLoc = savedLoc;
    if ( _maxAmbientLightingAlt > 0.0 )
    {
        clampedLoc.SetAltitude( std::min(clampedLoc.GetAltitude(), _maxAmbientLightingAlt) );
        _atmosphere->GetConditions()->SetLocation( clampedLoc );
    }

    _atmosphere->GetAmbientColor( &ra, &ga, &ba );
    _atmosphere->GetSunColor( &rd, &gd, &bd );

    // Restore the actual altitude.
    if ( _maxAmbientLightingAlt > 0.0 )
    {
        _atmosphere->GetConditions()->SetLocation( savedLoc );
    }

    if ( _srs->isGeographic() )
    {
        _atmosphere->GetSunPositionGeographic( &x, &y, &z );
    }
    else
    {
        _atmosphere->GetSunPosition(&x, &y, &z);
    }

    osg::Vec3 direction(x, y, z);
    direction.normalize();

    osg::Vec4 ambient(
        osg::clampAbove(ra, _minAmbient.r()),
        osg::clampAbove(ba, _minAmbient.g()),
        osg::clampAbove(ga, _minAmbient.b()),
        1.0);

    _light->setAmbient( ambient );
    _light->setDiffuse( osg::Vec4(rd, gd, bd, 1.0f) );
    _light->setPosition( osg::Vec4(direction, 0.0f) ); //w=0 means "at infinity"
}

void
ByySilverLiningContext::updateLocation()
{
    if ( !ready() || !_srs.valid() )
        return;

    if ( _srs->isGeographic() )
    {
        // Get new local orientation
        osg::Vec3d up = _cameraPos;
        up.normalize();
        osg::Vec3d north = osg::Vec3d(0, 1, 0);
        osg::Vec3d east = north ^ up;

        // Check for edge case of north or south pole
        if (east.length2() == 0)
        {
            east = osg::Vec3d(1, 0, 0);
        }

        east.normalize();

        _atmosphere->SetUpVector(up.x(), up.y(), up.z());
        _atmosphere->SetRightVector(east.x(), east.y(), east.z());

        // Get new lat / lon / altitude
        osg::Vec3d latLonAlt;
        _srs->transformFromWorld(_cameraPos, latLonAlt);

        ::SilverLining::Location loc;
        loc.SetAltitude ( latLonAlt.z() );
        loc.SetLongitude( latLonAlt.x() ); //osg::DegreesToRadians(latLonAlt.x()) );
        loc.SetLatitude ( latLonAlt.y() ); //osg::DegreesToRadians(latLonAlt.y()) );

        _atmosphere->GetConditions()->SetLocation( loc );
    }
}
