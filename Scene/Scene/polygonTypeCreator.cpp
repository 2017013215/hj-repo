#include "StdAfx.h"
#include "polygonTypeCreator.h"
#include "utils.h"

ByyPolygonVisualizer::ByyPolygonVisualizer( ByyIG& ig )
	:ByySceneObjCreateVisualizer(ig)
{
	myPolygon=new osgEarth::Symbology::Polygon;

	osgEarth::Symbology::Style polyStyle;
	polyStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = LINE_WIDTH;
	polyStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
	polyStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_DRAPE;

	
	myPolygonFeature=new osgEarth::Features::Feature(myPolygon, myMapNode->getMapSRS(), polyStyle);

	myPolygonNode = new osgEarth::Annotation::FeatureNode(myMapNode.get(), myPolygonFeature);
	myRoot->addChild(myPolygonNode);
}

ByyPolygonVisualizer::~ByyPolygonVisualizer()
{
	myRoot->removeChild(myPolygonNode);
}

void ByyPolygonVisualizer::updatePolygon( const ByyVec3dArray& pnts )
{
	myPolygon->resize(pnts.size());

	for (int i=0;i!=pnts.size();++i)
	{
		(*myPolygon)[i]=pnts[i];
	}

	myPolygonNode->init();
}


ByyPolygonTypeCreator::ByyPolygonTypeCreator( ByyIG& ig )
	:ByyPointSetCreator(ig)
	,ByyPolygonVisualizer(ig)
{
	
}

ByyPolygonTypeCreator::~ByyPolygonTypeCreator()
{
	
}

void ByyPolygonTypeCreator::update( const QVector<osg::Vec3d>& pts )
{
	updatePolygon(calcPlots(pts));
}

ByyVec3dArray ByyPolygonTypeCreator::calcPlots( const ByyVec3dArray& srcPoints )
{
	return srcPoints;
}

ByyFixedPolygonTypeCreator::ByyFixedPolygonTypeCreator( ByyIG& ig )
	:ByyFixedPointSetCreator(ig)
	,ByyPolygonVisualizer(ig)
{

}

ByyFixedPolygonTypeCreator::~ByyFixedPolygonTypeCreator()
{

}

void ByyFixedPolygonTypeCreator::update( const QVector<osg::Vec3d>& pts )
{
	updatePolygon(calcPlots(pts));
}

ByyVec3dArray ByyFixedPolygonTypeCreator::calcPlots( const ByyVec3dArray& srcPoints )
{
	return srcPoints;
}
