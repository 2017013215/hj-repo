#ifndef MODELPICKING_H
#define MODELPICKING_H

#include <QObject>
#include <osgGA/GUIEventHandler>
#include <osgUtil/LineSegmentIntersector>
#include <QList>

class ByyModel;

class ByyModelPicking :public QObject , public osgGA::GUIEventHandler
{
	Q_OBJECT
public:
	ByyModelPicking(QList<ByyModel*> *ml);
	~ByyModelPicking();

	virtual bool  handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);

signals:
	void currentModelChanged(int);

private:
	osgUtil::LineSegmentIntersector::Intersections intersections;
	QList<ByyModel*> *modelList;
};

#endif // MODELPICKING_H
