#ifndef FORESTATIONPROCESSOR_H
#define FORESTATIONPROCESSOR_H

#include "JSCGFSimDS/JSCGFSimDS.h"
#include "JSCGFSimDS/BaseDataType.h"
#include "JSCGFSimEvt/JSCGFSimEvt.h"
#include "JSCGFSimTsk/JSCGFSimTsk.h"
#include "JSCGFSimMF/JSCGFSimMF.h"
#include "JSCoreAppDS/JSCoreAppDS.h"

#include "JSCoreAppDS/ASMRoutePlanResultEvt.h"
#include "JSCGFSimEvt/ExplosionEvt.h"
//#include "CGF/TargetParaEvent.h"
//#include "CGF/TargetLossEvent.h"
//#include "CGF/CollisionEvent.h"
//#include "CGF/EntityCreateEvent.h"
//#include "CGF/EntityExitEvent.h"
//#include "CGF/SensorScanEvent.h"
#include "JSCGFSimEvt/CommStateEvt.h"
#include "JSCGFSimEvt/BackEngineStateEvt.h"
#include "JSCGFSimEvt/TargetReportEvt.h"
//#include "CGF/TargetIrradiateEvent.h"
#include "JSCGFSimEvt/WeaponLaunchedEvt.h"
#include "JSCGFSimTsk/LaunchWeaponTsk.h"
#include "JSCGFSimEvt/SimCtrlEvt.h"
//#include "CGF/Follow.h"

class ByyHdose;

class ByyForeStationProcessor
{
public:
	ByyForeStationProcessor(ByyHdose& hdose);
	~ByyForeStationProcessor();

	void onExplosion(CExplosionEvt* pevent);
	//void onTargetPara(CTargetParaEvent *pevent);
	//void onTargetLoss(CTargetLossEvent *pevent);
	//void onCollision(CCollisionEvent *pevent);
	//void onEntityCreateEvent(CEntityCreateEvent *pevent);
	//void onEntityEixtEvent(CEntityExitEvent *pevent);
	//void onSensorScanEvent(CSensorScanEvent *pevent);
	void onCommStateEvent(CCommStateEvt *pevent);

	void onBackEngineStateEvt(CBackEngineStateEvt *pevent);		// 后台引擎状态报告

	// 20160924
	//void onTargetReportEvt(CTargetReportEvt *pevent);
	void onTargetTrackingEvent(CTargetReportEvt *pevent);
	void onTargetIrradiateEvent(CTargetReportEvt *pevent);
	void onWeaponLaunchEvent(CWeaponLaunchedEvt *pevent);
	void onASMRoutePlanResult(CASMRoutePlanResultEvt *pevent);

	void onLaunchMissile(CLaunchWeaponTsk *pevent);
	void onSimCtrl(CSimCtrlEvt *pevent);
	//void onFollow(CFollow *pevent);


private:
	ByyHdose& myHdose;
};

#endif // FORESTATIONPROCESSOR_H
