#include <stdafx.h>
#include "sandboxPlacer.h"

#include <osgViewer/View>

ByySandBoxPlacer::ByySandBoxPlacer( osgEarth::Annotation::GeoPositionNode* anno)
	:d_annoNode(anno)
{
	d_startPos=d_annoNode->getPosition();
}

ByySandBoxPlacer::~ByySandBoxPlacer()
{

}

bool ByySandBoxPlacer::processOsgEvent(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
	if (ea.getEventType()==osgGA::GUIEventAdapter::MOVE)
	{
		osgViewer::View *view=dynamic_cast<osgViewer::View*>(&aa);

		osg::Vec3d world;

		if (d_annoNode->getMapNode()->getTerrain()->getWorldCoordsUnderMouse(view,ea.getX(),ea.getY(),world))
		{
			osgEarth::GeoPoint mapPoint;

			mapPoint.fromWorld(d_annoNode->getMapNode()->getMapSRS(),world);

			d_annoNode->setPosition(mapPoint);
		}
	}
	else if (ea.getEventType()==osgGA::GUIEventAdapter::RELEASE)
	{
		int result=-1;

		if (ea.getButton()==osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		{
			result=1;
		}
		else if (ea.getButton()==osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
		{
			result=0;

			d_annoNode->setPosition(d_startPos);
		}

		if (result!=-1)
		{
			emit finish(result);
		}
	}

	return false;
}