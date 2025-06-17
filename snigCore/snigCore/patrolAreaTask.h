#ifndef PATROLAREATASK_H
#define PATROLAREATASK_H

#include "task.h"

class SNIGCORE_EXPORT ByyPatrolAreaTask : public ByyTask
{
public:
	ByyPatrolAreaTask(ByyEntityObject& entObj);
	~ByyPatrolAreaTask();

	static const TypeInfo& theTypeInfo();
	virtual const TypeInfo& typeInfo();

	virtual QString string();

	void setArea(const QString& r);
	const QString& area()const;

	void setPatrolTime(int t);
	int patrolTime();

	virtual ByyKeyValueList getConfig();
	virtual void fromConfig(const ByyKeyValueList& kvs);
	virtual	void	notifyCtrlObjNameChanged(QString old,QString cur);
protected:
	QString myCurrentArea;
	int myPatrolTime;
};

typedef ByyTaskCreatorTemplate<ByyPatrolAreaTask> ByyPatrolAreaTaskCreator;

#endif // PATROLAREATASK_H
