#include "StdAfx.h"
#include "pointSetCreator.h"
#include "ByyEventProcessor.h"
#include "ByyIG.h"
#include "terrain.h"

ByyPointSetCreator::ByyPointSetCreator( ByyIG& ig ) 
	: ByyTacticalGraphicsCreator(ig)
{
	myDraggers=new osg::Group;
	myRoot->addChild(myDraggers);

	osg::StateSet* stateSet = myDraggers->getOrCreateStateSet();
	stateSet->setMode(GL_DEPTH_TEST, 0);
	stateSet->setRenderBinDetails(99, "RenderBin");
}

ByyPointSetCreator::~ByyPointSetCreator()
{
	myRoot->removeChild(myDraggers);
}

bool ByyPointSetCreator::mouseClicked( int button,int x,int y,osg::View *view )
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
		emit finished(getPointCount()>=getMinPointCount());
	}

	return true;
}

bool ByyPointSetCreator::mouseMove( int button,int x,int y,osg::View *view )
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

class ByySphereDragger : public osgEarth::Annotation::SphereDragger
{
public:
	ByySphereDragger(osgEarth::MapNode *mapNode)
		:osgEarth::Annotation::SphereDragger(mapNode)
	{
		setNumChildrenRequiringEventTraversal(0);
	}
};

void ByyPointSetCreator::createPointDragger( int index, const osgEarth::GeoPoint& point )
{
	osgEarth::Annotation::SphereDragger* sd = new ByySphereDragger(myMapNode.get());
	sd->setSize(4.0f);
	sd->setColor(osgEarth::Color::Magenta);
	sd->setPickColor(osgEarth::Color::Green);
	sd->setPosition(point);
	PointDraggerCallback* callback = new PointDraggerCallback(index, this);
	sd->addPositionChangedCallback(callback);

	myDraggers->addChild(sd);
}

void ByyPointSetCreator::addPoint( const osgEarth::GeoPoint& point,bool refresh/*=true*/ )
{
	myPoints.append(point.vec3d());

	myPoints.last().z()=0;

	createPointDragger(myPoints.size()-1, point);

	update(myPoints);
}

void ByyPointSetCreator::mouseMove( const osgEarth::GeoPoint& point )
{
	if (getPointCount()==0)
		return;

	QVector<osg::Vec3d> pts=myPoints;

	pts.append(point.vec3d());
	pts.last().z()=0;

	update(pts);
}

/*
void ByyPointSetCreator::setPoints( const ByyVec3dArray& points )
{
	myPoints=points;
}
*/

ByyVec3dArray ByyPointSetCreator::getPoints()
{
	return myPoints;
}

unsigned int ByyPointSetCreator::getPointCount()
{
	return myPoints.size();
}

ByyFixedPointSetCreator::ByyFixedPointSetCreator( ByyIG& ig )
	:ByyPointSetCreator(ig)
{

}

ByyFixedPointSetCreator::~ByyFixedPointSetCreator()
{

}

void ByyFixedPointSetCreator::addPoint( const osgEarth::GeoPoint& point,bool refresh/*=true*/ )
{
	if (getPointCount()>=getFixedPointCount())
		return;
	else
		__super::addPoint(point,refresh);
}

void ByyFixedPointSetCreator::mouseMove( const osgEarth::GeoPoint& point )
{
	if (getPointCount()>=getFixedPointCount())
		return;
	else
		__super::mouseMove(point);
}
