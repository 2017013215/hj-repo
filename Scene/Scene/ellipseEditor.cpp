#include "StdAfx.h"
#include "ellipseEditor.h"
#include "ByyEllipse.h"

ByyEllipseEditor::ByyEllipseEditor( ByyIG& ig,ByyEllipse* ellipse )
	:ByyTacticalGraphEditor(ig,ellipse)
	,myEllipse(ellipse)
{
	osgEarth::Symbology::Style myEllipseStyle;

	myEllipseStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color() = myFillColor;
	myEllipseStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = 0.0;
	myEllipseStyle.getOrCreate<osgEarth::Symbology::PolygonSymbol>()->fill()->color() = myFillColor;
	myEllipseStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
	myEllipseStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_DRAPE;

	myEllipseNode=new osgEarth::Annotation::EllipseNode(myMapNode.get(),osgEarth::GeoPoint(myMapNode->getMapSRS(),myEllipse->position()),
		osgEarth::Distance(myEllipse->majorSemiAxis()),osgEarth::Distance(myEllipse->minorSemiAxis()),osgEarth::Angle(myEllipse->rotationAngle()),myEllipseStyle);

	myRoot->addChild(myEllipseNode);

	myEditor=new osgEarth::Annotation::EllipseNodeEditor(myEllipseNode);

	myRoot->addChild(myEditor);
}

ByyEllipseEditor::~ByyEllipseEditor()
{
	myRoot->removeChild(myEditor);
	myRoot->removeChild(myEllipseNode);
}

ByyControlObject* ByyEllipseEditor::accept()
{
	double alti = myEllipse->position().z();
	myEllipse->setPosition(myEllipseNode->getPosition().vec3d());
	myEllipse->position().z() = alti; // 保持原高度不变
	myEllipse->setMajorSemiAxis(myEllipseNode->getRadiusMajor().getValue());
	myEllipse->setMinorSemiAxis(myEllipseNode->getRadiusMinor().getValue());
	myEllipse->setRotationAngle(myEllipseNode->getRotationAngle().getValue());

	return myEllipse;
}

ByyTacticalGraphicsCreator* ByyEllipseEditor::create( ByyIG& ig,ByyControlObject* obj )
{
	ByyEllipse *ellipse=dynamic_cast<ByyEllipse*>(obj);

	if (ellipse)
	{
		return new ByyEllipseEditor(ig,ellipse);
	}

	return 0;
}
