#ifndef BYYOBJECTIDMANAGER_H
#define BYYOBJECTIDMANAGER_H

#include <QMap>
#include "snigcore_global.h"

class ByyObject;

class SNIGCORE_EXPORT ByyObjectIdManager
{
public:
	ByyObjectIdManager();
	~ByyObjectIdManager();

	static ByyObjectIdManager& instance();

	void registerObject(ByyObject *obj);
	void unRegisterObject(ByyObject *obj);

	ByyObject* getObject(unsigned int id);

protected:
	unsigned int createNewObjectId();

protected:
	QMap<unsigned int,ByyObject*> myProgramObjects;
};

#endif // BYYOBJECTIDMANAGER_H
