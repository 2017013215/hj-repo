#include "StdAfx.h"
#include "ByyArcEditor.h"
#include "utils.h"

ByyArcEditor::ByyArcEditor(ByyIG& ig,ByyArc* arc):ByyTacticalGraphEditor(ig,arc)
	,myArc(arc)
{
	myArcNode=new osgEarth::Symbology::LineString;
	myArcNode->push_back(arc->position());

	double lon,lat;
	osgEarth::GeoMath::destination(osg::DegreesToRadians(arc->position().y()),
		osg::DegreesToRadians(arc->position().x()),
		osg::DegreesToRadians(arc->startAngle()),
		arc->radius(),lat,lon);
	myArcNode->push_back(osg::Vec3d(osg::RadiansToDegrees(lon),osg::RadiansToDegrees(lat),0));

	osgEarth::GeoMath::destination(osg::DegreesToRadians(arc->position().y()),
		osg::DegreesToRadians(arc->position().x()),
		osg::DegreesToRadians(arc->endAngle()),
		arc->radius(),lat,lon);
	myArcNode->push_back(osg::Vec3d(osg::RadiansToDegrees(lon),osg::RadiansToDegrees(lat),0));


	osgEarth::Symbology::Style style;
	style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color() = myPathColor;
	style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = LINE_WIDTH;
	style.getOrCreate<osgEarth::Symbology::LineSymbol>()->tessellation() = 20;
	//polyStyle.getOrCreate<osgEarth::Symbology::PolygonSymbol>()->fill()->color() = myFillColor;
	style.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
	style.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_DRAPE;

	myFeature=new osgEarth::Features::Feature(myArcNode, myMapNode->getMapSRS(), style);

	myNode = new osgEarth::Annotation::FeatureNode(myMapNode.get(), myFeature);
	myRoot->addChild(myNode);

	myEditor=new osgEarth::Annotation::FeatureEditor(myNode);

	myRoot->addChild(myEditor);
}


ByyArcEditor::~ByyArcEditor(void)
{
	myRoot->removeChild(myNode);
	myRoot->removeChild(myEditor);
}


ByyControlObject* ByyArcEditor::accept()
{
	osg::Vec3 pos=myArcNode->at(0);
	myArc->setPosition(pos);
	osg::Vec3 startPt=myArcNode->at(1);
	double radius1=osgEarth::GeoMath::distance(pos,startPt,myMapNode->getMapSRS());
	double angle1=osg::RadiansToDegrees(osgEarth::GeoMath::bearing(osg::DegreesToRadians(pos.y()),osg::DegreesToRadians(pos.x()),osg::DegreesToRadians(startPt.y()),osg::DegreesToRadians(startPt.x())));
	startPt=myArcNode->at(2);
	double radius2=osgEarth::GeoMath::distance(pos,startPt,myMapNode->getMapSRS());
	double angle2=osg::RadiansToDegrees(osgEarth::GeoMath::bearing(osg::DegreesToRadians(pos.y()),osg::DegreesToRadians(pos.x()),osg::DegreesToRadians(startPt.y()),osg::DegreesToRadians(startPt.x())));
	
	myArc->setRadius(osg::maximum(radius1,radius2));
	myArc->setStartAngle(angle1);
	myArc->setEndAngle(angle2);
	return myArc;
}

ByyTacticalGraphicsCreator* ByyArcEditor::create( ByyIG& ig,ByyControlObject* obj )
{
	ByyArc *arc=dynamic_cast<ByyArc*>(obj);

	if (arc)
	{
		return new ByyArcEditor(ig,arc);
	}

	return 0;
}
