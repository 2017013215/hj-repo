#include "snigVRLPlugin_global.h"

#include "vrlDriver.h"

extern "C" SNIGVRLPLUGIN_EXPORT  ByyDriver* createDriver(ByyRemoteObjectManager& objectManager)
{
	return new ByyVrlDriver(objectManager);
}