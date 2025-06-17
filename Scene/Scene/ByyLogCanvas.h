#ifndef BYYLOGCANVAS_H
#define BYYLOGCANVAS_H

#include <QObject>
#include <osgEarthUtil/Controls>
#include "scene_global.h"

class ByyChannel;

class SCENE_EXPORT ByyLogCanvas : public QObject
{
	Q_OBJECT

public:
	ByyLogCanvas(ByyChannel* channel,QObject *parent=0);
	~ByyLogCanvas();

	void setSource(QObject* sender,const char* signal);

	void setVisible(bool visible);

protected slots:
	void appendLog(double simTime,QString log);

private:
	osg::ref_ptr<osgEarth::Util::Controls::LabelControl> myOutputControl;

	QStringList myLogList;
};

#endif // BYYLOGCANVAS_H
