#ifndef ANIMATIONPATH_H
#define ANIMATIONPATH_H

#include "scene_global.h"

#include <string>
#include <osg/AnimationPath>

struct MyControlPoint
{
	std::string name;
	double time;
	osg::AnimationPath::ControlPoint ctrlPoints;
};

class SCENE_EXPORT ByyAnimationPath
{
public:
	ByyAnimationPath();
	~ByyAnimationPath();

	bool read(const std::string& filename);
	bool write(const std::string& filename);

	void push_back(const MyControlPoint& cp);
	void remove(int index);

	std::vector<std::string> getNameList();

	QVector<MyControlPoint>& getControlPointList(){return controlPointList;}

	MyControlPoint& getControlPoint(int index);

	double totalTime();

private:
	QVector<MyControlPoint> controlPointList;
	
};

#endif // ANIMATIONPATH_H
