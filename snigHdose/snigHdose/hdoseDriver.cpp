#include "hdoseDriver.h"
#include "hdoseEvent.h"
#include "hdoseEventPool.h"
#include "remoteEntity.h"
#include "ByyWaypoint.h"
#include "ByyPhaseLine.h"
#include "ByyRoute.h"
#include "ByyCircle.h"
#include "ByyPolygon.h"
#include "ByyEllipse.h"
#include "ByyRectangle.h"
#include "remoteObjectManager.h"
#include "assemble.h"
#include "ByyHdose.h"
#include "opd.h"
#include "ByyHdoseEntity.h"
//#include "ByyLog.h"

#include "JSCGFSimDS/JSCGFSimDS.h"
#include "JSCGFSimDS/BaseDataType.h"
#include "JSCGFSimEvt/JSCGFSimEvt.h"
#include "JSCGFSimTsk/JSCGFSimTsk.h"
#include "JSCGFSimMF/JSCGFSimMF.h"
#include "JSCoreAppDS/JSCoreAppDS.h"
#include "JSCGFSimMF/JSWayPoint.h"
#include "JSCGFSimMF/JSRoute.h"
#include "JSCGFSimMF/JSCircle.h"
#include "JSCGFSimMF/JSPolygon.h"
#include "JSCGFSimMF/JSRectangle.h"
#include "JSCGFSimMF/JSEllipse.h"
#include "JSCGFSimMF/JSWeaponESR.h"
//#include "CGF/Phaseline.h"

#include <JSCGFSimMF/JSSatelliteESR.h>
#include <JSCGFSimMF/JSWeaponSysModelSR.h>
#include <JSCGFSimMF/JSSensorModelSR.h>
#include <osg/Timer>

#include <osgEarth/GeoMath>

#include <qdebug.h>

osg::Vec3d GeoPt2Vec3d(GeoPt& p)
{
	return osg::Vec3d(p.Lon(),p.Lat(),p.Elev());
}

void convertPointList(GeoPtList &ptList, QVector<osg::Vec3d> &points )
{

	POSITION pos = ptList.GetHeadPosition();

	int i=0;

	while (pos)
	{
		GeoPt* pt = (GeoPt*) ptList.GetNext(pos);

		if (pt)
		{
			points[i]=GeoPt2Vec3d(*pt);
		}

		++i;
	}
	ReleaseGeoPtList(&ptList);
}

ByyHdoseDriver::ByyHdoseDriver( ByyRemoteObjectManager& objectManager ,ByyHdose& hdose)
	:ByyDriver(objectManager,"HDose")
	,myHdose(hdose)
	,myLastUpdateWeaponTime(0)
	,myLastUpdateSensorTime(0)
{
	myScenarioFilename="../../app/Model/nullscn.scn";
}

ByyHdoseDriver::~ByyHdoseDriver()
{
	qDeleteAll(myEntityObjects);
	qDeleteAll(myControlObjects);
}

bool ByyHdoseDriver::onStart()
{
	return true;
}

bool ByyHdoseDriver::onStop()
{
	{
		myIncompleteEntObjs.clear();

		EntityObjectMap::iterator it=myEntityObjects.begin();

		while (it!=myEntityObjects.end())
		{
			myObjectManager.removeEntity(*it);
			delete *it;
			it=myEntityObjects.erase(it);
		}
	}

	{
		ControlObjectMap::iterator it=myControlObjects.begin();

		while (it!=myControlObjects.end())
		{
			myObjectManager.removeContrlObject(*it);
			delete *it;
			it=myControlObjects.erase(it);
		}
	}

	return true;
}

bool ByyHdoseDriver::onTick()
{
	processEvent();
	processIncomplete();
	tickEntity();
	tickControlObject();
	return true;
}

void ByyHdoseDriver::processIncomplete()
{
	EntityObjectList::iterator it=myIncompleteEntObjs.begin();

	for(;it!=myIncompleteEntObjs.end();)
	{

		if ((*it)->complete())
		{
			ByyAssemble assemble=myHdose.opd().findAssemble((*it)->entityType());

			if (assemble.valid())
			{
				(*it)->setDescribeEntry(assemble.toDescribeEntry());
			}

			myObjectManager.appendEntity(*it);

			myHdose.simEventLogDatabase()->appendLog(myHdose.simTime(),QString::fromLocal8Bit("实体 %1 创建\n").arg((*it)->name()));

			it=myIncompleteEntObjs.erase(it);
		}
		else
			++it;
	}
	
}

void ByyHdoseDriver::processEvent()
{
	while(!ByyHDoseEventPool::instance().isEmpty())
	{
		QSharedPointer<ByyHdoseEvent> event=ByyHDoseEventPool::instance().dequeue();

		switch(event->type())
		{
		case ByyHdoseEvent::EntityAdd:
			processEntityAdd(event.data());
			break;
		case ByyHdoseEvent::EntityRemoved:
			processEntityRemoved(event.data());
			break;
		case ByyHdoseEvent::CtrlObjAdd:
			processCtrlObjAdd(event.data());
			break;
		case ByyHdoseEvent::CtrlObjRemoved:
			processCtrlObjRemoved(event.data());
			break;
		}
	}
}

void ByyHdoseDriver::processEntityAdd( ByyHdoseEvent* event )
{
	ByyRemoteEntity *entObj=new ByyHdoseEntity(event->getObj());
	entObj->setDriverName(instanceName());
	entObj->setId(event->id());
	entObj->setForceType((ByyForceType)event->side());
	entObj->setLocal(false);

	myEntityObjects.insert(event->id(),entObj);

	myIncompleteEntObjs.append(entObj);
}

void ByyHdoseDriver::processEntityRemoved( ByyHdoseEvent* event )
{
	EntityObjectMap::iterator it=myEntityObjects.find(event->id());

	if (it!=myEntityObjects.end())
	{
		myHdose.simEventLogDatabase()->appendLog(myHdose.simTime(),QString::fromLocal8Bit("实体 %1 移除\n").arg(it.value()->name()));

		myIncompleteEntObjs.removeOne(*it);
		myObjectManager.removeEntity(*it);
		delete *it;
		myEntityObjects.erase(it);
	}
}

void ByyHdoseDriver::processCtrlObjAdd( ByyHdoseEvent* event )
{
	ByyControlObject *ctrlObj=NULL;

	switch(event->shapeType())
	{
	case ByyHdoseEvent::WayPoint:
		ctrlObj=new ByyWaypoint;
		break;
	case ByyHdoseEvent::Phaseline:
		ctrlObj=new ByyPhaseLine;
		break;
	case ByyHdoseEvent::Route:
		ctrlObj=new ByyRoute;
		break;
	case ByyHdoseEvent::Circle:
		ctrlObj=new ByyCircle;
		break;
	case ByyHdoseEvent::Polygon:
		ctrlObj=new ByyPolygon;
		break;
	case ByyHdoseEvent::Ellipse:
		ctrlObj=new ByyEllipse;
		break;
	case ByyHdoseEvent::Rectangle:
		ctrlObj=new ByyRectangle;
		break;
	}

	if (ctrlObj)
	{
		ctrlObj->setId(event->id());
		ctrlObj->setForceType((ByyForceType)event->side());
		ctrlObj->setLocal(false);
		ctrlObj->setDriverName(instanceName());

		myObjectManager.appendContrlObject(ctrlObj);

		myControlObjects.insert(event->id(),ctrlObj);
	}
}

void ByyHdoseDriver::processCtrlObjRemoved( ByyHdoseEvent* event )
{
	ControlObjectMap::iterator it=myControlObjects.find(event->id());

	if (it!=myControlObjects.end())
	{
		myObjectManager.removeContrlObject(*it);
		delete *it;
		myControlObjects.erase(it);
	}
}

#include "ByySatelliteSR.h"
#include "ByyWeaponSysSR.h"
#include "ByySensorSR.h"

void ByyHdoseDriver::tickEntity()
{
	CEngine* pEngine = GetEngine(0);

	if (!pEngine)return;
	if (myEntityObjects.isEmpty())return;
	pEngine->Wait();
	//通用状态
	{
		int count=50;
		const int MAX_NUM = 100;
		CObjList * pObjListArray[ MAX_NUM ];

		pEngine->LookupAllObjFromClass("JSEntitySR", pObjListArray, count );

		//JSEntitySR* sr= JSEntitySR::LookupSr("水雷 1",pEngine);
		for (int i=0;i!=count;++i)
		{
			if (pObjListArray[i]->GetCount()<=0) 
				continue;

			POSITION pos=pObjListArray[i]->GetHeadPosition();

			while(pos)
			{
				JSEntitySR* psr = (JSEntitySR*)pObjListArray[i]->GetNext(pos);

				// 远程对象才请求更新
				if( !psr->IsInitiative() )
					psr->RequestUpdate();

				EntityObjectMap::iterator it=myEntityObjects.find(psr->GetID());

				if (it==myEntityObjects.end())
					continue;

				(*it)->setForceType((ByyForceType)psr->GetSide());

				if (psr->GetEntityName())
				{
					(*it)->setName(QString::fromLocal8Bit(psr->GetEntityName()));

					//qDebug()<<QString::fromLocal8Bit("------实体名称：--------")<<QString::fromLocal8Bit(psr->GetEntityName());
				}


				if (psr->m_Superior)
				{
					(*it)->setSuperiorName(QString::fromLocal8Bit(psr->m_Superior));
				}
				

				(*it)->setLevel(psr->GetLevel());

				//联机时有问题，注释掉后解决
				//(*it)->setItemClass(psr->GetEntity()->GetClsInfo()->className);

				StEntityType entType=psr->GetEntityType();

				(*it)->setEntityType(*(ByyEntityType*)&entType);

				(*it)->setPosition(osg::Vec3d(psr->m_Position.Lon(),psr->m_Position.Lat(),psr->m_Position.Elev()));

				//	printf("%f,%f,%f\n",psr->m_Position.Lon(),psr->m_Position.Lat(),psr->m_Position.Elev());

				(*it)->setOrientation(osg::Vec3d(psr->m_Heading,psr->m_Pitch,psr->m_Roll));

				(*it)->setSpeed(psr->m_Velocity);
				//TODO:模型没有设置 需要设置 2025.05.24
				(*it)->setHealthPoint(psr->m_HitPoint);

				(*it)->setVelocity(osg::Vec3d(psr->m_VelocityVector.x,psr->m_VelocityVector.y,psr->m_VelocityVector.z));

				//(*it)->setDamageState(psr->m_SurvivalState==0?ByyDamageNone:ByyDamageDestroyed);
				// xks 20230607
				ByyDamageState dmgState = ByyDamageNone;
				switch(psr->m_SurvivalState)
				{
				case Good:
					dmgState = ByyDamageNone;
					break;
				case MovabilityWound:
					dmgState = ByyDamageSlight;
					break;
				case FirePowerWound:
					dmgState = ByyDamageSlight;
					break;
				case SeriousWound:
					dmgState = ByyDamageModerate;
					break;
				case	Destroyed:
					dmgState = ByyDamageDestroyed;
					break;
				default:
					dmgState = ByyDamageNone;
					break;
				}
				//////
				(*it)->setDamageState( dmgState ); // xks, 20230607

				//TODO:需要确认，没有走到相关的分支 2025.05.24
				if(psr->IsDrivedFrom("JSWeaponESR"))
				{
					JSWeaponESR* esrObject = dynamic_cast<JSWeaponESR*>(psr);
					(*it)->setTargetName( esrObject->GetTargetName()? QString::fromLocal8Bit(esrObject->GetTargetName()):"" );
				}
			}
		}
	}

	//卫星状态
	{
		int count=50;

		const int MAX_NUM = 100;
		CObjList * pObjListArray[ MAX_NUM ];

		pEngine->LookupAllObjFromClass("JSSatelliteESR", pObjListArray, count );

		for (int i=0;i!=count;++i)
		{
			if (pObjListArray[i]->GetCount()<=0) 
				continue;

			POSITION pos=pObjListArray[i]->GetHeadPosition();

			while(pos)
			{
				JSSatelliteESR* psr = (JSSatelliteESR*)pObjListArray[i]->GetNext(pos);

				EntityObjectMap::iterator it=myEntityObjects.find(psr->GetID());

				if (it==myEntityObjects.end())
					continue;

				ByyRemoteEntity *ent=it.value();

				ByySatelliteSR *satelliteEsr=ent->getOrCreateSR<ByySatelliteSR>();

				StOrbitCoe hOrbit=psr->GetOrbit();
				ByySatelliteSR::ByyOrbitData snOrbit;

				memcpy(&snOrbit,&hOrbit,sizeof(hOrbit));

				satelliteEsr->setOrbitData(snOrbit);

			}
		}
	}

	//武器系统
	if (osg::Timer::instance()->time_s()-myLastUpdateWeaponTime>=2)
	{
		myLastUpdateWeaponTime=osg::Timer::instance()->time_s();

		int count=50;

		const int MAX_NUM = 100;
		CObjList * pObjListArray[ MAX_NUM ];

		pEngine->LookupAllObjFromClass("JSWeaponSysModelSR", pObjListArray, count );

		QMap<ByyRemoteEntity*,QList<JSWeaponSysModelSR*>> myWeaponSysList;

		for (int i=0;i!=count;++i)
		{
			int cc=pObjListArray[i]->GetCount();
			if (pObjListArray[i]->GetCount()<=0) 
				continue;

			POSITION pos=pObjListArray[i]->GetHeadPosition();

			while(pos)
			{
				JSWeaponSysModelSR* psr = (JSWeaponSysModelSR*)pObjListArray[i]->GetNext(pos);

				EntityObjectMap::iterator it=myEntityObjects.find(psr->GetEntityID());

				if (it==myEntityObjects.end())
					continue;

				myWeaponSysList[it.value()].append(psr);

			}
		}

		QMap<ByyRemoteEntity*,QList<JSWeaponSysModelSR*>>::iterator it=myWeaponSysList.begin();

		for (;it!=myWeaponSysList.end();++it)
		{
			ByyRemoteEntity *ent=it.key();

			ByyWeaponSysSR *weponSysSR=ent->getOrCreateSR<ByyWeaponSysSR>();

			QList<JSWeaponSysModelSR*>& msrList=it.value();

			ByyWeaponSysSR::WeaponSystemMap weaponSystemMap;

			for (int i=0;i!=msrList.size();++i)
			{
				JSWeaponSysModelSR* psr=msrList[i];

				ByyWeaponSysSR::WeaponSystem& weaponSys=weaponSystemMap[QString::fromLocal8Bit(psr->GetModelTag())];

				//weaponSys.setWeaponCount(psr->m_WeaponCount); // xks, 注释
				weaponSys.setWeaponRecCount(psr->m_WeaponRecCount);

				ByyWeaponSysSR::WeaponRecSmpDataList recDataList(psr->m_WeaponRecs.m_len);
				long weaponCount=0; // xks
				for (int i=0;i!=recDataList.size();++i)
				{
					recDataList[i].m_Available=psr->m_WeaponRecs[i].m_CountAvailable;
					recDataList[i].m_Count=psr->m_WeaponRecs[i].m_Count;
					recDataList[i].m_Name=QString::fromLocal8Bit(psr->m_WeaponRecs[i].m_Name);
					recDataList[i].m_TimeToFire=psr->m_WeaponRecs[i].m_LeftTimeToFire;
					recDataList[i].m_WeaponRecID=psr->m_WeaponRecs[i].m_WeaponRecID;
					weaponCount += psr->m_WeaponRecs[i].m_Count;
				}
				weaponSys.setWeaponCount(weaponCount); // xks
				weaponSys.setWeaponRecSmpData(recDataList);
			}

			weponSysSR->setSystemList(weaponSystemMap);

		}
	}

	//传感器系统
	if (osg::Timer::instance()->time_s()-myLastUpdateSensorTime>=0.01)
	{
		myLastUpdateSensorTime=osg::Timer::instance()->time_s();

		int count=50;

		const int MAX_NUM = 100;
		CObjList * pObjListArray[ MAX_NUM ];

		pEngine->LookupAllObjFromClass("JSSensorModelSR", pObjListArray, count );

		QMap<ByyRemoteEntity*,QList<JSSensorModelSR*>> mySensorModelList;

		for (int i=0;i!=count;++i)
		{
			int cc=pObjListArray[i]->GetCount();
			if (pObjListArray[i]->GetCount()<=0) 
				continue;

			POSITION pos=pObjListArray[i]->GetHeadPosition();

			while(pos)
			{
				JSSensorModelSR* psr = (JSSensorModelSR*)pObjListArray[i]->GetNext(pos);

				EntityObjectMap::iterator it=myEntityObjects.find(psr->GetEntityID());

				if (it==myEntityObjects.end())
					continue;

				mySensorModelList[it.value()].append(psr);
			}
		}

		QMap<ByyRemoteEntity*,QList<JSSensorModelSR*>>::iterator it=mySensorModelList.begin();

		for (;it!=mySensorModelList.end();++it)
		{
			ByyRemoteEntity *ent=it.key();

			ByySensorSR *sensorEsr=ent->getOrCreateSR<ByySensorSR>();

			QList<JSSensorModelSR*>& msrList=it.value();

			for (int i=0;i!=msrList.size();++i)
			{
				JSSensorModelSR* psr=msrList[i];

				sensorEsr->setBeamWidth(QString::fromLocal8Bit(psr->GetModelTag()), 5.0 ); //psr->m_BeamWidthHori
				sensorEsr->setAntennaAz(QString::fromLocal8Bit(psr->GetModelTag()), 5.0); //psr->m_AntennaAz
				sensorEsr->setDevicePower(QString::fromLocal8Bit(psr->GetModelTag()),psr->m_PowerOn);

				ByySensorSR::DetectDataList detectDataList(psr->m_Tracks.m_len);

				for (int i=0;i!=detectDataList.size();++i)
				{
					detectDataList[i].m_UpdateTime=psr->m_Tracks[i].m_UpdateTime;
					detectDataList[i].m_ContinueTime=psr->m_Tracks[i].m_ContinueTime; //chenpeng 2024.05.18 模型数据没有写入
					detectDataList[i].m_BatchNo=psr->m_Tracks[i].m_BatchNo;
					detectDataList[i].m_TargetID=psr->m_Tracks[i].m_TargetID;
					detectDataList[i].m_TargetName=QString::fromLocal8Bit(psr->m_Tracks[i].m_TargetName);
					detectDataList[i].m_CID=psr->m_Tracks[i].m_CID;
					detectDataList[i].m_Side=psr->m_Tracks[i].m_Side;
					detectDataList[i].m_EntityType=*(ByyEntityType*)&psr->m_Tracks[i].m_TargetEntType;
					detectDataList[i].m_ThreatLevel=psr->m_Tracks[i].m_ThreatLevel;
					detectDataList[i].m_Pos=GeoPt2Vec3d(psr->m_Tracks[i].m_Position);
					detectDataList[i].m_Velocity=psr->m_Tracks[i].m_Velocity;
					detectDataList[i].m_Course=psr->m_Tracks[i].m_Course;
					detectDataList[i].m_Distance=psr->m_Tracks[i].m_Distance;
					detectDataList[i].m_El=psr->m_Tracks[i].m_El;
					detectDataList[i].m_Az=psr->m_Tracks[i].m_Az;
					detectDataList[i].m_RadiationType=psr->m_Tracks[i].m_EmitterData.m_EmitterType;
					detectDataList[i].m_RadiationPt=psr->m_Tracks[i].m_EmitterData.m_EmitterPt;
					detectDataList[i].m_RadiationFreq=psr->m_Tracks[i].m_EmitterData.m_EmitterFreq;
					detectDataList[i].m_RadiationTu=psr->m_Tracks[i].m_EmitterData.m_EmitterPulseTu;
					detectDataList[i].m_RadiationPRF=psr->m_Tracks[i].m_EmitterData.m_EmitterPRF;
					detectDataList[i].m_RadiationSignalType=psr->m_Tracks[i].m_EmitterData.m_EmitterWaveType;
					detectDataList[i].m_RadiationPulseType=psr->m_Tracks[i].m_EmitterData.m_EmitterPulseModulateType;

					if (detectDataList[i].m_Pos==osg::Vec3d())
					{
						osg::Vec3d entPos=ent->position();
						double out_latRad,out_lonRad,out_alt;

						osgEarth::GeoMath::destination(osg::DegreesToRadians(entPos.y()),
							osg::DegreesToRadians(entPos.x()),osg::DegreesToRadians(detectDataList[i].m_Az),
							detectDataList[i].m_Distance,out_latRad,out_lonRad);

						out_alt=detectDataList[i].m_Distance*sin(osg::DegreesToRadians(detectDataList[i].m_El))+entPos.z();

						detectDataList[i].m_Pos=osg::Vec3d(osg::RadiansToDegrees(out_lonRad),
							osg::RadiansToDegrees(out_latRad),out_alt);
					}
				}

				sensorEsr->setDeviceDetectData(QString::fromLocal8Bit(psr->GetModelTag()),detectDataList);
			}
		}
	}

	pEngine->Release();
}

void ByyHdoseDriver::tickControlObject()
{
	CEngine* pEngine = GetEngine(0);

	if (!pEngine)return;

	if (myControlObjects.isEmpty())return;

	//WayPoint
	{
		CObjList *PPointList=(CObjList *)pEngine->LookupObjFromClass("JSWayPoint");

		if (PPointList&&PPointList->GetCount()>0)
		{
			POSITION pos=PPointList->GetHeadPosition();

			while(pos)
			{
				JSWayPoint *hwp=(JSWayPoint*) PPointList->GetNext(pos);

				if(!hwp)
					continue;

				ControlObjectMap::iterator it=myControlObjects.find(hwp->GetID());

				if (it==myControlObjects.end())
					continue;

				ByyWaypoint* bwp=dynamic_cast<ByyWaypoint*>(*it);

				if (bwp)
				{
					bwp->setPosition(GeoPt2Vec3d(hwp->GetPoint().toGeoPt()));
					bwp->setName(QString::fromLocal8Bit(hwp->GetCtrlObjName()));
					bwp->setForceType((ByyForceType)hwp->GetSide());
				}
			}
		}
	}

	//Phaseline
	/*{
		CObjList *PPointList=(CObjList *)pEngine->LookupObjFromClass("CPhaseline");

		if (PPointList&&PPointList->GetCount()>0)
		{
			POSITION pos=PPointList->GetHeadPosition();

			while(pos)
			{
				CPhaseline *hpl=(CPhaseline*) PPointList->GetNext(pos);

				if(!hpl)
					continue;

				ControlObjectMap::iterator it=myControlObjects.find(hpl->GetID());

				if (it==myControlObjects.end())
					continue;

				ByyPhaseLine* bpl=dynamic_cast<ByyPhaseLine*>(*it);

				if (bpl)
				{
					bpl->setStart(GeoPt2Vec3d(hpl->GetStartPoint().toGeoPt()));
					bpl->setEnd(GeoPt2Vec3d(hpl->GetEndPoint().toGeoPt()));
					bpl->setName(QString::fromLocal8Bit(hpl->GetCtrlObjName()));
					bpl->setForceType((ByyForceType)hpl->GetSide());
				}
			}
		}
	}*/

	//Route
	{
		CObjList *PPointList=(CObjList *)pEngine->LookupObjFromClass("JSRoute");

		if (PPointList&&PPointList->GetCount()>0)
		{
			POSITION pos=PPointList->GetHeadPosition();

			while(pos)
			{
				JSRoute *hpl=(JSRoute*) PPointList->GetNext(pos);

				if(!hpl || hpl->m_PointCount <=0)
					continue;

				ControlObjectMap::iterator it=myControlObjects.find(hpl->GetID());

				if (it==myControlObjects.end())
					continue;

				QVector<osg::Vec3d> points;

				points.resize(hpl->GetPointCount());

				GeoPtList ptList;

				hpl->GetPointsArray(ptList);
				convertPointList(ptList, points);
				
				ByyRoute* bpl=dynamic_cast<ByyRoute*>(*it);
				if (bpl)
				{
					bpl->setName(QString::fromLocal8Bit(hpl->GetCtrlObjName()));
					bpl->setForceType((ByyForceType)hpl->GetSide());
					bpl->setPoints(points);
				}
			}
		}
	}

	//Circle
	{
		CObjList *PPointList=(CObjList *)pEngine->LookupObjFromClass("JSCircle");

		if (PPointList&&PPointList->GetCount()>0)
		{
			POSITION pos=PPointList->GetHeadPosition();

			while(pos)
			{
				JSCircle *hpl=(JSCircle*) PPointList->GetNext(pos);

				if(!hpl)
					continue;

				ControlObjectMap::iterator it=myControlObjects.find(hpl->GetID());

				if (it==myControlObjects.end())
					continue;

				ByyCircle* bpl=dynamic_cast<ByyCircle*>(*it);

				if (bpl)
				{
					bpl->setPosition(GeoPt2Vec3d(hpl->GetCenterPos().toGeoPt()));
					bpl->setRadius(hpl->GetRadius());
					bpl->setName(QString::fromLocal8Bit(hpl->GetCtrlObjName()));
					bpl->setForceType((ByyForceType)hpl->GetSide());
				}
			}
		}
	}

	//Polygon
	{
		CObjList *PPointList=(CObjList *)pEngine->LookupObjFromClass("JSPolygon");

		if (PPointList&&PPointList->GetCount()>0)
		{
			POSITION pos=PPointList->GetHeadPosition();

			while(pos)
			{
				JSPolygon *hpl=(JSPolygon*) PPointList->GetNext(pos);

				if(!hpl)
					continue;

				ControlObjectMap::iterator it=myControlObjects.find(hpl->GetID());

				if (it==myControlObjects.end())
					continue;

				ByyPolygon* bpl=dynamic_cast<ByyPolygon*>(*it);

				QVector<osg::Vec3d> points;

				points.resize(hpl->GetPointCount());

				GeoPtList ptList;
				hpl->GetPointsArray(ptList);
				convertPointList(ptList, points);

				if (bpl)
				{
					bpl->setName(QString::fromLocal8Bit(hpl->GetCtrlObjName()));
					bpl->setForceType((ByyForceType)hpl->GetSide());
					bpl->setPoints(points);
				}
			}
		}
	}

	//Ellipse
	{
		CObjList *PPointList=(CObjList *)pEngine->LookupObjFromClass("JSEllipse");

		if (PPointList&&PPointList->GetCount()>0)
		{
			POSITION pos=PPointList->GetHeadPosition();

			while(pos)
			{
				JSEllipse *hpl=(JSEllipse*) PPointList->GetNext(pos);

				if(!hpl)
					continue;

				ControlObjectMap::iterator it=myControlObjects.find(hpl->GetID());

				if (it==myControlObjects.end())
					continue;

				ByyEllipse* bpl=dynamic_cast<ByyEllipse*>(*it);

				if (bpl)
				{
					bpl->setPosition(GeoPt2Vec3d(hpl->GetCenterPos().toGeoPt()));
					bpl->setMajorSemiAxis(hpl->GetMajorSemiAxis());
					bpl->setMinorSemiAxis(hpl->GetMinorSemiAxis());
					bpl->setName(QString::fromLocal8Bit(hpl->GetCtrlObjName()));
					bpl->setForceType((ByyForceType)hpl->GetSide());
				}
			}
		}
	}

	//Rectangle
	{
		CObjList *PPointList=(CObjList *)pEngine->LookupObjFromClass("JSRectangle");

		if (PPointList&&PPointList->GetCount()>0)
		{
			POSITION pos=PPointList->GetHeadPosition();

			while(pos)
			{
				JSRectangle *hpl=(JSRectangle*) PPointList->GetNext(pos);

				if(!hpl)
					continue;

				ControlObjectMap::iterator it=myControlObjects.find(hpl->GetID());

				if (it==myControlObjects.end())
					continue;

				ByyRectangle* bpl=dynamic_cast<ByyRectangle*>(*it);

				QVector<osg::Vec3d> points;

				points.resize(hpl->GetPointCount());

				GeoPtList ptList;
				hpl->GetPointsArray(ptList);
				convertPointList(ptList,points);

				if (bpl)
				{
					bpl->setName(QString::fromLocal8Bit(hpl->GetCtrlObjName()));
					bpl->setForceType((ByyForceType)hpl->GetSide());
					bpl->setPoints(points);
				}
			}
		}
	}
}

void ByyHdoseDriver::setCurrentScenario( const QString& scenarioFile )
{
	myScenarioFilename=scenarioFile;
}

void ByyHdoseDriver::startScenario()
{
	/*
	CEngine *pEngine = GetEngine(0);
	if (!pEngine)
		return;

	//1、发送加载想定命令
	if(pEngine->GetState() == STATUS_PAUSE)
		//ResumeEngine();
		pEngine->Resume();

	CScnCtrl * pScnCtrl = dynamic_cast <CScnCtrl*> (pEngine->CreateEvt("CScnCtrl"));
	if (!pScnCtrl)
		return;
	pScnCtrl->m_desobjid = 0;				// 远程本地都接收
	pScnCtrl->SetFileName(myScenarioFilename.toAscii().data());
	pScnCtrl->SetScnCtrlType(2);			//2表示[加载]

	pScnCtrl->Send();
	delete pScnCtrl;*/
}