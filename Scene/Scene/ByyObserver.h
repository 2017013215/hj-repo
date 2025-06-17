#ifndef OBSERVER_H
#define OBSERVER_H

#include <osg/ref_ptr>
#include <QObject>
#include <osgEarth/Viewpoint>
#include "scene_global.h"

class ByyEarthManipulator;

class SCENE_EXPORT ByyObserver :public QObject
{
	Q_OBJECT
public:
	ByyObserver();
	~ByyObserver();

	osgGA::CameraManipulator* manip();

	void setViewpoint(const osgEarth::Viewpoint& viewpoint);
	void setHomeViewpoint(const osgEarth::Viewpoint& viewpoint);
	osgEarth::Viewpoint getViewpoint();

	void reset();

	void attach(osg::Node* node);
	osg::Node* attachNode();

public slots:
	void set2D(bool is2d);

protected:
	osg::ref_ptr<ByyEarthManipulator> myManip;

};

SCENE_EXPORT ByyObserver* oberverInstance();

#endif // OBSERVER_H
