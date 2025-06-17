#ifndef ALTITUDECULLINGCALLBACK_H
#define ALTITUDECULLINGCALLBACK_H

#include <osg/NodeCallback>

class ByyAltitudeCullingCallback : public osg::NodeCallback
{
public:
	ByyAltitudeCullingCallback(const osgEarth::SpatialReference *srs,double altitude);
	~ByyAltitudeCullingCallback();

	void operator()(osg::Node* node, osg::NodeVisitor* nv);

private:
	double _altitude;
	bool _visible;
	osg::Vec3d _prevEye;

	osg::ref_ptr< const osgEarth::SpatialReference > _srs;
};

#endif // ALTITUDECULLINGCALLBACK_H
