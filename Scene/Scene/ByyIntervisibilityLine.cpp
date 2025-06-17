#include "StdAfx.h"
#include "ByyIntervisibilityLine.h"
#include "ByyIG.h"
#include "ByyRenderer.h"
#include "terrain.h"

#include <osgEarth/DPLineSegmentIntersector>

ByyIntervisibilityLine::ByyIntervisibilityLine(ByyIG& ig)
	: ByyMeasureItem(ig)
	,firstAltJustValue(4)
	,secondAltJustValue(4)
{
	myRoot=ig.renderer()->getControlObjectRoot();

	myFirstLine=new osgEarth::Symbology::LineString;
	myFirstLine->resize(2);

	osgEarth::Symbology::Style pathStyle;
	pathStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color() = osgEarth::Color::Green;
	pathStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = 2.0f;
	pathStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->tessellation() = 20;
	/*pathStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
	pathStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_GPU;*/
	//pathStyle.getOrCreate<osgEarth::Symbology::RenderSymbol>()->depthTest()=false;
	//pathStyle.getOrCreate<osgEarth::Symbology::RenderSymbol>()->order()=13;

	osgEarth::MapNode* myMapNode=myIG.terrain()->mapNode();
	myFirstFeature=new osgEarth::Features::Feature(myFirstLine, myMapNode->getMapSRS(), pathStyle);

	myFirstNode = new osgEarth::Annotation::FeatureNode(myMapNode, myFirstFeature);


	pathStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color() = osgEarth::Color::Red;

	mySecondLine=new osgEarth::Symbology::LineString;
	mySecondLine->resize(2);

	mySecondFeature=new osgEarth::Features::Feature(mySecondLine, myMapNode->getMapSRS(), pathStyle);

	mySecondNode = new osgEarth::Annotation::FeatureNode(myMapNode, mySecondFeature);

	myFirstNode->setNodeMask(0x0);
	mySecondNode->setNodeMask(0x0);

	myRoot->addChild(myFirstNode);
	myRoot->addChild(mySecondNode);
}

ByyIntervisibilityLine::~ByyIntervisibilityLine()
{
	myRoot->removeChild(myFirstNode);
	myRoot->removeChild(mySecondNode);
}

void ByyIntervisibilityLine::addPoint( const osgEarth::GeoPoint& point,bool refresh/*=true*/ )
{
	myPnts.resize(2);

	myPnts[0]=point.vec3d();
	myPnts[1]=point.vec3d();

	myPnts[0].z();
	myPnts[1].z();

	myFirstNode->setNodeMask(0x0);
	mySecondNode->setNodeMask(0x0);

	firstAltJustValue=4;
	secondAltJustValue=4;
}

void ByyIntervisibilityLine::mouseMove( const osgEarth::GeoPoint& point )
{
	if (myPnts.size()==0)
		return;

	myPnts[1]=point.vec3d();
	myPnts[1].z();

	secondAltJustValue=4;

	updateLine();
}

void ByyIntervisibilityLine::updateLine()
{
	if (myPnts.size()<2)
		return;

	osg::EllipsoidModel em;

	osg::Vec3d start,end;

	em.convertLatLongHeightToXYZ(osg::DegreesToRadians(myPnts[0].y()),osg::DegreesToRadians(myPnts[0].x()),myPnts[0].z()+firstAltJustValue,
		start.x(),start.y(),start.z());

	em.convertLatLongHeightToXYZ(osg::DegreesToRadians(myPnts[1].y()),osg::DegreesToRadians(myPnts[1].x()),myPnts[1].z()+secondAltJustValue,
		end.x(),end.y(),end.z());

	osgEarth::Util::DPLineSegmentIntersector* i = new osgEarth::Util::DPLineSegmentIntersector( start, end );
	i->setIntersectionLimit( osgUtil::Intersector::LIMIT_NEAREST );
	osgUtil::IntersectionVisitor iv;
	iv.setIntersector( i );
	myIG.renderer()->getTerrainRoot()->accept( iv );

	if (i->containsIntersections())
	{
		hasIntersect=true;

		osgUtil::LineSegmentIntersector::Intersection result = i->getFirstIntersection();

		const osg::Vec3d& intersetPnt=result.getWorldIntersectPoint();

		em.convertXYZToLatLongHeight(intersetPnt.x(),intersetPnt.y(),intersetPnt.z(),
			intersetPoint.y(),intersetPoint.x(),intersetPoint.z());

		intersetPoint.x()=osg::RadiansToDegrees(intersetPoint.x());
		intersetPoint.y()=osg::RadiansToDegrees(intersetPoint.y());

		myFirstNode->setNodeMask(0x1);
		mySecondNode->setNodeMask(0x1);
	}
	else
	{
		hasIntersect=false;

		myFirstNode->setNodeMask(0x1);
		mySecondNode->setNodeMask(0x0);
	}

	if (hasIntersect)
	{
		(*myFirstLine)[0]=myPnts[0];
		(*myFirstLine)[1]=intersetPoint;

		(*mySecondLine)[0]=intersetPoint;
		(*mySecondLine)[1]=myPnts[1];

		(*myFirstLine)[0].z()+=firstAltJustValue;
		(*mySecondLine)[1].z()+=secondAltJustValue;


		myFirstNode->init();
		mySecondNode->init();
	}
	else
	{
		(*myFirstLine)[0]=myPnts[0];
		(*myFirstLine)[1]=myPnts[1];

		(*myFirstLine)[0].z()+=firstAltJustValue;
		(*myFirstLine)[1].z()+=secondAltJustValue;

		myFirstNode->init();
	}
	
}

bool ByyIntervisibilityLine::processEvent( const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa )
{
	if (ea.getEventType()==ea.KEYUP)
	{
		bool process=false;

		switch(ea.getKey())
		{
		case 'd':
			firstAltJustValue+=1;
			process=true;
			break;
		case 'f':
			firstAltJustValue-=1;
			if (firstAltJustValue<0) firstAltJustValue=0;
			process=true;
			break;
		case 'c':
			secondAltJustValue+=1;
			process=true;
			break;
		case 'v':
			secondAltJustValue-=1;
			if (secondAltJustValue<0) secondAltJustValue=0;
			process=true;
			break;
		}

		if (process)
		{
			updateLine();
			return true;
		}
	}
	else if (ea.getEventType()==ea.KEYDOWN)
	{
		switch(ea.getKey())
		{
		case 'd':
		case 'f':
		case 'c':
		case 'v':
			return true;
		}
	}

	return __super::processEvent(ea,aa);
}
