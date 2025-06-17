#include "StdAfx.h"
#include "ByyIntervisibilityFan.h"
#include "ByyIG.h"
#include "ByyRenderer.h"
#include "terrain.h"
#include "osgFileCache.h"

#include <osgEarth/DPLineSegmentIntersector>

ByyIntervisibilityFan::ByyIntervisibilityFan(ByyIG& ig)
	: ByyMeasureItem(ig)
	,myJustValue(4)
{
	myRoot=myIG.renderer()->getControlObjectRoot();

	myMapNode=myIG.terrain()->mapNode();

	osg::Image* image=myIG.fileCache().readImageFile("../data/Textures/triangle.png");

	myPlaceNode=new osgEarth::Annotation::PlaceNode(myMapNode,osgEarth::GeoPoint(myMapNode->getMapSRS(),0,0),image,"");
	

	osgEarth::Symbology::Style style;
	style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color()=osgEarth::Symbology::Color::Blue;

	myCircleNode=new osgEarth::Annotation::CircleNode(myMapNode,osgEarth::GeoPoint(myMapNode->getMapSRS(),0,0),0,style);
	
	myPlaceNode->setNodeMask(0x0);
	myCircleNode->setNodeMask(0x0);

	myRoot->addChild(myPlaceNode);
	myRoot->addChild(myCircleNode);

	myLineData.resize(36);

	osgEarth::Symbology::Style pathStyle;
	pathStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = 2.0f;
	pathStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->tessellation() = 20;

	for (int i=0;i!=myLineData.size();++i)
	{
		LineData& lineData=myLineData[i];

		lineData.myFirstLine=new osgEarth::Symbology::LineString;
		lineData.myFirstLine->resize(2);

		pathStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color() = osgEarth::Color::Green;

		lineData.myFirstFeature=new osgEarth::Features::Feature(lineData.myFirstLine, myMapNode->getMapSRS(), pathStyle);

		lineData.myFirstNode = new osgEarth::Annotation::FeatureNode(myMapNode, lineData.myFirstFeature);


		pathStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color() = osgEarth::Color::Red;

		lineData.mySecondLine=new osgEarth::Symbology::LineString;
		lineData.mySecondLine->resize(2);

		lineData.mySecondFeature=new osgEarth::Features::Feature(lineData.mySecondLine, myMapNode->getMapSRS(), pathStyle);

		lineData.mySecondNode = new osgEarth::Annotation::FeatureNode(myMapNode, lineData.mySecondFeature);

		lineData.myFirstNode->setNodeMask(0x0);
		lineData.mySecondNode->setNodeMask(0x0);

		myRoot->addChild(lineData.myFirstNode);
		myRoot->addChild(lineData.mySecondNode);

	}
}

ByyIntervisibilityFan::~ByyIntervisibilityFan()
{
	myRoot->removeChild(myPlaceNode);
	myRoot->removeChild(myCircleNode);

	for (int i=0;i!=myLineData.size();++i)
	{
		LineData& lineData=myLineData[i];

		myRoot->removeChild(lineData.myFirstNode);
		myRoot->removeChild(lineData.mySecondNode);
	}
}

void ByyIntervisibilityFan::addPoint( const osgEarth::GeoPoint& point,bool refresh/*=true*/ )
{
	myPnts.resize(2);

	myPnts[0]=point.vec3d();
	myPnts[1]=point.vec3d();

	myJustValue=4;

	osgEarth::GeoPoint adjustPoint=point;
	adjustPoint.z()+=myJustValue;

	myPlaceNode->setPosition(adjustPoint);
	myCircleNode->setPosition(adjustPoint);

	myPlaceNode->setNodeMask(0x1);
	myCircleNode->setNodeMask(0x0);
	
	for (int i=0;i!=myLineData.size();++i)
	{
		LineData& lineData=myLineData[i];

		lineData.myFirstNode->setNodeMask(0x0);
		lineData.mySecondNode->setNodeMask(0x0);
	}
}

void ByyIntervisibilityFan::mouseMove( const osgEarth::GeoPoint& point )
{
	if (myPnts.isEmpty())
		return;

	myPnts[1]=point.vec3d();

	
	updateFan();
}

void ByyIntervisibilityFan::updateFan()
{
	if (myPnts.isEmpty())
		return;

	double myRadius=osgEarth::GeoMath::distance(myPnts[0],myPnts[1],myMapNode->getMapSRS());

	osgEarth::GeoPoint adjustPoint(myMapNode->getMapSRS(),myPnts[0]);
	adjustPoint.z()+=myJustValue;

	myPlaceNode->setPosition(adjustPoint);
	myCircleNode->setPosition(adjustPoint);

	myCircleNode->setRadius(myRadius);

	myPlaceNode->setNodeMask(0x1);
	myCircleNode->setNodeMask(0x1);

	osg::EllipsoidModel em;
	osg::Vec3d start,end;

	osg::Vec3d endLonLat;

	float perAngle=360.0/myLineData.size();

	for (int i=0;i!=myLineData.size();++i)
	{
		LineData& lineData=myLineData[i];
		

		float angle=i*perAngle;

		osgEarth::GeoMath::destination(osg::DegreesToRadians(myPnts[0].y()),osg::DegreesToRadians(myPnts[0].x()),
			osg::DegreesToRadians(angle),myRadius,endLonLat.y(),endLonLat.x());

		endLonLat.z()=myPnts[0].z();

		em.convertLatLongHeightToXYZ(osg::DegreesToRadians(myPnts[0].y()),osg::DegreesToRadians(myPnts[0].x()),myPnts[0].z()+myJustValue,
			start.x(),start.y(),start.z());

		em.convertLatLongHeightToXYZ(endLonLat.y(),endLonLat.x(),endLonLat.z()+myJustValue,
			end.x(),end.y(),end.z());

		endLonLat.x()=osg::RadiansToDegrees(endLonLat.x());
		endLonLat.y()=osg::RadiansToDegrees(endLonLat.y());

		osgEarth::Util::DPLineSegmentIntersector* intersector = new osgEarth::Util::DPLineSegmentIntersector( start, end );
		intersector->setIntersectionLimit( osgUtil::Intersector::LIMIT_NEAREST );
		osgUtil::IntersectionVisitor iv;
		iv.setIntersector( intersector );
		myIG.renderer()->getTerrainRoot()->accept( iv );

		bool hasIntersect=false;

		osg::Vec3d intersetPoint;

		if (intersector->containsIntersections())
		{
			hasIntersect=true;

			osgUtil::LineSegmentIntersector::Intersection result = intersector->getFirstIntersection();

			const osg::Vec3d& intersetPnt=result.getWorldIntersectPoint();

			em.convertXYZToLatLongHeight(intersetPnt.x(),intersetPnt.y(),intersetPnt.z(),
				intersetPoint.y(),intersetPoint.x(),intersetPoint.z());

			intersetPoint.x()=osg::RadiansToDegrees(intersetPoint.x());
			intersetPoint.y()=osg::RadiansToDegrees(intersetPoint.y());

			lineData.myFirstNode->setNodeMask(0x1);
			lineData.mySecondNode->setNodeMask(0x1);
		}
		else
		{
			hasIntersect=false;

			lineData.myFirstNode->setNodeMask(0x1);
			lineData.mySecondNode->setNodeMask(0x0);
		}

		if (hasIntersect)
		{
			(*lineData.myFirstLine)[0]=myPnts[0];
			(*lineData.myFirstLine)[1]=intersetPoint;

			(*lineData.mySecondLine)[0]=intersetPoint;
			(*lineData.mySecondLine)[1]=endLonLat;

			(*lineData.myFirstLine)[0].z()+=myJustValue;
			(*lineData.mySecondLine)[1].z()+=myJustValue;


			lineData.myFirstNode->init();
			lineData.mySecondNode->init();
		}
		else
		{
			(*lineData.myFirstLine)[0]=myPnts[0];
			(*lineData.myFirstLine)[1]=endLonLat;

			(*lineData.myFirstLine)[0].z()+=myJustValue;
			(*lineData.myFirstLine)[1].z()+=myJustValue;

			lineData.myFirstNode->init();
		}
	}
}

bool ByyIntervisibilityFan::processEvent( const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa )
{
	if (ea.getEventType()==ea.KEYUP)
	{
		bool process=false;

		switch(ea.getKey())
		{
		case 'd':
			myJustValue+=1;
			process=true;
			break;
		case 'f':
			myJustValue-=1;
			if (myJustValue<0) myJustValue=0;
			process=true;
			break;
		}

		if (process)
		{
			updateFan();
			return true;
		}
	}
	else if (ea.getEventType()==ea.KEYDOWN)
	{
		switch(ea.getKey())
		{
		case 'd':
		case 'f':
			return true;
		}
	}

	return __super::processEvent(ea,aa);
}
