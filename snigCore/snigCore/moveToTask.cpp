#include "moveToTask.h"
#include "entityObject.h"

ByyMoveToTask::ByyMoveToTask(ByyEntityObject& entObj)
	: ByyTask(entObj)
{
	
}

ByyMoveToTask::~ByyMoveToTask()
{

}

const ByyTask::TypeInfo& ByyMoveToTask::theTypeInfo()
{
	static TypeInfo info={QString("MoveToPoint"),QString::fromLocal8Bit("运动到点")};

	return info;
}

const ByyTask::TypeInfo& ByyMoveToTask::typeInfo()
{
	return theTypeInfo();
}

QString& ByyMoveToTask::waypoint()
{
	return myWaypoint;
}

void ByyMoveToTask::setWaypoint( const QString& pos )
{
	myWaypoint=pos;
}

QString ByyMoveToTask::string()
{
	return QString::fromLocal8Bit("%1:移动到{%2}").arg(myName).arg(myWaypoint);
}

ByyKeyValueList ByyMoveToTask::getConfig()
{
	ByyKeyValueList result=__super::getConfig();

	result.add("CtrlObjName",myWaypoint);

	return result;
}

void ByyMoveToTask::fromConfig( const ByyKeyValueList& kvs )
{
	__super::fromConfig(kvs);
	kvs.get("CtrlObjName",myWaypoint);
}

void ByyMoveToTask::notifyCtrlObjNameChanged(QString old,QString cur)
{
	if(myWaypoint==old)
	{
		myWaypoint=cur;
	}
}