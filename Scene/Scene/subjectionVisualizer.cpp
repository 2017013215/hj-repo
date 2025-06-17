#include "StdAfx.h"
#include "subjectionVisualizer.h"
#include "ByyIG.h"
#include "ByyRenderer.h"
#include "entityObject.h"
#include "observerSettings.h"
#include <osg/LineWidth>
#include <osg/LineStipple>
#include "radioCommsVisualizer.h"

ByySubjectionVisualizer::ByySubjectionVisualizer( ByyEntityObject* parent,ByyEntityObject* child,ByyIG& ig ) 
	:myParent(parent)
	,myChild(child)
	,myIG(ig)
{
	myVertexs=new osg::Vec3dArray;
	myVertexs->resize(2);

	osg::Vec4Array *colors=new osg::Vec4Array;
	colors->push_back(osg::Vec4(0,1,0,1));

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

	myGeometry->getOrCreateStateSet()->setAttributeAndModes(new osg::LineStipple(3,0xFFCCC),osg::StateAttribute::ON);

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

	setVisible(ByyObserverSettings::instance().subjectionVisible());
	connect(&ByyObserverSettings::instance(),SIGNAL(subjectionVisibleChanged(bool)),this,SLOT(setVisible(bool)));
	
	connect(myParent,SIGNAL(positionChanged(const osg::Vec3d&)),this,SLOT(update()));
	connect(myChild,SIGNAL(positionChanged(const osg::Vec3d&)),this,SLOT(update()));

	update();
}

ByySubjectionVisualizer::~ByySubjectionVisualizer()
{
	myIG.renderer()->removeNodeFromRoot(myRoot,ByyOsgRenderer::Entity);
}

void ByySubjectionVisualizer::update()
{
	osg::EllipsoidModel em;

	double x,y,z;
	em.convertLatLongHeightToXYZ(osg::DegreesToRadians(myParent->position().y()),
		osg::DegreesToRadians(myParent->position().x()),myParent->position().z(),x,y,z);

	(*myVertexs)[0]=osg::Vec3d(x,y,z);
	
	em.convertLatLongHeightToXYZ(osg::DegreesToRadians(myChild->position().y()),
		osg::DegreesToRadians(myChild->position().x()),myChild->position().z(),x,y,z);

	(*myVertexs)[1]=osg::Vec3d(x,y,z);

	myVertexs->dirty();

	myGeometry->dirtyBound();
}

void ByySubjectionVisualizer::setVisible( bool v )
{
	myRoot->setNodeMask(v);
}
