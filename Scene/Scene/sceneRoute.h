#ifndef SCENEROUTE_H
#define SCENEROUTE_H

#include "sceneControlObject.h"

class ByyRoute;

namespace osgEarth
{
	namespace Symbology
	{
		class LineString;
	}

	namespace Annotation
	{
		class FeatureNode;
	}
}

class ByySceneRoute : public ByySceneControlObject
{
	Q_OBJECT
public:
	ByySceneRoute(ByyRoute* route,ByyIG& ig);
	~ByySceneRoute();

	virtual void initiate();

	osg::Vec3d center();

	double radius();

	static ByySceneControlObject* create(ByyControlObject* obj,ByyIG& ig);

public slots:
	void updatePoints();
	virtual void setForceType(ByyForceType side);
	virtual void setClampToTerrain(bool clamp);

private:
	ByyRoute* myRoute;

	osg::ref_ptr<osgEarth::Symbology::LineString> myLineString;
	osg::ref_ptr<osgEarth::Features::Feature> myFeature;
	osg::ref_ptr<osgEarth::Annotation::FeatureNode> myFeatureNode;
};

#endif // SCENEROUTE_H
