#include "StdAfx.h"
#include "ByyObserver.h"
#include "earthManipulator.h"
#include "observerSettings.h"

ByyObserver* obIns=0;
ByyObserver* oberverInstance()
{
	return obIns;

}
ByyObserver::ByyObserver()
{
	obIns=this;
	myManip=new ByyEarthManipulator;

	myManip->setHomeViewpoint(osgEarth::Viewpoint("",122.185285550636,29.9609088672184,0, 0, -89,osg::EllipsoidModel().getRadiusEquator()*3.0 ));

	set2D(ByyObserverSettings::instance().is2D());

	connect(&ByyObserverSettings::instance(),SIGNAL(projectionChanged(bool)),this,SLOT(set2D(bool)));
}

ByyObserver::~ByyObserver()
{

}

void ByyObserver::setViewpoint( const osgEarth::Viewpoint& viewpoint )
{
	if (myManip->getTetherNode()!=NULL)
	{
		myManip->setTetherNode(NULL);
	}

	myManip->setViewpoint(viewpoint);
}

void ByyObserver::setHomeViewpoint(const osgEarth::Viewpoint& viewpoint)
{
	myManip->setHomeViewpoint(viewpoint);
}

osgEarth::Viewpoint ByyObserver::getViewpoint()
{
	return myManip->getViewpoint();
}

void ByyObserver::reset()
{
	myManip->home(0);
}

void ByyObserver::attach( osg::Node* node )
{
	myManip->setTetherNode(node);
}

osg::Node* ByyObserver::attachNode()
{
	return myManip->getTetherNode();
}

osgGA::CameraManipulator* ByyObserver::manip()
{
	return myManip;
}

void ByyObserver::set2D( bool is2d )
{
	if (is2d)
	{
		myManip->setRotation(osg::Quat());
		myManip->getSettings()->setMinMaxPitch(-90,-90);
		myManip->getSettings()->bindMouse( osgEarth::Util::EarthManipulator::ACTION_NULL, osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON );
		myManip->getSettings()->bindMouse( osgEarth::Util::EarthManipulator::ACTION_NULL, osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON | osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON );
	}
	else
	{
		myManip->getSettings()->setMinMaxPitch(-89.9,-4);
		myManip->getSettings()->bindMouse( osgEarth::Util::EarthManipulator::ACTION_ROTATE, osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON );
		myManip->getSettings()->bindMouse( osgEarth::Util::EarthManipulator::ACTION_ROTATE, osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON | osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON );
	}
}
