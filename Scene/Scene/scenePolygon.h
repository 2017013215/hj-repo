#ifndef SCENEPOLYGON_H
#define SCENEPOLYGON_H

#include <osgEarthAnnotation/FeatureNode>
#include <osgEarthSymbology/Geometry>
#include <osgEarthFeatures/Feature>
#include "sceneControlObject.h"

class ByyPolygon;

class SCENE_EXPORT ByyScenePolygon : public ByySceneControlObject
{
	Q_OBJECT
public:
	ByyScenePolygon(ByyPolygon* pol,ByyIG& ig);
	virtual ~ByyScenePolygon();

	virtual void initiate();

	virtual osg::Vec3d center();

	virtual double radius();

	static ByySceneControlObject* create(ByyControlObject* obj,ByyIG& ig);
	virtual	bool	isLineString(){return false;}

public slots:
	virtual void updatePoints();
	virtual void setForceType(ByyForceType side);
	virtual void setClampToTerrain(bool clamp);

protected:
	virtual ByyVec3dArray getPoints();

protected:
	ByyPolygon* myPolygon;

	osg::ref_ptr<osgEarth::Symbology::Geometry> myPolygonNode;
	osg::ref_ptr<osgEarth::Features::Feature> myFeature;
	osg::ref_ptr<osgEarth::Annotation::FeatureNode> myFeatureNode;
};

#endif // SCENEPOLYGON_H
