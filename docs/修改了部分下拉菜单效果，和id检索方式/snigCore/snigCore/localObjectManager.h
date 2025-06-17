#ifndef LOCALOBJECTMANAGER_H
#define LOCALOBJECTMANAGER_H

#include "absObjectManager.h"
#include <QMap>
#include <QList>
#include "enums.h"

class ByyEntityType;
class ByyKeyValueList;
class ParameterList;
class ByyDescribeEntry;

class SNIGCORE_EXPORT ByyLocalObjectManager : public ByyAbsObjectManager
{
	Q_OBJECT

public:
	ByyLocalObjectManager(QObject *parent=0);
	virtual ~ByyLocalObjectManager();

	ByyEntityObject* createEntity(const ByyEntityType& entType, ParameterList& param,const QString& type,ByyForceType side,ByyDescribeEntry& descEntry);
	ByyEntityObject* createEntity(const ByyEntityType& entType, ParameterList& param,const ByyKeyValueList& keyValues,ByyDescribeEntry& descEntry);

	void removeEntity(ByyEntityObject* entObj);
	void removeEntity(const QString& name);

	ByyEntityObject* getObjectByGlobalId(int objectId);

	typedef QMap<int,ByyEntityObject*> EntityObjectMap;

	EntityObjectMap& entityObjects();
	const EntityObjectMap& entityObjects() const;

	typedef QMap<int,ByyControlObject*> ControlObjectMap;

	ControlObjectMap& controlObjects();
	const ControlObjectMap& controlObjects() const;

	void appendControlObject(ByyControlObject *obj,ByyForceType ft);
	void removeControlObject(ByyControlObject *obj);

	void removeControlObject(const QString& name);

	ByyEntityObject* findEntity(const QString& name);

	void clear();

	QString nextName(const QString& obj);

	//20190727
	ByyControlObject* findEntity_ctrlobj(const QString& name);
	QString nextName_ctrlobj(const QString& obj);

	void resetNameHash();
	void resetIdHash();

	void	updateNameHashAfterScnLoaded();
	void	collectNameHash(QString name);

	template<typename T>
	QList<T> findControlObjects()
	{
		QList<T> result;

		ControlObjectMap::iterator it=myPubControlObjects.begin();

		for(;it!=myPubControlObjects.end();++it)
		{
			T t=dynamic_cast<T>(it.value());

			if (t)
				result.append(t);
		}

		return result;
	}

	void setVisible(bool b);

protected slots:
	void	onEntityNameChanged(const QString& oldName,const QString& newName);
	void	onDiscoverRemoteEntity(ByyEntityObject*);
	void	onDiscoverRemoteCtrlObj(ByyControlObject*);
	void	onCtrlObjNameChanged(QString newName,QString oldName);
protected:
	EntityObjectMap myPubEntityObjects;
	ControlObjectMap myPubControlObjects;

	typedef QMap<QString,int> NameCounts;
	NameCounts myNameCounts;
	NameCounts myNameCounts_ctrlobj;

	int myNextID;

	bool myVisible;
};

#endif // LOCALOBJECTMANAGER_H
