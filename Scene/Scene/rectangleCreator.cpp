#include "StdAfx.h"
#include "rectangleCreator.h"
#include "ByyRectangle.h"
#include "utils.h"

ByyRectangleCreator::ByyRectangleCreator( ByyIG& ig)
	:ByyFixedPointSetCreator(ig)
{
	osgEarth::Symbology::Style myRectStyle;


	myRectStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = LINE_WIDTH;
	myRectStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
	myRectStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_DRAPE;




	myRectNode=new osgEarth::Annotation::RectangleNode(myMapNode.get(),osgEarth::GeoPoint(myMapNode->getMapSRS(),0,0),
		osgEarth::Distance(),osgEarth::Distance(),myRectStyle);

	myRoot->addChild(myRectNode);
}

ByyRectangleCreator::~ByyRectangleCreator()
{
	myRoot->removeChild(myRectNode);
}

ByyControlObject* ByyRectangleCreator::accept()
{
	ByyRectangle *rectangle=new ByyRectangle;

	ByyVec3dArray result(4);

	result[0]=myRectNode->getUpperLeft().vec3d();
	result[0].z()=10000;
	result[1]=myRectNode->getLowerLeft().vec3d();
	result[1].z()=10000;
	result[2]=myRectNode->getLowerRight().vec3d();
	result[2].z()=10000;
	result[3]=myRectNode->getUpperRight().vec3d();
	result[3].z()=10000;

	rectangle->setPoints(result);

	return rectangle;
}

ByyTacticalGraphicsCreator* ByyRectangleCreator::create( ByyIG& ig )
{
	return new ByyRectangleCreator(ig);
}

unsigned int ByyRectangleCreator::getFixedPointCount()
{
	return 2;
}

void ByyRectangleCreator::update( const ByyVec3dArray& points )
{
	if (points.size()>=1)
	{
		myRectNode->setPosition(osgEarth::GeoPoint(myMapNode->getMapSRS(),points[0]));
	}

	if (points.size()>=2)
	{
		osg::Vec3d tempPoint=points[0];

		tempPoint.x()=points[1].x();

		double halfWidth=osgEarth::GeoMath::distance(points[0],tempPoint,myMapNode->getMapSRS());

		tempPoint=points[0];

		tempPoint.y()=points[1].y();

		double halfHeight=osgEarth::GeoMath::distance(points[0],tempPoint,myMapNode->getMapSRS());

		myRectNode->setSize(osgEarth::Linear(halfWidth*2,osgEarth::Units::METERS),osgEarth::Linear(halfHeight*2,osgEarth::Units::METERS));
	}
}
