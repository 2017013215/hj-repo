#ifndef ENGINEMANAGER_H
#define ENGINEMANAGER_H

#include <QObject>
#include "snigcore_global.h"

class ByyDriver;
class ByyRemoteObjectManager;

class SNIGCORE_EXPORT ByyDriverManager : public QObject
{
	Q_OBJECT

public:
	ByyDriverManager(ByyRemoteObjectManager* objectManager);
	virtual ~ByyDriverManager();

	void addDriver(ByyDriver* driver);
	void destroyDriver(ByyDriver* driver);

	void startDriver(ByyDriver* driver);
	void stopDriver(ByyDriver* driver);

	ByyDriver* findDriver(const QString& dirverName);

	void tick();

	QList<ByyDriver*>& drivers();

protected:
	QList<ByyDriver*> myDriverList;	
	ByyRemoteObjectManager* myObjectManager;
};

#endif // ENGINEMANAGER_H
