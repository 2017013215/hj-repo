#ifndef CRUISETASK_H
#define CRUISETASK_H

#include "task.h"

class SNIGCORE_EXPORT ByyCruiseTask : public ByyTask
{
public:
	ByyCruiseTask(ByyEntityObject& entObj);
	~ByyCruiseTask();

	static const TypeInfo& theTypeInfo();
	virtual const TypeInfo& typeInfo();

	virtual QString string();

	virtual ByyKeyValueList getConfig();
	virtual void fromConfig(const ByyKeyValueList& kvs);

private:
	
};

typedef ByyTaskCreatorTemplate<ByyCruiseTask> ByyCruiseTaskCreator;

#endif // CRUISETASK_H
