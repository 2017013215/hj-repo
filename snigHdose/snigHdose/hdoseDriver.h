#ifndef HDOSECONNECT_H
#define HDOSECONNECT_H

#include "driver.h"
#include "snighdose_global.h"
#include <QMap>

class ByyRemoteEntity;
class ByyControlObject;
class ByyHdoseEvent;
class ByyHdose;

class SNIGHDOSE_EXPORT ByyHdoseDriver : public ByyDriver
{
public:
	ByyHdoseDriver(ByyRemoteObjectManager& objectManager,ByyHdose& hdose);
	virtual ~ByyHdoseDriver();

	void setCurrentScenario(const QString& scenarioFile);
	void startScenario();

protected:
	virtual bool onStart();
	virtual bool onStop();
	virtual bool onTick();

protected:
	virtual void processEvent();
	virtual void processEntityAdd(ByyHdoseEvent* event);
	virtual void processEntityRemoved(ByyHdoseEvent* event);
	virtual void processCtrlObjAdd(ByyHdoseEvent* event);
	virtual void processCtrlObjRemoved(ByyHdoseEvent* event);
	virtual void processIncomplete();
	virtual void tickEntity();
	virtual void tickControlObject();

protected:
	ByyHdose& myHdose;
	QString myScenarioFilename;

	typedef QMap<int,ByyRemoteEntity*> EntityObjectMap;
	EntityObjectMap myEntityObjects;

	typedef QList<ByyRemoteEntity*> EntityObjectList;
	EntityObjectList myIncompleteEntObjs;


	typedef QMap<int,ByyControlObject*> ControlObjectMap;
	ControlObjectMap myControlObjects;

	double myLastUpdateWeaponTime;
	double myLastUpdateSensorTime;
};

#endif // HDOSECONNECT_H
