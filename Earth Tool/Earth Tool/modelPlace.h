#ifndef PLACEMODELEVENTHANDLE_H
#define PLACEMODELEVENTHANDLE_H

#include <osgGA/GUIEventHandler>
#include <osgEarthAnnotation/ModelNode>
#include <QObject>
#include <osgViewer/View>
#include <osgEarth/ElevationQuery>

class ByySceneController;

class ByyModelPlace : public QObject,public osgGA::GUIEventHandler
{
	Q_OBJECT
public:
	ByyModelPlace(const QString& modelfile,ByySceneController* con);
	~ByyModelPlace();

	QString getModelFileName();

protected:
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);
	void update( float x, float y, osgViewer::View* view );

signals:
	void placePosition( const osgEarth::GeoPoint&);
	void placeFinished();

private:
	ByySceneController *sceneController;
	osg::ref_ptr<osg::Node> _model;
	osg::ref_ptr<osgEarth::Annotation::ModelNode> _modelNode;
	osg::ref_ptr<osgEarth::MapNode> _mapNode;
	osg::ref_ptr<osg::Group> _root;
	const osgEarth::Terrain*   _terrain;
	osgEarth::ElevationQuery   _query;
	osgEarth::GeoPoint mapPoint;
	QString _modelFileName;
	bool draged;

	osg::Vec3d world;
};

#endif // PLACEMODELEVENTHANDLE_H
