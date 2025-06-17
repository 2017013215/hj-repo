#include "StdAfx.h"
#include "ByyArcCreator.h"
#include "ByyArc.h"

ByyArcCreator::ByyArcCreator(ByyIG& ig) : ByyCircleCreator(ig)
{
}


ByyArcCreator::~ByyArcCreator(void)
{
}



ByyControlObject* ByyArcCreator::accept()
{
	ByyArc *myArc=new ByyArc;

	myArc->setPosition(myCircleNode->getPosition().vec3d());
	myArc->setRadius(myCircleNode->getRadius().getValue());
	myArc->setStartAngle(myCircleNode->getArcStart().getValue());
	myArc->setEndAngle(myCircleNode->getArcEnd().getValue());

	return myArc;
}

ByyTacticalGraphicsCreator* ByyArcCreator::create( ByyIG& ig )
{
	return new ByyArcCreator(ig);
}

unsigned int ByyArcCreator::getFixedPointCount()
{
	return 3;
}

void ByyArcCreator::update( const ByyVec3dArray& points )
{
	if (points.size()>=1)
	{
		myCircleNode->setPosition(osgEarth::GeoPoint(myMapNode->getMapSRS(),points[0]));
	}

	if (points.size()>=2)
	{
		double distance=osgEarth::GeoMath::distance(points[0],points[1],myMapNode->getMapSRS());
		double _bearing=osgEarth::GeoMath::bearing(
			osg::DegreesToRadians(points[0].y()),
			osg::DegreesToRadians(points[0].x()),
			osg::DegreesToRadians(points[1].y()),
			osg::DegreesToRadians(points[1].x()));

		myCircleNode->setRadius(distance);
		myCircleNode->setArcStart(osg::RadiansToDegrees(_bearing));
	}

	if (points.size()>=3)
	{
		double _bearing=osgEarth::GeoMath::bearing(
			osg::DegreesToRadians(points[0].y()),
			osg::DegreesToRadians(points[0].x()),
			osg::DegreesToRadians(points[2].y()),
			osg::DegreesToRadians(points[2].x()));

		myCircleNode->setArcEnd(osg::RadiansToDegrees(_bearing));
	}
}
