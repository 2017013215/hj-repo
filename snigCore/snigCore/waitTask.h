#ifndef WAITTASK_H
#define WAITTASK_H

#include "task.h"

class SNIGCORE_EXPORT ByyWaitTask : public ByyTask
{
public:
	ByyWaitTask(ByyEntityObject& entObj);
	~ByyWaitTask();

	static const TypeInfo& theTypeInfo();
	virtual const TypeInfo& typeInfo();

	virtual QString string();

	virtual ByyKeyValueList getConfig();
	void fromConfig(const ByyKeyValueList& kvs);

	void setWaitTime(int t);
	int waitTime();

private:
	int myWaitTime;
};

typedef ByyTaskCreatorTemplate<ByyWaitTask> ByyWaitTaskCreator;

#endif // WAITTASK_H
