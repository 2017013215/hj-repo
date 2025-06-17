#ifndef BYYELLIPSOIDARCMODEL_H
#define BYYELLIPSOIDARCMODEL_H

#include <osg/Group>

class ByyEllipsoidArcModel : public osg::Group
{
public:
	ByyEllipsoidArcModel(float azCenter,float azSweep,
		float elCenter,float elSweep,double radius,
		const osg::Vec4& insideColor=osg::Vec4(1,0,1,0.75),
		const osg::Vec4& outsideColor=osg::Vec4(1,1,1,0.1));

	~ByyEllipsoidArcModel();

	void setAzimuthRange (float center, float sweep);
	void setElevationRange (float center, float sweep);
	void setInsideColor(const osg::Vec4& insideColor);
	void setOutsideColor(const osg::Vec4& outsideColor);
	void setRadius(double radius);

protected:
	void init();

protected:
	float  myAzimuthCenter;
	float  myAzimuthSweep;
	float  myElevationCenter;
	float  myElevationSweep;
	double  myRadius;

	osg::Vec4 myInsideColor;
	osg::Vec4 myOutSideColor;

	float myAngleSegmentMax;

	osg::ref_ptr<osg::MatrixTransform> myGraph;
	
};

#endif // BYYELLIPSOIDARCMODEL_H
