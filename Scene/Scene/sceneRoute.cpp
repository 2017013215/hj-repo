#include "StdAfx.h"
#include "sceneRoute.h"
#include "ByyRoute.h"

ByySceneRoute::ByySceneRoute( ByyRoute* route,ByyIG& ig )
	: ByySceneControlObject(route,ig)
	,myRoute(route)
{
	connect(myRoute,SIGNAL(pointsChanged()),SLOT(updatePoints()));
}

ByySceneRoute::~ByySceneRoute()
{

}

void ByySceneRoute::initiate()
{
	myLineString=new osgEarth::Symbology::LineString;

	updatePoints();

	osgEarth::Symbology::Style style;
	updateLineStyle(myRoute->forceType(),style);
	style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width()=3;

	style.getOrCreate<osgEarth::Symbology::LineSymbol>()->tessellation()=20.0;

	style.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping()=osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
	style.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique()=osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_GPU;

	myFeature=new osgEarth::Features::Feature(myLineString,srs(),style);

	myFeatureNode=new osgEarth::Annotation::FeatureNode(mapNode(),myFeature);

	myRoot->addChild(myFeatureNode);

	myLabelNode->setPosition(osgEarth::GeoPoint(srs(),center()));
}

osg::Vec3d ByySceneRoute::center()
{
	osgEarth::Bounds bd=myLineString->getBounds();

	return osg::Vec3d(bd.center2d(),bd.depth()/2.0);
}

double ByySceneRoute::radius()
{
	osg::BoundingBox bb;

	osg::EllipsoidModel em;

	double x,y,z;

	for (int i=0;i!=myLineString->size();++i)
	{
		em.convertLatLongHeightToXYZ(osg::DegreesToRadians(myLineString->at(i).y()),osg::DegreesToRadians(myLineString->at(i).x()),myLineString->at(i).z(),x,y,z);

		bb.expandBy(x,y,z);
	}

	return bb.radius();
}

void ByySceneRoute::updatePoints()
{
	const ByyVec3dArray &pnts=myRoute->points();

	myLineString->resize(pnts.size());

	for (int i=0;i!=pnts.size();++i)
	{
		(*myLineString)[i]=pnts[i];
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

void ByySceneRoute::setForceType( ByyForceType side )
{
	__super::setForceType(side);

	osgEarth::Symbology::Style style=myFeature->style().get();

	updateLineStyle(side,style);

	myFeature->style()=style;

	myFeatureNode->init();
}

void ByySceneRoute::setClampToTerrain( bool clamp )
{
	__super::setClampToTerrain(clamp);

	osgEarth::Symbology::Style style=myFeature->style().get();

	updateClampStyle(clamp,style);

	myFeatureNode->setStyle(style);

	/*myFeature->style()=style;

	myFeatureNode->init();*/
}

ByySceneControlObject* ByySceneRoute::create( ByyControlObject* obj,ByyIG& ig )
{
	ByyRoute* wp=dynamic_cast<ByyRoute*>(obj);

	if (wp)
	{
		return new ByySceneRoute(wp,ig);
	}

	return 0;
}
