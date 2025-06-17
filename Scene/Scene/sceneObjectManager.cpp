#include "StdAfx.h"
#include "sceneObjectManager.h"
#include "sceneEntity.h"
#include "ByyRenderer.h"
#include "ByyCore.h"
#include "remoteObjectManager.h"
#include "localObjectManager.h"
#include "ByyIG.h"
#include "sceneControlObject.h"
#include "ByyObserver.h"
#include "entityInfoWidget.h"
#include "entityObject.h"
#include "entitySettings.h"
#include "observerSettings.h"
#include "ByyControlObject.h"
#include "ByyInputController.h"
#include "ByyDetectObject.h"
#include "ByySceneDetect.h"

ByySceneObjectManager::ByySceneObjectManager( ByyIG& ig )
	:myIG(ig)
{
	myCtrlObjGroup=new osg::Group;
	myEntityGroup=new osg::Group;

	//myEntityGroup->addEventCallback(new ByyObjEventCallback(this));

	myIG.renderer()->addNodeToRoot(myCtrlObjGroup,ByyOsgRenderer::ControlObject);
	myIG.renderer()->addNodeToRoot(myEntityGroup,ByyOsgRenderer::Entity);

	ByyRemoteObjectManager& remoteObjMgr=ig.core().remoteObjectManager();
	ByyLocalObjectManager& localObjMgr=ig.core().localObjectManager();

	connect(&remoteObjMgr,SIGNAL(entityAppend(ByyEntityObject*)),&localObjMgr,SLOT(onDiscoverRemoteEntity(ByyEntityObject*)));
	connect(&remoteObjMgr,SIGNAL(contrlObjectAppend(ByyControlObject*)),&localObjMgr,SLOT(onDiscoverRemoteCtrlObj(ByyControlObject*)));


	connect(&remoteObjMgr,SIGNAL(entityAppend(ByyEntityObject*)),this,SLOT(appendEntObject(ByyEntityObject*)));
	connect(&remoteObjMgr,SIGNAL(entityRemoved(ByyEntityObject*)),this,SLOT(removeEntObject(ByyEntityObject*)));

	connect(&remoteObjMgr,SIGNAL(contrlObjectAppend(ByyControlObject*)),this,SLOT(appendContrlObject(ByyControlObject*)));
	connect(&remoteObjMgr,SIGNAL(contrlObjectRemoved(ByyControlObject*)),this,SLOT(removeContrlObject(ByyControlObject*)));

	connect(&localObjMgr,SIGNAL(entityAppend(ByyEntityObject*)),this,SLOT(appendEntObject(ByyEntityObject*)));
	connect(&localObjMgr,SIGNAL(entityRemoved(ByyEntityObject*)),this,SLOT(removeEntObject(ByyEntityObject*)));

	connect(&localObjMgr,SIGNAL(contrlObjectAppend(ByyControlObject*)),this,SLOT(appendContrlObject(ByyControlObject*)));
	connect(&localObjMgr,SIGNAL(contrlObjectRemoved(ByyControlObject*)),this,SLOT(removeContrlObject(ByyControlObject*)));


	connect(&remoteObjMgr,SIGNAL(detectAppend(ByyDetectObject*)),this,SLOT(appendDetectObject(ByyDetectObject*)));
	connect(&remoteObjMgr,SIGNAL(detectRemoved(ByyDetectObject*)),this,SLOT(removeDetectObject(ByyDetectObject*)));

	connect(&remoteObjMgr,SIGNAL(showRoleChanged(int)),this,SLOT(showRoleChanged(int)));
	//connect(&timer,SIGNAL(timeout()),SLOT(update()));


	setControlObjectVisible(ByyObserverSettings::instance().ctrlObjVisible());
	connect(&ByyObserverSettings::instance(),SIGNAL(ctrlObjVisibleChanged(bool)),this,SLOT(setControlObjectVisible(bool)));

	setEntityVisible(ByyObserverSettings::instance().entityVisible());
	connect(&ByyObserverSettings::instance(),SIGNAL(entityVisibleChanged(bool)),this,SLOT(setEntityVisible(bool)));

	//timer.start(200);
}

ByySceneObjectManager::~ByySceneObjectManager()
{

}

void ByySceneObjectManager::appendEntObject( ByyEntityObject* entObj )
{
	ByySceneEntity *sceneEntity=new ByySceneEntity(entObj,myIG);
	sceneEntity->initiate();

	myEntityGroup->addChild(sceneEntity->root());

	mySceneEntityMap.insert(entObj,SceneEntitySharedPtr(sceneEntity));
}

void ByySceneObjectManager::removeEntObject( ByyEntityObject* entObj )
{
	SceneEntityMap::iterator it=mySceneEntityMap.find(entObj);

	if (it==mySceneEntityMap.end())
		return;

	if ((*it)->modelNode()==myIG.inputController().currentObserver()->attachNode())
	{
		myIG.inputController().currentObserver()->attach(0);
	}

	myEntityGroup->removeChild((*it)->root());

	mySceneEntityMap.erase(it);
}

void ByySceneObjectManager::appendDetectObject( ByyDetectObject* obj )
{
	ByySceneDetectObject *sceneDetect=new ByySceneDetectObject(obj,myIG);
	sceneDetect->initiate();

	myEntityGroup->addChild(sceneDetect->root());

	mySceneDetectObjMap.insert(obj,SceneDetectObjSharedPtr(sceneDetect));
}

void ByySceneObjectManager::removeDetectObject( ByyDetectObject* obj )
{
	SceneDetectObjMap::iterator it=mySceneDetectObjMap.find(obj);

	if (it==mySceneDetectObjMap.end())
		return;

	if ((*it)->modelNode()==myIG.inputController().currentObserver()->attachNode())
	{
		myIG.inputController().currentObserver()->attach(0);
	}

	myEntityGroup->removeChild((*it)->root());

	mySceneDetectObjMap.erase(it);
}

void ByySceneObjectManager::showRoleChanged( int rule )
{
	QList<ByyDetectObject*> waitRemovedList=mySceneDetectObjMap.keys();

	for (int i=0;i!=waitRemovedList.size();++i)
	{
		removeDetectObject(waitRemovedList[i]);
	}

	ByyRemoteObjectManager& remoteObjMgr=myIG.core().remoteObjectManager();

	QList<ByyDetectObject*>& allDetectList=remoteObjMgr.detectObjects();

	for (int i=0;i!=allDetectList.size();++i)
	{
		ByyDetectObject* obj=allDetectList[i];

		if ((rule==ByyRemoteObjectManager::Red&&obj->forceType()!=ByyForceFriendly)||
			(rule==ByyRemoteObjectManager::Blue&&obj->forceType()!=ByyForceOpposing))
		{
			appendDetectObject(obj);
		}
	}
}

void ByySceneObjectManager::update()
{
	/*SceneEntMapIter entIt(mySceneEntityMap);
	while(entIt.hasNext())
	{
		entIt.next();

		entIt.value()->update();
	}

	SceneCtrlObjMapIter ctrlIt(mySceneCtrlObjMap);

	while(ctrlIt.hasNext())
	{
		ctrlIt.next();

		ctrlIt.value()->update();
	}*/
}

void ByySceneObjectManager::appendContrlObject( ByyControlObject* obj )
{
	ByySceneControlObjectCreatorList::iterator it=mySceneCtrlObjCreators.find(obj->type());

	if (it!=mySceneCtrlObjCreators.end())
	{
		ByySceneControlObject* sceneCtrlObj=(*it.value())(obj,myIG);

		sceneCtrlObj->initiate();

		myCtrlObjGroup->addChild(sceneCtrlObj->root());
		mySceneCtrlObjMap.insert(obj,SceneCtrlObjSharedPtr(sceneCtrlObj));
	}
}

void ByySceneObjectManager::removeContrlObject( ByyControlObject* obj )
{
	SceneCtrlObjMap::iterator it=mySceneCtrlObjMap.find(obj);

	if (it==mySceneCtrlObjMap.end())
		return;

	myCtrlObjGroup->removeChild((*it)->root());

	mySceneCtrlObjMap.erase(it);
}

ByySceneEntity* ByySceneObjectManager::findEntity( ByyEntityObject* obj )
{
	if (mySceneEntityMap.contains(obj))
		return mySceneEntityMap[obj].data();
	else
		return NULL;
}

ByySceneControlObject* ByySceneObjectManager::findCtrlObj( ByyControlObject* obj )
{
	if (mySceneCtrlObjMap.contains(obj))
		return mySceneCtrlObjMap[obj].data();
	else
		return NULL;
}

void ByySceneObjectManager::setControlObjectVisible( bool visible )
{
	myCtrlObjGroup->setNodeMask(visible);
}

void ByySceneObjectManager::setEntityVisible( bool visible )
{
	myEntityGroup->setNodeMask(visible);
}

void ByySceneObjectManager::addCreator( const QString& type,ByySceneControlObjectCreator creator )
{
	mySceneCtrlObjCreators[type]=creator;
}

osg::Group* ByySceneObjectManager::entityGroup()
{
	return myEntityGroup;
}

osg::Group* ByySceneObjectManager::controlObjectGroup()
{
	return myCtrlObjGroup;
}

ByyObjEventCallback::ByyObjEventCallback( ByySceneObjectManager* obj )
	:objManager(obj)
{

}

void ByyObjEventCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
	if (ByyEntitySettings::instance().entityInfoEnabled())
	{
		osgGA::EventVisitor* ev = static_cast<osgGA::EventVisitor*>(nv);
		osgGA::EventQueue::Events& events = ev->getEvents();

		osgViewer::View* view = static_cast<osgViewer::View*>(ev->getActionAdapter());

		bool _continue=true;

		for( osgGA::EventQueue::Events::const_iterator e = events.begin(); _continue&&e != events.end(); ++e )
		{
			osgGA::GUIEventAdapter* ea = dynamic_cast<osgGA::GUIEventAdapter*>(e->get());

			if (ea&& ea->getEventType() == osgGA::GUIEventAdapter::MOVE)
			{
				osgEarth::IntersectionPicker picker( view, node );

				osgEarth::IntersectionPicker::Hits hits;
				if (picker.pick( ea->getX(), ea->getY(), hits ))
				{
					for( osgEarth::IntersectionPicker::Hits::const_iterator h = hits.begin(); _continue&&h != hits.end(); ++h )
					{
						for( osg::NodePath::const_reverse_iterator i = h->nodePath.rbegin(); _continue&&i != h->nodePath.rend(); ++i )
						{
							ByySceneObjectManager::SceneEntMapIter it(objManager->mySceneEntityMap);

							while(_continue&&it.hasNext())
							{
								it.next();

								ByySceneEntity *entity=it.value().data();
								ByyEntityObject *entObj=entity->entityObject();

								if (entity->root()==(*i))
								{
									printf("%s\n",entObj->name().toLocal8Bit().data());
									/*myAttachedEntity=it.value().toWeakRef();
									ByyEntityInfoWidget::instance(objManager->myIG).setVisible(true);
									update();
									_continue=false;*/
								}

							}
						}
					}
				}

				if (_continue)
				{
					myAttachedEntity.clear();
					ByyEntityInfoWidget::instance(objManager->myIG).setVisible(false);
				}
			}
			else if ((!myAttachedEntity.isNull())&&ea&& ea->getEventType() == osgGA::GUIEventAdapter::FRAME)
			{
				static int frame=0;

				if (++frame==20)
				{
					update();

					frame=0;
				}
				
			}
		}
	}
	

	traverse(node,nv);
}

void ByyObjEventCallback::update()
{
	osg::Vec4 color(1,1,1,0.5);
	ByyEntityObject* entObj=myAttachedEntity.data()->entityObject();

	switch(entObj->forceType())
	{
	case ByyForceFriendly:
		color=osg::Vec4(1,0,0,0.7);
		break;
	case ByyForceOpposing:
		color=osg::Vec4(0,0,1,0.7);
		break;
	case ByyForceNeutral:
		color=osg::Vec4(0,1,0,0.7);
		break;
	default:
		color=osg::Vec4(1,1,1,0.7);

	}
	ByyEntityInfoWidget::instance(objManager->myIG).setBackColor(color);
	ByyEntityInfoWidget::instance(objManager->myIG).setPosition(myAttachedEntity.data()->worldPosition());

	QString text=QString::fromLocal8Bit("名称：        %1\n经度：        %2\n纬度：        %3\n高度：        %4\n速度：        %5")
		.arg(entObj->name())
		.arg(entObj->position().x())
		.arg(entObj->position().y())
		.arg(entObj->position().z())
		.arg(entObj->speed());

	ByyEntityInfoWidget::instance(objManager->myIG).setText(text);
}
