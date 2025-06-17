#include "sensorOperTask.h"

ByySensorOperTask::ByySensorOperTask( ByyEntityObject& entObj ) 
	: ByyTask(entObj)
{
	mySensorOperType=1;
}

ByySensorOperTask::~ByySensorOperTask()
{

}

const ByyTask::TypeInfo& ByySensorOperTask::theTypeInfo()
{
	static TypeInfo info={QString("SensorOper"),QString::fromLocal8Bit("传感器操作")};

	return info;
}

const ByyTask::TypeInfo& ByySensorOperTask::typeInfo()
{
	return theTypeInfo();
}

QString ByySensorOperTask::string()
{
	return myDeviceName+":"+QString::fromLocal8Bit("{%1}").arg(QString::fromLocal8Bit(myPowerOn?"开机":"关机"));
}

ByyKeyValueList ByySensorOperTask::getConfig()
{
	ByyKeyValueList result=__super::getConfig();

	result.add("DeviceName",myDeviceName);
	result.add("SensorOperType",mySensorOperType);
	result.add("PowerOn",myPowerOn);
	return result;
}

void ByySensorOperTask::fromConfig( const ByyKeyValueList& kvs )
{
	__super::fromConfig(kvs);

	kvs.get("DeviceName",myDeviceName);

	kvs.get("SensorOperType",mySensorOperType);

	kvs.get("PowerOn",myPowerOn);
}
