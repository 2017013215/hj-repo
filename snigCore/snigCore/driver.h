#ifndef CONNECT_H
#define CONNECT_H

#include <QObject>
#include "snigcore_global.h"

class ByyEntityObject;
class ByyControlObject;
class ByyRemoteObjectManager;

class SNIGCORE_EXPORT ByyDriver : public QObject
{
	Q_OBJECT
public:
	ByyDriver(ByyRemoteObjectManager& objectManager,const QString &instanceName);
	virtual ~ByyDriver();

	friend class ByyDriverManager;
	
	virtual QString instanceName() const;

	inline bool started() const;

	virtual bool isVRLink()const;

protected:
	void start();
	void stop();
	bool tick();

	virtual bool onStart()=0;
	virtual bool onStop()=0;
	virtual bool onTick()=0;


/*
signals:
	void entityDiscoverd(ByyEntityObject*);
	void entityRemoved(ByyEntityObject*); 

	void contrlObjectDiscoverd(ByyControlObject*);
	void contrlObjectRemoved(ByyControlObject*); */

protected:
	bool myStartedFlag;
	QString myName;

	ByyRemoteObjectManager& myObjectManager;
};

#endif // CONNECT_H
