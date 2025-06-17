#include "StdAfx.h"
#include "sceneEllipse.h"
#include "ByyEllipse.h"
#include "utils.h"

ByySceneEllipse::ByySceneEllipse( ByyEllipse* el,ByyIG& ig ) 
	: ByySceneControlObject(el,ig)
	,myEllipse(el)
{
	connect(myEllipse,SIGNAL(positionChanged(const osg::Vec3d&)),SLOT(setPosition(const osg::Vec3d&)));
	connect(myEllipse,SIGNAL(majorSemiAxisChanged(double)),SLOT(setMajorSemiAxis(double)));
	connect(myEllipse,SIGNAL(minorSemiAxisChanged(double)),SLOT(setMinorSemiAxis(double)));
	connect(myEllipse,SIGNAL(rotationAngleChanged(double)),SLOT(setRotationAngle(double)));
}

ByySceneEllipse::~ByySceneEllipse()
{

}

void ByySceneEllipse::initiate()
{
	osgEarth::Symbology::Style style;

	style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color() = forceTypeColorAlpha(myEllipse->forceType(),0.9);
	style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = LINE_WIDTH;
	style.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
	style.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique()=osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_DRAPE;

	myEllipseNode=new osgEarth::Annotation::EllipseNode(mapNode(),osgEarth::GeoPoint(srs(),myEllipse->position()),myEllipse->majorSemiAxis(),myEllipse->minorSemiAxis(),myEllipse->rotationAngle(),style);

	myRoot->addChild(myEllipseNode);

	setPosition(myEllipse->position());
}


osg::Vec3d ByySceneEllipse::center()
{
	return myEllipseNode->getPosition().vec3d();
}

double ByySceneEllipse::radius()
{
	return myEllipse->majorSemiAxis();
}

void ByySceneEllipse::setPosition( const osg::Vec3d& pos )
{
	myEllipseNode->setPosition(osgEarth::GeoPoint(srs(),pos));
	myLabelNode->setPosition(osgEarth::GeoPoint(srs(),pos));
}

void ByySceneEllipse::setMajorSemiAxis( double val )
{
	myEllipseNode->setRadiusMajor(val);
}

void ByySceneEllipse::setMinorSemiAxis( double val )
{
	myEllipseNode->setRadiusMinor(val);
}

void ByySceneEllipse::setRotationAngle( double val )
{
	myEllipseNode->setRotationAngle(val);
}

void ByySceneEllipse::setForceType( ByyForceType side )
{
	__super::setForceType(side);

	osgEarth::Symbology::Style style=myEllipseNode->getStyle();

	updatePolygonStyle(side,style);

	myEllipseNode->setStyle(style);
}

void ByySceneEllipse::setClampToTerrain( bool clamp )
{
	__super::setClampToTerrain(clamp);

	osgEarth::Symbology::Style style=myEllipseNode->getStyle();

	updateClampStyle(clamp,style);

	myEllipseNode->setStyle(style);
}

ByySceneControlObject* ByySceneEllipse::create( ByyControlObject* obj,ByyIG& ig )
{
	ByyEllipse* wp=dynamic_cast<ByyEllipse*>(obj);

	if (wp)
	{
		return new ByySceneEllipse(wp,ig);
	}

	return 0;
}
