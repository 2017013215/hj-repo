#include "StdAfx.h"
#include "circleEditor.h"
#include "ByyCircle.h"
#include "ByyArc.h"

ByyCircleEditor::ByyCircleEditor(ByyIG& ig,ByyCircle* circle)
	:ByyTacticalGraphEditor(ig,circle)
	,myCircle(circle)
{
	osgEarth::Symbology::Style myCircleStyle;

	myCircleStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color() = myFillColor;
	myCircleStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = 0.0;
	myCircleStyle.getOrCreate<osgEarth::Symbology::PolygonSymbol>()->fill()->color() = myFillColor;
	myCircleStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
	myCircleStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_DRAPE;

	myCircleNode=new osgEarth::Annotation::CircleNode(myMapNode.get(),osgEarth::GeoPoint(myMapNode->getMapSRS(),myCircle->position()),
		myCircle->radius(),myCircleStyle);

	if(circle->inherits("ByyArc"))
	{
		ByyArc* arc=(ByyArc*)circle;
		myCircleNode->setArcStart(arc->startAngle());
		myCircleNode->setArcEnd(arc->endAngle());
	}

	myRoot->addChild(myCircleNode);

	myCircleEditor=new osgEarth::Annotation::CircleNodeEditor(myCircleNode);

	myRoot->addChild(myCircleEditor);

}

ByyCircleEditor::~ByyCircleEditor()
{
	myRoot->removeChild(myCircleEditor);
	myRoot->removeChild(myCircleNode);
}

ByyTacticalGraphicsCreator* ByyCircleEditor::create( ByyIG& ig,ByyControlObject* obj )
{
	ByyCircle *circle=dynamic_cast<ByyCircle*>(obj);

	if (circle)
	{
		return new ByyCircleEditor(ig,circle);
	}

	return 0;
}

ByyControlObject* ByyCircleEditor::accept()
{
	double alti = myCircle->position().z();
	myCircle->setPosition(myCircleNode->getPosition().vec3d());
	myCircle->position().z() = alti; // 保持原高度不变
	myCircle->setRadius(myCircleNode->getRadius().getValue());

	return myCircle;
}