#include "StdAfx.h"
#include "waypointCreator.h"
#include "ByyWaypoint.h"
#include "utils.h"
#include "ByyIG.h"
#include "terrain.h"

#include <osgEarthAnnotation/PlaceNode>

ByyWaypointCreator::ByyWaypointCreator( ByyIG& ig )
	: ByyTacticalGraphicsCreator(ig)
{
	static osg::ref_ptr<osg::Image> waypointImage=osgDB::readImageFile("../data/Textures/triangle.png");

	myPlaceNode=new osgEarth::Annotation::PlaceNode(myMapNode.get(),osgEarth::GeoPoint(),waypointImage,"");
	myRoot->addChild(myPlaceNode);
}

ByyWaypointCreator::~ByyWaypointCreator()
{
	if (myPlaceNode)
	{
		myRoot->removeChild(myPlaceNode);
	}
}

ByyControlObject* ByyWaypointCreator::accept()
{
	ByyWaypoint *myWayPoint=new ByyWaypoint;

	if (myPlaceNode)
	{
		myWayPoint->setPosition(myPlaceNode->getPosition().vec3d());
	}

	return myWayPoint;
}

bool ByyWaypointCreator::processEvent( const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa )
{
	switch(ea.getEventType())
	{
	case ea.PUSH:
		myMouseDown=ea.getButton();
		myXDown=ea.getX();
		myYDown=ea.getY();
		return true;
	case ea.RELEASE:
		if (ea.getButton()==myMouseDown&&ea.getX()==myXDown&&ea.getY()==myYDown)
		{
			mouseClicked(ea.getButton(),ea.getX(),ea.getY(),aa.asView());
		}
		return true;
	case ea.MOVE:
		{
			mouseMove(ea.getX(),ea.getY(),aa.asView());
		}
		return true;
	default:
		return __super::processEvent(ea,aa);
	}
}

ByyTacticalGraphicsCreator* ByyWaypointCreator::create( ByyIG& ig )
{
	return new ByyWaypointCreator(ig);
}

bool ByyWaypointCreator::mouseClicked( int button,int x,int y,osg::View *view )
{
	osgEarth::MapNode* mapNode=myIG.terrain()->mapNode();

	if (button==osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
	{
		emit finished(1);
	}
	else if (button==osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
	{
		emit finished(0);
	}

	return true;
}

bool ByyWaypointCreator::mouseMove( int x,int y,osg::View *view )
{
	osgEarth::MapNode* mapNode=myIG.terrain()->mapNode();

	osg::Vec3d world;

	if (mapNode&&mapNode->getTerrain()->getWorldCoordsUnderMouse(view,x,y,world))
	{
		osgEarth::GeoPoint mapPoint;
		mapPoint.fromWorld(mapNode->getMapSRS(),world);

		if (mapPoint.alt()<0)
		{
			mapPoint.alt()=0;
		}

		myPlaceNode->setPosition(mapPoint);
	}
	return true;
}