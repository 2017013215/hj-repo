#include "StdAfx.h"
#include "clipPlaneCullCallback.h"


ByyClipPlaneCullCallback::ByyClipPlaneCullCallback()
	:mySrs(osgEarth::SpatialReference::get("wgs84"))
{

}

ByyClipPlaneCullCallback::~ByyClipPlaneCullCallback()
{

}

void ByyClipPlaneCullCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
	osgUtil::CullVisitor* cv = osgEarth::Culling::asCullVisitor(nv);
	osg::Camera* camera = cv->getCurrentCamera();
	osgViewer::View *view=dynamic_cast<osgViewer::View*>(camera->getView());
	osgEarth::Util::EarthManipulator *em=view?dynamic_cast<osgEarth::Util::EarthManipulator*>(view->getCameraManipulator()):0;

	if ( camera &&em&&em->getTetherNode()!=NULL)
	{
		static osg::Vec3d eye,local;
		static double cameraAltitude,distance;

		eye = osg::Vec3d(0,0,0) * view->getCamera()->getInverseViewMatrix();

		mySrs->transformFromWorld(eye, local, &cameraAltitude);

		{
			osg::MatrixList ml=em->getTetherNode()->getWorldMatrices();

			if (ml.size()>0)
			{
				distance=(osg::Vec3d(0,0,0)*ml[0]-eye).length();
			}
		}

		if (cameraAltitude<800000&&cameraAltitude>150&&distance<200)
		{
			camera->setNearFarRatio(distance/200*0.0001);
		}
		else if(camera->getNearFarRatio()!=0.0001)
		{
			camera->setNearFarRatio(0.0001);
		}
	}
	else if(camera->getNearFarRatio()!=0.0001)
	{
		camera->setNearFarRatio(0.0001);
	}

	traverse(node, nv);
}

bool ByyClampProjectionMatrixCallback::clampProjectionMatrixImplementation( osg::Matrixf &projection, double &znear, double &zfar ) const
{
	return _clampProjectionMatrix(projection,znear,zfar);
}

bool ByyClampProjectionMatrixCallback::clampProjectionMatrixImplementation( osg::Matrixd &projection, double &znear, double &zfar ) const
{
	return _clampProjectionMatrix(projection,znear,zfar);
}

ByyClampProjectionMatrixCallback::ByyClampProjectionMatrixCallback( osg::Camera *camera,ByyIG& ig ,ByyChannel& channel)
	:myCamera(camera)
	,myIG(ig)
	,myChannel(channel)
{

}

double ByyClampProjectionMatrixCallback::calculateFarClip( double alt,double zfar ) const
{
	if (alt>10000)
	{
		return zfar*2;
	}
	else if (alt>150)
	{
		return 200000;
	}
	else if (alt>3)
	{
		return 200000;
	}
	else
	{
		return 70000;
	}
}

double ByyClampProjectionMatrixCallback::calculateNearClip( double alt, double zfar ) const
{
	if (alt>10000)
	{
		return zfar*0.0001;
	}
	else if (alt>150)
	{
		return 20;
	}
	else if (alt>3)
	{
		return 0.5;
	}
	else
	{
		return 0.25;
	}
}
