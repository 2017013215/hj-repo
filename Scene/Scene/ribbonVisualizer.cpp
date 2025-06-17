#include "StdAfx.h"
#include "ribbonVisualizer.h"
#include "utils.h"
#include "entityObject.h"
#include "entitySettings.h"
#include <osg/LineWidth>
#include <osg/LineStipple>

ByyRibbonVisualizer::ByyRibbonVisualizer( ByyEntityObject& entObj,ByyIG& ig )
	:ByyStateVisualizer(entObj,ig)
	,myHalfWidth(300)
	,myNumPoints(800)
{
	setVisible(ByyEntitySettings::instance().trackHistoryEnabled());

	connect(&ByyEntitySettings::instance(),SIGNAL(trackHistoryEnabledChanged(bool)),SLOT(setVisible(bool)));
	connect(&myEntObj,SIGNAL(positionChanged(const osg::Vec3d&)),this,SLOT(update(const osg::Vec3d&)));
	connect(&myEntObj,SIGNAL(forceTypeChanged(ByyForceType)),this,SLOT(updateForceType()));
}

ByyRibbonVisualizer::~ByyRibbonVisualizer()
{

}

const QString& ByyRibbonVisualizer::theType()
{
	static QString _type("Ribbon");
	return _type;
}

const QString& ByyRibbonVisualizer::type()
{
	return theType();
}

void ByyRibbonVisualizer::updateForceType()
{
	if (myColors.valid())
	{
		(*myColors)[0]=forceTypeColorAlpha(myEntObj.forceType(),0.8);
		myColors->dirty();
	}
}

void ByyRibbonVisualizer::setVisible( bool v )
{
	if (v)
		init();
	else
		uninit();
}

void ByyRibbonVisualizer::init()
{
	if (myGeode.valid()||myEntObj.isLocal())
		return;

	myVertices=new osg::Vec3Array(myNumPoints);

	myColors=new osg::Vec4Array;
	myColors->push_back(forceTypeColorAlpha(myEntObj.forceType(),0.8));

	osg::EllipsoidModel em;

	osg::Vec3d pos=myEntObj.position();

	em.convertLatLongHeightToXYZ(osg::DegreesToRadians(pos.y()),osg::DegreesToRadians(pos.x()),pos.z(),
		myRibbonOrigin.x(),myRibbonOrigin.y(),myRibbonOrigin.z());

	myNormals=new osg::Vec3Array(myNumPoints);

	for (int i=0;i!=myNumPoints;++i)
	{
		(*myVertices)[i]=osg::Vec3d();

		(*myNormals)[i]=osg::Vec3(0,0,1);
	}

	myGeometry=new osg::Geometry;
	myGeometry->setVertexArray(myVertices);
	myGeometry->setColorArray(myColors);
	myGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
	myGeometry->setNormalArray(myNormals);
	myGeometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

	myGeometry->addPrimitiveSet(new osg::DrawArrays(GL_LINE_STRIP,0,myNumPoints));
	myGeometry->setDataVariance(osg::Object::DYNAMIC);

	myGeometry->setUseDisplayList(false);
	myGeometry->setUseVertexBufferObjects(true);

	osg::LineWidth *lineWidth=new osg::LineWidth(3);
	myGeometry->getOrCreateStateSet()->setAttributeAndModes(lineWidth);

	osg::LineStipple *lineStripple=new osg::LineStipple;
	lineStripple->setFactor(1);
	lineStripple->setPattern(0xf0);
	myGeometry->getOrCreateStateSet()->setAttributeAndModes(lineStripple);

	myGeode=new osg::Geode;

	myGeode->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);
	myGeode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	//myGeode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);

	myGeode->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	myGeode->getOrCreateStateSet()->setAttributeAndModes(new osg::Program);
	//myGeode->getOrCreateStateSet()->setRenderBinDetails(99,"RenderBin");

	myGeode->addDrawable(myGeometry);

	myRibbonMt=new osg::MatrixTransform;

	myRibbonMt->setMatrix(osg::Matrix::translate(myRibbonOrigin));

	myRibbonMt->addChild(myGeode);

	myRoot->addChild(myRibbonMt);

	updateForceType();
}

void ByyRibbonVisualizer::uninit()
{
	if (!myGeode.valid())
		return;

	myRoot->removeChild(myGeode);

	myGeode=0;
	myGeometry=0;
	myVertices=0;
	myColors=0;
	myNormals=0;
	myRibbonMt=0;
}

void ByyRibbonVisualizer::update( const osg::Vec3d& position )
{
	if (!myGeode.valid())
		return;

	for (int i=0;i<myNumPoints-1;++i)
	{
		(*myVertices)[i]=(*myVertices)[i+1];

		(*myNormals)[i]=(*myNormals)[i+1];
	}

	osg::EllipsoidModel em;

	osg::Vec3d world;

	em.convertLatLongHeightToXYZ(osg::DegreesToRadians(position.y()),osg::DegreesToRadians(position.x()),position.z(),
		world.x(),world.y(),world.z());

	(*myVertices)[myNumPoints-1]=world-myRibbonOrigin;

	osg::Vec3 normal=osg::Vec3(0,0,1);
	(*myNormals)[myNumPoints-1]=normal;

	myVertices->dirty();
	myNormals->dirty();

	myGeometry->dirtyBound();
}
