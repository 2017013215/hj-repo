#include "patrolAreaTask.h"

ByyPatrolAreaTask::ByyPatrolAreaTask( ByyEntityObject& entObj ) 
	: ByyTask(entObj)
{
	myPatrolTime=7200.0;
}

ByyPatrolAreaTask::~ByyPatrolAreaTask()
{

}

const ByyTask::TypeInfo& ByyPatrolAreaTask::theTypeInfo()
{
	static TypeInfo info={QString("PatrolInZone"),QString::fromLocal8Bit("ÇøÓòÄÚÑ²Âß")};

	return info;
}

const ByyTask::TypeInfo& ByyPatrolAreaTask::typeInfo()
{
	return theTypeInfo();
}

QString ByyPatrolAreaTask::string()
{
	return myName+":"+QString::fromLocal8Bit("ÇøÓòÄÚÑ²Âß{%1}").arg(myCurrentArea);
}

void ByyPatrolAreaTask::setArea( const QString& r )
{
	myCurrentArea=r;
}

const QString& ByyPatrolAreaTask::area() const
{
	return myCurrentArea;
}

ByyKeyValueList ByyPatrolAreaTask::getConfig()
{
	ByyKeyValueList result=__super::getConfig();

	result.add("CtrlObjName",myCurrentArea);
	result.add("PatrolTime",myPatrolTime);

	return result;
}

void ByyPatrolAreaTask::fromConfig( const ByyKeyValueList& kvs )
{
	__super::fromConfig(kvs);
	kvs.get("CtrlObjName",myCurrentArea);
	kvs.get("PatrolTime",myPatrolTime);
}

void ByyPatrolAreaTask::setPatrolTime( int t )
{
	myPatrolTime=t;
}

int ByyPatrolAreaTask::patrolTime()
{
	return myPatrolTime;
}

void ByyPatrolAreaTask::notifyCtrlObjNameChanged(QString old,QString cur)
{
	if(myCurrentArea==old)
	{
		myCurrentArea=cur;
	}
}