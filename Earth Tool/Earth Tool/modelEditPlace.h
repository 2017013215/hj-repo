#ifndef MODELEDITPLACE_H
#define MODELEDITPLACE_H

#include <QObject>
#include <osgGA/GUIEventHandler>
#include <osgEarthAnnotation/ModelNode>

#include <osgViewer/View>
#include <osgEarth/ElevationQuery>

class ByySceneController;

class ByyModelEditPlace : public QObject ,public osgGA::GUIEventHandler
{
	Q_OBJECT

public:
	ByyModelEditPlace(osgEarth::Annotation::ModelNode* modelnode,ByySceneController* con);
	~ByyModelEditPlace();

	osgEarth::Annotation::ModelNode* getModelNode();

protected:
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);
	void update( float x, float y, osgViewer::View* view );

signals:
	void placeFinished(bool);

private:
	ByySceneController *sceneController;
	osg::ref_ptr<osgEarth::Annotation::ModelNode> _modelNode;
	osg::ref_ptr<osgEarth::MapNode> _mapNode;
	const osgEarth::Terrain*   _terrain;
	osgEarth::ElevationQuery   _query;
	osgEarth::GeoPoint mapPoint;
	bool draged;
	osgEarth::GeoPoint initialGeoPoint;
	osg::Vec3d world;
};

#endif // MODELEDITPLACE_H
