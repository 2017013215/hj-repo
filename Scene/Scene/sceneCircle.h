#ifndef SCENECIRCLE_H
#define SCENECIRCLE_H

#include "sceneControlObject.h"

class ByyCircle;

namespace osgEarth
{
	namespace Annotation
	{
		class CircleNode;
	}
}

class ByySceneCircle : public ByySceneControlObject
{
	Q_OBJECT
public:
	ByySceneCircle(ByyCircle *ci,ByyIG& ig);
	~ByySceneCircle();

	virtual void initiate();

	virtual void update();

	osg::Vec3d center();

	double radius();

	static ByySceneControlObject* create(ByyControlObject* obj,ByyIG& ig);
	
public slots:
	void setPosition(const osg::Vec3d& pos);
	void setRadius(double r);
	virtual void setForceType(ByyForceType side);
	virtual void setClampToTerrain(bool clamp);

protected:
	ByyCircle *myCircle;

	osg::ref_ptr<osgEarth::Annotation::CircleNode> myCircleNode;
};

#endif // SCENECIRCLE_H
