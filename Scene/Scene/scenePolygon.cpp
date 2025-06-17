#include "StdAfx.h"
#include "scenePolygon.h"
#include "ByyPolygon.h"
#include "utils.h"

ByyScenePolygon::ByyScenePolygon( ByyPolygon* pol,ByyIG& ig ) 
	: ByySceneControlObject(pol,ig)
	,myPolygon(pol)
{
	connect(myPolygon,SIGNAL(pointsChanged()),SLOT(updatePoints()));
}

ByyScenePolygon::~ByyScenePolygon()
{

}

void ByyScenePolygon::initiate()
{
	if(isLineString())
		myPolygonNode=new osgEarth::Symbology::LineString;
	else
		myPolygonNode=new osgEarth::Symbology::Polygon;

	updatePoints();

	osgEarth::Symbology::Style style;
	style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color() = forceTypeColorAlpha(myPolygon->forceType(),0.9);
	style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = LINE_WIDTH;
	style.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
	style.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique()=osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_DRAPE;

	//style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->stipple() = 0x0FFF;

	//updatePolygonStyle(myPolygon->forceType(),style);
	//style.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping()=osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;

	myFeature=new osgEarth::Features::Feature(myPolygonNode,srs(),style);
	myFeatureNode=new osgEarth::Annotation::FeatureNode(mapNode(),myFeature);

	myRoot->addChild(myFeatureNode);

	myLabelNode->setPosition(osgEarth::GeoPoint(srs(),center()));
}

osg::Vec3d ByyScenePolygon::center()
{
	return osg::Vec3d(myPolygonNode->getBounds().center2d(),myPolygonNode->getBounds().depth()/2.0);
}

double ByyScenePolygon::radius()
{
	osg::BoundingBox bb;

	osg::EllipsoidModel em;

	double x,y,z;

	for (int i=0;i!=myPolygonNode->size();++i)
	{
		em.convertLatLongHeightToXYZ(osg::DegreesToRadians(myPolygonNode->at(i).y()),osg::DegreesToRadians(myPolygonNode->at(i).x()),myPolygonNode->at(i).z(),x,y,z);

		bb.expandBy(x,y,z);
	}

	return bb.radius();
}

void ByyScenePolygon::updatePoints()
{
	const ByyVec3dArray& pnts=getPoints();
	myPolygonNode->resize(pnts.size());

	for (int i=0;i!=pnts.size();++i)
	{
		(*myPolygonNode)[i]=pnts.at(i);
	}

	if (myFeatureNode)
	{
		myFeatureNode->init();
	}

	if (myLabelNode)
	{
		myLabelNode->setPosition(osgEarth::GeoPoint(srs(),center()));
	}
}

void ByyScenePolygon::setForceType( ByyForceType side )
{
	__super::setForceType(side);

	osgEarth::Symbology::Style style=myFeature->style().get();

	updatePolygonStyle(side,style);

	myFeature->style()=style;
	myFeatureNode->init();
}

void ByyScenePolygon::setClampToTerrain( bool clamp )
{
	__super::setClampToTerrain(clamp);

	osgEarth::Symbology::Style style=myFeatureNode->getStyle();

	updateClampStyle(clamp,style);

	myFeatureNode->setStyle(style);
}

ByySceneControlObject* ByyScenePolygon::create( ByyControlObject* obj,ByyIG& ig )
{
	ByyPolygon* wp=dynamic_cast<ByyPolygon*>(obj);

	if (wp)
	{
		return new ByyScenePolygon(wp,ig);
	}

	return 0;
}

ByyVec3dArray ByyScenePolygon::getPoints()
{
	return myPolygon->points();
}
