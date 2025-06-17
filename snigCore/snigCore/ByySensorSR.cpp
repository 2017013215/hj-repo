#include "ByySensorSR.h"
#include "ByyDetectObject.h"

ByySensorSR::ByySensorSR(ByyRemoteEntity* ent)
	: ByyStateRepository(ent),
	sensorOpr(false)
{

}

ByySensorSR::~ByySensorSR()
{
	DeviceStateList::iterator it=myDeviceStateList.begin();

	for (;it!=myDeviceStateList.end();++it)
	{
		DeviceState& ds=*it;
		DetectObjectMap::iterator objIt=ds.myDetectObjList.begin();

		for (;objIt!=ds.myDetectObjList.end();++objIt)
		{
			ByySensorSRSignaler::instance().emit_detectRemoved(*objIt,this);
		}

		qDeleteAll(ds.myDetectObjList);
	}

	//printf("ByySensorSR::~ByySensorSR()\n");
}

const QString& ByySensorSR::type()
{
	return theType();
}

const QString& ByySensorSR::theType()
{
	static QString _type("Sensor");
	return _type;
}

bool ByySensorSR::getDevicePower( const QString& device )
{
	// 20230912,xks
	if( getDeviceState(device))
		return myDeviceStateList[device].myPowerOn;
	else
		return false;
}

bool ByySensorSR::switchPower( const QString& device )
{
	DeviceState &ds=myDeviceStateList[device];

	ds.myPowerOn=!ds.myPowerOn;

	sensorOpr = true;

	ByySensorSRSignaler::instance().emit_powerSwitch(myEnt,this,device,ds.myPowerOn);

	return ds.myPowerOn;
}

void ByySensorSR::setDevicePower( const QString& device,bool onOrFalse )
{
	if (!sensorOpr && myDeviceStateList[device].myPowerOn!=onOrFalse)
	{
		myDeviceStateList[device].myPowerOn=onOrFalse;

		ByySensorSRSignaler::instance().emit_powerSwitch(myEnt,this,device,onOrFalse);
	}
}

void ByySensorSR::setAntennaAz( const QString& device, double az )
{
	myDeviceStateList[device].myAntennaAz=az;
}

void ByySensorSR::setBeamWidth( const QString& device, float bw )
{
	myDeviceStateList[device].myBeamWidth=bw;
}

bool ByySensorSR::isTracked(const QString& targetName, const QString& device)
{
	bool ret = false;
	if(myDeviceStateList.contains(device))
	{
		QList<ByyDetectObject*> objects = myDeviceStateList.value(device).myDetectObjList.values();
		foreach(ByyDetectObject* indexObject, objects)
		{
			if(indexObject->name().contains(targetName))
			{
				return true;
			}
		}
	}
	return ret;
}

void ByySensorSR::setDeviceDetectData( const QString& device,const DetectDataList& detData )
{
	DeviceState& device_state=myDeviceStateList[device];


	DetectObjectMap tempObjList=device_state.myDetectObjList;


	for (int i=0;i!=detData.size();++i)
	{
		const DetectData& detect_data=detData[i];

		DetectObjectMap::iterator it=tempObjList.find(detect_data.m_TargetID);

		if (it!=tempObjList.end())
		{
			updateDetectObject(it.value(),detect_data);

			tempObjList.erase(it);
		}
		else
		{
			ByyDetectObject *obj=new ByyDetectObject;

			obj->setTargetID(detect_data.m_TargetID);
			obj->setDeviceName(device);
			updateDetectObject(obj,detect_data);

			device_state.myDetectObjList.insert(detect_data.m_TargetID,obj);

			ByySensorSRSignaler::instance().emit_discoverDetect(obj,this);
		}
	}

	QList<ByyDetectObject*> waitRemovedList;

	{
		DetectObjectMap::iterator it=tempObjList.begin();

		for (;it!=tempObjList.end();++it)
		{
			waitRemovedList.push_back(device_state.myDetectObjList.take(it.key()));
		}
	}

	{
		QList<ByyDetectObject*>::iterator it=waitRemovedList.begin();

		for (;it!=waitRemovedList.end();++it)
		{
			ByySensorSRSignaler::instance().emit_detectRemoved(*it,this);
		}
	}

	qDeleteAll(waitRemovedList);
}

ByySensorSR::DeviceStateList* ByySensorSR::getDeviceList()
{
	return &myDeviceStateList;
}

void ByySensorSR::updateDetectObject( ByyDetectObject* obj,const DetectData& detect_data )
{
	obj->setUpdateTime(detect_data.m_UpdateTime);
	obj->setContinueTime(detect_data.m_ContinueTime);
	obj->setBatchNo(detect_data.m_BatchNo);
	obj->setTargetName(detect_data.m_TargetName);
	obj->setCID(detect_data.m_CID);
	obj->setForceType((ByyForceType)detect_data.m_Side);
	obj->setEntityType(detect_data.m_EntityType);
	obj->setThreatLevel(detect_data.m_ThreatLevel);
	obj->setPosition(detect_data.m_Pos);
	obj->setVelocity(detect_data.m_Velocity);
	obj->setCourse(detect_data.m_Course);
	obj->setDistance(detect_data.m_Distance);
	obj->setEl(detect_data.m_El);
	obj->setAz(detect_data.m_Az);
	obj->setRadiationType(detect_data.m_RadiationType);
	obj->setRadiationPt(detect_data.m_RadiationPt);
	obj->setRadiationFreq(detect_data.m_RadiationFreq);
	obj->setRadiationTu(detect_data.m_RadiationTu);
	obj->setRadiationPRF(detect_data.m_RadiationPRF);
	obj->setRadiationSignalType(detect_data.m_RadiationSignalType);
	obj->setRadiationPulseType(detect_data.m_RadiationPulseType);
}

ByySensorSR::DeviceState* ByySensorSR::getDeviceState( const QString& device )
{
	// 20230912,xks
	DeviceStateList::Iterator it= myDeviceStateList.find(device);
	if(it != myDeviceStateList.end())
		return &it.value();
	else
		return 0;
}

ByySensorSRSignaler& ByySensorSRSignaler::instance()
{
	static ByySensorSRSignaler obj; 
	return obj;
}

void ByySensorSRSignaler::emit_discoverDetect( ByyDetectObject* obj,ByySensorSR* sensor)
{
	emit discoverDetect(obj,sensor);
}

void ByySensorSRSignaler::emit_detectRemoved( ByyDetectObject* obj,ByySensorSR* sensor )
{
	emit detectRemoved(obj,sensor);
}

void ByySensorSRSignaler::emit_powerSwitch( ByyRemoteEntity* ent,ByySensorSR* sensor,QString device,bool power )
{
	emit powerSwitch(ent,sensor,device,power);
}
