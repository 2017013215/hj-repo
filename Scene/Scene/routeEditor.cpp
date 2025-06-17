#include "StdAfx.h"
#include "routeEditor.h"
#include "ByyRoute.h"

ByyRouteEditor::ByyRouteEditor( ByyIG& ig,ByyRoute* route )
	:ByyTacticalGraphEditor(ig,route)
	,myRoute(route)
{
	myPathLine=new osgEarth::Symbology::LineString;

	ByyVec3dArray pts=myRoute->points();

	for (int i=0;i!=pts.size();++i)
	{
		myPathLine->push_back(pts[i]);
	}

	osgEarth::Symbology::Style pathStyle;
	pathStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color() = osgEarth::Color::White;
	pathStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = 2.0f;
	pathStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->tessellation() = 20;
	pathStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
	pathStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_GPU;
	//pathStyle.getOrCreate<osgEarth::Symbology::RenderSymbol>()->depthTest()=false;
	//pathStyle.getOrCreate<osgEarth::Symbology::RenderSymbol>()->order()=13;

	myPathFeature=new osgEarth::Features::Feature(myPathLine, myMapNode->getMapSRS(), pathStyle);

	myPathNode = new osgEarth::Annotation::FeatureNode(myMapNode.get(), myPathFeature);
	myRoot->addChild(myPathNode);

	myEditor=new osgEarth::Annotation::FeatureEditor(myPathNode);

	myRoot->addChild(myEditor);
}

ByyRouteEditor::~ByyRouteEditor()
{
	myRoot->removeChild(myEditor);
	myRoot->removeChild(myPathNode);
}

ByyControlObject* ByyRouteEditor::accept()
{
	QVector<osg::Vec3d> result(myPathLine->size());

	for (int i=0;i!=myPathLine->size();++i)
	{
		result[i]=myPathLine->at(i);
	}

	//myRoute->setPoints(result);
	myRoute->setPoints_xy(result);  // 20240708, ÷ª…Ë÷√xy÷µ

	return myRoute;
}

ByyTacticalGraphicsCreator* ByyRouteEditor::create( ByyIG& ig,ByyControlObject* obj )
{
	ByyRoute *route=dynamic_cast<ByyRoute*>(obj);

	if (route)
	{
		return new ByyRouteEditor(ig,route);
	}

	return 0;
}
