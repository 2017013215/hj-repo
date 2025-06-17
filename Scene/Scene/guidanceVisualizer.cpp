#include "StdAfx.h"
#include "guidanceVisualizer.h"
#include "ByyIG.h"
#include "ByyRenderer.h"
#include "entityObject.h"
#include "observerSettings.h"
#include <osg/LineWidth>
#include <osg/LineStipple>
#include "radioCommsVisualizer.h"

ByyGuidanceVisualizer::ByyGuidanceVisualizer( ByyEntityObject* sender,ByyEntityObject* target,ByyIG& ig ) 
	:myIG(ig)
	,mySender(sender)
	,myTarget(target)
	,myLastTickTime(0)
{
	myVertexs=new osg::Vec3dArray;
	myVertexs->resize(2);

	osg::Vec4Array *colors=new osg::Vec4Array;
	colors->push_back(osg::Vec4(1,1,0,1));

	osg::Vec3Array *normals=new osg::Vec3Array;
	normals->push_back(osg::X_AXIS);

	myGeometry=new osg::Geometry;

	myGeometry->setUseDisplayList(false);
	myGeometry->setUseVertexBufferObjects(true);

	myGeometry->setVertexArray(myVertexs);
	myGeometry->setColorArray(colors);
	myGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
	myGeometry->setNormalArray(normals);
	myGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);

	myGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,2));

	myGeometry->getOrCreateStateSet()->setAttributeAndModes(new osg::LineWidth(2),osg::StateAttribute::ON);

//	myGeometry->getOrCreateStateSet()->setAttributeAndModes(new osg::LineStipple(3,0xFFCCC),osg::StateAttribute::ON);

	osg::Geode *geode=new osg::Geode;

	geode->addDrawable(myGeometry);
	//geode->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);
	geode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	geode->getOrCreateStateSet()->setRenderBinDetails(2, "DepthSortedBin" );
	geode->getOrCreateStateSet()->setAttributeAndModes(new osg::Program);
	myRoot=new osg::Group;
	myRoot->addChild(geode);

	myIG.renderer()->addNodeToRoot(myRoot,ByyOsgRenderer::Entity);

	setVisible(ByyObserverSettings::instance().guidanceVisible());
	connect(&ByyObserverSettings::instance(),SIGNAL(guidanceVisibleChanged(bool)),this,SLOT(setVisible(bool)));
	
	connect(mySender,SIGNAL(positionChanged(const osg::Vec3d&)),this,SLOT(update()));
	connect(myTarget,SIGNAL(positionChanged(const osg::Vec3d&)),this,SLOT(update()));
	connect(&myIG,SIGNAL(postTick()),this,SLOT(update()));

	myLastTickTime=osg::Timer::instance()->time_m();

	update();
}

ByyGuidanceVisualizer::~ByyGuidanceVisualizer()
{
	myIG.renderer()->removeNodeFromRoot(myRoot,ByyOsgRenderer::Entity);
}

void ByyGuidanceVisualizer::update()
{
	double ms=osg::Timer::instance()->time_m();

	double t=qMin((ms-myLastTickTime)/500.0,1.0);

	osg::Vec3d senderPt=mySender->position();
	osg::Vec3d targetPt=myTarget->position();

	osg::Vec3d lineEndPt;

	osgEarth::GeoMath::interpolate(osg::DegreesToRadians(senderPt.y()),
		osg::DegreesToRadians(senderPt.x()),
		osg::DegreesToRadians(targetPt.y()),
		osg::DegreesToRadians(targetPt.x()),
		t,lineEndPt.y(),lineEndPt.x());

	lineEndPt.x()=osg::RadiansToDegrees(lineEndPt.x());
	lineEndPt.y()=osg::RadiansToDegrees(lineEndPt.y());

	lineEndPt.z()=senderPt.z()+(targetPt.z()-senderPt.z())*t;


	osg::EllipsoidModel em;

	double x,y,z;
	em.convertLatLongHeightToXYZ(osg::DegreesToRadians(senderPt.y()),
		osg::DegreesToRadians(senderPt.x()),senderPt.z(),x,y,z);

	(*myVertexs)[0]=osg::Vec3d(x,y,z);
	
	em.convertLatLongHeightToXYZ(osg::DegreesToRadians(lineEndPt.y()),
		osg::DegreesToRadians(lineEndPt.x()),lineEndPt.z(),x,y,z);

	(*myVertexs)[1]=osg::Vec3d(x,y,z);

	myVertexs->dirty();

	myGeometry->dirtyBound();

	if (t==1.0)
	{
		myLastTickTime=ms;
	}
}

void ByyGuidanceVisualizer::setVisible( bool v )
{
	myRoot->setNodeMask(v);
}
