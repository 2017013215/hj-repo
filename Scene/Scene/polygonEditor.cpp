#include "StdAfx.h"
#include "polygonEditor.h"
#include "ByyPolygon.h"

ByyPolygonEditor::ByyPolygonEditor( ByyIG& ig,ByyPolygon* polygon )
	:ByyTacticalGraphEditor(ig,polygon)
	,myPolygon(polygon)
{
	myOEPolygon=new osgEarth::Symbology::Polygon;

	ByyVec3dArray pts=myPolygon->points();

	for (int i=0;i!=pts.size();++i)
	{
		myOEPolygon->push_back(pts[i]);
	}

	osgEarth::Symbology::Style polyStyle;
	polyStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color() = myPathColor;
	polyStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = 12.0;
	polyStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->tessellation() = 20;
	polyStyle.getOrCreate<osgEarth::Symbology::PolygonSymbol>()->fill()->color() = myFillColor;
	polyStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
	polyStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_DRAPE;

	myPoyFeature=new osgEarth::Features::Feature(myOEPolygon, myMapNode->getMapSRS(), polyStyle);

	myPoyNode = new osgEarth::Annotation::FeatureNode(myMapNode.get(), myPoyFeature);
	myRoot->addChild(myPoyNode);

	myEditor=new osgEarth::Annotation::FeatureEditor(myPoyNode);

	myRoot->addChild(myEditor);
}

ByyPolygonEditor::~ByyPolygonEditor()
{
	myRoot->removeChild(myPoyNode);
	myRoot->removeChild(myEditor);
}

ByyControlObject* ByyPolygonEditor::accept()
{
	QVector<osg::Vec3d> result(myOEPolygon->size());

	for (int i=0;i!=myOEPolygon->size();++i)
	{
		result[i]=myOEPolygon->at(i);
	}

	//myPolygon->setPoints(result);
	myPolygon->setPoints_xy(result);   // 20240708, ÷ª…Ë÷√xy÷µ

	return myPolygon;
}

ByyTacticalGraphicsCreator* ByyPolygonEditor::create( ByyIG& ig,ByyControlObject* obj )
{
	ByyPolygon *polygon=dynamic_cast<ByyPolygon*>(obj);

	if (polygon)
	{
		return new ByyPolygonEditor(ig,polygon);
	}

	return 0;
}
