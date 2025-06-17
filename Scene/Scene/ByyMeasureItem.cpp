#include "StdAfx.h"
#include "ByyMeasureItem.h"
#include "ByyIG.h"
#include "terrain.h"
#include "ByyRenderer.h"
#include "ByyInputController.h"

ByyMeasureItem::ByyMeasureItem( ByyIG& ig )
	: myIG(ig)
	, myEventProcessor(this)
{
	myIG.inputController().addEventProcessor(&myEventProcessor);
}

ByyMeasureItem::~ByyMeasureItem()
{
	myIG.inputController().removeEventProcessor(&myEventProcessor);
}

bool ByyMeasureItem::processEvent( const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa )
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
			return mouseClicked(ea.getButton(),ea.getX(),ea.getY(),aa.asView());
		}
		return false;
	case ea.MOVE:
		return mouseMove(ea.getButton(),ea.getX(),ea.getY(),aa.asView());
	}

	return false;
}

bool ByyMeasureItem::mouseClicked( int button,int x,int y,osg::View *view )
{
	osgEarth::MapNode* mapNode=myIG.terrain()->mapNode();

	osg::Vec3d world;

	if (button==osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON&&mapNode&&
		mapNode->getTerrain()->getWorldCoordsUnderMouse(view,x,y,world))
	{
		osgEarth::GeoPoint mapPoint;
		mapPoint.fromWorld(mapNode->getMapSRS(),world);
		if (mapPoint.alt()<0)
		{
			mapPoint.alt()=0;
		}
		addPoint(mapPoint);
	}
	else if (button==osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
	{
		emit finished();
	}

	return true;
}

bool ByyMeasureItem::mouseMove( int button,int x,int y,osg::View *view )
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
		mouseMove(mapPoint);
	}
	return true;
}

void ByyMeasureItem::mouseMove( const osgEarth::GeoPoint& point )
{

}

void ByyMeasureItem::addPoint( const osgEarth::GeoPoint& point,bool refresh/*=true*/ )
{

}
