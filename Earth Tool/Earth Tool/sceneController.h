#ifndef SCENECONTROLLER_H
#define SCENECONTROLLER_H

#include <QObject>
#include <osgViewer/Viewer>
#include <osgEarth/MapNode>
#include <osgEarth/Viewpoint>

class QString;

class ByySceneController :public QObject
{
	Q_OBJECT
public:
	ByySceneController();
	~ByySceneController();
	void newMap();
	void clear();
	bool readFile(const QString &fileName);
	bool writeFile(const QString &fileName);

	osg::ref_ptr<osgViewer::Viewer> getViewer();
	osgEarth::MapNode* getMapNode();
	osg::ref_ptr<osgEarth::Map> getMap();
	osg::ref_ptr<osg::Group> getRootNode();
	osgEarth::Viewpoint getViewpoint();

	void addChild(osg::Node* node);
	void removeChild(osg::Node* node);

public slots: 
	void changeViewpoint(osgEarth::Viewpoint& viewpoint);
	void changeViewpoint(osg::Vec3d& v);

private:
	osg::ref_ptr<osgViewer::Viewer> viewer;
	osg::ref_ptr<osg::Group> root;
	osg::ref_ptr<osg::Node> earthNode;
	osg::ref_ptr<osgEarth::MapNode> mapNode;
	osg::ref_ptr<osgEarth::Map> _map;
};

#endif // SCENECONTROLLER_H
