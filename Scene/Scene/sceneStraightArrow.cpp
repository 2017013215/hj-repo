#include "StdAfx.h"
#include "sceneStraightArrow.h"
#include "ByyStraightArrow.h"
#include "plotUtils.h"
#include "utils.h"

ByySceneStraightArrow::ByySceneStraightArrow( ByyStraightArrow* straightArrow,ByyIG& ig )
	: ByySceneControlObject(straightArrow,ig)
	,myStraightArrow(straightArrow)
{
	connect(myStraightArrow,SIGNAL(pointsChanged()),SLOT(updatePoints()));
}

ByySceneStraightArrow::~ByySceneStraightArrow()
{

}

void ByySceneStraightArrow::initiate()
{
	myLine=new osgEarth::Symbology::LineString;
	myArrow=new osgEarth::Symbology::Polygon;

	myBody=new osgEarth::Symbology::MultiGeometry;
	myBody->add(myLine);
	myBody->add(myArrow);

	updatePoints();

	osgEarth::Symbology::Style style;
	style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color() = forceTypeColorAlpha(myStraightArrow->forceType(),0.9);
	style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width()=LINE_WIDTH;
	style.getOrCreate<osgEarth::Symbology::PolygonSymbol>()->fill()->color()=forceTypeColorAlpha(myStraightArrow->forceType(),0.9);
	style.getOrCreate<osgEarth::Symbology::LineSymbol>()->tessellation()=20.0;

	style.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping()=osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
	style.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique()=osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_GPU;

	myFeature=new osgEarth::Features::Feature(myBody,srs(),style);

	myFeatureNode=new osgEarth::Annotation::FeatureNode(mapNode(),myFeature);
	
	myRoot->addChild(myFeatureNode);

	myLabelNode->setPosition(osgEarth::GeoPoint(srs(),center()));
}

osg::Vec3d ByySceneStraightArrow::center()
{
	osg::Vec3d& start=myStraightArrow->points().value(0);
	osg::Vec3d& end=myStraightArrow->points().value(1);

	double out_lat,out_lon;

	osgEarth::GeoMath::midpoint(osg::DegreesToRadians(start.y()),osg::DegreesToRadians(start.x()),osg::DegreesToRadians(end.y()),osg::DegreesToRadians(end.x()),out_lat,out_lon);

	double alt=(start.z()-end.z())/2.0+end.z();

	double test=osg::RadiansToDegrees(out_lon);
	double test2=osg::RadiansToDegrees(out_lat);

	return osg::Vec3d(osg::RadiansToDegrees(out_lon),osg::RadiansToDegrees(out_lat),alt);
}

double ByySceneStraightArrow::radius()
{
	osg::BoundingBox bb;

	osg::EllipsoidModel em;

	double x,y,z;

	for (int i=0;i!=myLine->size();++i)
	{
		em.convertLatLongHeightToXYZ(osg::DegreesToRadians(myLine->at(i).y()),osg::DegreesToRadians(myLine->at(i).x()),myLine->at(i).z(),x,y,z);

		bb.expandBy(x,y,z);
	}

	return bb.radius();
}

void ByySceneStraightArrow::setForceType( ByyForceType side )
{
	__super::setForceType(side);

	osgEarth::Symbology::Style style=myFeature->style().get();

	updateLineStyle(side,style);
	style.getOrCreate<osgEarth::Symbology::PolygonSymbol>()->fill()->color()=forceTypeColorAlpha(myStraightArrow->forceType(),0.9);
	myFeature->style()=style;

	myFeatureNode->init();
}

void ByySceneStraightArrow::setClampToTerrain( bool clamp )
{
	__super::setClampToTerrain(clamp);

	osgEarth::Symbology::Style style=myFeatureNode->getStyle();

	updateClampStyle(clamp,style);

	myFeatureNode->setStyle(style);
}

ByySceneControlObject* ByySceneStraightArrow::create( ByyControlObject* obj,ByyIG& ig )
{
	ByyStraightArrow* wp=dynamic_cast<ByyStraightArrow*>(obj);

	if (wp)
	{
		return new ByySceneStraightArrow(wp,ig);
	}

	return 0;
}

void ByySceneStraightArrow::updatePoints()
{
	ByyVec3dArray pnts=ByyPlotUtils::generateStraightArrow(myStraightArrow->points());
	myLine->resize(2);
 	myArrow->resize(3);
// 	for (int i=0;i!=pnts.size();++i)
// 	{
// 		(*myLine)[i]=pnts.at(i);
// 	}
	(*myLine)[0]=myStraightArrow->points().at(0);
	(*myLine)[1]=myStraightArrow->points().at(1);
	(*myArrow)[0]=pnts.at(2);
	(*myArrow)[1]=pnts.at(3);
	(*myArrow)[2]=pnts.at(4);

	if (myFeatureNode)
	{
		myFeatureNode->init();
	}

	if (myLabelNode)
	{
		myLabelNode->setPosition(osgEarth::GeoPoint(srs(),center()));
	}
}
