#ifndef MOVETOTASK_H
#define MOVETOTASK_H

#include "task.h"

class SNIGCORE_EXPORT ByyMoveToTask : public ByyTask
{
public:
	ByyMoveToTask(ByyEntityObject& entObj);
	~ByyMoveToTask();


	static const TypeInfo& theTypeInfo();
	virtual const TypeInfo& typeInfo();

	virtual QString string();

	QString& waypoint();
	void setWaypoint(const QString& pos);

	virtual ByyKeyValueList getConfig();
	virtual void fromConfig(const ByyKeyValueList& kvs);
	virtual	void	notifyCtrlObjNameChanged(QString old,QString cur);
private:
	QString myWaypoint;
};

typedef ByyTaskCreatorTemplate<ByyMoveToTask> ByyMoveToTaskCreator;

#endif // MOVETOTASK_H
