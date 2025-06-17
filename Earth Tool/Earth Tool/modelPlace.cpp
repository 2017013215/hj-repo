#include "stdafx.h"
#include "modelPlace.h"
#include "sceneController.h"
#include <QMetaType>

ByyModelPlace::ByyModelPlace( const QString& modelfile,ByySceneController* con ) 
	:_modelFileName(modelfile),
	sceneController(con),
	_root(con->getRootNode()),
	_mapNode(con->getMapNode()),
	_terrain(_mapNode->getTerrain()),
	draged(false),
	_query(_mapNode->getMap())
{
	qRegisterMetaType<osgEarth::GeoPoint>("osgEarth::GeoPoint");
	osgEarth::Style style;
	style.getOrCreate<osgEarth::Symbology::ModelSymbol>()->setModel( osgDB::readNodeFile(modelfile.toStdString()) );
	_modelNode = new osgEarth::Annotation::ModelNode(_mapNode, style );

	_root->addChild(_modelNode);
}

ByyModelPlace::~ByyModelPlace()
{
	_root->removeChild(_modelNode);
}

bool ByyModelPlace::handle( const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa )
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
				emit placePosition(_modelNode->getPosition());
			}
			else if (ea.getButton()==osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
			{
				emit placeFinished();
			}
		}
		break;
	case  osgGA::GUIEventAdapter::DRAG:
		draged=true;
		break;
	}

	return false;
}

void ByyModelPlace::update( float x, float y, osgViewer::View* view )
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

QString ByyModelPlace::getModelFileName()
{
	return _modelFileName;
}