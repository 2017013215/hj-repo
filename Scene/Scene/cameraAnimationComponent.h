#ifndef VIEWPOINTFLY_H
#define VIEWPOINTFLY_H

#include <QObject>

#include "scene_global.h"

#include "animationPath.h"

#include <osgEarthAnnotation/FeatureNode>
#include <osg/AnimationPath>

class ByyIG;

class SCENE_EXPORT ByyCameraAnimationComponent : public QObject
{
	Q_OBJECT
public:
	ByyCameraAnimationComponent(ByyIG& ig);
	~ByyCameraAnimationComponent();

	ByyAnimationPath& addPath(const std::string& filename="");
	void closePath(int index);

	void enablePath(bool enable);
	void enablePoints(bool enable);

	void setMultiplier(double value);

	void enableLoop(bool enable);

	void start();
	void pause();
	void reset();

	bool running(){return d_running;}

	void setCurrentPathPoint(int path,int point);

	ByyAnimationPath& getAnimationPath(int index);
	
	void setAutoFly(bool enable);

	void setAutoFlyScale(float scale);

protected slots:
	void tick();

private:
	void checkPointChange();
	void createPathPointsNode(ByyAnimationPath& animationPath);

signals:
	void currentPointChanged(int path,int point);
	void finish();

private:
	QVector<ByyAnimationPath> animationPathList;

	osg::ref_ptr<osg::Group> myRoot;

	int currentPath;
	int currentPoint;

	double _point_elapse_time;

	double _lastTickTime;

	bool d_running;

	bool d_loop;

	double _time_multiplier;

	osg::ref_ptr<osg::Group> d_pathGroups;
	osg::ref_ptr<osg::Group> d_pointsGroups;

	float myAutoFlyScale;
	bool myAutoFly;

	ByyIG& myIG;
};

#endif // VIEWPOINTFLY_H
