#ifndef CLIPPLANECULLCALLBACK_H
#define CLIPPLANECULLCALLBACK_H

#include "ByyIG.h"
#include "ByyChannel.h"
#include "terrain.h"

class ByyClampProjectionMatrixCallback: public osg::CullSettings::ClampProjectionMatrixCallback 
{
public:
	ByyClampProjectionMatrixCallback(osg::Camera *camera,ByyIG& ig,ByyChannel& channel);
	bool  clampProjectionMatrixImplementation (osg::Matrixf &projection, double &znear, double &zfar) const; 
	bool  clampProjectionMatrixImplementation (osg::Matrixd &projection, double &znear, double &zfar) const;

protected:
	double calculateFarClip (double alt,double zfar)const;
	double calculateNearClip (double alt, double zfar)const;

protected:
	template<class matrix_type>
	bool _clampProjectionMatrix(matrix_type& projection, double& znear, double& zfar) const
	{
		static osg::EllipsoidModel emm;

		static osg::Vec3f eye, center1, up;
		static double latitude,longitude,height;

		myCamera->getViewMatrixAsLookAt(eye,center1,up);

		emm.convertXYZToLatLongHeight(eye.x(),eye.y(),eye.z(),latitude,longitude,height);

		osgEarth::MapNode* mapNode=myIG.terrain()->mapNode();

		if (mapNode)
		{
			double  out_heightAboveMSL=0;

			if (mapNode->getTerrain()->getHeight(mapNode->getMapSRS(),osg::RadiansToDegrees(longitude),
				osg::RadiansToDegrees(latitude),&out_heightAboveMSL))
			{
				if (out_heightAboveMSL>0)
				{
					height-=out_heightAboveMSL;
				}
			}
		}

		zfar=calculateFarClip(height,zfar);
		znear=calculateNearClip(height,zfar);

		osgEarth::Util::EarthManipulator *em=static_cast<osgEarth::Util::EarthManipulator*>(myChannel.view()->getCameraManipulator());

		if (height>=150&&em&&em->getTetherNode())
		{
			osg::MatrixList ml=em->getTetherNode()->getWorldMatrices();

			if (ml.size()>0)
			{
				double distance=(osg::Vec3d(0,0,0)*ml[0]-eye).length();

				double tetherNear=calculateNearClip(distance,zfar);

				if (tetherNear<znear)
					znear=tetherNear;
			}
		}

		/*if (height>10000)
		{
			znear=zfar*0.0001;
		}
		else if (height>150)
		{
			znear=20;
			zfar=200000;
		}
		else if (height>3)
		{
			znear=0.5;
			zfar=200000;
		}
		else
		{
			znear=0.25;
			zfar=70000;
		}*/

		double trans_near_plane = (-znear*projection(2,2)+projection(3,2))/(-znear*projection(2,3)+projection(3,3));
		double trans_far_plane = (-zfar*projection(2,2)+projection(3,2))/(-zfar*projection(2,3)+projection(3,3));

		double ratio = fabs(2.0/(trans_near_plane-trans_far_plane));
		double center = -(trans_near_plane+trans_far_plane)/2.0;

		projection.postMult(osg::Matrix(1.0f,0.0f,0.0f,0.0f,
			0.0f,1.0f,0.0f,0.0f,
			0.0f,0.0f,ratio,0.0f,
			0.0f,0.0f,center*ratio,1.0f));

		return true;
	}

protected:
	ByyIG& myIG;
	ByyChannel& myChannel;
	osg::Camera *myCamera;
};

class ByyClipPlaneCullCallback :public osg::NodeCallback
{
public:
	ByyClipPlaneCullCallback();
	~ByyClipPlaneCullCallback();

	void operator()(osg::Node* node, osg::NodeVisitor* nv);

private:
	const osgEarth::SpatialReference* mySrs;
};

#endif // CLIPPLANECULLCALLBACK_H
