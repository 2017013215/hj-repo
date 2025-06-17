#include "StdAfx.h"
#include "circleCreator.h"
#include "ByyEventProcessor.h"
#include "ByyCircle.h"
#include "ByyIG.h"
#include "terrain.h"
#include "utils.h"

ByyCircleCreator::ByyCircleCreator( ByyIG& ig ) 
	: ByyFixedPointSetCreator(ig)
{
	osgEarth::Symbology::Style myCircleStyle;


	myCircleStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = LINE_WIDTH;
	myCircleStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
	myCircleStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_DRAPE;

	myCircleNode=new osgEarth::Annotation::CircleNode(myMapNode.get(),osgEarth::GeoPoint(),
		osgEarth::Distance(),myCircleStyle);

	myRoot->addChild(myCircleNode);
}

ByyCircleCreator::~ByyCircleCreator()
{
	myRoot->removeChild(myCircleNode);
}

ByyControlObject* ByyCircleCreator::accept()
{
	ByyCircle *myCircle=new ByyCircle;

	myCircle->setPosition(myCircleNode->getPosition().vec3d());
	myCircle->setRadius(myCircleNode->getRadius().getValue());

	return myCircle;
}

ByyTacticalGraphicsCreator* ByyCircleCreator::create( ByyIG& ig )
{
	return new ByyCircleCreator(ig);
}

unsigned int ByyCircleCreator::getFixedPointCount()
{
	return 2;
}

void ByyCircleCreator::update( const ByyVec3dArray& points )
{
	if (points.size()>=1)
	{
		myCircleNode->setPosition(osgEarth::GeoPoint(myMapNode->getMapSRS(),points[0]));
	}

	if (points.size()>=2)
	{
		double distance=osgEarth::GeoMath::distance(points[0],points[1],myMapNode->getMapSRS());

		myCircleNode->setRadius(distance);
	}
}
