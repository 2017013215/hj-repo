#include "localObjectManager.h"
#include "localEntity.h"
#include "ByyControlObject.h"
#include "../snigCore/coreUtils.h"
#include "ByyUserOperateLog.h"
#include "taskManager.h"
#include "task.h"

ByyLocalObjectManager::ByyLocalObjectManager(QObject *parent)
	: ByyAbsObjectManager(parent)
	,myNextID(5000)
	,myVisible(true)
{

}

ByyLocalObjectManager::~ByyLocalObjectManager()
{
	qDeleteAll(myPubEntityObjects);
}

ByyEntityObject* ByyLocalObjectManager::createEntity( const ByyEntityType& entType, ParameterList& param,const QString& type,ByyForceType side ,ByyDescribeEntry& descEntry)
{
	ByyLocalEntity* entObj=new ByyLocalEntity;
	entObj->setParameters(param);

	entObj->setDescribeEntry(descEntry);

	entObj->setEntityType(entType);
	entObj->setLevel(1);
	entObj->setName(nextName(type));

	entObj->setForceType(side);
	entObj->setId(myNextID++);

	entObj->setVisible(myVisible);

	myPubEntityObjects.insert(entObj->id(),entObj);

	emit entityAppend(entObj);

	connect(entObj,SIGNAL(nameChanged1(const QString&,const QString&)),this,SLOT(onEntityNameChanged(const QString&,const QString&)));

	QString info=QString(CNTR("创建对象\t%1\t%2")).arg(type).arg(entObj->name());
	ByyUserOperateLog::instance().addLog(info);

	return entObj;
}

ByyEntityObject* ByyLocalObjectManager::createEntity( const ByyEntityType& entType, ParameterList& param,const ByyKeyValueList& keyValues ,ByyDescribeEntry& descEntry)
{
	ByyLocalEntity* entObj=new ByyLocalEntity;
	entObj->setParameters(param);
	entObj->setEntityType(entType);
	entObj->setDescribeEntry(descEntry);
	
	entObj->setId(myNextID++);
	entObj->setVisible(myVisible);

	entObj->fromConfig(keyValues);

	myPubEntityObjects.insert(entObj->id(),entObj);

	emit entityAppend(entObj);

	connect(entObj,SIGNAL(nameChanged1(const QString&,const QString&)),this,SLOT(onEntityNameChanged(const QString&,const QString&)));

	return entObj;
}

void ByyLocalObjectManager::removeEntity( ByyEntityObject* entObj )
{
	if (entObj->isLocal())
	{
		QString info=QString(CNTR("删除对象\t%1")).arg(entObj->name());
		ByyUserOperateLog::instance().addLog(info);

		EntityObjectMap::iterator it=myPubEntityObjects.find(entObj->id());

		if (it!=myPubEntityObjects.end()&&it.value()==entObj)
		{
			emit entityRemoved(entObj);

			delete *it;
			myPubEntityObjects.erase(it);
		}
	}
}

void ByyLocalObjectManager::removeEntity( const QString& name )
{
	EntityObjectMap::iterator it=myPubEntityObjects.begin();

	for (;it!=myPubEntityObjects.end();++it)
	{
		if (it.value()->name()==name)
		{
			removeEntity(it.value());
			break;
		}
	}
}

ByyEntityObject* ByyLocalObjectManager::getObjectByGlobalId(int objectId)
{
	ByyEntityObject* ret = nullptr;
	QList<ByyEntityObject*> objects= myPubEntityObjects.values();
	for(int i = 0; i < objects.size(); i++)
	{
		ByyEntityObject* tmpObject = objects.at(i);
		if(tmpObject && (objectId == tmpObject->objectId()))
		{
			ret =tmpObject;
			break;
		}
	}
	return ret;
}

void ByyLocalObjectManager::clear()
{
	while(myPubEntityObjects.size()>0)
		removeEntity(myPubEntityObjects.begin().value());

	while(myPubControlObjects.size()>0)
		removeControlObject(myPubControlObjects.begin().value());
}

ByyLocalObjectManager::EntityObjectMap& ByyLocalObjectManager::entityObjects()
{
	return myPubEntityObjects;
}

const ByyLocalObjectManager::EntityObjectMap& ByyLocalObjectManager::entityObjects() const
{
	return myPubEntityObjects;
}

void ByyLocalObjectManager::appendControlObject( ByyControlObject *obj,ByyForceType ft )
{
	obj->setLocal(true);
	obj->setForceType(ft);
	obj->setId(myNextID++);
	obj->setVisible(myVisible);
	if (obj->name().isEmpty())
	{
		obj->setName(nextName(obj->type()));
	}

	myPubControlObjects.insert(obj->id(),obj);

	connect(obj,SIGNAL(nameChangedWithOld(QString,QString)),this,SLOT(onCtrlObjNameChanged(QString,QString)));

	emit contrlObjectAppend(obj);
}

void ByyLocalObjectManager::removeControlObject( ByyControlObject *obj )
{
	if (obj->isLocal())
	{
		ControlObjectMap::iterator it=myPubControlObjects.find(obj->id());

		if (it!=myPubControlObjects.end()&&it.value()==obj)
		{
			emit contrlObjectRemoved(obj);

			delete *it;
			myPubControlObjects.erase(it);
		}
	}
}

void ByyLocalObjectManager::removeControlObject( const QString& name )
{
	ControlObjectMap::iterator it=myPubControlObjects.begin();

	for (;it!=myPubControlObjects.end();++it)
	{
		if (it.value()->name()==name)
		{
			removeControlObject(it.value());
			break;
		}
	}
}

void ByyLocalObjectManager::onCtrlObjNameChanged(QString newName,QString oldName)
{
	EntityObjectMap::iterator it=myPubEntityObjects.begin();

	for (;it!=myPubEntityObjects.end();++it)
	{
		ByyEntityObject* ety=*it;
		foreach(ByyTask* task,ety->taskManager().taskList())
		{
			task->notifyCtrlObjNameChanged(oldName,newName);
		}
	}
}

ByyLocalObjectManager::ControlObjectMap& ByyLocalObjectManager::controlObjects()
{
	return myPubControlObjects;
}

const ByyLocalObjectManager::ControlObjectMap& ByyLocalObjectManager::controlObjects() const
{
	return myPubControlObjects;
}

QString ByyLocalObjectManager::nextName( const QString& objname )
{
	QString obj=objname.trimmed();
	if (!myNameCounts.contains(obj))
	{
		myNameCounts[obj]=0;
	}

	QString name;

	do 
	{
		name= obj+" "+QString::number(++myNameCounts[obj]);

	} while (findEntity(name));

	return name;
}

ByyControlObject* ByyLocalObjectManager::findEntity_ctrlobj( const QString& name )
{
	ControlObjectMap::iterator it=myPubControlObjects.begin();

	for (;it!=myPubControlObjects.end();++it)
	{
		if (it.value()->name()==name)
		{
			return it.value();
		}
	}
	return 0;
}

QString ByyLocalObjectManager::nextName_ctrlobj( const QString& obj )
{
	if (!myNameCounts_ctrlobj.contains(obj))
	{
		myNameCounts_ctrlobj[obj]=0;
	}

	QString name;

	do 
	{
		name= obj+" "+QString::number(++myNameCounts_ctrlobj[obj]);

	} while (findEntity_ctrlobj(name));

	return name;
}

void ByyLocalObjectManager::setVisible( bool b )
{
	if (b!=myVisible)
	{
		myVisible=b;

		EntityObjectMap::iterator it=myPubEntityObjects.begin();

		for (;it!=myPubEntityObjects.end();++it)
		{
			it.value()->setVisible(myVisible);
		}

		ControlObjectMap::iterator it1=myPubControlObjects.begin();

		for (;it1!=myPubControlObjects.end();++it1)
		{
			it1.value()->setVisible(myVisible);
		}
	}
}

ByyEntityObject* ByyLocalObjectManager::findEntity( const QString& name )
{
	EntityObjectMap::iterator it=myPubEntityObjects.begin();

	for (;it!=myPubEntityObjects.end();++it)
	{
		if (it.value()->name()==name)
		{
			return it.value();
		}
	}
	return 0;
}

void ByyLocalObjectManager::resetNameHash()
{
	myNameCounts.clear();
	myNameCounts_ctrlobj.clear();
}

void ByyLocalObjectManager::resetIdHash()
{
	myNextID=0;
}

void ByyLocalObjectManager::onEntityNameChanged( const QString& oldName,const QString& newName )
{
	ByyLocalEntity* entObj=dynamic_cast<ByyLocalEntity*>(sender());

	if (!entObj)
		return;

	EntityObjectMap::iterator it=myPubEntityObjects.begin();

	QString sp;

	for (;it!=myPubEntityObjects.end();++it)
	{
		sp=it.value()->superiorName();

		if (it.value()->level()-entObj->level()==1&&
			it.value()->superiorName()==oldName)
		{
			it.value()->setSuperiorName(newName);
		}
	}
}

void ByyLocalObjectManager::updateNameHashAfterScnLoaded()
{
	foreach(int id,myPubEntityObjects.keys())
	{
		ByyEntityObject* obj=myPubEntityObjects[id];
		collectNameHash(obj->name());
	}
	foreach(int id,myPubControlObjects.keys())
	{
		ByyControlObject* obj=myPubControlObjects[id];
		collectNameHash(obj->name());
	}
}

void ByyLocalObjectManager::collectNameHash(QString name)
{
	QString numstr="0123456789";
	int numidx=-1;
	for(int i=0;i<name.size();i++)
	{
		QChar c=name.at(i);
		if(numstr.indexOf(c)>=0)
		{
			bool isnum=true;
			for(int j=i+1;j<name.size();j++)
			{
				c=name.at(j);
				if(numstr.indexOf(c)<0)
				{
					isnum=false;
					break;
				}
			}
			if(isnum)
			{
				numidx=i;
				break;
			}
		}
	}
	if(numidx<0)
		return;

	QString prefix=name.left(numidx).trimmed();
	QString countstr=name.right(name.size()-numidx);
	if(prefix.size()>0&&numstr.size()>0)
	{
		if(myNameCounts[prefix]<countstr.toInt())
			myNameCounts[prefix]=countstr.toInt();
	}
}

void ByyLocalObjectManager::onDiscoverRemoteEntity(ByyEntityObject* obj)
{
	collectNameHash(obj->name());
}
void ByyLocalObjectManager::onDiscoverRemoteCtrlObj(ByyControlObject* obj)
{
	collectNameHash(obj->name());
}