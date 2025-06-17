#ifndef RETURNBASETASK_H
#define RETURNBASETASK_H

#include "task.h"

class SNIGCORE_EXPORT ByyReturnBaseTask : public ByyTask
{
public:
	ByyReturnBaseTask(ByyEntityObject& entObj);
	~ByyReturnBaseTask();

	static const TypeInfo& theTypeInfo();
	virtual const TypeInfo& typeInfo();

	virtual QString string();

	enum AssignBaseType{Default,BasePos,BaseEntName};

	ByyKeyValueList getConfig();
	void fromConfig(const ByyKeyValueList& kvs);

	void setAssignBaseType(AssignBaseType m);
	AssignBaseType getAssignBaseType()const;

	void setBaseEnt(const QString& n);
	const QString& getBaseEnt()const;

	void setBasePos(const osg::Vec3d& p);
	const osg::Vec3d& getBasePos()const;

private:
	AssignBaseType myAssignBaseType;
	osg::Vec3d myBasePos;
	QString myBaseEntName;

};

typedef ByyTaskCreatorTemplate<ByyReturnBaseTask> ByyReturnBaseTaskCreator;

#endif // RETURNBASETASK_H
