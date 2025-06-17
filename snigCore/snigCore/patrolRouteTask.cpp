#include "patrolRouteTask.h"

ByyPatrolRouteTask::ByyPatrolRouteTask( ByyEntityObject& entObj )
	: ByyTask(entObj)
{
	myPatrolTime=7200.0;
}

ByyPatrolRouteTask::~ByyPatrolRouteTask()
{

}

const ByyTask::TypeInfo& ByyPatrolRouteTask::theTypeInfo()
{
	static TypeInfo info={QString("PatrolAlongRoute"),QString::fromLocal8Bit("沿路线巡逻")};

	return info;
}

const ByyTask::TypeInfo& ByyPatrolRouteTask::typeInfo()
{
	return theTypeInfo();
}

QString ByyPatrolRouteTask::string()
{
	return myName+":"+QString::fromLocal8Bit("沿路线巡逻{%1}").arg(myCurrentRoute);
}

void ByyPatrolRouteTask::setRoute( const QString& r )
{
	myCurrentRoute=r;
}

const QString& ByyPatrolRouteTask::route() const
{
	return myCurrentRoute;
}

ByyKeyValueList ByyPatrolRouteTask::getConfig()
{
	ByyKeyValueList result=__super::getConfig();

	result.add("CtrlObjName",myCurrentRoute);
	result.add("PatrolTime",myPatrolTime);

	return result;
}

void ByyPatrolRouteTask::fromConfig( const ByyKeyValueList& kvs )
{
	__super::fromConfig(kvs);
	kvs.get("CtrlObjName",myCurrentRoute);
	kvs.get("PatrolTime",myPatrolTime);
}

void ByyPatrolRouteTask::setPatrolTime( int t )
{
	myPatrolTime=t;
}

int ByyPatrolRouteTask::patrolTime()
{
	return myPatrolTime;
}

void ByyPatrolRouteTask::notifyCtrlObjNameChanged(QString old,QString cur)
{
	if(myCurrentRoute==old)
	{
		myCurrentRoute=cur;
	}
}