#ifndef FOLLOWTASK_H
#define FOLLOWTASK_H

#include "task.h"

class SNIGCORE_EXPORT ByyFollowTask : public ByyTask
{
public:
	ByyFollowTask(ByyEntityObject& entObj);
	~ByyFollowTask();

	static const TypeInfo& theTypeInfo();
	virtual const TypeInfo& typeInfo();

	virtual QString string();

	virtual ByyKeyValueList getConfig();
	virtual void fromConfig(const ByyKeyValueList& kvs);

	void setFollowedObj(const QString& obj);
	QString followedObj();

	void setDistance(int val);
	int distance();

	void setRelativeOrientation(int val);
	int relativeOrientation();

	void setHighOffset(int val);
	int highOffset();

protected:
	QString myFollowedObj;
	int myDistance;
	int myRelativeOrientation;
	int myHighOffset;
};

typedef ByyTaskCreatorTemplate<ByyFollowTask> ByyFollowTaskCreator;

#endif // FLLOWTASK_H
