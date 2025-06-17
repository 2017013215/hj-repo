#ifndef SCENEWAYPOINT_H
#define SCENEWAYPOINT_H

#include "sceneControlObject.h"

class ByyWaypoint;
class ByyIG;

namespace osgEarth
{
	namespace Annotation
	{
		class PlaceNode;
	}
}

class ByySceneWayPoint : public ByySceneControlObject
{
	Q_OBJECT
public:
	ByySceneWayPoint(ByyWaypoint* wp,ByyIG& ig);
	~ByySceneWayPoint();

	virtual void initiate();

	osg::Vec3d center();

	double radius();

	static ByySceneControlObject* create(ByyControlObject* obj,ByyIG& ig);

public slots:
	void setPosition(const osg::Vec3d& pos);
	void setName(const QString& text);
	void setForceType(ByyForceType side);
	virtual void setClampToTerrain(bool clamp);
	virtual void setCtrlObjLableVisible(bool);
protected:
	ByyWaypoint* myWayPoint;

	osg::ref_ptr<osgEarth::Annotation::PlaceNode> myLocalNode;
};

#endif // SCENEWAYPOINT_H
