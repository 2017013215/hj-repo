#include "moveAlongRouteTask.h"

ByyMoveAlongRouteTask::ByyMoveAlongRouteTask( ByyEntityObject& entObj )
	: ByyTask(entObj)
{

}

ByyMoveAlongRouteTask::~ByyMoveAlongRouteTask()
{

}

const ByyTask::TypeInfo& ByyMoveAlongRouteTask::theTypeInfo()
{
	static TypeInfo info={QString("MoveAlongRoute"),QString::fromLocal8Bit("沿路线运动")};

	return info;
}

const ByyTask::TypeInfo& ByyMoveAlongRouteTask::typeInfo()
{
	return theTypeInfo();
}

QString ByyMoveAlongRouteTask::string()
{
	return myName+":"+QString::fromLocal8Bit("沿路线运动{%1}").arg(myCurrentRoute);
}

void ByyMoveAlongRouteTask::setRoute( const QString& r )
{
	myCurrentRoute=r;
}

const QString& ByyMoveAlongRouteTask::route() const
{
	return myCurrentRoute;
}

ByyKeyValueList ByyMoveAlongRouteTask::getConfig()
{
	ByyKeyValueList result=__super::getConfig();

	result.add("CtrlObjName",myCurrentRoute);

	return result;
}

void ByyMoveAlongRouteTask::fromConfig( const ByyKeyValueList& kvs )
{
	__super::fromConfig(kvs);
	kvs.get("CtrlObjName",myCurrentRoute);
}

void ByyMoveAlongRouteTask::notifyCtrlObjNameChanged(QString old,QString cur)
{
	if(myCurrentRoute==old)
	{
		myCurrentRoute=cur;
	}
}
