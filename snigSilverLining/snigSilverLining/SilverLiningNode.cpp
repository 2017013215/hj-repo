#include <SilverLining.h>

#include "SilverLiningNode.h"
#include "SilverLiningContext.h"
#include "SilverLiningSkyDrawable.h"
#include "SilverLiningCloudsDrawable.h"

#include <osg/Light>
#include <osg/LightSource>
#include <osgEarth/CullingUtils>

#undef  LC
#define LC "[SilverLiningNode] "

ByySilverLiningNode::ByySilverLiningNode(const osgEarth::SpatialReference*    mapSRS,
                                   const ByySilverLiningOptions& options,
                                   SilverLiningCallback*                  callback) :
_options(options),
_mapSRS(mapSRS),
_callback(callback),
_lastAltitude(DBL_MAX)
{
	// Create a new Light for the Sun.
	_light = new osg::Light();
	_light->setLightNum( 0 );
	_light->setDiffuse( osg::Vec4(1,1,1,1) );
	_light->setAmbient( osg::Vec4(0.2f, 0.2f, 0.2f, 1) );
	_light->setPosition( osg::Vec4(1, 0, 0, 0) ); // w=0 means infinity
	_light->setDirection( osg::Vec3(-1,0,0) );

	_lightSource = new osg::LightSource();
	_lightSource->setLight( _light.get() );
	_lightSource->setReferenceFrame(osg::LightSource::RELATIVE_RF);

	// The main silver lining data:
	_SL = new ByySilverLiningContext( options );
	_SL->setLight( _light);
	_SL->setSRS  ( mapSRS );
	_SL->setCallback( callback );
	_SL->setMinimumAmbient(getMinimumAmbient() );

	// Geode to hold each of the SL drawables:
	_geode = new osg::Geode();
	_geode->setCullingActive( false );

	// Draws the sky before everything else
	_skyDrawable = new SkyDrawable(_SL);
	_skyDrawable->getOrCreateStateSet()->setRenderBinDetails( -99, "RenderBin" );
	_skyDrawable->setCullingActive(false);
	_geode->addDrawable(_skyDrawable.get());

	// Clouds draw after everything else
	_cloudsDrawable = new CloudsDrawable(_SL);
	_cloudsDrawable->getOrCreateStateSet()->setRenderBinDetails( 99, "DepthSortedBin" );
	_cloudsDrawable->setCullingActive(false);
	_geode->addDrawable(_cloudsDrawable.get());

    

    // scene lighting
    osg::StateSet* stateset = this->getOrCreateStateSet();
    _lighting = new osgEarth::PhongLightingEffect();
    _lighting->setCreateLightingUniform( false );
    _lighting->attach( stateset );

    // need update traversal.
    ADJUST_UPDATE_TRAV_COUNT(this, +1);

	onSetDateTime();
}


ByySilverLiningNode::~ByySilverLiningNode()
{
    if ( _lighting.valid() )
        _lighting->detach();
}

void
ByySilverLiningNode::attach(osg::View* view, int lightNum)
{
    _light->setLightNum( lightNum );
    view->setLight( _light.get() );
    view->setLightingMode( osg::View::SKY_LIGHT );
}

void
ByySilverLiningNode::onSetDateTime()
{
	// set the SL local time to UTC/epoch.
	::SilverLining::LocalTime utcTime;
	utcTime.SetFromEpochSeconds(getDateTime().asTimeStamp() );
	_SL->getAtmosphere()->GetConditions()->SetTime( utcTime );
}

void
ByySilverLiningNode::onSetMinimumAmbient()
{
    _SL->setMinimumAmbient( getMinimumAmbient() );
}

void
ByySilverLiningNode::traverse(osg::NodeVisitor& nv)
{
	if ( _SL && _SL->ready() )
	{
		/*if ( nv.getVisitorType() == nv.UPDATE_VISITOR )
		{
			int frameNumber = nv.getFrameStamp()->getFrameNumber();
			

			if( _cloudsDrawable )
			{
				if ( _lastAltitude <= *_options.cloudsMaxAltitude() )
				{
					if ( _cloudsDrawable->getNumParents() == 0 )
						_geode->addDrawable( _cloudsDrawable.get() );
				}
				else
				{
					if ( _cloudsDrawable->getNumParents() > 0 )
						_geode->removeDrawable( _cloudsDrawable.get() );
				}
			}
		}

		else*/ if ( nv.getVisitorType() == nv.CULL_VISITOR )
		{
			// TODO: make this multi-camera safe
			_skyDrawable->dirtyBound();

			_SL->setCameraPosition( nv.getEyePoint() );
			osgUtil::CullVisitor* cv = osgEarth::Culling::asCullVisitor(nv);
			_SL->getAtmosphere()->SetCameraMatrix( cv->getModelViewMatrix()->ptr() );
			_SL->getAtmosphere()->SetProjectionMatrix( cv->getProjectionMatrix()->ptr() );

			_SL->updateLocation();
			_SL->updateLight();
			_cloudsDrawable->dirtyBound();
			//_SL->getAtmosphere()->UpdateSkyAndClouds();
			//_SL->getAtmosphere()->CullObjects();
		}
	}

	if ( _geode.valid() )
	{
		_geode->accept(nv);
	}

	if (_lightSource.valid())
	{
		_lightSource->accept(nv);
	}

    osgEarth::Util::SkyNode::traverse(nv);
}
