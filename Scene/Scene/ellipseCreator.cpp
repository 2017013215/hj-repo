#include "StdAfx.h"
#include "ellipseCreator.h"
#include "ByyEventProcessor.h"
#include "ByyEllipse.h"
#include "ByyIG.h"
#include "terrain.h"
#include "utils.h"

ByyEllipseCreator::ByyEllipseCreator( ByyIG& ig ) 
	: ByyFixedPointSetCreator(ig)
{
	osgEarth::Symbology::Style myEllipseStyle;

	myEllipseStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = LINE_WIDTH;
	myEllipseStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
	myEllipseStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_DRAPE;

	myEllipseNode=new osgEarth::Annotation::EllipseNode(myMapNode.get(),osgEarth::GeoPoint(),
		osgEarth::Distance(),osgEarth::Distance(),osgEarth::Angle(),myEllipseStyle);

	myRoot->addChild(myEllipseNode);
}

ByyEllipseCreator::~ByyEllipseCreator()
{
	myRoot->removeChild(myEllipseNode);
}

ByyControlObject* ByyEllipseCreator::accept()
{
	ByyEllipse *myEllipse=new ByyEllipse;

	myEllipse->setPosition(myEllipseNode->getPosition().vec3d());
	myEllipse->setMajorSemiAxis(myEllipseNode->getRadiusMajor().getValue());
	myEllipse->setMinorSemiAxis(myEllipseNode->getRadiusMinor().getValue());
	myEllipse->setRotationAngle(myEllipseNode->getRotationAngle().getValue());

	return myEllipse;
}

ByyTacticalGraphicsCreator* ByyEllipseCreator::create( ByyIG& ig )
{
	return new ByyEllipseCreator(ig);
}

unsigned int ByyEllipseCreator::getFixedPointCount()
{
	return 3;
}

void ByyEllipseCreator::update( const ByyVec3dArray& points )
{
	if (points.size()>=1)
	{
		myEllipseNode->setPosition(osgEarth::GeoPoint(myMapNode->getMapSRS(),points[0]));
	}

	if (points.size()>=2)
	{
		double distance=osgEarth::GeoMath::distance(points[0],points[1],myMapNode->getMapSRS());
		
		myEllipseNode->setRadiusMajor(osgEarth::Distance(distance,osgEarth::Units::METERS));
		myEllipseNode->setRadiusMinor(osgEarth::Distance(distance,osgEarth::Units::METERS));

		/*double out_latRad,out_lonRad;

		osgEarth::GeoPoint position=myEllipseNode->getPosition();

		osgEarth::GeoMath::destination(osg::DegreesToRadians(position.y()),osg::DegreesToRadians(position.x()),
			osg::DegreesToRadians(0.0),distance,out_latRad,out_lonRad);

		position.x()=osg::RadiansToDegrees(out_lonRad);
		position.y()=osg::RadiansToDegrees(out_latRad);*/
	}

	if (points.size()>=3)
	{
		double distance=osgEarth::GeoMath::distance(points[0],points[2],myMapNode->getMapSRS());
		myEllipseNode->setRadiusMinor(osgEarth::Distance(distance,osgEarth::Units::METERS));

		/*double out_latRad,out_lonRad;

		osgEarth::GeoPoint position=myEllipseNode->getPosition();

		myEllipseNode->setRadiusMinor(osgEarth::Distance(distance,osgEarth::Units::METERS));

		osgEarth::GeoMath::destination(osg::DegreesToRadians(position.y()),osg::DegreesToRadians(position.x()),
			osg::DegreesToRadians(90.0),distance,out_latRad,out_lonRad);

		position.x()=osg::RadiansToDegrees(out_lonRad);
		position.y()=osg::RadiansToDegrees(out_latRad);*/
	}
}
