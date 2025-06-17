#ifndef BYYLAUNCHMISSILETASK_H
#define BYYLAUNCHMISSILETASK_H

#include "task.h"

class SNIGCORE_EXPORT ByyLaunchMissileTask : public ByyTask
{
public:
	ByyLaunchMissileTask(ByyEntityObject& entObj);
	~ByyLaunchMissileTask();

	static const TypeInfo& theTypeInfo();
	virtual const TypeInfo& typeInfo();

	virtual QString string();

	virtual ByyKeyValueList getConfig();
	virtual void fromConfig(const ByyKeyValueList& kvs);

	enum TargetMode{Entity=1,Position};

	void setTargetMode(TargetMode m);
	TargetMode targetMode()const;

	void setTargetObjName(const QString& n);
	const QString& targetObjName()const;

	void setTargetPos(const osg::Vec3d& p);
	const osg::Vec3d& targetPos()const;

private:
	TargetMode myTargetMode;
	QString myTargetObj;
	osg::Vec3d myTargetPos;
};

typedef ByyTaskCreatorTemplate<ByyLaunchMissileTask> ByyLaunchMissileTaskCreator;

#endif // BYYLAUNCHMISSILETASK_H
