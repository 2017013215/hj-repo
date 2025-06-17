//ForeStation.cpp: implementation of the CForeStation module.
//!!HDOSE_CLASS(CForeStation,CNode)
//////////////////////////////////////////////////////////////////////

#include "ForeStation.h"
#include "ByyHdose.h"
#include "foreStationProcessor.h"
#include "JSCoreAppDS/ASMRoutePlanResultEvt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_CREATE_EX(CForeStation,CNode,ClassInit);

BEGIN_EVENTMAP(CForeStation)
	//{{HDOSE_EVENT_MAP(CForeStation)
	ON_HDOSEEVENT("CExplosionEvt",OnExplosion)

	//ON_HDOSEEVENT("CTargetParaEvent",OnTargetPara)
	//ON_HDOSEEVENT("CTargetLossEvent",OnTargetLoss)
	//ON_HDOSEEVENT("CCollisionEvent",OnCollision)
	//ON_HDOSEEVENT("CEntityCreateEvent",OnEntityCreateEvent)
	//ON_HDOSEEVENT("CEntityExitEvent",OnEntityEixtEvent)
	//ON_HDOSEEVENT("CSensorScanEvent",OnSensorScanEvent)

	ON_HDOSEEVENT("CCommStateEvt",OnCommStateEvent)

	ON_HDOSEEVENT("CBackEngineStateEvt",OnBackEngineStateEvt)

	ON_HDOSEEVENT("CTargetReportEvt",onTargetReportEvt)
	//ON_HDOSEEVENT("CTargetIrradiateEvent",OnTargetIrradiateEvent)
	ON_HDOSEEVENT("CWeaponLaunchedEvt",OnWeaponLaunchEvent)

	ON_HDOSEEVENT("CASMRoutePlanResultEvt",OnASMRoutePlanResult)

	ON_HDOSEEVENT("CLaunchWeaponTsk",OnLaunchMissile)
	ON_HDOSEEVENT("CSimCtrlEvt",OnSimCtrl)
	//ON_HDOSEEVENT("CFollow",OnFollow)
	//}}HDOSE_EVENT_MAP
	END_EVENTMAP

CForeStation::CForeStation()
{ 
	//{{HDOSE_ATTRIBUTE_INIT(CForeStation)
	//}}HDOSE_ATTRIBUTE_INIT

	count=0;
	myEngineStateCallback=0;
	myState=STATUS_NULL;

	myHdose=0;
}

CForeStation::~CForeStation()
{

}

void CForeStation::ClassInit(ClassInfo *clsinfo)
{
	//{{HDOSE_ATTRIBUTE_REGISTER(CForeStation)
	//}}HDOSE_ATTRIBUTE_REGISTER
}

void CForeStation::OnInit()
{
	CNode::OnInit();
}

void CForeStation::OnClose()
{
	CNode::OnClose();
}

void CForeStation::tick(double lasttime)
{
	CNode::tick(lasttime);
	if(!m_bInitiative)
		return;
}

void CForeStation::Simulation(double lasttime)
{
	CNode::Simulation(lasttime);
}

void CForeStation::Output()
{
	CNode::Output();
}

void CForeStation::OnExplosion(CEvt *pevent)
{
	CExplosionEvt *pExp = dynamic_cast <CExplosionEvt*> (pevent);
	if (pExp&&myHdose)
	{
		myHdose->foreStationProcessor()->onExplosion(pExp);
		//Trace("接受到爆炸事件\n");
		//printf("Postion:%f %f %f 爆炸当量 = %f\n", pExp->m_Location.m_latitude,pExp->m_Location.m_longitude,
		//	pExp->m_Location.m_height, pExp->m_Equivalent);
	}
}

//void CForeStation::OnTargetPara(CEvt *pevent)
//{
//	CTargetParaEvent *pTar = dynamic_cast <CTargetParaEvent*> (pevent);
//
//	if (pTar&&myHdose)
//	{
//		myHdose->foreStationProcessor()->onTargetPara(pTar);
//	}
//}
//
//void CForeStation::OnTargetLoss(CEvt *pevent)
//{
//	CTargetLossEvent *pTarLoss = dynamic_cast <CTargetLossEvent*> (pevent);
//	if (pTarLoss&&myHdose)
//	{
//		myHdose->foreStationProcessor()->onTargetLoss(pTarLoss);
//	}
//}
//void CForeStation::OnCollision(CEvt *pevent)
//{
//	CCollisionEvent *pCol = dynamic_cast <CCollisionEvent*> (pevent);
//	if (pCol&&myHdose)
//	{
//		myHdose->foreStationProcessor()->onCollision(pCol);
//	}
//}
//
//void CForeStation::OnEntityCreateEvent(CEvt *pevent)
//{
//	CEntityCreateEvent *pcreate = dynamic_cast <CEntityCreateEvent*> (pevent);
//	if (pcreate&&myHdose)
//	{
//		myHdose->foreStationProcessor()->onEntityCreateEvent(pcreate);
//	}
//}
//void CForeStation::OnEntityEixtEvent(CEvt *pevent)
//{
//	CEntityExitEvent *pcreate = dynamic_cast <CEntityExitEvent*> (pevent);
//	if (pcreate&&myHdose)
//	{
//		myHdose->foreStationProcessor()->onEntityEixtEvent(pcreate);
//	}
//}
//void CForeStation::OnSensorScanEvent(CEvt *pevent)
//{
//	CSensorScanEvent *pcreate = dynamic_cast <CSensorScanEvent*> (pevent);
//	if (pcreate&&myHdose)
//	{
//		myHdose->foreStationProcessor()->onSensorScanEvent(pcreate);
//	}
//}

void CForeStation::OnCommStateEvent(CEvt *pevent)
{
	CCommStateEvt abc;
	CCommStateEvt *pcreate = (CCommStateEvt *) (pevent);

	if (pcreate&&myHdose)
	{
		myHdose->foreStationProcessor()->onCommStateEvent(pcreate);
	}
}

// 接收后台引擎状态事件，并进行相应处理，20160315
void CForeStation::OnBackEngineStateEvt(CEvt *pevent)
{
	if(!m_bInitiative)	return;
	CBackEngineStateEvt* pEvt = (CBackEngineStateEvt*) pevent;

	int backeng = pEvt->GetEngineState();
	myState=backeng;

	int fastb = pEvt->GetFastBest();
	if(backeng != GetEngine(0)->GetState())
	{
		if(backeng == STATUS_PAUSE)
			GetEngine(0)->Pause();
		if(backeng>=STATUS_RUNING && backeng<STATUS_PAUSE)
			GetEngine(0)->Resume();
	}

	if(fastb != GetEngine(0)->GetFastBest())
		GetEngine(0)->SetFastBest(fastb);

	if (myEngineStateCallback)
	{
		(*myEngineStateCallback)(backeng,fastb,myUser);
	}

	if (pEvt&&myHdose)
	{
		myHdose->foreStationProcessor()->onBackEngineStateEvt(pEvt);
	}
}

// 目标处理报告事件
void CForeStation::onTargetReportEvt(CEvt *pevent)
{
	if(!m_bInitiative)	return;
	CTargetReportEvt* pEvt = (CTargetReportEvt*) pevent;

	if (pEvt && pEvt->m_ReportType==TargetTracked && myHdose)
	{
		//myHdose->core().radioCommsManager().addTrack(pEvt->GetSourceID(),pEvt->GetTargetID(),pEvt->GetTrackState());
		//printf("src id:%d,tar id:%d connented:%d\n",pcreate->GetSourceEntityID(),pcreate->GetDesEntityID(),pcreate->GetIsConnected());

		myHdose->foreStationProcessor()->onTargetTrackingEvent(pEvt);

		/*bool state = pEvt->GetTrackState();
		int srcId = pEvt->GetSourceID();
		int tarId = pEvt->GetTargetID();
	
		printf("目标跟踪---state:%d,src:%d,tar:%d\n",state,srcId,tarId);*/
	}


	if (pEvt && pEvt->m_ReportType==TargetIllumniated && myHdose)
	{
		//	myHdose->core().radioCommsManager().addGuidance(pEvt->GetSourceID(),pEvt->GetTargetID(),pEvt->GetIrradiateState());
		//printf("src id:%d,tar id:%d connented:%d\n",pcreate->GetSourceEntityID(),pcreate->GetDesEntityID(),pcreate->GetIsConnected());
		myHdose->foreStationProcessor()->onTargetIrradiateEvent(pEvt);

		/*bool state = pEvt->GetIrradiateState();
		int srcId = pEvt->GetSourceID();
		int tarId = pEvt->GetTargetID();

		printf("目标照射---state:%d,src:%d,tar:%d\n",state,srcId,tarId);*/
	}

}

//// 目标照射事件
//void CForeStation::OnTargetIrradiateEvent(CEvt *pevent)
//{
//	if(!m_bInitiative)	return;
//	CTargetIrradiateEvent* pEvt = (CTargetIrradiateEvent*) pevent;
//
//	if (pEvt&&myHdose)
//	{
//	//	myHdose->core().radioCommsManager().addGuidance(pEvt->GetSourceID(),pEvt->GetTargetID(),pEvt->GetIrradiateState());
//		//printf("src id:%d,tar id:%d connented:%d\n",pcreate->GetSourceEntityID(),pcreate->GetDesEntityID(),pcreate->GetIsConnected());
//		myHdose->foreStationProcessor()->onTargetIrradiateEvent(pEvt);
//	}
//
//	/*bool state = pEvt->GetIrradiateState();
//	int srcId = pEvt->GetSourceID();
//	int tarId = pEvt->GetTargetID();
//
//	printf("目标照射---state:%d,src:%d,tar:%d\n",state,srcId,tarId);*/
//}

// ----武器发射事件-----
// m_SrcMountType == 1 为MK41垂直发射系统
// m_WeaponId == 7 SM1
// m_WeaponId == 8 SM2
// m_WeaponId == 16 ESSM
// m_WeaponId == 17 SSM
// m_WeaponId == 18 SeaRAM

// m_SrcMountType == 10 为MK36 SRBoc发射系统
// m_WeaponId == 0 箔条弹
// m_WeaponId == 1 红外弹

void CForeStation::OnWeaponLaunchEvent(CEvt *pevent)
{
	if(!m_bInitiative)	return;
	CWeaponLaunchedEvt* pEvt = (CWeaponLaunchedEvt*) pevent;

	if (pEvt&&myHdose)
	{
		myHdose->foreStationProcessor()->onWeaponLaunchEvent(pEvt);
	}
}

void CForeStation::OnLaunchMissile(CEvt *pevent)
{
	if(!m_bInitiative)	return;

	CLaunchWeaponTsk* pEvt=(CLaunchWeaponTsk*) pevent;

	if (pEvt&&myHdose)
	{
		myHdose->foreStationProcessor()->onLaunchMissile(pEvt);
	}

}
// 如果要用，需要用到DataStructComponentDlld.lib和DataStructComponentDllr.lib
void CForeStation::OnASMRoutePlanResult(CEvt *pevent)
{
	if(!m_bInitiative)	return;
	CASMRoutePlanResultEvt* pEvt = (CASMRoutePlanResultEvt*) pevent;

	// 取出已规划的航路数据
	m_pEngine->AddLogLine(1,"前端 收到航路规划结果：实体号：%d, 发射架序号:%d, 目标号：%d\n",pEvt->GetLchPltID(),pEvt->GetLaunchVehicleNo(),pEvt->GetTargetID());

	if (pEvt&&myHdose)
	{
		myHdose->foreStationProcessor()->onASMRoutePlanResult(pEvt);
	}

//	myHdose->core().radioCommsManager().addRoutePlanning(pEvt->GetFieldNo(),pnts);

}

void CForeStation::OnSimCtrl(CEvt *pevent)
{
	if(!m_bInitiative)	return;

	CSimCtrlEvt* pEvt=(CSimCtrlEvt*)pevent;

	if (pEvt&&myHdose)
	{
		myHdose->foreStationProcessor()->onSimCtrl(pEvt);
	}
}

//void CForeStation::OnFollow(CEvt *pevent)
//{
//	CFollow* pEvt=(CFollow*)pevent;
//	double t=m_lasttime;
//	int test = 0;
//
//	if (pEvt&&myHdose)
//	{
//		myHdose->foreStationProcessor()->onFollow(pEvt);
//	}
//}

void CForeStation::setBackEngineStateCallback( BackEngineStateCallback func,void *user )
{
	myEngineStateCallback=func;
	myUser=user;
}

int CForeStation::getState()
{
	return myState;
}

void CForeStation::setHdose( ByyHdose* hd )
{
	myHdose=hd;
}
