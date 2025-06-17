#include "StdAfx.h"
#include "radioCommsVisualizer.h"
#include "ByyIG.h"
#include "ByyRenderer.h"
#include "terrain.h"
#include "entityObject.h"
#include "osgFileCache.h"
#include "observerSettings.h"

#include <osg/LineWidth>
#include <osg/LineStipple>
#include <osg/ImageSequence>
#include <osgEarthAnnotation/LocalGeometryNode>


class LineStippleAnimation: public osg::NodeCallback
{
public:
	LineStippleAnimation(osg::Geometry* geom,osg::LineStipple* ls)
	{
		myGeom=geom;
		myLineStipple=ls;

		stipples.resize(14);
		stipples[0]=255;
		stipples[1]=510;
		stipples[2]=1020;
		stipples[3]=2040;
		stipples[4]=4080;
		stipples[5]=8160;
		stipples[6]=16320;
		stipples[7]=32640;
		stipples[8]=65280;
		stipples[9]=64515;
		stipples[10]=63495;
		stipples[11]=61455;
		stipples[12]=57375;
		stipples[13]=49215;

		myLastUpdateTime=0;
		myIndex=0;
	}

	void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		if (osg::Timer::instance()->time_m()-myLastUpdateTime>10)
		{
			myLineStipple->setPattern(stipples[myIndex++]);

			myLastUpdateTime=osg::Timer::instance()->time_m();

			if (myIndex==stipples.size())
			{
				myIndex=0;
			}
		}
	}

	osg::Geometry* myGeom;
	osg::LineStipple* myLineStipple;

	std::vector<unsigned int> stipples;

	double myLastUpdateTime;
	int myIndex;

};

ByyRadioCommsVisualizer::ByyRadioCommsVisualizer( ByyEntityObject* sender,ByyEntityObject* target,ByyIG& ig ) 
	:myIG(ig)
	,mySenderObj(sender)
	,myTargetObj(target)
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
	myGeometry->setDataVariance(osg::Object::DYNAMIC);

	myGeometry->setVertexArray(myVertexs);
	myGeometry->setColorArray(colors);
	myGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
	myGeometry->setNormalArray(normals);
	myGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);

	myGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,2));

	myGeometry->getOrCreateStateSet()->setAttributeAndModes(new osg::LineWidth(2),osg::StateAttribute::ON);

	osg::LineStipple *ls=new osg::LineStipple;

	ls->setFactor(1);

	myGeometry->getOrCreateStateSet()->setAttributeAndModes(ls,osg::StateAttribute::ON);

	osg::Geode *geode=new osg::Geode;

	geode->addUpdateCallback(new LineStippleAnimation(myGeometry,ls));

	geode->addDrawable(myGeometry);
	//geode->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);
	geode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	geode->getOrCreateStateSet()->setRenderBinDetails(2, "DepthSortedBin" );
	geode->getOrCreateStateSet()->setAttributeAndModes(new osg::Program);

		
	myRoot=new osg::Group;
	myRoot->addChild(geode);
		
	//myRoot->getOrCreateStateSet()->setRenderBinDetails(13, "RenderBin");

	myIG.renderer()->addNodeToRoot(myRoot,ByyOsgRenderer::ControlObject);

	osgEarth::MapNode *mapNode=myIG.terrain()->mapNode();

	if (mapNode)
	{
		osg::ImageSequence *imageSeq=new osg::ImageSequence;
		imageSeq->addImage(myIG.fileCache().readImageFile("../data/Overlays/SquawkIndicator1.png"));
		imageSeq->addImage(myIG.fileCache().readImageFile("../data/Overlays/SquawkIndicator2.png"));
		imageSeq->addImage(myIG.fileCache().readImageFile("../data/Overlays/SquawkIndicator3.png"));
		imageSeq->addImage(myIG.fileCache().readImageFile("../data/Overlays/SquawkIndicator4.png"));
		imageSeq->addImage(myIG.fileCache().readImageFile("../data/Overlays/SquawkIndicator5.png"));
		imageSeq->addImage(myIG.fileCache().readImageFile("../data/Overlays/SquawkIndicator6.png"));
		imageSeq->addImage(myIG.fileCache().readImageFile("../data/Overlays/SquawkIndicator7.png"));
		imageSeq->setLength(1);
		imageSeq->play();


		osgEarth::Symbology::Style style;

		style.getOrCreate<osgEarth::Symbology::IconSymbol>()->alignment()=osgEarth::Symbology::IconSymbol::ALIGN_CENTER_CENTER;
		style.getOrCreate<osgEarth::Symbology::IconSymbol>()->declutter()=false;


		myPlaceNode=new osgEarth::Annotation::ByyPlaceNode(mapNode,osgEarth::GeoPoint(mapNode->getMapSRS(),0,0,0),imageSeq,"",style);

		myPlaceNode->setIconColor(osg::Vec4(0,1,0,0.8));

		//myPlaceNode->getOrCreateStateSet()->setRenderBinDetails(13,"RenderBin");

		myPlaceNode->setDynamic(true);

		myRoot->addChild(myPlaceNode);
	}

	connect(&myIG,SIGNAL(preTick()),SLOT(update()));

	setVisible(ByyObserverSettings::instance().radioCommVisible());
	connect(&ByyObserverSettings::instance(),SIGNAL(radioCommVisibleChanged(bool)),this,SLOT(setVisible(bool)));
}

ByyRadioCommsVisualizer::~ByyRadioCommsVisualizer()
{
	myIG.renderer()->removeNodeFromRoot(myRoot,ByyOsgRenderer::ControlObject);
}

void ByyRadioCommsVisualizer::update()
{
	osg::EllipsoidModel em;

	double x,y,z;
	em.convertLatLongHeightToXYZ(osg::DegreesToRadians(mySenderObj->position().y()),
		osg::DegreesToRadians(mySenderObj->position().x()),mySenderObj->position().z(),x,y,z);

	(*myVertexs)[0]=osg::Vec3d(x,y,z);

	em.convertLatLongHeightToXYZ(osg::DegreesToRadians(myTargetObj->position().y()),
		osg::DegreesToRadians(myTargetObj->position().x()),myTargetObj->position().z(),x,y,z);

	(*myVertexs)[1]=osg::Vec3d(x,y,z);

	myVertexs->dirty();

	myPlaceNode->setPosition(osgEarth::GeoPoint(myIG.terrain()->mapNode()->getMapSRS(),mySenderObj->position()));
}

void ByyRadioCommsVisualizer::setVisible( bool v )
{
	myRoot->setNodeMask(v);
}
