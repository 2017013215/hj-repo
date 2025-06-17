#ifndef MOVEALONGROUTETASK_H
#define MOVEALONGROUTETASK_H

#include "task.h"

class SNIGCORE_EXPORT ByyMoveAlongRouteTask : public ByyTask
{
public:
	ByyMoveAlongRouteTask(ByyEntityObject& entObj);
	~ByyMoveAlongRouteTask();

	static const TypeInfo& theTypeInfo();
	virtual const TypeInfo& typeInfo();

	virtual QString string();

	void setRoute(const QString& r);
	const QString& route()const;

	virtual ByyKeyValueList getConfig();
	virtual void fromConfig(const ByyKeyValueList& kvs);
	virtual	void	notifyCtrlObjNameChanged(QString old,QString cur);
protected:
	QString myCurrentRoute;
};

typedef ByyTaskCreatorTemplate<ByyMoveAlongRouteTask> ByyMoveAlongRouteTaskCreator;

#endif // MOVEALONGROUTETASK_H
