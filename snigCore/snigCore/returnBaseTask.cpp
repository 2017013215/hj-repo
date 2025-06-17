#include "returnBaseTask.h"

ByyReturnBaseTask::ByyReturnBaseTask( ByyEntityObject& entObj ) 
	: ByyTask(entObj)
{
	myAssignBaseType=Default;
}

ByyReturnBaseTask::~ByyReturnBaseTask()
{

}

const ByyTask::TypeInfo& ByyReturnBaseTask::theTypeInfo()
{
	static TypeInfo info={QString("ReturnBase"),QString::fromLocal8Bit("·µº½")};

	return info;
}

const ByyTask::TypeInfo& ByyReturnBaseTask::typeInfo()
{
	return theTypeInfo();
}

QString ByyReturnBaseTask::string()
{
	QString target;

	if (myAssignBaseType==Default)
	{
		target=QString::fromLocal8Bit("Ä¬ÈÏ½µÂäÎ»ÖÃ");
	}
	else if (myAssignBaseType==BasePos)
	{
		target=QString("%1,%2,%3").arg(myBasePos.x()).arg(myBasePos.y()).arg(myBasePos.z());
	}
	else
	{
		target=myBaseEntName;
	}

	return myName+":"+QString::fromLocal8Bit("·µº½µ½{%1}").arg(target);
}

ByyKeyValueList ByyReturnBaseTask::getConfig()
{
	ByyKeyValueList result=__super::getConfig();

	result.add("AssignBaseType",myAssignBaseType);
	result.add("BasePos",myBasePos);
	result.add("BaseEntName",myBaseEntName);

	return result;
}

void ByyReturnBaseTask::fromConfig( const ByyKeyValueList& kvs )
{
	__super::fromConfig(kvs);
	int temp;
	kvs.get("AssignBaseType",temp);
	myAssignBaseType=(AssignBaseType)temp;
	kvs.get("BasePos",myBasePos);
	kvs.get("BaseEntName",myBaseEntName);
}

void ByyReturnBaseTask::setAssignBaseType( AssignBaseType m )
{
	myAssignBaseType=m;
}

ByyReturnBaseTask::AssignBaseType ByyReturnBaseTask::getAssignBaseType() const
{
	return myAssignBaseType;
}

void ByyReturnBaseTask::setBaseEnt( const QString& n )
{
	myBaseEntName=n;
}

const QString& ByyReturnBaseTask::getBaseEnt() const
{
	return myBaseEntName;
}

void ByyReturnBaseTask::setBasePos( const osg::Vec3d& p )
{
	myBasePos=p;
}

const osg::Vec3d& ByyReturnBaseTask::getBasePos() const
{
	return myBasePos;
}
