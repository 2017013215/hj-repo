#include "task.h"
#include "entityObject.h"


ByyTask::ByyTask(ByyEntityObject& entObj)
	:myEntObj(entObj)
	,myName(QString::fromLocal8Bit("ÈÎÎñ"))
	,myStartTime(0)
{

}

ByyTask::~ByyTask()
{

}

void ByyTask::setName( const QString& str )
{
	myName=str;
}

const QString& ByyTask::name() const
{
	return myName;
}

void ByyTask::setStartTime( double t )
{
	myStartTime=t;
}

double ByyTask::startTime() const
{
	return myStartTime;
}

ByyKeyValueList ByyTask::getConfig()
{
	ByyKeyValueList result;

	result.add("Executant",myEntObj.name());
	result.add("TaskName",name());
	result.add("TaskType",typeInfo().type);
	result.add("StartTime",myStartTime);

	return result;
}

void ByyTask::fromConfig( const ByyKeyValueList& kvs )
{
	kvs.get("TaskName",myName);
	kvs.get("StartTime",myStartTime);
}

ByyEntityObject& ByyTask::entity()
{
	return myEntObj;
}
