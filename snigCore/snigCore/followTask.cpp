#include "followTask.h"

ByyFollowTask::ByyFollowTask( ByyEntityObject& entObj )
	: ByyTask(entObj)
{

}

ByyFollowTask::~ByyFollowTask()
{

}

const ByyTask::TypeInfo& ByyFollowTask::theTypeInfo()
{
	static TypeInfo info={QString("Fllow"),QString::fromLocal8Bit("¸úËæ")};

	return info;
}

const ByyTask::TypeInfo& ByyFollowTask::typeInfo()
{
	return theTypeInfo();
}

QString ByyFollowTask::string()
{
	return myName+":"+QString::fromLocal8Bit("¸úËæ{%1}").arg(myFollowedObj);
}

ByyKeyValueList ByyFollowTask::getConfig()
{
	ByyKeyValueList result=__super::getConfig();
	result.add("Az",myRelativeOrientation);
	result.add("El",myHighOffset);
	result.add("Distance",myDistance);
	return result;
}

void ByyFollowTask::fromConfig( const ByyKeyValueList& kvs )
{
	__super::fromConfig(kvs);
	kvs.get("Az",myRelativeOrientation);
	kvs.get("El",myHighOffset);
	kvs.get("Distance",myDistance);
}

void ByyFollowTask::setFollowedObj( const QString& obj )
{
	myFollowedObj=obj;
}

QString ByyFollowTask::followedObj()
{
	return myFollowedObj;
}

void ByyFollowTask::setDistance( int val )
{
	myDistance=val;
}

int ByyFollowTask::distance()
{
	return myDistance;
}

void ByyFollowTask::setRelativeOrientation( int val )
{
	myRelativeOrientation=val;
}

int ByyFollowTask::relativeOrientation()
{
	return myRelativeOrientation;
}

void ByyFollowTask::setHighOffset( int val )
{
	myHighOffset=val;
}

int ByyFollowTask::highOffset()
{
	return myHighOffset;
}
