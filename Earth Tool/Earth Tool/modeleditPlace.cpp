#include "stdafx.h"
#include "modeleditPlace.h"
#include "sceneController.h"

ByyModelEditPlace::ByyModelEditPlace(osgEarth::Annotation::ModelNode* modelnode,ByySceneController* con)
	:
	sceneController(con),
	_mapNode(con->getMapNode()),
	_terrain(_mapNode->getTerrain()),
	draged(false),
	_query(_mapNode->getMap()),
	_modelNode(modelnode),
	initialGeoPoint(_modelNode->getPosition())
{

}

ByyModelEditPlace::~ByyModelEditPlace()
{

}
bool ByyModelEditPlace::handle( const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa )
{
	switch(ea.getEventType())
	{
	case osgGA::GUIEventAdapter::MOVE:
	case osgGA::GUIEventAdapter::SCROLL:
		{
			osgViewer::View* view = static_cast<osgViewer::View*>(aa.asView());
			update( ea.getX(), ea.getY(), view );
		}
		break;
	case  osgGA::GUIEventAdapter::RELEASE:
		if (draged)
		{
			draged=false;
		}
		else
		{
			if (ea.getButton()==osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
			{
				emit placeFinished(true);
			}
			else if (ea.getButton()==osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
			{
				_modelNode->setPosition(initialGeoPoint);
				emit placeFinished(false);
			}
		}
		break;
	case  osgGA::GUIEventAdapter::DRAG:
		draged=true;
		break;
	}

	return false;
}

void ByyModelEditPlace::update( float x, float y, osgViewer::View* view )
{
	if ( _terrain->getWorldCoordsUnderMouse(view, x, y, world) )
	{
		mapPoint.fromWorld( _terrain->getSRS(), world );

		double out_hamsl        = 0.0;

		_query.getElevation( mapPoint,out_hamsl );

		mapPoint.z() = out_hamsl;
		_modelNode->setPosition(mapPoint);
	}
}

osgEarth::Annotation::ModelNode* ByyModelEditPlace::getModelNode()
{
	return _modelNode;
}
