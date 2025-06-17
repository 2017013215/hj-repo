#include "cruiseTask.h"

ByyCruiseTask::ByyCruiseTask( ByyEntityObject& entObj )
	: ByyTask(entObj)
{

}

ByyCruiseTask::~ByyCruiseTask()
{

}

const ByyTask::TypeInfo& ByyCruiseTask::theTypeInfo()
{
	static TypeInfo info={QString("Cruise"),QString::fromLocal8Bit("”ŒﬂÆ")};

	return info;
}

const ByyTask::TypeInfo& ByyCruiseTask::typeInfo()
{
	return theTypeInfo();
}

QString ByyCruiseTask::string()
{
	return QString::fromLocal8Bit("”ŒﬂÆ");
}

ByyKeyValueList ByyCruiseTask::getConfig()
{
	ByyKeyValueList result=__super::getConfig();

	return result;
}

void ByyCruiseTask::fromConfig( const ByyKeyValueList& kvs )
{
	__super::fromConfig(kvs);
}
