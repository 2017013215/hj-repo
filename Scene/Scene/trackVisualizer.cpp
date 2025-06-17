#include "StdAfx.h"
#include "trackVisualizer.h"
#include "ByyIG.h"
#include "ByyRenderer.h"
#include "entityObject.h"
#include "observerSettings.h"
#include <osg/LineWidth>
#include <osg/LineStipple>
#include "radioCommsVisualizer.h"

ByyTrackVisualizer::ByyTrackVisualizer( ByyEntityObject* sender,ByyEntityObject* target,ByyIG& ig ) 
	:myIG(ig)
	,mySender(sender)
	,myTarget(target)
	,myLastTickTime(0)
{
	myVertexs=new osg::Vec3dArray;
	myVertexs->resize(2);

	osg::Vec4Array *colors=new osg::Vec4Array;
	colors->push_back(osg::Vec4(1,0.77,0.211,1));

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

	myGeometry->getOrCreateStateSet()->setAttributeAndModes(new osg::LineStipple(5,0x1FF9C),osg::StateAttribute::ON);

	myGeode=new osg::Geode;

	myGeode->addDrawable(myGeometry);
	//geode->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);
	myGeode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	myGeode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	myGeode->getOrCreateStateSet()->setRenderBinDetails(2, "DepthSortedBin" );
	myGeode->getOrCreateStateSet()->setAttributeAndModes(new osg::Program);
	myRoot=new osg::Group;
	myRoot->addChild(myGeode);

	myIG.renderer()->addNodeToRoot(myRoot,ByyOsgRenderer::Entity);

	setVisible(ByyObserverSettings::instance().trackVisible());
	connect(&ByyObserverSettings::instance(),SIGNAL(trackVisibleChanged(bool)),this,SLOT(setVisible(bool)));
	
	connect(mySender,SIGNAL(positionChanged(const osg::Vec3d&)),this,SLOT(update()));
	connect(myTarget,SIGNAL(positionChanged(const osg::Vec3d&)),this,SLOT(update()));

	//connect(&myIG,SIGNAL(postTick()),this,SLOT(tickAnimation()));

	update();
}

ByyTrackVisualizer::~ByyTrackVisualizer()
{
	myIG.renderer()->removeNodeFromRoot(myRoot,ByyOsgRenderer::Entity);
}

void ByyTrackVisualizer::update()
{
	osg::EllipsoidModel em;

	double x,y,z;
	em.convertLatLongHeightToXYZ(osg::DegreesToRadians(mySender->position().y()),
		osg::DegreesToRadians(mySender->position().x()),mySender->position().z(),x,y,z);

	(*myVertexs)[0]=osg::Vec3d(x,y,z);
	
	em.convertLatLongHeightToXYZ(osg::DegreesToRadians(myTarget->position().y()),
		osg::DegreesToRadians(myTarget->position().x()),myTarget->position().z(),x,y,z);

	(*myVertexs)[1]=osg::Vec3d(x,y,z);

	myVertexs->dirty();

	myGeometry->dirtyBound();
}

void ByyTrackVisualizer::setVisible( bool v )
{
	myRoot->setNodeMask(v);
}

void ByyTrackVisualizer::tickAnimation()
{
	double ms=osg::Timer::instance()->time_m();

	if (ms-myLastTickTime<800)
		return;

	bool isShow=myGeode->getNodeMask();

	myGeode->setNodeMask(!isShow);

	myLastTickTime=ms;
}
