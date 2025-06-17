#ifndef SCENEPHASELINE_H
#define SCENEPHASELINE_H

#include "sceneControlObject.h"

class ByyPhaseLine;

class ByyScenePhaseLine : public ByySceneControlObject
{
	Q_OBJECT
public:
	ByyScenePhaseLine(ByyPhaseLine* pl,ByyIG& ig);
	~ByyScenePhaseLine();

	virtual void initiate();

	osg::Vec3d center();

	double radius();

	static ByySceneControlObject* create(ByyControlObject* obj,ByyIG& ig);

public slots:
	void setStart(const osg::Vec3d& p);
	void setEnd(const osg::Vec3d& p);
	virtual void setForceType(ByyForceType side);
	virtual void setClampToTerrain(bool clamp);

private:
	ByyPhaseLine* myPaseline;

	osg::ref_ptr<osgEarth::Symbology::LineString> myLineString;
	osg::ref_ptr<osgEarth::Features::Feature> myFeature;
	osg::ref_ptr<osgEarth::Annotation::FeatureNode> myFeatureNode;
};

#endif // SCENEPHASELINE_H
