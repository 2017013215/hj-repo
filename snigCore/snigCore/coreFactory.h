#ifndef COMMITTEEFACTORY_H
#define COMMITTEEFACTORY_H

#include "snigcore_global.h"

class ByyDriver;
class ByyRemoteObjectManager;
class ByySimulationController;
class ByyScenarioV2;
class ByyCore;

//typedef ByyDriver *(*ByyEngineCreatorFcn)(ByyRemoteObjectManager& objectManager);
//typedef ByyRemoteObjectManager *(*ByyRemoteObjectManagerCreatorFcn)();
//typedef ByyScenario *(*ByyScenarioCreatorFcn)(ByyCore&);

class SNIGCORE_EXPORT ByyCoreFactory
{
public:
	virtual ~ByyCoreFactory();

	static ByyCoreFactory& instance();

	//virtual void setEngineCreator(ByyEngineCreatorFcn fcn);
	//ByyDriver* createEngine(ByyRemoteObjectManager& objectManager);

	//virtual void setObjectManagerCreator(ByyRemoteObjectManagerCreatorFcn fcn);
	//ByyRemoteObjectManager* createObjectManager();

	//virtual void setScenarioCreator(ByyScenarioCreatorFcn fcn);
	//ByyScenario* createScenario(ByyCore& cgf);

protected:
	ByyCoreFactory();

protected:
	//ByyEngineCreatorFcn myEngineCreatorFcn;
	//ByyRemoteObjectManagerCreatorFcn myObjectManagerCreatorFcn;
	//ByyScenarioCreatorFcn myScenarioCreatorFcn;
};

#endif // COMMITTEEFACTORY_H
