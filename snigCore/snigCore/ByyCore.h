#ifndef COMMITTEE_H
#define COMMITTEE_H

#include <QObject>
#include <QTimer>

#include "snigcore_global.h"

class ByyLocalObjectManager;
class ByyRemoteObjectManager;
class ByyDriverManager;
class DtRadioCommsManager;
class ByyTaskFactory;

class SNIGCORE_EXPORT ByyCore :public QObject
{
	Q_OBJECT
public:
	ByyCore();
	virtual ~ByyCore();

	virtual void initialize();

	ByyDriverManager& driverManager();
	ByyRemoteObjectManager& remoteObjectManager();
	ByyLocalObjectManager& localObjectManager();

	DtRadioCommsManager& radioCommsManager();

	ByyTaskFactory& taskFactory();

	void block(bool bl);

protected slots:
	void tick();

signals:
	void postTick();

protected:
	ByyLocalObjectManager* myLocalObjManager;
	ByyRemoteObjectManager* myRemoteObjManager;
	ByyDriverManager *myDriverManager;
	DtRadioCommsManager* myRadioCommsManager;
	

	ByyTaskFactory* myTaskFactory; 

	QTimer timer;

	bool myBlock;
};

SNIGCORE_EXPORT ByyCore *byyCore();

#endif // COMMITTEE_H
