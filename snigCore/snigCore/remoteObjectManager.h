#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include "absObjectManager.h"

class ByyEntityType;

class ByyDetectObject;

class SNIGCORE_EXPORT ByyRemoteObjectManager : public ByyAbsObjectManager
{
	Q_OBJECT

public:
	ByyRemoteObjectManager();
	virtual ~ByyRemoteObjectManager();

	ByyEntityObject* findEntity(const QString& name);
	ByyEntityObject* findEntity(int id);
	ByyControlObject* findControlObj(const QString& name);

	QList<ByyDetectObject*>& detectObjects();
	QList<ByyEntityObject*>& entityObjects();

	enum ShowRole{White,Red,Blue};

	void setShowRole(ShowRole sr);

	ShowRole getShowRole();
	void setCanSynEntityEvent(bool isCan);

	template<typename T>
	QList<T> findControlObjects()
	{
		QList<T> result;

		for(int index = 0; index < myRefCtrlObjList.size(); ++index)
		{
			T t=dynamic_cast<T>(myRefCtrlObjList.at(index));

			if (t)
				result.append(t);
		}

		return result;
	}

public slots:
	void appendEntity(ByyEntityObject* entObj);
	void removeEntity(ByyEntityObject* entObj);

	void appendContrlObject(ByyControlObject* obj);
	void removeContrlObject(ByyControlObject* obj);

protected slots:
	void appendDetectObject(ByyDetectObject* obj);
	void removeDetectObject(ByyDetectObject* obj);

signals:
	void showRoleChanged(int);

	void detectAppend(ByyDetectObject*);
	void detectRemoved(ByyDetectObject*);
	void removeEntityObject(int side, const QString& typeName, const QString& objectName);
	void addMissileEntityObject(int side, const QString& typeName, const QString& objectName);

protected:
	QList<ByyEntityObject*>  myRefEntObjList;
	QList<ByyControlObject*> myRefCtrlObjList;
	QList<ByyDetectObject*>  myRefDetectObjList;

	ShowRole myShowRole;
	bool     canSynEntityEvent;
};

#endif // OBJECTMANAGER_H
