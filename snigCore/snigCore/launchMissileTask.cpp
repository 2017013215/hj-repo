#include "launchMissileTask.h"
#include "ByyObject.h"

ByyLaunchMissileTask::ByyLaunchMissileTask( ByyEntityObject& entObj ) 
	: ByyTask(entObj)
{

}

ByyLaunchMissileTask::~ByyLaunchMissileTask()
{

}

const ByyTask::TypeInfo& ByyLaunchMissileTask::theTypeInfo()
{
	static TypeInfo info={QString("LaunchMissile"),QString::fromLocal8Bit("发射导弹")};

	return info;
}

const ByyTask::TypeInfo& ByyLaunchMissileTask::typeInfo()
{
	return theTypeInfo();
}

QString ByyLaunchMissileTask::string()
{
	QString target;

	if (myTargetMode==Entity)
	{
		target=myTargetObj;
	}
	else
	{
		target=QString("%1,%2,%3").arg(myTargetPos.x()).arg(myTargetPos.y()).arg(myTargetPos.z());
	}

	return myName+":"+QString::fromLocal8Bit("发射导弹{%1}").arg(target);
}

ByyKeyValueList ByyLaunchMissileTask::getConfig()
{
	ByyKeyValueList result=__super::getConfig();

	result.add("TargetAssignType",myTargetMode);
	result.add("TargetObjectName",myTargetObj);
	result.add("TargetPosition",myTargetPos);

	return result;
}

void ByyLaunchMissileTask::fromConfig( const ByyKeyValueList& kvs )
{
	__super::fromConfig(kvs);
	int temp;
	kvs.get("TargetAssignType",temp);
	myTargetMode=(TargetMode)temp;
	kvs.get("TargetObjectName",myTargetObj);
	kvs.get("TargetPosition",myTargetPos);
}

void ByyLaunchMissileTask::setTargetMode( TargetMode m )
{
	myTargetMode=m;
}

ByyLaunchMissileTask::TargetMode ByyLaunchMissileTask::targetMode() const
{
	return myTargetMode;
}

void ByyLaunchMissileTask::setTargetObjName( const QString& n )
{
	myTargetObj=n;
}

const QString& ByyLaunchMissileTask::targetObjName() const
{
	return myTargetObj;
}

void ByyLaunchMissileTask::setTargetPos( const osg::Vec3d& p )
{
	myTargetPos=p;
}

const osg::Vec3d& ByyLaunchMissileTask::targetPos() const
{
	return myTargetPos;
}
