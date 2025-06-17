#ifndef SCENEOBJECTMANAGER_H
#define SCENEOBJECTMANAGER_H

#include <QObject>
#include <QTimer>
#include "scene_global.h"

class ByyEntityObject;
class ByySceneEntity;
class ByyIG;
class ByyControlObject;
class ByySceneControlObject;
class ByyDetectObject;
class ByySceneDetectObject;

typedef ByySceneControlObject* (*ByySceneControlObjectCreator)(ByyControlObject* obj,ByyIG& ig);

class SCENE_EXPORT ByySceneObjectManager : public QObject
{
	Q_OBJECT

public:
	ByySceneObjectManager(ByyIG& ig);
	~ByySceneObjectManager();

	ByySceneEntity* findEntity(ByyEntityObject* obj);
	ByySceneControlObject* findCtrlObj(ByyControlObject* obj);

	void addCreator(const QString& type,ByySceneControlObjectCreator creator);

	osg::Group* entityGroup();
	osg::Group* controlObjectGroup();

public slots:
	void appendEntObject(ByyEntityObject* entObj);
	void removeEntObject(ByyEntityObject* entObj);

	void appendContrlObject(ByyControlObject* obj);
	void removeContrlObject(ByyControlObject* obj);

	void setControlObjectVisible(bool visible);
	void setEntityVisible(bool visible);

	void appendDetectObject(ByyDetectObject* obj);
	void removeDetectObject(ByyDetectObject* obj);

	void showRoleChanged(int rule);

protected slots:
	void update();

protected:
	ByyIG& myIG;

	typedef QSharedPointer<ByySceneEntity> SceneEntitySharedPtr;
	typedef QMap<ByyEntityObject*,SceneEntitySharedPtr> SceneEntityMap;
	typedef QMapIterator<ByyEntityObject*,SceneEntitySharedPtr> SceneEntMapIter;

	SceneEntityMap mySceneEntityMap;

	typedef QSharedPointer<ByySceneControlObject> SceneCtrlObjSharedPtr;
	typedef QMap<ByyControlObject*,SceneCtrlObjSharedPtr> SceneCtrlObjMap;
	typedef QMapIterator<ByyControlObject*,SceneCtrlObjSharedPtr> SceneCtrlObjMapIter;

	SceneCtrlObjMap mySceneCtrlObjMap;

	typedef QSharedPointer<ByySceneDetectObject> SceneDetectObjSharedPtr;
	typedef QMap<ByyDetectObject*,SceneDetectObjSharedPtr> SceneDetectObjMap;
	typedef QMapIterator<ByyDetectObject*,SceneDetectObjSharedPtr> SceneDetectObjMapIter;

	SceneDetectObjMap mySceneDetectObjMap;

	osg::ref_ptr<osg::Group> myEntityGroup;
	osg::ref_ptr<osg::Group> myCtrlObjGroup;

//	QTimer timer;


	typedef QMap<QString,ByySceneControlObjectCreator> ByySceneControlObjectCreatorList;

	ByySceneControlObjectCreatorList mySceneCtrlObjCreators;

	friend class ByyObjEventCallback;
};


class ByyObjEventCallback : public osg::NodeCallback
{
public:
	ByyObjEventCallback(ByySceneObjectManager* obj);
	virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );

protected:
	void update();

	ByySceneObjectManager* objManager;
	QWeakPointer<ByySceneEntity> myAttachedEntity;
};

#endif // SCENEOBJECTMANAGER_H
