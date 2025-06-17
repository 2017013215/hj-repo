#include "StdAfx.h"
#include "ByySceneArc.h"
#include "ByyArc.h"
#include "utils.h"

ByySceneArc::ByySceneArc(ByyArc *ci,ByyIG& ig) : ByySceneCircle(ci,ig)
{
	connect(ci,SIGNAL(startAngleChanged(double)),SLOT(setStartAngle(double)));
	connect(ci,SIGNAL(endAngleChanged(double)),SLOT(setEndAngle(double)));
	myArc=ci;
}


ByySceneArc::~ByySceneArc(void)
{
}

void ByySceneArc::initiate()
{
	osgEarth::Symbology::Style style;

	style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color() = forceTypeColorAlpha(myCircle->forceType(),0.9);
	style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = LINE_WIDTH;
	style.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
	style.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique()=osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_DRAPE;

	myCircleNode=new osgEarth::Annotation::CircleNode(mapNode(),osgEarth::GeoPoint(srs(),myArc->position()),myArc->radius(),style,myArc->startAngle(),myArc->endAngle());

	myRoot->addChild(myCircleNode);

	setPosition(myCircle->position());
}


void ByySceneArc::setStartAngle(double angle)
{
	myCircleNode->setArcStart(angle);
}

void ByySceneArc::setEndAngle( double angle )
{
	myCircleNode->setArcEnd(angle);
}


ByySceneControlObject* ByySceneArc::create( ByyControlObject* obj,ByyIG& ig )
{
	ByyArc* wp=dynamic_cast<ByyArc*>(obj);

	if (wp)
	{
		return new ByySceneArc(wp,ig);
	}

	return 0;
}
