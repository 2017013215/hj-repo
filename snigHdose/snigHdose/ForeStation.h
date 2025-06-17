//ForeStation.h: interface for the CForeStation module.
//!!HDOSE_CLASS(CForeStation,CNode)
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_ForeStation_H)
#define AFX_ForeStation_H

//{{HDOSE_CLS_DES(CForeStation)
/*
Author:		
Version:	1.0
Descript:	
*/
//}}HDOSE_CLS_DES

#include "JSCGFSimDS/JSCGFSimDS.h"
#include "JSCGFSimDS/BaseDataType.h"
#include "JSCGFSimEvt/JSCGFSimEvt.h"
#include "JSCGFSimTsk/JSCGFSimTsk.h"
#include "JSCGFSimMF/JSCGFSimMF.h"
#include "JSCoreAppDS/JSCoreAppDS.h"

typedef void (*BackEngineStateCallback)(int state,int bestFast,void *user);

class ByyHdose;

class CForeStation /*ǰ    ̨*/ : public CNode 
{
	DECLARE_CREATE;
	DECLARE_EVENTMAP;
public:
	CForeStation();
	virtual ~CForeStation();
	static	void ClassInit(ClassInfo *clsinfo);
	
	virtual	void OnInit();
	virtual void OnClose();
	virtual	void tick(double lasttime);
	virtual void Simulation(double lasttime);
	virtual void Output();


	//{{HDOSE_MEMBER_FUNCTION(CForeStation)
	//}}HDOSE_MEMBER_FUNCTION

protected:
	//{{HDOSE_EVENT_HANDLER(CForeStation)
	void	OnExplosion(CEvt *pevent);
	//void	OnTargetPara(CEvt *pevent);
	//void	OnTargetLoss(CEvt *pevent);
	//void	OnCollision(CEvt *pevent);
	//void	OnEntityCreateEvent(CEvt *pevent);
	//void	OnEntityEixtEvent(CEvt *pevent);
	//void	OnSensorScanEvent(CEvt *pevent);
	void	OnCommStateEvent(CEvt *pevent);

	void	OnBackEngineStateEvt(CEvt *pevent);		// 后台引擎状态报告

	// 20160924
	void onTargetReportEvt(CEvt *pevent);
	//void	OnTargetTrackingEvent(CEvt *pevent);
	//void	OnTargetIrradiateEvent(CEvt *pevent);
	void	OnWeaponLaunchEvent(CEvt *pevent);
	void	OnASMRoutePlanResult(CEvt *pevent);

	void	OnLaunchMissile(CEvt *pevent);
	void	OnSimCtrl(CEvt *pevent);
	//void	OnFollow(CEvt *pevent);

	
	//}}HDOSE_EVENT_HANDLER
public:
	void setBackEngineStateCallback(BackEngineStateCallback func,void *user);
	void setHdose(ByyHdose* hd);
	int count;		//定时计数
	//{{HDOSE_ATTRIBUTE(CForeStation)
	//}}HDOSE_ATTRIBUTE

	int getState();

protected:
	BackEngineStateCallback myEngineStateCallback;
	void *myUser;

	int myState;

	ByyHdose* myHdose;
};

#endif