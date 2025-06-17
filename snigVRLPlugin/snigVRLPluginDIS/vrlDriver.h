#ifndef HLA13ENGINE_H
#define HLA13ENGINE_H

#include "driver.h"

#include <QMap>
#include <matrix/topoCoord.h>

class DtExerciseConn;
class DtReflectedEntity;
class DtReflectedObject;
class DtReflectedEntityList;
class DtReflectedEnvironmentProcess;
class DtReflectedEnvironmentProcessList;
class ByyEntityObject;
class ByyControlObject;
class DtClock;

class ByyVrlDriver : public ByyDriver
{
	Q_OBJECT

public:
	ByyVrlDriver(ByyRemoteObjectManager& objectManager);
	~ByyVrlDriver();

	static void entityAddedCallback(DtReflectedEntity *obj, void *userData);
	static void entityRemovalCallback(DtReflectedEntity *obj, void *userData); 
	static void entityUpdateCallback(DtReflectedEntity *obj, void *userData);

	static void environmentProcessAdditionCallback(DtReflectedEnvironmentProcess *obj, void *userData);
	static void environmentProcessRemovalCallback(DtReflectedEnvironmentProcess *obj, void *userData); 
	static void environmentProcessUpdateCallback(DtReflectedEnvironmentProcess *obj, void *userData);
	static bool discoveryConditionCallback(DtReflectedObject *refObj, void *usr);

	virtual bool isVRLink()const;

protected:
	virtual bool onStart();
	virtual bool onStop();
	virtual bool onTick();

protected:
	void processEntityAdded(DtReflectedEntity *obj);
	void processEntityRemoved(DtReflectedEntity *obj);
	void processEntityUpdate(DtReflectedEntity *obj);

	void processEnvironmentAdded(DtReflectedEnvironmentProcess *obj);
	void processEnvironmentRemoved(DtReflectedEnvironmentProcess *obj);
	void processEnvironmentUpdate(DtReflectedEnvironmentProcess *obj);

protected:
	DtExerciseConn *exConn;
	DtReflectedEntityList *relEntList;
	DtReflectedEnvironmentProcessList *relEnvirList;
	DtClock* clock;
	DtGeodeticCoord geod;

	typedef QMap<DtReflectedEntity*,ByyEntityObject*> ByyEntityMap;
	ByyEntityMap myEntityList;

	typedef QMap<DtReflectedEnvironmentProcess*,ByyControlObject*> ByyCtrlObjMap;
	ByyCtrlObjMap myCtrlObjList;
	
};

#endif // HLA13ENGINE_H
