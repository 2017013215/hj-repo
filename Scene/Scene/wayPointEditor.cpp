#include "StdAfx.h"
#include "wayPointEditor.h"
#include "ByyWaypoint.h"

ByyWaypointEditor::ByyWaypointEditor( ByyIG& ig,ByyWaypoint* waypoint )
	:ByyWaypointCreator(ig)
	,myWaypoint(waypoint)
{
	static osg::ref_ptr<osg::Image> waypointImage=osgDB::readImageFile("../data/Textures/triangle.png");

	myPlaceNode=new osgEarth::Annotation::PlaceNode(myMapNode.get(),osgEarth::GeoPoint(myMapNode->getMapSRS(),myWaypoint->position()),waypointImage,"");
	myRoot->addChild(myPlaceNode);

	myWaypoint->setVisible(false);
}

ByyWaypointEditor::~ByyWaypointEditor()
{
	myWaypoint->setVisible(true);
}

ByyControlObject* ByyWaypointEditor::accept()
{
	double alti = myWaypoint->position().z();
	myWaypoint->setPosition(myPlaceNode->getPosition().vec3d());
	myWaypoint->position().z() = alti; // 保持原高度不变

	return myWaypoint;
}

ByyTacticalGraphicsCreator* ByyWaypointEditor::create( ByyIG& ig,ByyControlObject* obj )
{
	ByyWaypoint *waypoint=dynamic_cast<ByyWaypoint*>(obj);

	if (waypoint)
	{
		return new ByyWaypointEditor(ig,waypoint);
	}

	return 0;
}
