#include "simuController.h"
#include "scenarioManager.h"
#include "ForeStation.h"
#include "ByyHdose.h"
#include "ByyUserOperateLog.h"
#include "ByyCore.h"
#include "remoteObjectManager.h"

#include "JSCGFSimDS/JSCGFSimDS.h"
#include "JSCGFSimDS/BaseDataType.h"
#include "JSCGFSimEvt/JSCGFSimEvt.h"
#include "JSCGFSimTsk/JSCGFSimTsk.h"
#include "JSCGFSimMF/JSCGFSimMF.h"
#include "JSCoreAppDS/JSCoreAppDS.h"

#include "JSCGFSimEvt/RequestBackEngineStateEvt.h"
#include "JSCGFSimEvt/CreateObjRequestEvt.h"
#include "JSCGFSimEvt/CreateObjResultEvt.h"
#include "JSCGFSimEvt/RemoveObjRequestEvt.h"
#include "JSCGFSimEvt/RemoveObjResultEvt.h"
#include "JSCGFSimEvt/SimCtrlEvt.h"
#include "JSCGFSimEvt/ScnCtrlEvt.h"
#include "JSCGFSimTsk/TakeOffTsk.h"
#include "JSCGFSimTsk/MoveToTsk.h"
#include "JSCGFSimTsk/PatrolTsk.h"
#include "JSCGFSimTsk/WaitTsk.h"
#include "JSCGFSimTsk/FollowTsk.h"
//#include "JSCGFSimTsk/CruiseTsk.h"
#include "JSCGFSimTsk/MoveAlongTsk.h"
#include "JSCGFSimTsk/ReturnBaseTsk.h"
#include "JSCGFSimTsk/LaunchWeaponTsk.h"
#include "JSCGFSimTsk/SensorOperTsk.h"
#include "TargetControlEvt.h"
#include <QTimer>

ByySimuController::ByySimuController(ByyCore& core, ByyHdose& hdose,QObject *parent/*=0*/ ) 
	: QObject(parent)
	,myStatus(S_NULL)
	,myHdose(hdose)
	,myCore(core)
	,myScenarioManager(hdose.scenarioManager())
	,myForeStation(0)
	,myWaitBackendReply(false)
	,myScnLoaded(false)
	,myBackendNoReplyNum(0)
	,myFastBest(false)
{
}

ByySimuController::~ByySimuController()
{

}

void ByySimuController::init( int mode )
{
	if(!GetEngine(0))
		return;

	CObj* StaObj = GetEngine(0)->CreateObj("CForeStation");
	myForeStation=dynamic_cast<CForeStation*>(StaObj);

	if (myForeStation)
	{
		myForeStation->setHdose(&myHdose);
		myForeStation->setBackEngineStateCallback(backEngineStateCallback,this);
	}

	if (mode==1)
	{
		/*CScnMgr *pSche = dynamic_cast <CScnMgr*> (GetEngine(0)->LookupObj(1));
		pSche->AddAggregateObj(StaObj);*/

		myBackendNoReplyNum=0;

		// 向后台发送状态报告请求
		this->requestBackEngState();

		GetEngine(0)->AddLogLine(1,"向后台请求引擎状态事件\n");

		changeStatus(S_NULL);
	}
	else
	{
		changeStatus(S_PAUSE);
	}
	
	myFastBest=false;
	myScnLoaded=false;
}

void ByySimuController::release()
{
	pause();
	closeScenario();

	myFastBest=0;
	myForeStation=0;
	myBackendNoReplyNum=0;
	myScnLoaded=false;
	myStartDateTime=QDateTime();
	changeStatus(S_NULL);
}

CForeStation* ByySimuController::foreStation()
{
	return myForeStation;
}

void ByySimuController::backEngineStateCallback( int state,int bestFast,void* user )
{
	ByySimuController* ctrl=static_cast<ByySimuController*>(user);
	
	QMetaObject::invokeMethod(ctrl,"processBackEngineStateCallback",Qt::QueuedConnection,Q_ARG(int,state),Q_ARG(int,bestFast));
}


void ByySimuController::processBackEngineStateCallback( int state,int bestFast )
{
	myWaitBackendReply=false;
	myBackendNoReplyNum=0;

	if (isRunning())
	{
		myScnLoaded=true;
	}

	if ((bool)bestFast!=myFastBest)
	{
		myFastBest=bestFast;
		emit signal_fastBestChanged(myFastBest);
	}

	if (state==-1)
	{
		processBackExit();
	}
	else
	{
		changeStatus((SimuStatus)state);
	}
}

void ByySimuController::checkBackState()
{
	if (myForeStation==0)
		return;

	if (myWaitBackendReply)
	{
		++myBackendNoReplyNum;
	}

	if (isActive()&&myBackendNoReplyNum>2)
	{
		processBackExit();
	}
	
	requestBackEngState();
}

bool ByySimuController::requestBackEngState()
{
	if(!GetEngine(0))
	{
		return false;
	}

	CRequestBackEngineStateEvt*pevt=(CRequestBackEngineStateEvt*)GetEngine(0)->CreateEvt("CRequestBackEngineStateEvt");
	if(pevt)
	{
		pevt->m_scope = 2;
		pevt->Send();
		delete pevt;
	}


	static int timeMsec = 1000;
	QTimer::singleShot(timeMsec,this,SLOT(checkBackState()));

	timeMsec += 1000;
	if(timeMsec > 9000)
		timeMsec = 10000;


	myWaitBackendReply=true;

	return true;
}
bool ByySimuController::start()
{
	if(!GetEngine(0))
	{
		return false;
	}

	if(GetEngine(0)->GetState() == STATUS_PAUSE)
		GetEngine(0)->Resume();

	CSimCtrlEvt*pSimCtrl=(CSimCtrlEvt*)GetEngine(0)->CreateEvt("CSimCtrlEvt");
	if(pSimCtrl)
	{
		pSimCtrl->m_scope=2;
		pSimCtrl->SetSimCtrlType(1);
		pSimCtrl->Send();
		delete pSimCtrl;

		changeStatus(S_RUNING);
	}

	myStartDateTime=QDateTime::currentDateTime();
	myCore.remoteObjectManager().setCanSynEntityEvent(true);

	ByyUserOperateLog::instance().addLog(QString::fromLocal8Bit("开始仿真"));
	QTimer::singleShot(2000, &myHdose, SIGNAL(signal_entityUpdate()));

	return true;
}

bool ByySimuController::pause()
{
	if(!GetEngine(0))
	{
		return false;
	}


	GetEngine(0)->Pause();

	changeStatus(S_PAUSE);

	ByyUserOperateLog::instance().addLog(QString::fromLocal8Bit("暂停仿真"));
	QTimer::singleShot(2000, &myHdose, SIGNAL(signal_stopVerdict()));

	return true;
}

bool ByySimuController::restart()
{
	if(!GetEngine(0))
	{
		return false;
	}

	CSimCtrlEvt*pSimCtrl=(CSimCtrlEvt*)GetEngine(0)->CreateEvt("CSimCtrlEvt");
	if(pSimCtrl)
	{
		pSimCtrl->m_scope=2;
		pSimCtrl->SetSimCtrlType(3);
		pSimCtrl->Send();
		delete pSimCtrl;
	}

	myStartDateTime=QDateTime::currentDateTime();
	myCore.remoteObjectManager().setCanSynEntityEvent(false);
	emit signal_reloadScn();

	return true;
}

bool ByySimuController::fastBest()
{
	if(!GetEngine(0))
	{
		return false;
	}

	CSimCtrlEvt*pSimCtrl=(CSimCtrlEvt*)GetEngine(0)->CreateEvt("CSimCtrlEvt");
	if(pSimCtrl)
	{
		pSimCtrl->m_scope=2;
		pSimCtrl->SetSimCtrlType(4);
		pSimCtrl->Send();
		delete pSimCtrl;
	}

	GetEngine(0)->SetFastBest(1);

	myFastBest=true;

	return true;
}

bool ByySimuController::normal()
{
	if(!GetEngine(0))
	{
		return false;
	}

	CSimCtrlEvt*pSimCtrl=(CSimCtrlEvt*)GetEngine(0)->CreateEvt("CSimCtrlEvt");
	if(pSimCtrl)
	{
		pSimCtrl->m_scope=2;
		pSimCtrl->SetSimCtrlType(5);
		pSimCtrl->Send();
		delete pSimCtrl;
	}

	GetEngine(0)->SetFastBest(0);

	myFastBest=false;

	return true;
}

ByySimuController::SimuStatus ByySimuController::status() const
{
	return myStatus;
}

void ByySimuController::changeStatus( SimuStatus statue )
{
	myStatus=statue;
	emit signal_StatusChanged(myStatus);
}

bool ByySimuController::loadScenario( char *fn )
{
	if(!GetEngine(0))
	{
		return false;
	}

	CScnCtrlEvt*pevt=(CScnCtrlEvt*)GetEngine(0)->CreateEvt("CScnCtrlEvt");
	if(pevt)
	{
		//GetEngine(0)->LoadScenario(fn);
		pevt->m_scope = 2;
		pevt->SetScnCtrlType(2);
		pevt->SetFileName(fn);
		pevt->Send();
		delete pevt;
	}

	myScnLoaded=true;

	::Sleep(3000);

	emit signal_scnLoaded();
	emit signal_reloadScn();

	return true;
}

bool ByySimuController::closeScenario()
{
	if(!GetEngine(0))
	{
		return false;
	}

	if(myHdose.mode() == ByyHdose::Single)
	{
		
		CScnCtrlEvt*pevt=(CScnCtrlEvt*)GetEngine(0)->CreateEvt("CScnCtrlEvt");
		if(pevt)
		{
			pevt->m_scope = 2;
			pevt->SetScnCtrlType(3);
			pevt->Send();
			delete pevt;
		}
	}

	myScnLoaded=false;

	changeStatus(S_PAUSE);

	return true;
}

const QDateTime& ByySimuController::startDateTime()
{
	return myStartDateTime;
}

bool ByySimuController::scnLoaded()
{
	return myScnLoaded;
}

bool ByySimuController::isRunning()
{
	return myStatus>=S_RUNING&&myStatus<S_PAUSE;
}

bool ByySimuController::isPaused()
{
	return myStatus==S_PAUSE;
}

void ByySimuController::createObject( int objType,const QString& itemClass,const QString& attributeValueSet )
{
	CCreateObjRequestEvt*pCreateObj=(CCreateObjRequestEvt*)GetEngine(0)->CreateEvt("CCreateObjRequestEvt");
	if(pCreateObj)
	{
		pCreateObj->m_scope=2;

		pCreateObj->SetObjType(objType);
		pCreateObj->SetObjClass(itemClass.toLocal8Bit().data());
		pCreateObj->SetAttributeValueSet(attributeValueSet.toLocal8Bit().data());

		pCreateObj->Post();
		//delete pCreateObj;

		printf("发送创建对象事件\n");
	}
}

void ByySimuController::removeObject( int objType,long id )
{
	CRemoveObjRequestEvt* pRemoveObj=(CRemoveObjRequestEvt*)GetEngine(0)->CreateEvt("CRemoveObjRequestEvt");
	if (pRemoveObj)
	{
		pRemoveObj->m_scope=2;

		pRemoveObj->SetObjType(objType);
		pRemoveObj->SetObjId(id);

		pRemoveObj->Send();
		delete pRemoveObj;
		printf("发送删除对象事件\n");
	}
}

void ByySimuController::processBackExit()
{
	myScnLoaded=false;
	changeStatus(S_NULL);
	emit signal_backExited();

}

void ByySimuController::sendTargetControlEvt(string targetName, int direction)
{
	if(!GetEngine(0))
	{
		return;
	}
	CTargetControlEvt *pevt=(CTargetControlEvt*)GetEngine(0)->CreateEvt("CTargetControlEvt");
	if(pevt)
	{
		pevt->m_scope = 2;
		pevt->setDirection(direction);
		pevt->setTargetName(targetName);
		pevt->Send();
		delete pevt;
	}else{
		return;
	}
}

void ByySimuController::sendMoveToTask( int recverId,const QString& ctrlObjName )
{
	CEngine* pEng=(CEngine*)GetEngine(0);
	CMoveToTsk* pTask = (CMoveToTsk*) pEng->CreateEvt("CMoveToTsk");
	if(pTask)
	{
		pTask->m_scope = 2;
		pTask->SetRecver(recverId);	// 接收者实体Id

		pTask->SetAssignCtrlObjName_Id(1);
		pTask->SetControlObjName(ctrlObjName.toLocal8Bit().data());	// 控制对象名称

		//pTask->SetLimitConditionType(1);	// 运动限制条件类型,1给速度、给定运动时间
		//pTask->SetMoveVel(10);

		pTask->SetDest("JSModel");

		pTask->Send();
		delete pTask;
		printf("发送MoveTo任务\n");
	}

}

void ByySimuController::sendMoveAlongTask( int recverId,const QString& ctrlObjName )
{
	CEngine* pEng=(CEngine*)GetEngine(0);
	CMoveAlongTsk* pTask = (CMoveAlongTsk*) pEng->CreateEvt("CMoveAlongTsk");
	if(pTask)
	{
		pTask->m_scope = 2;
		pTask->SetRecver(recverId);	// 接收者实体Id

		pTask->SetAssignCtrlObjName_Id(1);
		pTask->SetControlObjName(ctrlObjName.toLocal8Bit().data());	// 控制对象名称

		//pTask->SetLimitConditionType(1);	// 运动限制条件类型,1给速度、给定运动时间
		//pTask->SetMoveVel(10);

		pTask->SetDest("JSModel");

		pTask->Send();
		delete pTask;
		printf("发送MoveAlong任务\n");
	}

}

void ByySimuController::sendPatrolTask( int recverId,int type,const QString& ctrlObjName,long time )
{
	CEngine* pEng=(CEngine*)GetEngine(0);
	CPatrolTsk* pMove=(CPatrolTsk*)pEng->CreateEvt("CPatrolTsk");

	if(pMove && recverId>0)
	{
		pMove->m_scope=2;
		pMove->SetRecver(recverId);
		pMove->SetPatrolType(type);
		pMove->SetAssignCtrlObjName_Id(1);
		pMove->SetControlObjName(ctrlObjName.toLocal8Bit().data());
		pMove->SetPatrolTime(time);
		pMove->SetDest("JSModel");
		pMove->Send();
		delete pMove;
		printf("发送Patrol任务\n");
	}
}

void ByySimuController::sendFollowTask( int recverId,const QString& ObjName,int distance,int relativeAngle,int highOffset )
{
	CEngine* pEng=(CEngine*)GetEngine(0);
	//CFollow* pMove=(CFollow*)pEng->CreateEvt("CFollow");

	//if(pMove)
	//{
	//	pMove->m_scope=2;
	//	pMove->SetRecver(recverId);
	//	pMove->SetFollowedObjName(ObjName.toLocal8Bit().data());
	//	pMove->SetDistance(distance);
	//	pMove->SetRelativeOrientation(relativeAngle);
	//	pMove->SetHighOffset(highOffset);
	//	pMove->SetDest("JSModel");
	//	pMove->Send();
	//	delete pMove;
	//	printf("发送Follow任务\n");
	//}
}

void ByySimuController::sendWaitTask( int recverId,int waitTime )
{
	CEngine* pEng=(CEngine*)GetEngine(0);
	CWaitTsk* pMove=(CWaitTsk*)pEng->CreateEvt("CWaitTsk");
	if(pMove)
	{
		pMove->m_scope=2;
		pMove->SetRecver(recverId);
		pMove->SetWaitTime(waitTime);
		pMove->SetDest("JSModel");
		pMove->Send();
		delete pMove;
		printf("发送Wait任务\n");
	}
}

void ByySimuController::sendCruiseTask( int recverId )
{
	CEngine* pEng=(CEngine*)GetEngine(0);
	//CCruise* pMove=(CCruise*)pEng->CreateEvt("CCruise");

	//if(pMove)
	//{
	//	pMove->m_scope=2;
	//	pMove->SetRecver(recverId);
	//	pMove->SetDest("JSModel");
	//	pMove->Send();
	//	delete pMove;
	//	printf("发送Cruise任务\n");
	//}
}

void ByySimuController::sendLaunchMissileTask( int recverId,int targetId )
{
	CEngine* pEng=(CEngine*)GetEngine(0);
	CLaunchWeaponTsk* pLMTask=(CLaunchWeaponTsk*)pEng->CreateEvt("CLaunchWeaponTsk");

	if(pLMTask)
	{
		//pLMTask->m_scope=2;		// 远程
		pLMTask->m_scope=2;
		pLMTask->SetRecver(recverId);

		pLMTask->m_Target.m_TargetID = targetId;
		pLMTask->SetDest("JSModel");
		pLMTask->Send();
		delete pLMTask;
		printf("发送LaunchMissile任务\n");
	}
}

void ByySimuController::sendTakeOffTask( int recverId )
{
	CEngine* pEng=(CEngine*)GetEngine(0);

	CTakeOffTsk* pTask = (CTakeOffTsk*) pEng->CreateEvt("CTakeOffTsk");
	if(pTask)
	{
		pTask->m_scope = 2;
		pTask->SetRecver(recverId);	// 接收者实体Id

		pTask->SetDest("JSModel");

		pTask->Send();
		delete pTask;
		printf("发送TakeOff任务\n");
	}

}

void ByySimuController::sendSenorOper( int recverId,const QString& device,bool powerOn )
{
	CEngine* pEng=(CEngine*)GetEngine(0);

	CSensorOperTsk* pTask = (CSensorOperTsk*) pEng->CreateEvt("CSensorOperTsk");

	if (pTask)
	{
		pTask->m_scope = 2;
		pTask->SetRecver(recverId);	// 接收者实体Id
		pTask->SetEquipmentName(device.toLocal8Bit().data());
		pTask->SetOperType(1);
		pTask->SetPowerOn(powerOn);
		pTask->SetDest("JSModel");

		pTask->Send();
		delete pTask;
		printf("发送SenorOper任务\n");
	}

}

bool ByySimuController::isActive()
{
	return isPaused()||isRunning();
}

bool ByySimuController::isFastBest()
{
	return myFastBest;
}
