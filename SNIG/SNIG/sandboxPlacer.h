#ifndef SANDBOXPLACER_H
#define SANDBOXPLACER_H

#include <QObject>
#include <osgEarthAnnotation/GeoPositionNode>
#include "ByyEventProcessor.h"

class ByySandBoxPlacer :public QObject, public ByyEventProcessor
{
	Q_OBJECT

public:
	ByySandBoxPlacer(osgEarth::Annotation::GeoPositionNode* anno);
	~ByySandBoxPlacer();

	bool processOsgEvent(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);

	osgEarth::Annotation::GeoPositionNode* getNode(){return d_annoNode;}

signals:
	void finish(int);

private:
	osg::ref_ptr<osgEarth::Annotation::GeoPositionNode> d_annoNode;
	osgEarth::GeoPoint d_startPos;
	
};

#endif // SANDBOXPLACER_H
