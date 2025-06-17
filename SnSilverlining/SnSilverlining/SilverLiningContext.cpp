/* -*-c++-*- */
/* osgEarth - Dynamic map generation toolkit for OpenSceneGraph
 * Copyright 2008-2013 Pelican Mapping
 * http://osgearth.org
 *
 * osgEarth is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */
#include "SilverLiningContext.h"
#include <SilverLining.h> // SilverLinking SDK
#include <osg/Light>
#include <osgEarth/SpatialReference>
#include <osg/io_utils>
#define LC "[SilverLiningContext] "

using namespace osgEarth;
using namespace SNIG;


SilverLiningContext::SilverLiningContext(const SilverLiningOptions& options) :
_options              ( options ),
_initAttempted        ( false ),
_initFailed           ( false ),
_maxAmbientLightingAlt( -1.0 ),
_atmosphere           ( 0L ),
windHandle(-1)
{
    // Create a SL atmosphere (the main SL object).
    // TODO: plug in the username + license key.
    _atmosphere = new ::SilverLining::Atmosphere(
        options.user()->c_str(),
        options.licenseCode()->c_str() );
///////////////////////////////////////////////////
	_atmosphere->DisableFarCulling( true );
	_atmosphere->EnableLensFlare( true );
}

SilverLiningContext::~SilverLiningContext()
{
	if ( _atmosphere )
		delete _atmosphere;

	OE_INFO << LC << "Destroyed\n";
}

void
SilverLiningContext::setLight(osg::Light* light)
{
    _light = light;
}

void
SilverLiningContext::setSRS(const SpatialReference* srs)
{
    _srs = srs;
}

void
SilverLiningContext::initialize(osg::RenderInfo& renderInfo)
{
	if ( !_initAttempted && !_initFailed )
	{
		// lock/double-check:
		Threading::ScopedMutexLock excl(_initMutex);
		if ( !_initAttempted && !_initFailed )
		{
			_initAttempted = true;

			// constant random seed ensures consistent clouds across windows
			// TODO: replace this with something else since this is global! -gw
			::srand(1234);

			int result = _atmosphere->Initialize(
				::SilverLining::Atmosphere::OPENGL,
				_options.resourcePath()->c_str(),
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

#if 0 // todo: review this
				_maxAmbientLightingAlt = 
					_atmosphere->GetConfigOptionDouble("atmosphere-height");
#endif

				if ( _options.drawClouds() == true )
				{
					OE_INFO << LC << "Initializing clouds\n";
					addCloudLayer(CUMULUS_CONGESTUS);
					setWind(10,0);
				}
			}
		}
	}
}

void
SilverLiningContext::updateLight()
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

	_light->setAmbient( osg::Vec4(ra, ga, ba, 1.0f) );
	_light->setDiffuse( osg::Vec4(rd, gd, bd, 1.0f) );
	_light->setPosition( osg::Vec4(direction, 0.0f) ); //w=0 means "at infinity"

}

void SilverLiningContext::updateLocation()
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
		loc.SetLongitude( latLonAlt.x() );
		loc.SetLatitude (latLonAlt.y());

		_atmosphere->GetConditions()->SetLocation( loc );


		static int c=2;

		if (c>0)
		{
			--c;
			for (std::list<int>::iterator it=cloundLayerHandles.begin();it!=cloundLayerHandles.end();++it)
			{
				::SilverLining::CloudLayer * cloundLayer=NULL;
				_atmosphere->GetConditions()->GetCloudLayer(*it,&cloundLayer);

				if (cloundLayer)
				{
					cloundLayer->SetLayerPosition(0,0);
				}

			}
		}

//         if ( _clouds )
//         {
// #if 1 //TODO: figure out why we need to call this a couple times before
//       //      it takes effect. -gw
//             static int c = 2;
//             if ( c > 0 ) {
//                 --c;
//                 _clouds->SetLayerPosition(0, 0);
//             }
//         }
// #endif
     }
}


void SNIG::SilverLiningContext::addCloudLayer( int type )
{
	::SilverLining::CloudLayer* coludLayer=0;

	switch(type)
	{
	case CIRROCUMULUS:
		coludLayer=createCIRROCUMULUSClouds(); //卷积云不可用
		break;
	case CIRRUS_FIBRATUS:
		coludLayer=createCIRRUS_FIBRATUSClouds();//毛卷云
		break;
	case STRATUS:
		coludLayer=createSTRATUSClouds();//层云
		break;
	case CUMULUS_MEDIOCRIS:
		coludLayer=createCUMULUS_MEDIOCRISClouds();//中积云
		break;
	case CUMULUS_CONGESTUS:
		coludLayer=createCUMULUS_CONGESTUSClouds();//浓积云
		break;
	case CUMULUS_CONGESTUS_HI_RES:
		coludLayer=createCUMULUS_CONGESTUS_HI_RESClouds();//高分辨率浓积云
		break;
	case CUMULONIMBUS_CAPPILATUS:
		coludLayer=createCUMULONIMBUS_CAPPILATUSClouds();//积雨云
		break;
	case STRATOCUMULUS:
		coludLayer=createSTRATOCUMULUSClouds();//层积云
		break;
	case TOWERING_CUMULUS:
		coludLayer=createTOWERING_CUMULUSClouds();//塔状积云
		break;
	}

	if (coludLayer)
	{
		int handle=_atmosphere->GetConditions()->AddCloudLayer(coludLayer);
		cloundLayerHandles.push_back(handle);
	}
}

void SNIG::SilverLiningContext::removeCloudLayer( int index )
{
	std::list<int>::iterator it=cloundLayerHandles.begin();

	for (int i=0;it!=cloundLayerHandles.end();++it)
	{

		if (i==index)
		{
			cloundLayerHandles.erase(it);
			break;
		}
		else
		{
			++i;
		}
	}
}

::SilverLining::CloudLayer* SNIG::SilverLiningContext::createCIRROCUMULUSClouds() //卷积云 不可用
{
	return NULL;
}

::SilverLining::CloudLayer* SNIG::SilverLiningContext::createCIRRUS_FIBRATUSClouds() //毛卷云
{
	::SilverLining::CloudLayer *cirrusCloudLayer;

	cirrusCloudLayer = ::SilverLining::CloudLayerFactory::Create(CIRRUS_FIBRATUS);
	cirrusCloudLayer->SetBaseAltitude(6000);
	cirrusCloudLayer->SetThickness(0);
	cirrusCloudLayer->SetBaseLength(100000);
	cirrusCloudLayer->SetBaseWidth(100000);
	cirrusCloudLayer->SetLayerPosition(0, 0);
	cirrusCloudLayer->SeedClouds(*_atmosphere);

	return cirrusCloudLayer;
}

::SilverLining::CloudLayer* SNIG::SilverLiningContext::createSTRATUSClouds() //层云
{
	::SilverLining::CloudLayer *stratusLayer;

	stratusLayer = ::SilverLining::CloudLayerFactory::Create(STRATUS);
	stratusLayer->SetIsInfinite(true);
	stratusLayer->SetBaseAltitude(15000);
	stratusLayer->SetThickness(600);
	stratusLayer->SetDensity(0.5);
	stratusLayer->SetLayerPosition(0, 0);
	stratusLayer->SeedClouds(*_atmosphere);

	return stratusLayer;
}

::SilverLining::CloudLayer* SNIG::SilverLiningContext::createCUMULUS_MEDIOCRISClouds() //中积云
{
	::SilverLining::CloudLayer *cumulusMediocrisLayer;

	cumulusMediocrisLayer = ::SilverLining::CloudLayerFactory::Create(CUMULUS_MEDIOCRIS);
	cumulusMediocrisLayer->SetIsInfinite(true);
	cumulusMediocrisLayer->SetBaseAltitude(1000);
	cumulusMediocrisLayer->SetThickness(100);
	cumulusMediocrisLayer->SetBaseLength(20000);
	cumulusMediocrisLayer->SetBaseWidth(20000);
	cumulusMediocrisLayer->SetDensity(0.2);
	cumulusMediocrisLayer->SetLayerPosition(0, 0);
	cumulusMediocrisLayer->SeedClouds(*_atmosphere);

	return cumulusMediocrisLayer;
}

::SilverLining::CloudLayer* SNIG::SilverLiningContext::createCUMULUS_CONGESTUSClouds()// 浓积云  osgearth自带
{
	::SilverLining::CloudLayer *_clouds;
	_clouds = ::SilverLining::CloudLayerFactory::Create( CUMULUS_CONGESTUS );
	_clouds->SetIsInfinite( true );
	_clouds->SetFadeTowardEdges(true);
	_clouds->SetBaseAltitude( 3000 );
	_clouds->SetThickness( 200 );
	_clouds->SetBaseLength( 100000 );
	_clouds->SetBaseWidth( 100000 );
	_clouds->SetDensity( 0.6 );
	_clouds->SetAlpha( 0.8 );

	_clouds->SeedClouds( *_atmosphere );
	//_clouds->GenerateShadowMaps( false );

	_clouds->SetLayerPosition(0, 0);

	return _clouds;
}

::SilverLining::CloudLayer* SNIG::SilverLiningContext::createCUMULUS_CONGESTUS_HI_RESClouds()//高分辨率浓积云
{
	::SilverLining::CloudLayer* cumulusCongestusLayer;
	cumulusCongestusLayer = ::SilverLining::CloudLayerFactory::Create(CUMULUS_CONGESTUS_HI_RES);
	cumulusCongestusLayer->SetIsInfinite(true);
	cumulusCongestusLayer->SetBaseAltitude(1500);
	cumulusCongestusLayer->SetThickness(100);
	cumulusCongestusLayer->SetBaseLength(30000);
	cumulusCongestusLayer->SetBaseWidth(30000);
	cumulusCongestusLayer->SetDensity(0.8);
	cumulusCongestusLayer->SetLayerPosition(0, 0);
	cumulusCongestusLayer->SetCloudAnimationEffects(0.1, false);
	cumulusCongestusLayer->SeedClouds(*_atmosphere);
	cumulusCongestusLayer->SetAlpha(0.5);
	cumulusCongestusLayer->SetFadeTowardEdges(true);

	return cumulusCongestusLayer;
}

::SilverLining::CloudLayer* SNIG::SilverLiningContext::createCUMULONIMBUS_CAPPILATUSClouds()  //积雨云  //不可用
{
	::SilverLining::CloudLayer *cumulonimbusLayer;

	cumulonimbusLayer = ::SilverLining::CloudLayerFactory::Create(CUMULONIMBUS_CAPPILATUS);
	cumulonimbusLayer->SetBaseAltitude(1000);
	cumulonimbusLayer->SetThickness(3000);
	cumulonimbusLayer->SetBaseLength(3000);
	cumulonimbusLayer->SetBaseWidth(5000);
	cumulonimbusLayer->SetLayerPosition(0, -5000);
	cumulonimbusLayer->SeedClouds(*_atmosphere);

	return cumulonimbusLayer;
}

::SilverLining::CloudLayer* SNIG::SilverLiningContext::createSTRATOCUMULUSClouds()//层积云
{
	::SilverLining::CloudLayer *stratocumulusLayer;

	stratocumulusLayer = ::SilverLining::CloudLayerFactory::Create(STRATOCUMULUS);
	stratocumulusLayer->SetBaseAltitude(1000);
	stratocumulusLayer->SetThickness(3000);
	stratocumulusLayer->SetBaseLength(30000);
	stratocumulusLayer->SetBaseWidth(30000);
	stratocumulusLayer->SetDensity(0.5);
	stratocumulusLayer->SetIsInfinite(true);
	stratocumulusLayer->SetAlpha(1.0);
	stratocumulusLayer->SetFadeTowardEdges(true);
	stratocumulusLayer->SetLayerPosition(0, 0);
	stratocumulusLayer->SeedClouds(*_atmosphere);

	return stratocumulusLayer;
}

::SilverLining::CloudLayer* SNIG::SilverLiningContext::createTOWERING_CUMULUSClouds()//塔状积云不可用
{
	return NULL;
}

void SNIG::SilverLiningContext::setWind( double speed, double direction )
{
	if (windHandle!=-1)
	{
		_atmosphere->GetConditions()->RemoveWindVolume(windHandle);
	}
	::SilverLining::WindVolume wv;
	wv.SetDirection(direction);
	wv.SetMinAltitude(0);
	wv.SetMaxAltitude(10000);
	wv.SetWindSpeed(speed);
	windHandle=_atmosphere->GetConditions()->SetWind(wv);
}
