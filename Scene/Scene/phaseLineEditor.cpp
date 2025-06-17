#include "StdAfx.h"
#include "phaseLineEditor.h"
#include "ByyPhaseLine.h"

ByyPhaseLineEditor::ByyPhaseLineEditor( ByyIG& ig,ByyPhaseLine* phaseLine )
	:ByyTacticalGraphEditor(ig,phaseLine)
	,myPhaseLine(phaseLine)
{
	myPathLine=new osgEarth::Symbology::LineString;

	ByyVec3dArray pts=myPhaseLine->points();

	for (int i=0;i!=pts.size();++i)
	{
		myPathLine->push_back(pts[i]);
	}

	osgEarth::Symbology::Style pathStyle;
	pathStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color() = osgEarth::Color::White;
	pathStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = 2.0f;
	pathStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->tessellation() = 20;
	pathStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
	pathStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_SCENE;

	myPathFeature=new osgEarth::Features::Feature(myPathLine, myMapNode->getMapSRS(), pathStyle);

	myPathNode = new osgEarth::Annotation::FeatureNode(myMapNode.get(), myPathFeature);
	myRoot->addChild(myPathNode);

	myEditor=new osgEarth::Annotation::FeatureEditor(myPathNode);

	myRoot->addChild(myEditor);
}

ByyPhaseLineEditor::~ByyPhaseLineEditor()
{
	myRoot->removeChild(myEditor);
	myRoot->removeChild(myPathNode);
}

ByyControlObject* ByyPhaseLineEditor::accept()
{
	QVector<osg::Vec3d> result(myPathLine->size());

	for (int i=0;i!=myPathLine->size();++i)
	{
		result[i]=myPathLine->at(i);
	}

	//myPhaseLine->setPoints(result);
	myPhaseLine->setPoints_xy(result);   // 20240708, ÷ª…Ë÷√xy÷µ

	return myPhaseLine;
}

ByyTacticalGraphicsCreator* ByyPhaseLineEditor::create( ByyIG& ig,ByyControlObject* obj )
{
	ByyPhaseLine *phaseLine=dynamic_cast<ByyPhaseLine*>(obj);

	if (phaseLine)
	{
		return new ByyPhaseLineEditor(ig,phaseLine);
	}

	return 0;
}
