#ifndef ABSOBJECTMANAGER_H
#define ABSOBJECTMANAGER_H

#include <QObject>
#include "snigcore_global.h"

class ByyEntityObject;
class ByyControlObject;

class SNIGCORE_EXPORT ByyAbsObjectManager : public QObject
{
	Q_OBJECT
public:
	ByyAbsObjectManager(QObject *parent=0);
	~ByyAbsObjectManager();

	virtual ByyEntityObject* findEntity(const QString& name)=0;

signals:
	void entityAppend(ByyEntityObject*);
	void entityRemoved(ByyEntityObject*);

	void contrlObjectAppend(ByyControlObject*);
	void contrlObjectRemoved(ByyControlObject*); 
	
};

#endif // ABSOBJECTMANAGER_H
