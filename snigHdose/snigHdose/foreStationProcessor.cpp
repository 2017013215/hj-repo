#include "foreStationProcessor.h"
#include "ByyHdose.h"
#include "ByyCore.h"
#include "remoteObjectManager.h"
#include "radioCommsManager.h"
#include "entityObject.h"

#include "qdebug.h"

ByyForeStationProcessor::ByyForeStationProcessor(ByyHdose& hdose)
	:myHdose(hdose)
{

}

ByyForeStationProcessor::~ByyForeStationProcessor()
{

}

void ByyForeStationProcessor::onExplosion( CExplosionEvt* pevent )
{
	QString str=QString::fromLocal8Bit("爆炸，位置：{经度：%1 纬度：%2 高度：%3}，")
		.arg(pevent->m_Location.Lon(),0,'f',8)
		.arg(pevent->m_Location.Lat(),0,'f',8)
		.arg(pevent->m_Location.Elev());
	
	str+=QString::fromLocal8Bit("参照目标：");

	ByyEntityObject* entObj=myHdose.core().remoteObjectManager().findEntity(pevent->m_RefrenceObjId);
	ByyEntityObject* entObjMb=myHdose.core().remoteObjectManager().findEntity(pevent->m_HittedObjId);

	// wqb 20240428 毁伤计算 临时修改
	if (entObj && entObjMb )
	{
		QVariantMap eventinfo;
		eventinfo.insert("Event.time",		QString::number(myHdose.simTime(), 'f', 2));
		eventinfo.insert("Refrence.name",	pevent->GetRefrenceObjName());
		eventinfo.insert("Refrence.kind",	pevent->GetRefrenceObjType().m_kind);
		eventinfo.insert("Refrence.domain",	pevent->GetRefrenceObjType().m_domain); 
		eventinfo.insert("Refrence.category",	pevent->GetRefrenceObjType().m_category);	//炸弹类型
		eventinfo.insert("Location.x",		pevent->m_Location.x);
		eventinfo.insert("Location.y",		pevent->m_Location.y);
		eventinfo.insert("Location.z",		pevent->m_Location.z);							//爆炸位置
		eventinfo.insert("Hitted.name",		entObjMb->name());
		eventinfo.insert("Hitted.kind",		entObjMb->entityType().kind);
		eventinfo.insert("Hitted.domain",	entObjMb->entityType().domain);					
		eventinfo.insert("Hitted.currentHealth",	entObjMb->getHealthPoint());
		eventinfo.insert("Hitted.damageState",		entObjMb->damageState());					//目标信息
		eventinfo.insert("HittedObjPos.x",	pevent->m_HittedObjPos.x);
		eventinfo.insert("HittedObjPos.y",	pevent->m_HittedObjPos.y);
		eventinfo.insert("HittedObjPos.z",	pevent->m_HittedObjPos.z);						//攻击位置
		eventinfo.insert("HittedObjItemCls", entObjMb->itemClass()); // 目标实体类名， xks 20240529
		myHdose.onHit(eventinfo);
		//// 鱼雷 2 7 1 
		//if (pevent->m_RefrenceObjType.m_kind ==2 && pevent->m_RefrenceObjType.m_domain== 7 && pevent->m_RefrenceObjType.m_category ==1)
		//{
		//	myHdose.onHit(pevent->m_HittedObjId,pevent->GetHittedObjName(), pevent->m_RecverEntID, pevent->m_RefrenceObjName ,pevent->m_HittedObjPos.x,pevent->m_HittedObjPos.y,pevent->m_HittedObjPos.z) ;
		//}
		//// 深水炸弹 2 7 2
		//if (pevent->m_RefrenceObjType.m_kind ==2 && pevent->m_RefrenceObjType.m_domain== 7 && pevent->m_RefrenceObjType.m_category == 0 )
		//{
		//	myHdose.onHit(pevent->m_HittedObjId,pevent->GetHittedObjName(),pevent->m_RecverEntID, pevent->m_RefrenceObjName,pevent->m_HittedObjPos.x,pevent->m_HittedObjPos.y,pevent->m_HittedObjPos.z) ;
		//}
	}

	if(entObj)
	{
		//反舰导弹
		if(entObj->entityType().kind == 2 && entObj->entityType().domain != 1)
		{
			//myHdose.setExploInfo(1);
			//qDebug()<<"----OnExplosion---1---";
			if(entObjMb)
			{
				//qDebug()<<"----OnExplosion---2---";
				//if(entObjMb->entityType().kind == 1 && entObjMb->entityType().domain != 3)
				myHdose.setExploInfo(3, entObjMb->name());
			}

		}
		else if(entObj->entityType().kind == 2 && entObj->entityType().domain == 1)//拦截弹 && 空空导弹
		{
			//myHdose.setExploInfo(2);
			// todo wqb 空空导弹命中时 m_HittedObjId == 0 待修改 2024/04/26 
			//if(pevent->m_HittedObjId != 0)
			//{
				//qDebug()<<"----OnExplosion---3---";
				myHdose.setExploInfo(4);
			//}
		}
	}

	if (entObj)
		str+=entObj->name();
	else
		str+=QString::number(pevent->m_RefrenceObjId);

	str+="\n";

	QMetaObject::invokeMethod(myHdose.simEventLogDatabase(),"appendLog",Qt::QueuedConnection,
		Q_ARG(double, myHdose.simTime()),
		Q_ARG(QString, str));
}

//void ByyForeStationProcessor::onTargetPara( CTargetParaEvent *pevent )
//{
//	QString str=QString::fromLocal8Bit("目标参数事件");
//
//	str+="\n";
//
//	QMetaObject::invokeMethod(myHdose.simEventLogDatabase(),"appendLog",Qt::QueuedConnection,
//		Q_ARG(double, myHdose.simTime()),
//		Q_ARG(QString, str));
//}

//void ByyForeStationProcessor::onTargetLoss( CTargetLossEvent *pevent )
//{
//	QString str=QString::fromLocal8Bit("目标消失，");
//	str+=QString::fromLocal8Bit("目标批号：%1").arg(pevent->m_BatchNo);
//
//	str+="\n";
//
//	QMetaObject::invokeMethod(myHdose.simEventLogDatabase(),"appendLog",Qt::QueuedConnection,
//		Q_ARG(double, myHdose.simTime()),
//		Q_ARG(QString, str));
//}
//
//void ByyForeStationProcessor::onCollision( CCollisionEvent *pevent )
//{
//	QString str=QString::fromLocal8Bit("碰撞事件");
//
//	str+="\n";
//
//	QMetaObject::invokeMethod(myHdose.simEventLogDatabase(),"appendLog",Qt::QueuedConnection,
//		Q_ARG(double, myHdose.simTime()),
//		Q_ARG(QString, str));
//}
//
//void ByyForeStationProcessor::onEntityCreateEvent( CEntityCreateEvent *pevent )
//{
//	QString str=QString::fromLocal8Bit("实体创建事件");
//
//	QMetaObject::invokeMethod(myHdose.simEventLogDatabase(),"appendLog",Qt::QueuedConnection,
//		Q_ARG(double, myHdose.simTime()),
//		Q_ARG(QString, str));
//}
//
//void ByyForeStationProcessor::onEntityEixtEvent( CEntityExitEvent *pevent )
//{
//	QString str=QString::fromLocal8Bit("实体退出事件");
//
//	str+="\n";
//
//	QMetaObject::invokeMethod(myHdose.simEventLogDatabase(),"appendLog",Qt::QueuedConnection,
//		Q_ARG(double, myHdose.simTime()),
//		Q_ARG(QString, str));
//}
//
//void ByyForeStationProcessor::onSensorScanEvent( CSensorScanEvent *pevent )
//{
//	QString str;
//	ByyEntityObject* entObj=myHdose.core().remoteObjectManager().findEntity(pevent->m_EntityID);
//
//	if (entObj)
//		str=entObj->name();
//	else
//		str=QString::number(pevent->m_EntityID);
//
//	str+=QString::fromLocal8Bit(" #%1 传感器").arg(pevent->m_SensorID);
//	str+=QString::fromLocal8Bit(pevent->m_PowerOn?"开启":"关闭");
//	str+=QString::fromLocal8Bit("扫描");
//	str+="\n";
//
//	QMetaObject::invokeMethod(myHdose.simEventLogDatabase(),"appendLog",Qt::QueuedConnection,
//		Q_ARG(double, myHdose.simTime()),
//		Q_ARG(QString, str));
//}

void ByyForeStationProcessor::onCommStateEvent( CCommStateEvt *pevent )
{
	QMetaObject::invokeMethod(&myHdose.core().radioCommsManager(),"addComm",Qt::QueuedConnection,
		Q_ARG(int, pevent->GetSrcEntID()),
		Q_ARG(int, pevent->GetDesEntityID()),
		Q_ARG(bool, pevent->GetIsConnected()));

	QString sourceStr,targetStr;

	ByyEntityObject* srcEntObj=myHdose.core().remoteObjectManager().findEntity(pevent->m_SrcEntID);
	ByyEntityObject* targetEntObj=myHdose.core().remoteObjectManager().findEntity(pevent->m_DesEntityID);

	if (srcEntObj)
		sourceStr=srcEntObj->name();
	else
		sourceStr=QString::number(pevent->m_SrcEntID);

	if (targetEntObj)
		targetStr=targetEntObj->name();
	else
		targetStr=QString::number(pevent->m_DesEntityID);


	QString str;
	str=sourceStr+" "+QString::fromLocal8Bit(pevent->m_IsConnected?"通信":"不再通信");
	str+=" ";
	str+=targetStr;
	// 消息内容在事件的m_buffer变量中存储
	if(pevent->m_IsConnected && pevent->m_buffer)
	{
		str+=QString::fromLocal8Bit(", 消息内容: ");
		str+=QString::fromLocal8Bit(pevent->m_buffer) ;
	}

	str+="\n";

	QMetaObject::invokeMethod(myHdose.simEventLogDatabase(),"appendLog",Qt::QueuedConnection,
		Q_ARG(double, myHdose.simTime()),
		Q_ARG(QString, str));
}

void ByyForeStationProcessor::onBackEngineStateEvt( CBackEngineStateEvt *pevent )
{
	
}

void ByyForeStationProcessor::onTargetTrackingEvent( CTargetReportEvt *pevent )
{
	if(pevent->m_ReportType != TargetTracked )
		return;

	QMetaObject::invokeMethod(&myHdose.core().radioCommsManager(),"addTrack",Qt::QueuedConnection,
		Q_ARG(int, pevent->GetSrcEntID()),
		Q_ARG(int, pevent->GetTargetID()),
		Q_ARG(bool,pevent->GetReportState()));

	QString sourceStr,targetStr;

	ByyEntityObject* srcEntObj=myHdose.core().remoteObjectManager().findEntity(pevent->m_SrcEntID);
	ByyEntityObject* targetEntObj=myHdose.core().remoteObjectManager().findEntity(pevent->m_TargetID);

	if (srcEntObj)
		sourceStr=srcEntObj->name();
	else
		sourceStr=QString::number(pevent->m_SrcEntID);

	if (targetEntObj)
		targetStr=targetEntObj->name();
	else
		targetStr=QString::number(pevent->m_TargetID);


	QString str;
	str=sourceStr+" "+QString::fromLocal8Bit(pevent->m_ReportState?"跟踪":"不再跟踪");
	str+=" ";
	str+=targetStr;

	str+="\n";

	QMetaObject::invokeMethod(myHdose.simEventLogDatabase(),"appendLog",Qt::QueuedConnection,
		Q_ARG(double, myHdose.simTime()),
		Q_ARG(QString, str));
}

void ByyForeStationProcessor::onTargetIrradiateEvent( CTargetReportEvt *pevent )
{
	if(pevent->m_ReportType != TargetIllumniated )
		return;

	QMetaObject::invokeMethod(&myHdose.core().radioCommsManager(),"addGuidance",Qt::QueuedConnection,
		Q_ARG(int, pevent->GetSrcEntID()),
		Q_ARG(int, pevent->GetTargetID()),
		Q_ARG(bool,pevent->GetReportState()));

	QString sourceStr,targetStr;

	ByyEntityObject* srcEntObj=myHdose.core().remoteObjectManager().findEntity(pevent->m_SrcEntID);
	ByyEntityObject* targetEntObj=myHdose.core().remoteObjectManager().findEntity(pevent->m_TargetID);

	if (srcEntObj)
		sourceStr=srcEntObj->name();
	else
		sourceStr=QString::number(pevent->m_SrcEntID);

	if (targetEntObj)
		targetStr=targetEntObj->name();
	else
		targetStr=QString::number(pevent->m_TargetID);


	QString str;
	str=sourceStr+" "+QString::fromLocal8Bit(pevent->m_ReportState?"照射":"不再照射");
	str+=" ";
	str+=targetStr;

	str+="\n";

	QMetaObject::invokeMethod(myHdose.simEventLogDatabase(),"appendLog",Qt::QueuedConnection,
		Q_ARG(double, myHdose.simTime()),
		Q_ARG(QString, str));
}

void ByyForeStationProcessor::onWeaponLaunchEvent( CWeaponLaunchedEvt *pevent )
{
	QString str;
	ByyEntityObject* entObj=myHdose.core().remoteObjectManager().findEntity(pevent->m_SrcEntID);
	if (entObj)
		str=entObj->name();
	else
		str=QString::number(pevent->m_SrcEntID);

	str+=QString::fromLocal8Bit(" 发射武器，");
	//str+=QString::fromLocal8Bit("发射平台名称：%1，").arg(QString::fromLocal8Bit(pevent->GetSrcEntName()));
	str+=QString::fromLocal8Bit("发射系统名称：%1，").arg(QString::fromLocal8Bit(pevent->GetSrcModelTag()));
	str+=QString::fromLocal8Bit("发射武器名称：%1，").arg(QString::fromLocal8Bit(pevent->GetWeaponEntName()));
	char* wpEnttype = new char[32];
	pevent->GetWeaponEntType().FormatString(wpEnttype);
	str+=QString::fromLocal8Bit("发射武器类别：%1").arg(wpEnttype);
	str+="\n";
	delete wpEnttype;

	QMetaObject::invokeMethod(myHdose.simEventLogDatabase(),"appendLog",Qt::QueuedConnection,
		Q_ARG(double, myHdose.simTime()),
		Q_ARG(QString, str));

	//return;

	qDebug()<<"----OnWeaponLaunchEvent------";
	//QString str;

	//ByyEntityObject* entObj=myHdose.core().remoteObjectManager().findEntity(pevent->m_SrcEntID);


	//ByyEntityObject* entObjWeapon=myHdose.core().remoteObjectManager().findEntity(pevent->m_WeaponEntID); // TODO: 新创建的实体还未更新过来，找不到??
	//if(entObjWeapon)
	{
		//判定导弹类型 统计发射数量 反舰导弹
		if(pevent->m_WeaponEntType.m_kind == 2 && pevent->m_WeaponEntType.m_domain != 1)
		{
			myHdose.setExploInfo(1);
		}
		//拦截弹
		else if(pevent->m_WeaponEntType.m_kind == 2 && pevent->m_WeaponEntType.m_domain == 1)//拦截弹
		{
			myHdose.setExploInfo(2);
		}
		//todo 
		if ( pevent->m_WeaponEntType.m_kind == 2 && pevent->m_WeaponEntType.m_category == 1)
		{
			int side =0 ; // 0红方 1蓝方
			if (entObj->forceType() == ByyForceOpposing)
			{
				side = 1;
			}
			else if(entObj->forceType() == ByyForceFriendly)
			{
				side = 0;
			}
			myHdose.onMissileLaunch(side,pevent->m_InitTargetID,pevent->m_InitTargetName,pevent->m_WeaponEntID,pevent->m_WeaponEntName);
		}
	}
}

void ByyForeStationProcessor::onASMRoutePlanResult( CASMRoutePlanResultEvt *pevent )
{
	QVector<osg::Vec3d> pnts(pevent->m_Route.m_len);

	for (int i=0;i!=pnts.size();++i)
	{
		//StWayPoint* pt=pevent->m_Route+i;

		pnts[i].x()=pevent->m_Route[i].m_y;
		pnts[i].y()=pevent->m_Route[i].m_x;
		pnts[i].z()=pevent->m_Route[i].m_z;
	}

	QMetaObject::invokeMethod(&myHdose.core().radioCommsManager(),"addRoutePlanning",Qt::QueuedConnection,
		Q_ARG(int,pevent->m_LchPltID),
		Q_ARG(QVector<osg::Vec3d>,pnts));

	QString fieldStr,targetStr;

	ByyEntityObject* fieldEntObj=myHdose.core().remoteObjectManager().findEntity(pevent->m_LchPltID);

	ByyEntityObject* targetEntObj=myHdose.core().remoteObjectManager().findEntity(pevent->m_TargetID);

	if (fieldEntObj)
		fieldStr=fieldEntObj->name();
	else
		fieldStr=QString::number(pevent->m_LchPltID);


	if (targetEntObj)
		targetStr=targetEntObj->name();
	else
		targetStr=QString::number(pevent->m_TargetID);

	QString str=QString::fromLocal8Bit(
		"收到航路规划结果，实体：%1，发射架序号：%2，目标：%3").arg(fieldStr)
		.arg(pevent->GetLaunchVehicleNo()).arg(targetStr);

	str+="\n";

	QMetaObject::invokeMethod(myHdose.simEventLogDatabase(),"appendLog",Qt::QueuedConnection,
		Q_ARG(double, myHdose.simTime()),
		Q_ARG(QString, str));
}

void ByyForeStationProcessor::onLaunchMissile( CLaunchWeaponTsk *pevent )
{
	QString str=QString::fromLocal8Bit("任务：发射导弹，");

	QString targetStr;

	ByyEntityObject* targetEntObj=myHdose.core().remoteObjectManager().findEntity(pevent->m_Target.m_TargetID);

	if (targetEntObj)
		targetStr=targetEntObj->name();
	else
		targetStr=QString::number(pevent->m_Target.m_TargetID);

	str+=QString::fromLocal8Bit("目标 ")+targetStr;

	str+="\n";

	QMetaObject::invokeMethod(myHdose.simEventLogDatabase(),"appendLog",Qt::QueuedConnection,
		Q_ARG(double, myHdose.simTime()),
		Q_ARG(QString, str));
}

void ByyForeStationProcessor::onSimCtrl( CSimCtrlEvt *pevent )
{
	QString str;

	switch(pevent->m_SimCtrlType)
	{
	case 1:
		str=QString::fromLocal8Bit("仿真开始");
		break;
	case 2:
		str=QString::fromLocal8Bit("仿真暂停");
		break;
	case 3:
		str=QString::fromLocal8Bit("仿真重置");
		break;
	case 4:
		str=QString::fromLocal8Bit("仿真尽量快");
		break;
	case 5:
		str=QString::fromLocal8Bit("取消仿真尽量快");
		break;
	default:
		return;
	}

	str+="\n";

	QMetaObject::invokeMethod(myHdose.simEventLogDatabase(),"appendLog",Qt::QueuedConnection,
		Q_ARG(double, myHdose.simTime()),
		Q_ARG(QString, str));
}

//void ByyForeStationProcessor::onFollow( CFollow *pevent )
//{
//	QString str=QString::fromLocal8Bit("任务：跟随，");
//
//	QString targetStr;
//
//	ByyEntityObject* targetEntObj=myHdose.core().remoteObjectManager().findEntity(pevent->m_FollowedObjId);
//
//	if (targetEntObj)
//		targetStr=targetEntObj->name();
//	else
//		targetStr=QString::number(pevent->m_FollowedObjId);
//
//	str+=QString::fromLocal8Bit("目标：%1，").arg(targetStr);
//
//	str+=QString::fromLocal8Bit("水平距离：%1").arg(pevent->m_Distance);
//
//	str+="\n";
//
//	QMetaObject::invokeMethod(myHdose.simEventLogDatabase(),"appendLog",Qt::QueuedConnection,
//		Q_ARG(double, myHdose.simTime()),
//		Q_ARG(QString, str));
//}
