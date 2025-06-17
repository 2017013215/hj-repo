#include "StdAfx.h"
#include "routePlanVisualizer.h"
#include "ByyIG.h"
#include "terrain.h"
#include "ByyRenderer.h"
#include "entityObject.h"
#include "observerSettings.h"

ByyRoutePlanVisualizer::ByyRoutePlanVisualizer( ByyEntityObject* entObj,const QVector<osg::Vec3d>& pnts,ByyIG& ig )
	:myIG(ig)
{
	myRoot=new osg::Group;

	osgEarth::Symbology::LineString *myLineString=new osgEarth::Symbology::LineString;
	myLineString->resize(pnts.size());

	for (int i=0;i!=myLineString->size();++i)
	{
		(*myLineString)[i]=pnts[i];
	}

	osgEarth::MapNode *mapNode=ig.terrain()->mapNode();

	{
		osgEarth::Symbology::Style style;
		style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width()=3;
		style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color()=osgEarth::Symbology::Color::Red;
		style.getOrCreate<osgEarth::Symbology::LineSymbol>()->tessellation()=20.0;


		switch(entObj->forceType())
		{
		case ByyForceFriendly:
			style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color()=osgEarth::Symbology::Color::Red;
			break;
		case ByyForceOpposing:
			style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color()=osgEarth::Symbology::Color::Blue;
			break;
		default:
			style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color()=osgEarth::Symbology::Color::White;
		}

		osgEarth::DepthOffsetOptions doo;
		doo.automatic()=true;

		style.getOrCreate<osgEarth::Symbology::RenderSymbol>()->depthOffset()=doo;

		osgEarth::Features::Feature *myFeature=new osgEarth::Features::Feature(myLineString,mapNode->getMapSRS(),style);

		osgEarth::Annotation::FeatureNode *myFeatureNode=new osgEarth::Annotation::FeatureNode(mapNode,myFeature);

		myRoot->addChild(myFeatureNode);
	}
	{
		osgEarth::Symbology::Style style;
		style.getOrCreate<osgEarth::Symbology::PointSymbol>()->size()=8;
		style.getOrCreate<osgEarth::Symbology::PointSymbol>()->fill()->color()=osg::Vec4(0,1,0,1);

		osgEarth::DepthOffsetOptions doo;
		doo.automatic()=true;

		style.getOrCreate<osgEarth::Symbology::RenderSymbol>()->depthOffset()=doo;

		osgEarth::Features::Feature *myFeature=new osgEarth::Features::Feature(myLineString,mapNode->getMapSRS(),style);

		osgEarth::Annotation::FeatureNode *myFeatureNode=new osgEarth::Annotation::FeatureNode(mapNode,myFeature);

		myRoot->addChild(myFeatureNode);
	}

	ig.renderer()->addNodeToRoot(myRoot,ByyOsgRenderer::Entity);

	setVisible(ByyObserverSettings::instance().routePlanVisible());

	connect(&ByyObserverSettings::instance(),SIGNAL(routePlanVisibleChanged(bool)),this,SLOT(setVisible(bool)));
}

ByyRoutePlanVisualizer::~ByyRoutePlanVisualizer()
{
	myIG.renderer()->removeNodeFromRoot(myRoot,ByyOsgRenderer::Entity);
}

void ByyRoutePlanVisualizer::setVisible( bool b )
{
	myRoot->setNodeMask(b);
}
