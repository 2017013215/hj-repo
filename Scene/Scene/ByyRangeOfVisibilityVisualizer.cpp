#include "StdAfx.h"
#include "ByyRangeOfVisibilityVisualizer.h"

#include "ByyIG.h"
#include "terrain.h"
#include "entitySettings.h"
#include "ByyDescribeComponent.h"
#include "ByyDescParamList.h"
#include "entityObject.h"

ByyRangeOfVisibilityVisualizer::ByyRangeOfVisibilityVisualizer( ByyEntityObject& entObj,ByyIG& ig )
	:ByyStateVisualizer(entObj,ig)
	,myRadius(1000)
	,myAzMin(-90)
	,myAzMax(90)
	,myElevMin(-20)
	,myElevMax(20)
	,myLocatedH(0)
{
	osgEarth::Symbology::Style style;

	style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color()=osgEarth::Symbology::Color(1,0,1,0.75);
	style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width()=3;

	style.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping()=osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;

	style.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique()=osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_DRAPE;

	myCircleNode=new osgEarth::Annotation::CircleNode(
		myIG.terrain()->mapNode(),osgEarth::GeoPoint(),1,style);

	root()->addChild(myCircleNode);

	myCircleNode->setNodeMask(0x0);

	setVisible(ByyEntitySettings::instance().electronicEnabled());
	connect(&ByyEntitySettings::instance(),SIGNAL(electronicEnabledChanged(bool)),this,SLOT(setVisible(bool)));
	connect(&myEntObj,SIGNAL(positionChanged(const osg::Vec3d&)),this,SLOT(update(const osg::Vec3d&)));
}

ByyRangeOfVisibilityVisualizer::~ByyRangeOfVisibilityVisualizer()
{

}

const QString& ByyRangeOfVisibilityVisualizer::theType()
{
	static QString _type("RangeOfVisibility");
	return _type;
}

const QString& ByyRangeOfVisibilityVisualizer::type()
{
	return theType();
}

void ByyRangeOfVisibilityVisualizer::setComponent( const ByyDescribeComponent& component )
{
	const ByyDescParamList& paramList=component.getParamList();

	myTag=component.getTag();

	myRadius=paramList.value("MaxRange").toDouble()*1000;
	myAzMin=paramList.value("MinAz").toFloat();
	myAzMax=paramList.value("MaxAz").toFloat();
	myElevMin=paramList.value("MinEl").toFloat();
	myElevMax=paramList.value("MaxEl").toFloat();
	myLocatedH=paramList.value("LocatedH").toFloat();

	update(myEntObj.position());
}

void ByyRangeOfVisibilityVisualizer::update( const osg::Vec3d& position )
{
	myCircleNode->setPosition(osgEarth::GeoPoint(mapNode()->getMapSRS(),position.x(),position.y()));

	double visibilityRange=4.12*1000*(sqrt(position.z())+sqrt(myLocatedH));

	if (visibilityRange<myRadius)
	{
		myCircleNode->setRadius(visibilityRange);
	}

	myCircleNode->setNodeMask(visibilityRange<myRadius);
}

const QString& ByyRangeOfVisibilityVisualizer::getSensorTag()
{
	return myTag;
}
