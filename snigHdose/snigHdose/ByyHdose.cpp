#include "ByyHdose.h"
#include "hdoseEvent.h"
#include "hdoseEventPool.h"
#include "hdoseDriver.h"
#include "ByyCore.h"
#include "driverManager.h"
#include "opd.h"
#include "rsm.h"
#include "ByyAttributeEditMng.h"
#include "simuController.h"
#include "scenarioManager.h"
#include "enums.h"
#include "localObjectManager.h"
#include "entityObject.h"
#include "ByyControlObject.h"
#include "remoteObjectManager.h"
#include "hdoseSettings.h"
#include "ByyUserOperateLog.h"
#include "ByyRelativePath.h"
#include "foreStationProcessor.h"

#include <QDir>
#include "JSCGFSimDS/JSCGFSimDS.h"
#include "JSCGFSimDS/BaseDataType.h"
#include "JSCGFSimEvt/JSCGFSimEvt.h"
#include "JSCGFSimTsk/JSCGFSimTsk.h"
#include "JSCGFSimMF/JSCGFSimMF.h"
#include "JSCoreAppDS/JSCoreAppDS.h"
#include "JSCGFSimMF/JSScnProfile.h"
#include "scenarioManager.h"
#include "scenarioV2.h"
#include "JSCGFSimDS/StEntityType.h"
#include <QtNetwork>
#include "coreUtils.h"

ByyHdose::ByyHdose(ByyCore& core,QObject *parent)
	: QObject(parent)
	,myCore(core)
	,myOpd(0)
	,myRsm(0)
	,myScenarioManager(0)
	,myStarted(false)
	,myConnectMode(End)
{
	myDriver=new ByyHdoseDriver(myCore.remoteObjectManager(),*this);

	myCore.driverManager().addDriver(myDriver);

	myOpd=new ByyOpd;

	myRsm=new ByyRsm;

	myAEMng = ByyAttributeEditMng::instance();

	myScenarioManager=new ByyScenarioManager(*this);

	mySimuController=new ByySimuController(core, *this);

	mySimEventLogDatabase=new ByyHdoseSimEventLogDatabase;

	myForeStationProcessor=new ByyForeStationProcessor(*this);

	connect(myScenarioManager,SIGNAL(signal_scenarioClosed()),mySimuController,SLOT(closeScenario()));

	connect(mySimuController,SIGNAL(signal_scnLoaded()),this,SLOT(onBackScnLoaded()));
	connect(mySimuController,SIGNAL(signal_backExited()),this,SLOT(onBackExited()));
	connect(mySimuController,SIGNAL(signal_StatusChanged(int)),this,SLOT(onBackStateChanged(int)));
	connect(mySimuController, SIGNAL(signal_reloadScn()), this, SIGNAL(signal_scnLoaded()));

	connect(this,SIGNAL(signal_started()),this,SIGNAL(signal_runningChanged()));
	connect(this,SIGNAL(signal_stopped()),this,SIGNAL(signal_runningChanged()));

	connect(&myCore.remoteObjectManager(), SIGNAL(removeEntityObject(int, const QString&, const QString&)), this, SIGNAL(signal_entityDm(int, const QString&, const QString&)));
	connect(&myCore.remoteObjectManager(), SIGNAL(addMissileEntityObject(int, const QString&, const QString&)), this, SIGNAL(signal_MissileLaunch(int, const QString&, const QString&)));
	
	//connect(&core,SIGNAL(postTick()),this,SLOT(onTick()));
	connect(&timer,SIGNAL(timeout()),SLOT(onTick()));
	//timer.start(300);

	m_currNum = 1;
}

ByyHdose::~ByyHdose()
{
	stop();

	delete myForeStationProcessor;
	delete mySimEventLogDatabase;
	delete myOpd;
	delete myRsm;
	delete mySimuController;
	delete myScenarioManager;
}

void objMgrCallback( CEngine * pEng, DWORD type, LPVOID pObj )
{
	ByyHDoseEventPool::instance().enqueue(ByyHdoseEvent::create(pEng,type,pObj));
}

void logNotifyCallback(CEngine *pEng,char * content)
{
	ByyHdoseLogDatabase::instance().appendLog(QString::fromLocal8Bit(content));
}

void engineStateChangeCallback( CEngine * pEng, int s )
{
	if (s==STATUS_LOADING)
	{
		GetEngine(0)->SetLogFile("HJFront_SimEng.log",logNotifyCallback);
	}
	if( s == STATUS_LOADED )
	{
		pEng->RegisterNotify( (EngineNotifyCB)objMgrCallback );
	}
}

#include <Psapi.h>
bool ByyHdose::start( Mode mode,QString ip)
{
	ByyUserOperateLog::instance().addLog(QString::fromLocal8Bit("启动HDOSE"));

	QDir currentPath=QDir::current();

	if (mode==Single)
	{
		QString workDir=ByyHdoseSettings::instance().getSingleWorkDir();
		if (workDir.isEmpty())
		{
			workDir="../../app/Model/";
		}

		workDir=myRelativePath.resolvePath(workDir);

		//OutputDebugString(workDir.toLocal8Bit().data());

		QString simfile=ByyHdoseSettings::instance().getSingleSimFile();

		if (simfile.isEmpty())
		{
			simfile="JSModel_all.SIM";
		}

		QDir::setCurrent(workDir);
		StartHdose(simfile.toLocal8Bit().data(),1,engineStateChangeCallback,0,STATUS_PAUSE);
		/* // xks
		StartHdose(simfile.toLocal8Bit().data(),1,engineStateChangeCallback,0,4);
		if(GetEngine(0)->GetState()==STATUS_NULL)
			return false;
		while(GetEngine(0)->GetState()!=4)
		{
			Sleep(1);
		}
		GetEngine(0)->Pause();
		*/
	}
	else if(mode==Create)
	{
		QString hostname=QHostInfo::localHostName();
		QHostInfo info=QHostInfo::fromName(hostname);
		QString fip;
		foreach(QHostAddress addr,info.addresses())
		{
			if(addr.protocol()==QAbstractSocket::IPv4Protocol)
				fip=addr.toString();
		}
		qputenv("RTI_HOST",fip.toLatin1());
		
		bool netsrvOpend=false;
		DWORD aprs[1024],cbNeeded,cprs;
		if(EnumProcesses(aprs,sizeof(aprs),&cbNeeded))
		{
			QStringList lprs;
			cprs=cbNeeded/sizeof(DWORD);
			for(int i=0;i<cprs;i++)
			{
				if(aprs[i]==0)
					continue;
				HANDLE hprs=OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,0,aprs[i]);
				if(hprs!=NULL)
				{
					HMODULE hMode;
					if(EnumProcessModules(hprs,&hMode,sizeof(hMode),&cbNeeded))
					{
						char prname[MAX_PATH];
						GetModuleBaseName(hprs,hMode,prname,MAX_PATH);
						if(strcmp(prname,"QNetSrv.exe")==0)
						{
							netsrvOpend=true;
							break;
						}
					}
					
				}
				CloseHandle(hprs);
			}
		}

		if(!netsrvOpend)
			QProcess::startDetached(ByyHdoseSettings::instance().getLocalSvr());

		QString workDir=ByyHdoseSettings::instance().getCreateWorkDir();
		if (workDir.isEmpty())
		{
			workDir="../../app/Model/";
		}

		workDir=myRelativePath.resolvePath(workDir);

		//OutputDebugString(workDir.toLocal8Bit().data());
		QString simfile=ByyHdoseSettings::instance().getCreateSimFile();

		if (simfile.isEmpty())
		{
			simfile="JSModel_all_create.SIM";
		}

		QDir::setCurrent(workDir);
		StartHdose(simfile.toLocal8Bit().data(),1,engineStateChangeCallback,0,STATUS_PAUSE);

	}
	else if(mode==Join)
	{
		qputenv("RTI_HOST",ip.toLatin1());

		QString workDir=ByyHdoseSettings::instance().getJoinWorkDir();
		if (workDir.isEmpty())
		{
			workDir="../../app/Model/";
		}

		workDir=myRelativePath.resolvePath(workDir);

		//OutputDebugString(workDir.toLocal8Bit().data());
		QString simfile=ByyHdoseSettings::instance().getJoinSimFile();

		if (simfile.isEmpty())
		{
			simfile="JSModel_all_join.SIM";
		}

		QDir::setCurrent(workDir);
		StartHdose(simfile.toLocal8Bit().data(),1,engineStateChangeCallback,0,STATUS_PAUSE);
	}

	mySimuController->init(mode);

	myConnectMode=mode;

	QDir::setCurrent(currentPath.absolutePath());

	myCore.driverManager().startDriver(myDriver);

	myStarted=true;

	emit signal_started();

//	myCore.localObjectManager().setVisible(false);
	timer.start(300); 
	return true;
}

bool ByyHdose::stop()
{
	myStarted=false;
	emit signal_stopped();
	timer.stop();

	mySimuController->release();

	myCore.driverManager().stopDriver(myDriver);

	Sleep(200);
	EndHdose(0);
	Sleep(200);

	//myStarted=false;
	//emit signal_stopped();

	myCore.localObjectManager().setVisible(true);

	ByyUserOperateLog::instance().addLog(QString::fromLocal8Bit("停止HDOSE"));

	return true;
}

bool ByyHdose::started() const
{
	return myStarted;
}

ByyOpd& ByyHdose::opd()
{
	return *myOpd;
}

ByyRsm& ByyHdose::rsm()
{
	return *myRsm;
}

ByyAttributeEditMng& ByyHdose::attrEditTypeMng()
{
	return *myAEMng;
}

ByyCore& ByyHdose::core()
{
	return myCore;
}

ByySimuController& ByyHdose::simuController()
{
	return *mySimuController;
}

ByyScenarioManager& ByyHdose::scenarioManager()
{
	return *myScenarioManager;
}

ByyHdose::Mode ByyHdose::mode()
{
	return myConnectMode;
}

void ByyHdose::onBackScnLoaded()
{
	myCore.localObjectManager().setVisible(false);
	emit signal_scnLoaded();
}

void ByyHdose::onBackExited()
{
	myCore.localObjectManager().setVisible(true);
}

void ByyHdose::onBackStateChanged(int state)
{
	if (mySimuController->isRunning())
	{
		myCore.localObjectManager().setVisible(false);
	}
}

#include "JSCGFSimMF/JSEntitySR.h"
#include <qdebug.h>

void ByyHdose::onTick()
{
	if(!GetEngine(0) || GetEngine(0)->GetState()<=0)
		return;

	GetEngine(0)->Wait();//获取互斥区

	//通用状态
	{
		int count=50;

		const int MAX_NUM = 100;
		CObjList * pObjListArray[ MAX_NUM ];
		
		GetEngine(0)->LookupAllObjFromClass("JSEntitySR", pObjListArray, count );

		for (int i=0;i<count;++i)
		{
			if (pObjListArray[i]->GetCount()<=0) 
				continue;

			POSITION pos=pObjListArray[i]->GetHeadPosition();

			while(pos)
			{
				JSEntitySR* psr = (JSEntitySR*)pObjListArray[i]->GetNext(pos);
				if(psr)
				{
					// 远程对象才请求更新
					//if( !psr->IsInitiative() )
					//	psr->RequestUpdate();

					if(psr->m_EntityType.m_kind == 1 && psr->m_EntityType.m_domain == 3)  //船
					{
						if(psr->m_SurvivalState >= 3)
						{
							emit signal_entityDm(psr->m_Side, QString::fromLocal8Bit(psr->GetEntityName()));
						}
					}

					if(psr->m_EntityType.m_kind == 2)
					{
						int type = 1;
						if(psr->m_EntityType.m_domain == 1)
							type = 2;
						else
							type = 1;
						emit signal_misslieInfo(psr->GetID(), psr->GetPosition().Elev(), simTime(), psr->GetVelocity(), psr->GetEntityName(), type);
					}
				}

			}
		}
	}

	GetEngine(0)->Release();//释放互斥区

	//!2
	
	ByyScenarioV2* scenarioV2 = scenarioManager().scenario();
	ByyScenarioV2::ScenarioInfo scenarioInfo;
	if(scenarioV2)		// xks, 20220406
		scenarioInfo = scenarioV2->getScnInfo();
	double t_time = scenarioInfo.myStartDateTime.secsTo(scenarioInfo.myEndDateTime);

	if(t_time <= 0)
		return;

	if(simTime() > t_time)
	{
		if(m_currNum < scenarioInfo.m_ScnRunN)
		{
			m_currNum++;
			//重置
			GetEngine(0)->Pause();
			mySimuController->closeScenario();

			//!2
			CParaEntry* Fedpara = (CParaEntry*)GetEngine(0)->LookupObj("CParaEntry", "FedParameter");
			if(Fedpara)
			{
				ByyScenarioV2* scenarioV2 = scenarioManager().scenario();
				ByyScenarioV2::ScenarioInfo scenarioInfo;
				scenarioInfo = scenarioV2->getScnInfo();
				Fedpara->SetParameterValue("ExternScnParaSet", "1");
				Fedpara->SetParameterValue("ScnRunN", QString::number(m_currNum).toStdString().c_str());
			}

			mySimuController->loadScenario((char*)(myScenarioManager->scenario()->filename().toStdString().c_str()));
			mySimuController->start();
		}
		else
		{
			//结束
			stop();
		}
	}

}

ByyEntityObject* ByyHdose::createEntity( const QString& assemble,const ByyKeyValueList& keyValues )
{
	ByyAssemble assembleObj=myOpd->findAssemble(assemble);

	if (assembleObj.valid())
	{
		ByyEntityObject* ent=myCore.localObjectManager().createEntity(assembleObj.getEntityType(),myRsm->getParametersFromEsr(assembleObj.getEsrItemClass()),keyValues,assembleObj.toDescribeEntry());

		ent->setItemClass(assemble);

		if (mySimuController->scnLoaded())
		{
			//mySimuController->cr
		}

		return ent;
	}
	else
		return 0;
}

ByyEntityObject* ByyHdose::createEntity( const QString& assemble,ByyForceType ft,const osg::Vec3d& position )
{
	ByyAssemble assembleObj=myOpd->findAssemble(assemble);

	if (assembleObj.valid())
	{
		ByyEntityObject* ent=myCore.localObjectManager().createEntity(assembleObj.getEntityType(),myRsm->getParametersFromEsr(assembleObj.getEsrItemClass()),assembleObj.myTag,ft,assembleObj.toDescribeEntry());

		ent->setItemClass(assemble);
		ent->setPosition(position);

		/*if (mySimuController->scnLoaded())
		{
			mySimuController->createObject(1,assemble,ent->getAttributeValueSet());
		}*/

		return ent;
	}
	else
		return 0;
}

void ByyHdose::removeEntity( long id,const QString& name )
{
	if (mySimuController->scnLoaded())
	{
		mySimuController->removeObject(1,id);
		myCore.localObjectManager().removeEntity(name);
	}
}

void ByyHdose::removeControlObject( long id,const QString& name )
{
	if (mySimuController->scnLoaded())
	{
		mySimuController->removeObject(2,id);
		myCore.localObjectManager().removeControlObject(name);
	}
}

void ByyHdose::appendControlObject( ByyControlObject *obj,ByyForceType ft )
{
	myCore.localObjectManager().appendControlObject(obj,ft);

	if (mySimuController->scnLoaded())
	{
		mySimuController->createObject(2,obj->itemClass(),obj->getAttributeValueSet());
	}
}

#include "moveToTask.h"
#include "moveAlongRouteTask.h"
#include "patrolRouteTask.h"
#include "patrolAreaTask.h"
#include "waitTask.h"
#include "followTask.h"
#include "cruiseTask.h"
#include "launchMissileTask.h"
#include "takeOffTask.h"
#include "sensorOperTask.h"

void ByyHdose::sendTask( ByyTask *task )
{
	if (!mySimuController->scnLoaded()||task==0)
		return;

	ByyUserOperateLog::instance().addLog(QString::fromLocal8Bit("发送任务：")+task->string());

	int recverId=task->entity().id();

	if (task->typeInfo().type==ByyMoveToTask::theTypeInfo().type)
	{
		ByyMoveToTask *moveto=dynamic_cast<ByyMoveToTask*>(task);

		mySimuController->sendMoveToTask(recverId,moveto->waypoint());
	}
	else if (task->typeInfo().type==ByyMoveAlongRouteTask::theTypeInfo().type)
	{
		ByyMoveAlongRouteTask *moveAlong=dynamic_cast<ByyMoveAlongRouteTask*>(task);

		mySimuController->sendMoveAlongTask(recverId,moveAlong->route());
	}
	else if (task->typeInfo().type==ByyPatrolAreaTask::theTypeInfo().type)
	{
		ByyPatrolAreaTask *patrolTask=dynamic_cast<ByyPatrolAreaTask*>(task);

		mySimuController->sendPatrolTask(recverId,3,patrolTask->area(),patrolTask->patrolTime());
	}
	else if (task->typeInfo().type==ByyPatrolRouteTask::theTypeInfo().type)
	{
		ByyPatrolRouteTask *patrolTask=dynamic_cast<ByyPatrolRouteTask*>(task);

		mySimuController->sendPatrolTask(recverId,2,patrolTask->route(),patrolTask->patrolTime());

	}
	else if (task->typeInfo().type==ByyWaitTask::theTypeInfo().type)
	{
		ByyWaitTask *waitTask=dynamic_cast<ByyWaitTask*>(task);

		mySimuController->sendWaitTask(recverId,waitTask->waitTime());
	}
	else if (task->typeInfo().type==ByyFollowTask::theTypeInfo().type)
	{
		ByyFollowTask *followTask=dynamic_cast<ByyFollowTask*>(task);

		mySimuController->sendFollowTask(recverId,followTask->followedObj(),
			followTask->distance(),
			followTask->relativeOrientation(),
			followTask->highOffset());
	}
	else if (task->typeInfo().type==ByyCruiseTask::theTypeInfo().type)
	{
		mySimuController->sendCruiseTask(recverId);
	}
	else if (task->typeInfo().type==ByyTakeOffTask::theTypeInfo().type)
	{
		mySimuController->sendTakeOffTask(recverId);
	}
	else if (task->typeInfo().type==ByyLaunchMissileTask::theTypeInfo().type)
	{
		ByyLaunchMissileTask *lmTask=dynamic_cast<ByyLaunchMissileTask*>(task);

		ByyEntityObject* entObj=myCore.remoteObjectManager().findEntity(lmTask->targetObjName());

		if (entObj)
		{
			mySimuController->sendLaunchMissileTask(recverId,entObj->id());
		}
		else
			ByyUserOperateLog::instance().addLog(QString::fromLocal8Bit("发送任务失败!"));
	}
	else if (task->typeInfo().type==ByySensorOperTask::theTypeInfo().type)
	{
		ByySensorOperTask *soTask=dynamic_cast<ByySensorOperTask*>(task);

		mySimuController->sendSenorOper(recverId,soTask->getDeviceName(),soTask->getPowerOn());
	}
}

double ByyHdose::simTime()
{
	double t=0;
	
	if (myStarted)
	{
		StUTCTime tm1,tm2,utt;
		GetBackScnTime(tm1,tm2,t,utt,GetEngine(0));
	}

	return t;
}

double ByyHdose::logicClock()
{
	return GetEngine(0)->GetLogicClock();
}

ByyForeStationProcessor* ByyHdose::foreStationProcessor()
{
	return myForeStationProcessor;
}

ByyHdoseSimEventLogDatabase* ByyHdose::simEventLogDatabase()
{
	return mySimEventLogDatabase;
}

void ByyHdose::setSimMultiplier( double val )
{
	CEngine* engine = GetEngine(0);
	if(!engine)
	{
		return;
	}

	if (val<0.1) val=0.1;

	else if (val>1000) val=1000;

	double logicClock=engine->GetLogicClock();

	double wallClock=logicClock/val;

	if (wallClock<0.001) wallClock=0.001;

	engine->ResetWallClock(wallClock);
}

#include <qdebug.h>
void ByyHdose::setExploInfo( int type, QString name )
{
	
	// 反舰导弹
	if(type == 1)
	{
		//emit signal_entityDm(2, name);
		emit signal_missileTJ(type);
	}
	//拦截弹
	if(type == 2)
	{
		//emit signal_entityDm(2, name);
		emit signal_missileTJ(type);
	}
	if(type == 3)
	{
		//emit signal_entityDm(2, name);
	}
	if (type == 4)
	{
		emit signal_entityDm(2, name);
	}
}

void ByyHdoseSimEventLogDatabase::appendLog( double simTime,QString log )
{
	Log log_st={simTime,log};

	myLogList.append(log_st);

	emit logAdded(simTime,log);
}

ByyHdoseSimEventLogDatabase::LogList& ByyHdoseSimEventLogDatabase::logList()
{
	return myLogList;
}

void ByyHdoseSimEventLogDatabase::clear()
{
	myLogList.clear();

	emit cleard();
}

bool ByyHdose::simDateTime(QDateTime& ret)
{
	double t;
	StUTCTime tm1,tm2,utt;
	if(!GetBackScnTime(tm1,tm2,t,utt,GetEngine(0)))
		return false;
	QDateTime buf(QDate(tm1.m_year,tm1.m_mon,tm1.m_mday),QTime(tm1.m_hour,tm1.m_min,tm1.m_sec) );
	//ret.setDate(QDate(tm1.EpTm_year,tm1.EpTm_mon,tm1.EpTm_mday));
	//ret.setTime(QTime(tm1.EpTm_hour,tm1.EpTm_min,tm1.EpTm_sec));
	ret=buf.addSecs(t);
	//ret
	return true;
}

void ByyHdose::snapShot()
{
	CEngine* pEngine=GetEngine(0);
	if(pEngine==0)
		return;
	if(pEngine->GetRecDir()==0)
		return;
	QString file=QString("%1/%2_%3.scn").arg(pEngine->GetRecDir()).arg(myScenarioManager->scenario()->getScnInfo().myName)
		.arg(QDateTime::currentDateTime().toString(DateTime_fmt));
	pEngine->SnapshotWrite(file.toLatin1().data());
	//CItem* item=(CItem*)pEngine->LookupObj(1);
}

void ByyHdose::onHit(int targetId,QString targetEntName,int RecverEntID,QString RecverEntName,Real HittedPosX,Real HittedPosY,Real HittedPosZ)
{
	emit signal_Hit(targetId,targetEntName,RecverEntID,RecverEntName,HittedPosX,HittedPosY,HittedPosZ);
}

void ByyHdose::onHit(const QVariantMap& hitEventInfo)
{
	emit signal_Hit(hitEventInfo);
}

void ByyHdose::onMissileLaunch(int side, int targetId,QString targetEntName ,int RecverEntID,QString RecverEntName)
{
	emit signal_MissileLaunch(side, targetId,targetEntName,RecverEntID,RecverEntName);
}

void ByyHdose::onMissileLaunch(int side, const QString& missileName, const QString& missileType)
{
	emit signal_MissileLaunch(side, missileName, missileType);
}

