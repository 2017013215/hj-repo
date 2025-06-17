#include <Triton.h>
#include "TritonContext.h"
#include <osg/GLExtensions>
#include <osg/Math>
#include <osgDB/FileNameUtils>
#include <osgEarth/SpatialReference>
#include <cstdlib>

#define LC "[TritonContext] "

ByyTritonContext::ByyTritonContext(const ByyTritonOptions& options) :
_options              ( options ),
_initAttempted        ( false ),
_initFailed           ( false ),
_resourceLoader       ( 0L ),
_environment          ( 0L ),
_ocean                ( 0L )
{    
    //nop
}

ByyTritonContext::~ByyTritonContext()
{
    if ( _ocean )
        delete _ocean;

    if ( _environment )
        delete _environment;

    if ( _resourceLoader )
        delete _resourceLoader;
}

void
ByyTritonContext::setSRS(const osgEarth::SpatialReference* srs)
{
    _srs = srs;
}

void
ByyTritonContext::setCallback(TritonCallback* callback)
{
    _callback = callback;
}

bool
ByyTritonContext::passHeightMapToTriton() const
{
    return _options.useHeightMap() == true;
}

int
ByyTritonContext::getHeightMapSize() const
{
    return osg::clampBetween(_options.heightMapSize().get(), 64, 2048);
}

void
ByyTritonContext::initialize(osg::RenderInfo& renderInfo)
{
    if ( !_initAttempted && !_initFailed )
    {
        // lock/double-check:
        osgEarth::Threading::ScopedMutexLock excl(_initMutex);
        if ( !_initAttempted && !_initFailed )
        {
            _initAttempted = true;

            std::string resourcePath = _options.resourcePath().get();
            if (resourcePath.empty() && ::getenv("TRITON_PATH"))
            {
                resourcePath = osgDB::concatPaths(::getenv("TRITON_PATH"), "Resources");
            }

            _resourceLoader = new ::Triton::ResourceLoader(resourcePath.c_str());

            _environment = new ::Triton::Environment();

            _environment->SetLicenseCode(
                _options.user()->c_str(),
                _options.licenseCode()->c_str() );

            // "WGS84" is used to represent any ellipsoid.
            ::Triton::CoordinateSystem cs =
                _srs->isGeographic() ? ::Triton::WGS84_ZUP :
                ::Triton::FLAT_ZUP;

            // Set the ellipsoid to match the one in our map's SRS.
            if ( _srs->isGeographic() )
            {
                const osg::EllipsoidModel* ellipsoid = _srs->getEllipsoid();
                
                std::string eqRadius = osgEarth::Stringify() << ellipsoid->getRadiusEquator();
                std::string poRadius = osgEarth::Stringify() << ellipsoid->getRadiusPolar();

                _environment->SetConfigOption( "equatorial-earth-radius-meters", eqRadius.c_str() );
                _environment->SetConfigOption( "polar-earth-radius-meters",      poRadius.c_str() );
            }

            float openGLVersion = osg::getGLVersionNumber();
            enum ::Triton::Renderer tritonOpenGlVersion = ::Triton::OPENGL_2_0;
            if( openGLVersion == 4.1 )
                tritonOpenGlVersion = ::Triton::OPENGL_4_1;
            else if( openGLVersion == 4.0 )
                tritonOpenGlVersion = ::Triton::OPENGL_4_0;
            else if( openGLVersion == 3.2 )
                tritonOpenGlVersion = ::Triton::OPENGL_3_2;

            ::Triton::EnvironmentError err = _environment->Initialize(
                cs,
                tritonOpenGlVersion,
                _resourceLoader );

            if ( err == ::Triton::SUCCEEDED )
            {
                ::Triton::WindFetch wf;
                wf.SetWind( 10.0, 0.0 );
                _environment->AddWindFetch( wf );

                _ocean = ::Triton::Ocean::Create(
                    _environment, 
                    ::Triton::JONSWAP );
            }

            if ( _ocean )
            {
                // fire init callback if available
                if (_callback.valid())
                {
                    _callback->onInitialize(getEnvironment(), getOcean());
                }

                OE_INFO << LC << "Triton initialized OK!" << std::endl;                
            }
            else
            {
                _initFailed = true;
                OE_WARN << LC << "Triton initialization failed- err=" << err << std::endl;
            }
        }
    }
}

void
ByyTritonContext::update(double simTime)
{
    if ( _ocean )
    {
        // fmod requires b/c CUDA is limited to single-precision values
        _ocean->UpdateSimulation( fmod(simTime, 86400.0) );
    }
}
