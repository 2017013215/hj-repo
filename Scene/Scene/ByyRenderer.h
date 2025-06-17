#ifndef SCENECONTROLLER_H
#define SCENECONTROLLER_H


#include "scene_global.h"

namespace osgText
{
	class Font;
}

/*
namespace osgEarth
{
	namespace Util
	{
		namespace Controls
		{
			class Control;
			class ControlCanvas;
		}
	}
}*/

class SCENE_EXPORT ByyOsgRenderer : public QObject
{
	Q_OBJECT

public:
	ByyOsgRenderer(QObject *parent=NULL);
	~ByyOsgRenderer();

	void initiate();

	enum RootDomain{SceneRoot,Environment,Terrain,Entity,ControlObject,Prop,End};

	osg::Group* getSceneRoot();
	osg::Group* getEnvironmentRoot();
	osg::Group* getTerrainRoot();
	osg::Group* getEntityRoot();
	osg::Group* getControlObjectRoot();
	osg::Group* getPropRoot();

	void setEnvironmentRootNode(osg::Group* root);

	void addNodeToRoot(osg::Node* node,RootDomain rd);
	void removeNodeFromRoot(osg::Node* node,RootDomain rd);

	//void addOverlayControl(osgEarth::Util::Controls::Control *control);

	bool needSave();

protected:
	osg::Group* domainToRoot(RootDomain rd);

protected:
	osg::ref_ptr<osg::Group> mySceneRoot;
	osg::ref_ptr<osg::Group> myEnvironmentRoot;
	osg::ref_ptr<osg::Group> myTerrainRoot;
	osg::ref_ptr<osg::Group> myEntityRoot;
	osg::ref_ptr<osg::Group> myControlObjectRoot;
	osg::ref_ptr<osg::Group> myPropRoot;
	//osg::ref_ptr<osgEarth::Util::Controls::ControlCanvas> myCanvas;
 };

#endif // SCENECONTROLLER_H
