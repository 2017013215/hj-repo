#include "StdAfx.h"
#include "sceneCircle.h"
#include "ByyCircle.h"
#include "utils.h"

ByySceneCircle::ByySceneCircle( ByyCircle *ci,ByyIG& ig ) 
	: ByySceneControlObject(ci,ig)
	,myCircle(ci)
{
	connect(myCircle,SIGNAL(positionChanged(const osg::Vec3d&)),SLOT(setPosition(const osg::Vec3d&)));
	connect(myCircle,SIGNAL(radiusChanged(double)),SLOT(setRadius(double)));
}

ByySceneCircle::~ByySceneCircle()
{

}

void ByySceneCircle::initiate()
{
	osgEarth::Symbology::Style style;

	style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color() = forceTypeColorAlpha(myCircle->forceType(),0.9);
	style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = LINE_WIDTH;
	style.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
	style.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique()=osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_DRAPE;

	myCircleNode=new osgEarth::Annotation::CircleNode(mapNode(),osgEarth::GeoPoint(srs(),myCircle->position()),myCircle->radius(),style);

	myRoot->addChild(myCircleNode);

	setPosition(myCircle->position());
}

void ByySceneCircle::update()
{

}

osg::Vec3d ByySceneCircle::center()
{
	return myCircleNode->getPosition().vec3d();
}

double ByySceneCircle::radius()
{
	return myCircle->radius();
}

void ByySceneCircle::setPosition( const osg::Vec3d& pos )
{
	myCircleNode->setPosition(osgEarth::GeoPoint(srs(),pos));

	myLabelNode->setPosition(osgEarth::GeoPoint(srs(),pos));
}

void ByySceneCircle::setRadius( double r )
{
	myCircleNode->setRadius(r);
}

void ByySceneCircle::setForceType( ByyForceType side )
{
	__super::setForceType(side);

	osgEarth::Symbology::Style style=myCircleNode->getStyle();

	updatePolygonStyle(side,style);

	myCircleNode->setStyle(style);
}

void ByySceneCircle::setClampToTerrain( bool clamp )
{
	__super::setClampToTerrain(clamp);

	osgEarth::Symbology::Style style=myCircleNode->getStyle();

	updateClampStyle(clamp,style);

	myCircleNode->setStyle(style);
}

ByySceneControlObject* ByySceneCircle::create( ByyControlObject* obj,ByyIG& ig )
{
	ByyCircle* wp=dynamic_cast<ByyCircle*>(obj);

	if (wp)
	{
		return new ByySceneCircle(wp,ig);
	}

	return 0;
}
