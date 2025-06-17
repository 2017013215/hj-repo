#ifndef SCENEELLIPSE_H
#define SCENEELLIPSE_H

#include "sceneControlObject.h"

class ByyEllipse;

namespace osgEarth
{
	namespace Annotation
	{
		class EllipseNode; 
	}
}

class ByySceneEllipse : public ByySceneControlObject
{
	Q_OBJECT
public:
	ByySceneEllipse(ByyEllipse* el,ByyIG& ig);
	~ByySceneEllipse();

	virtual void initiate();

	osg::Vec3d center();

	double radius();

	static ByySceneControlObject* create(ByyControlObject* obj,ByyIG& ig);

public slots:
	void setPosition(const osg::Vec3d& pos);
	void setMajorSemiAxis(double val);
	void setMinorSemiAxis(double val);
	void setRotationAngle(double val);
	virtual void setForceType(ByyForceType side);
	virtual void setClampToTerrain(bool clamp);

private:
	ByyEllipse* myEllipse;
	osg::ref_ptr<osgEarth::Annotation::EllipseNode> myEllipseNode;
};

#endif // SCENEELLIPSE_H
