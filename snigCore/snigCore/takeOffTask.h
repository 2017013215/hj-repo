#ifndef TAKEOFFTASK_H
#define TAKEOFFTASK_H

#include "task.h"

class SNIGCORE_EXPORT ByyTakeOffTask : public ByyTask
{
public:
	ByyTakeOffTask(ByyEntityObject& entObj);
	~ByyTakeOffTask();

	static const TypeInfo& theTypeInfo();
	virtual const TypeInfo& typeInfo();

	virtual QString string();

	virtual ByyKeyValueList getConfig();
	virtual void fromConfig(const ByyKeyValueList& kvs);
	
};

typedef ByyTaskCreatorTemplate<ByyTakeOffTask> ByyTakeOffTaskCreator;

#endif // TAKEOFFTASK_H
