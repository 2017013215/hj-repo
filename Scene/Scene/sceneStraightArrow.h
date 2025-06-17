#ifndef SCENESTRAIGHTARROW_H
#define SCENESTRAIGHTARROW_H

#include "sceneControlObject.h"

class ByyStraightArrow;

class ByySceneStraightArrow : public ByySceneControlObject
{
	Q_OBJECT
public:
	ByySceneStraightArrow(ByyStraightArrow* straightArrow,ByyIG& ig);
	~ByySceneStraightArrow();

	virtual void initiate();

	osg::Vec3d center();

	double radius();

	static ByySceneControlObject* create(ByyControlObject* obj,ByyIG& ig);

public slots:
	virtual void updatePoints();
	virtual void setForceType(ByyForceType side);
	virtual void setClampToTerrain(bool clamp);

private:
	ByyStraightArrow* myStraightArrow;

	osg::ref_ptr<osgEarth::Symbology::LineString> myLine;
	osg::ref_ptr<osgEarth::Symbology::Polygon>	  myArrow;
	osg::ref_ptr<osgEarth::Symbology::MultiGeometry> myBody;

	osg::ref_ptr<osgEarth::Features::Feature> myFeature;
	osg::ref_ptr<osgEarth::Annotation::FeatureNode> myFeatureNode;
	
};

#endif // SCENESTRAIGHTARROW_H
