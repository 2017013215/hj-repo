#include "StdAfx.h"
#include "lineStringTypeCreator.h"
#include "utils.h"

ByyLineStringVisualizer::ByyLineStringVisualizer( ByyIG& ig )
	:ByySceneObjCreateVisualizer(ig)
{
	myPathLine=new osgEarth::Symbology::LineString;

	osgEarth::Symbology::Style pathStyle;
	pathStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color() = osgEarth::Color::White;
	pathStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = LINE_WIDTH;
	pathStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->tessellation() = 20;
	pathStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
	pathStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_GPU;


	myPathFeature=new osgEarth::Features::Feature(myPathLine, myMapNode->getMapSRS(), pathStyle);

	myPathNode = new osgEarth::Annotation::FeatureNode(myMapNode.get(), myPathFeature);
	myRoot->addChild(myPathNode);
}

ByyLineStringVisualizer::~ByyLineStringVisualizer()
{
	myRoot->removeChild(myPathNode);
}

void ByyLineStringVisualizer::updateLineString( const ByyVec3dArray& pnts )
{
	myPathLine->resize(pnts.size());

	for (int i=0;i!=pnts.size();++i)
	{
		(*myPathLine)[i]=pnts[i];
	}

	myPathNode->init();
}

ByyLineStringTypeCreator::ByyLineStringTypeCreator( ByyIG& ig )
	:ByyPointSetCreator(ig)
	,ByyLineStringVisualizer(ig)
{

}

ByyLineStringTypeCreator::~ByyLineStringTypeCreator()
{

}

void ByyLineStringTypeCreator::update( const QVector<osg::Vec3d>& pts )
{
	updateLineString(calcPlots(pts));
}

ByyVec3dArray ByyLineStringTypeCreator::calcPlots( const ByyVec3dArray& srcPoints )
{
	return srcPoints;
}

ByyFixedLineStringTypeCreator::ByyFixedLineStringTypeCreator( ByyIG& ig )
	:ByyFixedPointSetCreator(ig)
	,ByyLineStringVisualizer(ig)
{

}

ByyFixedLineStringTypeCreator::~ByyFixedLineStringTypeCreator()
{

}

void ByyFixedLineStringTypeCreator::update( const QVector<osg::Vec3d>& pts )
{
	updateLineString(calcPlots(pts));
}

ByyVec3dArray ByyFixedLineStringTypeCreator::calcPlots( const ByyVec3dArray& srcPoints )
{
	return srcPoints;
}
