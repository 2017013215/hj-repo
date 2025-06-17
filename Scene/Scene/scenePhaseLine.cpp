#include "StdAfx.h"
#include "scenePhaseLine.h"
#include "ByyPhaseLine.h"

ByyScenePhaseLine::ByyScenePhaseLine( ByyPhaseLine* pl,ByyIG& ig ) 
	: ByySceneControlObject(pl,ig)
	,myPaseline(pl)
{
	connect(myPaseline,SIGNAL(startChanged(const osg::Vec3d&)),SLOT(setStart(const osg::Vec3d&)));
	connect(myPaseline,SIGNAL(endChanged(const osg::Vec3d&)),SLOT(setEnd(const osg::Vec3d&)));
}

ByyScenePhaseLine::~ByyScenePhaseLine()
{

}

void ByyScenePhaseLine::initiate()
{
	myLineString=new osgEarth::Symbology::LineString;

	myLineString->resize(2);

	(*myLineString)[0]=myPaseline->start();
	(*myLineString)[1]=myPaseline->end();

	osgEarth::Symbology::Style style;
	updateLineStyle(myPaseline->forceType(),style);
	style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width()=3;
	style.getOrCreate<osgEarth::Symbology::LineSymbol>()->tessellation()=20.0;

	style.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping()=osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
	style.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique()=osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_GPU;

	myFeature=new osgEarth::Features::Feature(myLineString,srs(),style);

	myFeatureNode=new osgEarth::Annotation::FeatureNode(mapNode(),myFeature);

	myRoot->addChild(myFeatureNode);

	myLabelNode->setPosition(osgEarth::GeoPoint(srs(),center()));
}

osg::Vec3d ByyScenePhaseLine::center()
{
	osg::Vec3d& start=myPaseline->start();
	osg::Vec3d& end=myPaseline->end();

	double out_lat,out_lon;

	osgEarth::GeoMath::midpoint(osg::DegreesToRadians(start.y()),osg::DegreesToRadians(start.x()),osg::DegreesToRadians(end.y()),osg::DegreesToRadians(end.x()),out_lat,out_lon);

	double alt=(start.z()-end.z())/2.0+end.z();

	return osg::Vec3d(osg::RadiansToDegrees(out_lon),osg::RadiansToDegrees(out_lat),alt);
}

double ByyScenePhaseLine::radius()
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

void ByyScenePhaseLine::setStart( const osg::Vec3d& p )
{
	(*myLineString)[0]=p;
	myFeatureNode->init();

	myLabelNode->setPosition(osgEarth::GeoPoint(srs(),center()));
}

void ByyScenePhaseLine::setEnd( const osg::Vec3d& p )
{
	(*myLineString)[1]=p;
	myFeatureNode->init();

	myLabelNode->setPosition(osgEarth::GeoPoint(srs(),center()));
}

void ByyScenePhaseLine::setForceType( ByyForceType side )
{
	__super::setForceType(side);

	osgEarth::Symbology::Style style=myFeature->style().get();

	updateLineStyle(side,style);

	myFeature->style()=style;

	myFeatureNode->init();
}

void ByyScenePhaseLine::setClampToTerrain( bool clamp )
{
	__super::setClampToTerrain(clamp);

	osgEarth::Symbology::Style style=myFeatureNode->getStyle();

	updateClampStyle(clamp,style);

	myFeatureNode->setStyle(style);
}

ByySceneControlObject* ByyScenePhaseLine::create( ByyControlObject* obj,ByyIG& ig )
{
	ByyPhaseLine* wp=dynamic_cast<ByyPhaseLine*>(obj);

	if (wp)
	{
		return new ByyScenePhaseLine(wp,ig);
	}

	return 0;
}
