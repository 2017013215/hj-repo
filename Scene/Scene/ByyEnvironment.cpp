#include "stdafx.h"
#include "ByyEnvironment.h"
#include "observerSettings.h"
#include "SilverLiningNode.h"
#include "SilverLiningUserInterface.h"
#include "TritonNode.h"
#include "TritonUserInterface.h"

#include "ByyIG.h"
#include "ByyChannel.h"
#include "ByyDisplaySignaler.h"
#include "ByyRenderer.h"
#include "terrain.h"
#include <osgEarthUtil/Fog>
#include "observerSettings.h"

ByyEnvironment::ByyEnvironment( ByyIG& ig ) 
	:myIG(ig)
{
	d_timeScale=0;

	d_dateTime=QDateTime::currentDateTime();

	connect(&timer,SIGNAL(timeout()),SLOT(timeout()));

	connect(ig.terrain(),SIGNAL(terrainLoaded(osgEarth::MapNode*)),SLOT(onTerrainLoaded(osgEarth::MapNode*)));

	connect(&ByyObserverSettings::instance(),SIGNAL(oceanVisibleChanged(bool)),SLOT(setOceanVisible(bool)));

	connect(ByyDisplaySignaler::instance(),SIGNAL(channelCreated(ByyChannel*)),this,SLOT(attachChannel(ByyChannel*)));
}

ByyEnvironment::~ByyEnvironment()
{

}

void ByyEnvironment::initiate()
{
	ByySilverLiningOptions silverLiningOptions;

	silverLiningOptions.user()="Singapore Technologies Engineering";
	silverLiningOptions.licenseCode()="031b0f0415190c1309001516171a010103021e49121a542d4e1e061b174531080204";
	silverLiningOptions.resourcePath()="..\\data\\Environment\\Sky\\Resources";
	silverLiningOptions.drawClouds()=false;
	silverLiningOptions.cloudsMaxAltitude()=1000;

	mySilverLiningUserInterface=new SilverLiningUserInterface;

	silverLiningSky=new ByySilverLiningNode(osgEarth::SpatialReference::get("wgs84"),silverLiningOptions,mySilverLiningUserInterface);
	silverLiningSky->setMinimumAmbient(osg::Vec4(0.6,0.6,0.6,1));
	myIG.renderer()->setEnvironmentRootNode(silverLiningSky);

	osg::Vec4 fogColor(0.66f, 0.7f, 0.81f, 1.0f);
	myFog = new osg::Fog;            
	myFog->setColor(fogColor);                
	myFog->setDensity( 0 );    
	myFog->setMode(osg::Fog::EXP);
	silverLiningSky->getOrCreateStateSet()->setAttributeAndModes( myFog, osg::StateAttribute::ON );   
	myFog->setUpdateCallback(new osgEarth::Util::FogCallback());

	osgEarth::Util::FogEffect* fogEffect = new osgEarth::Util::FogEffect;
	fogEffect->attach( silverLiningSky->getOrCreateStateSet() );

	mySilverLiningUserInterface->setFog(myFog);

	set2D(ByyObserverSettings::instance().is2D());

	connect(&ByyObserverSettings::instance(),SIGNAL(projectionChanged(bool)),this,SLOT(set2D(bool)));

	QDateTime dateTime=QDateTime::currentDateTime();
	dateTime.setTime(QTime(9,0,0));
	setDateTime(dateTime);
}

void ByyEnvironment::setDateTime( const QDateTime& datetime/*=QDateTime::currentDateTime()*/ )
{
	if (silverLiningSky)
	{
		QDateTime utcTime=datetime.toUTC();
		silverLiningSky->setDateTime(osgEarth::DateTime(utcTime.date().year(),
			utcTime.date().month(),utcTime.date().day(),utcTime.time().hour()));
	}

	d_dateTime=datetime;
}

void ByyEnvironment::setAmbient( double value )
{
	if (silverLiningSky)
	{
		silverLiningSky->setMinimumAmbient(osg::Vec4(value,value,value,1));
	}
}

void ByyEnvironment::setSeaLevel( int value )
{
	if (tritonOcean)
	{
		tritonOcean->setSeaLevel(value);
	}
}

void ByyEnvironment::setVisibility( double range )
{
	if (mySilverLiningUserInterface)
	{
		mySilverLiningUserInterface->setVisibility(range);
	}
	if (myTritonUserInterface.valid())
	{
		myTritonUserInterface->setVisibility(range);
	}
}

void ByyEnvironment::setTimeScale( double scale )
{
	d_timeScale=scale;

	if (scale==0&&timer.isActive())
	{
		timer.stop();
	}
	else if (scale!=0&&!timer.isActive())
	{
		timer.start(500);
	}
}

void ByyEnvironment::timeout()
{
	setDateTime(d_dateTime.addMSecs(d_timeScale*500));
}

int ByyEnvironment::seaLevel()
{
	int result=0;

	if (tritonOcean)
	{
		result=tritonOcean->getSeaLevel();
	}
	return result;
}

void ByyEnvironment::setPrecipitation( int precipitationType,double precipitationRate,double nearClip /*= -1*/, double farClip /*= -1*/, bool bUseDepthBuffer /*= false */ )
{
	if (mySilverLiningUserInterface)
	{
		mySilverLiningUserInterface->setPrecipitation(precipitationType,precipitationRate,nearClip,farClip,bUseDepthBuffer);
	}
}

void ByyEnvironment::setWind( double speed, double direction )
{
	if (myTritonUserInterface.get())
	{
		myTritonUserInterface->setWind(speed,osg::DegreesToRadians(direction));
	}
	if (mySilverLiningUserInterface)
	{
		mySilverLiningUserInterface->setWind(speed,direction-45);
	}
}

void ByyEnvironment::setCloudLayerType( int type )
{
	if (mySilverLiningUserInterface)
	{
		mySilverLiningUserInterface->setCloudLayerType(type);
	}
}

void ByyEnvironment::removeCloudLayer( )
{
	if (mySilverLiningUserInterface)
	{
		mySilverLiningUserInterface->setCloudLayerType(-1);
	}
}

void ByyEnvironment::onTerrainLoaded( osgEarth::MapNode* mapNode )
{
	silverLiningSky->removeChild(tritonOcean);

	ByyTritonOptions  tritonOptions;
	tritonOptions.user()="Cai Le";
	tritonOptions.licenseCode()="060d05596c20";
	tritonOptions.resourcePath()="..\\data\\Environment\\Ocean\\Resources";
	tritonOptions.maxAltitude()=20000;

	osg::ref_ptr<TritonUserInterface> myCallback=new TritonUserInterface;

	tritonOcean=new ByyTritonNode(mapNode,tritonOptions,myCallback);
	silverLiningSky->addChild(tritonOcean);

	tritonOcean->setNodeMask(ByyObserverSettings::instance().oceanVisible());

	myTritonUserInterface=myCallback;

	myTritonUserInterface->setFog(myFog);
}

void ByyEnvironment::setOceanVisible( bool b )
{
	if (tritonOcean)
	{
		tritonOcean->setNodeMask(b);
	}
}

void ByyEnvironment::set2D( bool is2d )
{
	if (tritonOcean)
	{
		tritonOcean->setNodeMask(!is2d);
	}
}

void ByyEnvironment::attachChannel( ByyChannel* channel )
{
	silverLiningSky->attach(channel->view(),0);
}