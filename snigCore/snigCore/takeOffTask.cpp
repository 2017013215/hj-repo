#include "takeOffTask.h"

ByyTakeOffTask::ByyTakeOffTask(ByyEntityObject& entObj)
	: ByyTask(entObj)
{

}

ByyTakeOffTask::~ByyTakeOffTask()
{

}

const ByyTask::TypeInfo& ByyTakeOffTask::theTypeInfo()
{
	static TypeInfo info={QString("TakeOff"),QString::fromLocal8Bit("Æð·É")};

	return info;
}

const ByyTask::TypeInfo& ByyTakeOffTask::typeInfo()
{
	return theTypeInfo();
}

QString ByyTakeOffTask::string()
{
	return QString::fromLocal8Bit("%1").arg(myName);
}

ByyKeyValueList ByyTakeOffTask::getConfig()
{
	ByyKeyValueList result=__super::getConfig();

	return result;
}

void ByyTakeOffTask::fromConfig( const ByyKeyValueList& kvs )
{
	__super::fromConfig(kvs);
}
