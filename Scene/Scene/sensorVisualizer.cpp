#include "StdAfx.h"
#include "sensorVisualizer.h"
#include "entitySettings.h"
#include "ByyDescribeComponent.h"
#include "ByyDescParamList.h"
#include "entityObject.h"
#include <osg/FrontFace>
#include <osg/LineWidth>
#include <osg/BlendFunc>
#include <osgUtil/SmoothingVisitor>

#include "ByyIG.h"
#include "terrain.h"
#include "remoteEntity.h"
#include "ByyEllipsoidArcModel.h"

ByySensorVisualizer::ByySensorVisualizer( ByyEntityObject& entObj,ByyIG& ig )
	:ByyStateVisualizer(entObj,ig)
	,myRadius(1000)
	,myAzMin(-90)
	,myAzMax(90)
	,myElevMin(-20)
	,myElevMax(20)
	,myLocatedH(0)
	,myRunningState(0)
{
	setVisible(ByyEntitySettings::instance().electronicEnabled());

	connect(&ByyEntitySettings::instance(),SIGNAL(electronicEnabledChanged(bool)),this,SLOT(setVisible(bool)));

	if (!entObj.isLocal())
	{
		connect(&ByySensorSRSignaler::instance(),SIGNAL(powerSwitch(ByyRemoteEntity*,ByySensorSR*,QString,bool)),
			this,SLOT(hasPowerSwitch(ByyRemoteEntity*,ByySensorSR*,QString,bool)));

		connect(&ig,SIGNAL(preTick()),this,SLOT(update()));
	}
}

ByySensorVisualizer::~ByySensorVisualizer()
{

}

const QString& ByySensorVisualizer::theType()
{
	static QString _type("MissileElectronic");
	return _type;
}

const QString& ByySensorVisualizer::type()
{
	return theType();
}

bool ByySensorVisualizer::isLocalized() const
{
	return true;
}

void ByySensorVisualizer::setComponent( const ByyDescribeComponent& component )
{
	const ByyDescParamList& paramList=component.getParamList();

	myTag=component.getTag();

	myRadius=paramList.value("MaxDetectDistance").toDouble()*1000;
	myAzMin=paramList.value("MinAz").toFloat();
	myAzMax=paramList.value("MaxAz").toFloat();
	myElevMin=paramList.value("MinEl").toFloat();
	myElevMax=paramList.value("MaxEl").toFloat();
	myLocatedH=paramList.value("LocatedH").toFloat();

	init();
}

void ByySensorVisualizer::init()
{
	if (mySensorRange.valid())
	{
		root()->removeChild(mySensorRange);
		mySensorRange=0;
	}

	if (myAzMin==myAzMax||myElevMin==myElevMax)
		return;

	float azCenter=(myAzMax+myAzMin)/2.0;
	float azSweep=(myAzMax-myAzMin)/2.0;
	float elCenter=(myElevMax+myElevMin)/2.0;
	float elSweep=(myElevMax-myElevMin)/2.0;

	mySensorRange=new ByyEllipsoidArcModel(azCenter,azSweep,elCenter,elSweep,myRadius);

	myRoot->addChild(mySensorRange);
}

const QString& ByySensorVisualizer::getSensorTag()
{
	return myTag;
}

void ByySensorVisualizer::update()
{
	if (myRunningState==0||myRunningState->myBeamWidth==0||(!myPlane&&myRunningState->myAntennaAz==0))
	{
		return;
	}

	if (!myPlane.valid())
	{
		if (myAzMin==myAzMax||myElevMin==myElevMax)
			return;


		float azCenter=0;
		float azSweep=myRunningState->myBeamWidth>0?(myRunningState->myBeamWidth/2.0):10;

		float elCenter=(myElevMax+myElevMin)/2.0;
		float elSweep=(myElevMax-myElevMin)/2.0;

		ByyEllipsoidArcModel* plane=new ByyEllipsoidArcModel(azCenter,azSweep,elCenter,elSweep,myRadius);

		myPlane=new osg::MatrixTransform;

		myPlane->addChild(plane);

		root()->addChild(myPlane);
	}

	myPlane->setMatrix(osg::Matrix::rotate(osg::DegreesToRadians(myRunningState->myAntennaAz),osg::Z_AXIS));
}

void ByySensorVisualizer::hasPowerSwitch( ByyRemoteEntity* ent,ByySensorSR* sensor,QString device,bool power )
{
	if (ent!=&myEntObj)
		return;

	if (device!=myTag)
		return;

	if (power)
	{
		myRunningState=sensor->getDeviceState(device);
	}
	else
	{
		myRunningState=0;

		if (myPlane)
		{
			root()->removeChild(myPlane);
			myPlane=0;
		}
	}
}
