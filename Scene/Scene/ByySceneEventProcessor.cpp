#include "StdAfx.h"
#include "ByySceneEventProcessor.h"

#include "ByyIG.h"
#include "ByyNodeBank.h"
#include "ByySelectionManager.h"
#include "ByyInputController.h"

#include "entitySettings.h"

#include <QApplication>
#include "terrain.h"

ByySceneEventProcessor::ByySceneEventProcessor( ByyIG& ig )
	:myIG(ig)
{

}

ByySceneEventProcessor::~ByySceneEventProcessor()
{

}

bool ByySceneEventProcessor::processOsgEvent( const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa )
{
	if (ea.getEventType()==ea.RELEASE)
	{
		int selectType;
		unsigned int selectId;

		if (hasSelection(ea,aa,selectType,selectId))
		{
			ByySelectionManager::instance().setCurrentSelection((ByySelectionManager::SelectionType)selectType,selectId);

			if (ea.getButton()==ea.RIGHT_MOUSE_BUTTON)
			{
				emit myIG.inputController().requestContextMenu();
			}
		}
	}
	else if (ea.getEventType()==ea.DOUBLECLICK&&ea.getButton()==ea.LEFT_MOUSE_BUTTON)
	{
		int selectType;
		unsigned int selectId;

		if (hasSelection(ea,aa,selectType,selectId))
		{
			emit myIG.inputController().requsetDoubleClick();

			return true;
		}
	}

	return false;
}

bool ByySceneEventProcessor::hasSelection( const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa, int& out_select_type,unsigned int& out_select_id )
{
	osgViewer::View *view=dynamic_cast<osgViewer::View*>(aa.asView());

	osgEarth::IntersectionPicker picker(view);
	picker.setLimit(osgEarth::IntersectionPicker::NO_LIMIT);

	osgEarth::IntersectionPicker::Hits hits;

	if (!picker.pick( ea.getX(), ea.getY(), hits ))
		return false;

	const osg::Camera * cam = view->getCamera();
	const osg::Viewport * viewport = cam->getViewport();
	osg::Matrixd windowMatrix = viewport->computeWindowMatrix();
	osg::Matrixd viewProjectionWindowMatrix = cam->getViewMatrix() * cam->getProjectionMatrix() * windowMatrix;

	osg::Vec3d position;

	osg::Vec3d v3;

	osg::Vec2d v2;

	osg::Vec2d mousePos(ea.getX(),ea.getY());

	ByyNodeBank::PlaceNodeList& placeNodeList=ByyNodeBank::instance().getPlaceNodeList();

	ByyNodeBank::PlaceNodeList::iterator it=placeNodeList.begin();

	for (;it!=placeNodeList.end();++it)
	{
		//if (it.key()->getNodeMask()==0||!it.key()->getPosition().toWorld(position))
		//	continue;
		if( it.key()->getNodeMask()==0 )
			continue;
		osgEarth::GeoPoint gp=it.key()->getPosition();
		if(gp.altitudeMode()==osgEarth::ALTMODE_RELATIVE)
		{
			gp.makeAbsolute(myIG.terrain()->mapNode()->getTerrain());
		}
		if(!gp.toWorld(position))
		{
			continue;
		}

		v3=position * viewProjectionWindowMatrix;


		bool inScreen=viewport->x() < v3.x() && v3.x() < viewport->x() + viewport->width() &&
			viewport->y() < v3.y() && v3.y() < viewport->y() + viewport->height();

		if(!inScreen)
			continue;

		v2.set(v3.x(),v3.y());

		if ((v2-mousePos).length()>16)
			continue;

		out_select_type=ByySelectionManager::Entity;
		out_select_id=it.value();

		return true;
	}
	
	for( osgEarth::IntersectionPicker::Hits::const_iterator h = hits.begin();h != hits.end(); ++h )
	{
		for( osg::NodePath::const_reverse_iterator i = h->nodePath.rbegin();i != h->nodePath.rend(); ++i )
		{
			ByyNodeBank::NodeData nd=ByyNodeBank::instance().getNodeData(*i);

			if (!nd.valid())
				continue;
			

			if (nd.majorType==ByyNodeBank::Entity&&nd.minorType==ByyNodeBank::MainObject)
			{
				out_select_type=ByySelectionManager::Entity;
				out_select_id=nd.objectId;

				return true;
			}
			else if (nd.majorType==ByyNodeBank::ControlObject&&nd.minorType==ByyNodeBank::MainObject)
			{
				out_select_type=ByySelectionManager::ControlObject;
				out_select_id=nd.objectId;

				return true;
			}
		}
	}

	if(!ByyEntitySettings::instance().entityLabelEnabled())
		return false;

	

	return false;
}
