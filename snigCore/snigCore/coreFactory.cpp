#include "coreFactory.h"

ByyCoreFactory::ByyCoreFactory()
{

}

ByyCoreFactory::~ByyCoreFactory()
{

}

ByyCoreFactory& ByyCoreFactory::instance()
{
	static ByyCoreFactory _instance;
	return _instance;
}
/*

void ByyCoreFactory::setEngineCreator( ByyEngineCreatorFcn fcn )
{
	myEngineCreatorFcn=fcn;
}

ByyDriver* ByyCoreFactory::createEngine( ByyRemoteObjectManager& objectManager )
{
	return myEngineCreatorFcn?(*myEngineCreatorFcn)(objectManager):0;
}

void ByyCoreFactory::setObjectManagerCreator( ByyRemoteObjectManagerCreatorFcn fcn )
{
	myObjectManagerCreatorFcn=fcn;
}

ByyRemoteObjectManager* ByyCoreFactory::createObjectManager()
{
	return myObjectManagerCreatorFcn?(*myObjectManagerCreatorFcn)():0;
}

void ByyCoreFactory::setScenarioCreator( ByyScenarioCreatorFcn fcn )
{
	myScenarioCreatorFcn=fcn;
}

ByyScenario* ByyCoreFactory::createScenario( ByyCore& cgf )
{
	return myScenarioCreatorFcn?(*myScenarioCreatorFcn)(cgf):0;
}*/