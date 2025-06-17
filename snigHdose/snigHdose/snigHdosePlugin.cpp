#include "snighdose_global.h"

#include "ByyHdose.h"

extern "C" SNIGHDOSE_EXPORT  ByyHdose* createHdose(ByyCore& core)
{
	return new ByyHdose(core);
}