#include "stdafx.h"
#include "earthManipulator.h"

ByyEarthManipulator::ByyEarthManipulator()
	: EarthManipulator()
{
	getSettings()->bindKey(EarthManipulator::ACTION_HOME,osgGA::GUIEventAdapter::KEY_Escape);
	getSettings()->bindKey(EarthManipulator::ACTION_NULL,osgGA::GUIEventAdapter::KEY_Space);

	getSettings()->bindScroll(EarthManipulator::ACTION_ZOOM_IN,osgGA::GUIEventAdapter::SCROLL_UP);
	getSettings()->bindScroll(EarthManipulator::ACTION_ZOOM_OUT,osgGA::GUIEventAdapter::SCROLL_DOWN);
}

ByyEarthManipulator::~ByyEarthManipulator()
{

}

void ByyEarthManipulator::setTetherNode( osg::Node* node )
{
	EarthManipulator::setTetherNode(node);
	if (node!=NULL)
	{
		adjustViewpoint();
	}
}

bool ByyEarthManipulator::handleKeyboardAction( const Action &action, double duration_s/*=DBL_MAX*/ )
{
	bool bl=osgEarth::Util::EarthManipulator::handleKeyboardAction(action,duration_s);
	if (action._type==ACTION_HOME&&getTetherNode()!=NULL)
	{
		adjustViewpoint();
	}
	return bl;
}

void ByyEarthManipulator::adjustViewpoint()
{
	osgEarth::Viewpoint vp=getViewpoint();
	if (getTetherNode())
	{
		vp.setRange(getTetherNode()->getBound().radius()*5);
		osg::DisplaySettings::instance()->setEyeSeparation(0.1);
	}
	else
	{
		vp.setRange(20);
		osg::DisplaySettings::instance()->setEyeSeparation(0.05);
	}
	
	setViewpoint(vp);
}

bool ByyEarthManipulator::handleMouseAction( const Action &action, osg::View *view )
{
	// return if less then two events have been added.
	if (_ga_t0.get()==NULL || _ga_t1.get()==NULL) return false;

	//if ( osgEarth::getNotifyLevel() > osg::INFO )
	//    dumpActionInfo( action, osg::DEBUG_INFO );

	double dx=0;


	if (osg::DisplaySettings::instance()->getStereo())
	{
		dx =(2.0f*(_ga_t0->getX()-_ga_t0->getXmin())/(_ga_t0->getXmax()-_ga_t0->getXmin())-1.0f)-(2.0f*(_ga_t1->getX()-_ga_t1->getXmin())/(_ga_t1->getXmax()-_ga_t1->getXmin())-1.0f);
	}
	else
	{
		dx = _ga_t0->getXnormalized()-_ga_t1->getXnormalized();
	}
	
	double dy = _ga_t0->getYnormalized()-_ga_t1->getYnormalized();

	//printf("%f,%f,%f\n",_ga_t0->getXnormalized(),_ga_t1->getXnormalized(),dx);
	// return if there is no movement.
	if (dx==0 && dy==0) return false;

	// here we adjust for action scale, global sensitivy
	dx *= _settings->getMouseSensitivity();
	dy *= _settings->getMouseSensitivity();

	applyOptionsToDeltas( action, dx, dy );

	// in "continuous" mode, we accumulate the deltas each frame - thus
	// the deltas act more like speeds.
	if ( _continuous )
	{
		_continuous_dx += dx * 0.01;
		_continuous_dy += dy * 0.01;
	}
	else
	{

		_dx = dx;
		_dy = dy;
		handleMovementAction( action._type, dx, dy, view );
	}

	return true;
}

void ByyEarthManipulator::rotate( double dx,double dy )
{
	osgEarth::Util::EarthManipulator::rotate(dx,dy);

	adjustDistance();
}

void ByyEarthManipulator::adjustDistance()
{
	if(osg::DisplaySettings::instance()->getStereo())
	{
		osgEarth::Viewpoint vp=getViewpoint();
		float dis=0.5;
		float eye=0.05;

		if(vp.getPitch()<(-75))
			dis=2.0;
		else if(vp.getPitch()<(-65))
			dis=2.0;
		else if(vp.getPitch()<(-55))
			dis=2.0;
		else if(vp.getPitch()<(-45))
			dis=2.5;
		else if(vp.getPitch()<(-40))
			dis=3.0;
		else if(vp.getPitch()<(-35))
			dis=3.5;
		else if(vp.getPitch()<(-30))
			dis=4.0;
		else if(vp.getPitch()<(-25))
			dis=4.5;
		else if(vp.getPitch()<(-20))
			dis=5.0;
		else if(vp.getPitch()<(-15))
		{
			dis=5.5;
			//eye=0.2;
		}
		else	
		{
			dis=5.5;
			//eye=0.2;
		}

		osg::DisplaySettings::instance()->setScreenDistance(dis);

		vp.focalPoint()->vec3d().x()-=9000;
		vp.focalPoint()->vec3d().y()-=9000;
		vp.focalPoint()->vec3d().z()-=9000;
	}
}

void ByyEarthManipulator::setViewpoint( const osgEarth::Viewpoint &vp, double duration_s/*=0.0*/ )
{
	osgEarth::Util::EarthManipulator::setViewpoint(vp,duration_s);

	adjustDistance();
}

void ByyEarthManipulator::pan( double dx, double dy )
{
	if (getTetherNode())
	{
		setTetherNode(0);
	}

	__super::pan(dx,dy);
}



// 
// void SnEarthManipulator::handleContinuousAction( const Action &action, osg::View *view )
// {
// 
// }
