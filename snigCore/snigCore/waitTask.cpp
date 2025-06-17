#include "waitTask.h"

ByyWaitTask::ByyWaitTask( ByyEntityObject& entObj ) 
	: ByyTask(entObj)
{
	myWaitTime=0;
}

ByyWaitTask::~ByyWaitTask()
{

}

const ByyTask::TypeInfo& ByyWaitTask::theTypeInfo()
{
	static TypeInfo info={QString("Wait"),QString::fromLocal8Bit("µÈ´ý")};

	return info;
}

const ByyTask::TypeInfo& ByyWaitTask::typeInfo()
{
	return theTypeInfo();
}

QString ByyWaitTask::string()
{
	return QString::fromLocal8Bit("µÈ´ý{%1}").arg(myWaitTime);
}

ByyKeyValueList ByyWaitTask::getConfig()
{
	ByyKeyValueList result=__super::getConfig();
	result.add("WaitTime",myWaitTime);
	return result;
}

void ByyWaitTask::setWaitTime( int t )
{
	myWaitTime=t;
}

int ByyWaitTask::waitTime()
{
	return myWaitTime;
}

void ByyWaitTask::fromConfig( const ByyKeyValueList& kvs )
{
	__super::fromConfig(kvs);
	kvs.get("WaitTime",myWaitTime);
}
