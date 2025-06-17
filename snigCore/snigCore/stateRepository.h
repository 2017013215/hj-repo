#ifndef ENTITYSTATEREPOSITORY_H
#define ENTITYSTATEREPOSITORY_H

#include <QObject>
#include <QString>
#include "snigcore_global.h"

class ByyRemoteEntity;

class SNIGCORE_EXPORT ByyStateRepository : public QObject
{
public:
	ByyStateRepository(ByyRemoteEntity* ent);
	~ByyStateRepository();
	
	virtual const QString& type()=0;

protected:
	ByyRemoteEntity* myEnt;
};

#endif // ENTITYSTATEREPOSITORY_H
