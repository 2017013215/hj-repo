#ifndef PATROLROUTETASK_H
#define PATROLROUTETASK_H

#include "task.h"

class SNIGCORE_EXPORT ByyPatrolRouteTask : public ByyTask
{
public:
	ByyPatrolRouteTask(ByyEntityObject& entObj);
	~ByyPatrolRouteTask();

	static const TypeInfo& theTypeInfo();
	virtual const TypeInfo& typeInfo();

	virtual QString string();

	void setRoute(const QString& r);
	const QString& route()const;

	void setPatrolTime(int t);
	int patrolTime();

	virtual ByyKeyValueList getConfig();
	virtual void fromConfig(const ByyKeyValueList& kvs);
	virtual	void	notifyCtrlObjNameChanged(QString old,QString cur);
protected:
	QString myCurrentRoute;
	int myPatrolTime;

};

typedef ByyTaskCreatorTemplate<ByyPatrolRouteTask> ByyPatrolRouteTaskCreator;

#endif // PATROLROUTETASK_H
