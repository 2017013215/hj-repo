#ifndef ROADPICK_H
#define ROADPICK_H

#include <QObject>
#include <osgGA/GUIEventHandler>
#include <osgUtil/LineSegmentIntersector>

class ByyRoadPick : public QObject ,public osgGA::GUIEventHandler
{
	Q_OBJECT

public:
	ByyRoadPick(osg::Group *root);
	~ByyRoadPick();

	virtual bool  handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

signals:
	void currentRoadChanged(int);

private:
	osgUtil::LineSegmentIntersector::Intersections intersections;
	osg::observer_ptr<osg::Group> d_root;
	
};

#endif // ROADPICK_H
