#include "stdafx.h"
#include "Verdict.h"
#include "comApp.h"
#include "ByyApp.h"
#include "ByyHdose.h"
#include "ByyCore.h"
#include "remoteObjectManager.h"
#include "settings.h"
#include <QDialog>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QMutexLocker>
#include <QAction>

#include "AirCombatAdvantageCalculation.h"
#include "AircraftTargetTracking.h"
#include "CollisionDetection.h"
#include "DepthChargeVsSubmarineDamageCalculation.h"
#include "EntityAlert.h"
#include "ExercisePartyAlert.h"
#include "ExitJudgment.h"
#include "FireControlLockingCalculation.h"
#include "MissileHitProbabilityCalculation.h"
#include "MissileTargetTracking.h"
#include "MissileVsShipDamageCalculation.h"
#include "NavalEquipmentStatusJudgment.h"
#include "SensorAlarmCalculation.h"
#include "SubmarineEquipmentStatusJudgment.h"
#include "TorpedoVsShipDamageCalculation.h"
#include "TorpedoVsSubmarineDamageCalculation.h"
#include "WarDamageStatistics.h"
#include "JGeoCal/JGeoCalExp.h"

Verdict* Verdict::m_pVerdict = nullptr;

/*设置相关的决策类别*/
Verdict::Verdict(QObject *parent)
	:QObject(parent)
{
	setApp(comApp::instance()->getApp());
}

Verdict* Verdict::getInstance()
{
	if(!m_pVerdict)
	{
		m_pVerdict = new Verdict;
	}
	return m_pVerdict;
}

Verdict::~Verdict()
{
}

void Verdict::setApp(ByyApp* app)
{
	myApp =app;
	connect(&app->hdose(), SIGNAL(signal_started(int)), this, SLOT(slot_started()));
	connect(&app->hdose(), SIGNAL(signal_scnLoaded()), this, SLOT(slot_started()));
	connect(&app->hdose(), SIGNAL(signal_entityUpdate()), this, SLOT(slot_resetEntity()));
	connect(&app->hdose(), SIGNAL(signal_stopVerdict()), this, SLOT(slot_stopHandle()));
	//connect(&app->hdose(), SIGNAL(signal_missileTJ(int)), this, SLOT(slot_missileTJ(int)));
	connect(&app->hdose(), SIGNAL(signal_entityDm(int, const QString&, const QString&)), this, SLOT(slot_entityDm(int, const QString&, const QString&)));
	//connect(&app->hdose(), SIGNAL(signal_Hit(int,QString ,int,QString ,double,double,double)), this, SLOT(slot_OnHit(int,QString ,int,QString ,double,double,double)));
	connect(&app->hdose(), SIGNAL(signal_Hit(const QVariantMap&)), this, SLOT(slot_OnHit(const QVariantMap&)));
	connect(&app->hdose(), SIGNAL(signal_MissileLaunch(int, const QString&, const QString&)), this, SLOT(slot_missileLaunch(int, const QString&, const QString&)));

	m_verdictBaseData.initEntityDamageProb(ByyConfigIni::instance()->entityDamageProbTable());
	m_verdictBaseData.initWeaponHitProb(ByyConfigIni::instance()->weaponHitProbTable());

	//数据测试 以及使用方法
#if 1
	double tntkg;
	qDebug() << __FUNCDNAME__ << __LINE__ << m_verdictBaseData.getEntityDamageProbItems().value("Zero72Ship").warheadProbTables.value("1001").impactProbTables.value("1001").getImpactProb(89, 256.23, 36, tntkg);
	qDebug() << __FUNCDNAME__ << __LINE__ << tntkg;
	qDebug() << __FUNCDNAME__ << __LINE__ << m_verdictBaseData.getEntityDamageProbItems().value("Zero52DDDG").warheadProbTables.value("1001").impactProbTables.value("1001").getImpactProb(150, 256.23, 36, tntkg);
	qDebug() << __FUNCDNAME__ << __LINE__ << tntkg;

	qDebug() << __FUNCDNAME__ << __LINE__ << m_verdictBaseData.getWeaponHitProbItems().value("CYJ83K_DL8Entity").hitProbTables.value("1-3").corrFactors.value("HitHight").factorByDistance(800.0);
	qDebug() << __FUNCDNAME__ << __LINE__ << m_verdictBaseData.getWeaponHitProbItems().value("CYJ83K_DL8Entity").hitProbTables.value("1-3").corrFactors.value("HitHight").factorByDistance(400.0);
	qDebug() << __FUNCDNAME__ << __LINE__ << m_verdictBaseData.getWeaponHitProbItems().value("CYJ83K_DL8Entity").hitProbTables.value("1-3").corrFactors.value("TgtJamType").factorByType(2);
	qDebug() << __FUNCDNAME__ << __LINE__ << m_verdictBaseData.getWeaponHitProbItems().value("CYJ12Entity").hitProbTables.value("1-3").corrFactors.value("LchRange").factorByDistance(5000.0);
	qDebug() << __FUNCDNAME__ << __LINE__ << m_verdictBaseData.getWeaponHitProbItems().value("CYJ12Entity").hitProbTables.value("1-3").corrFactors.value("TgtJamType").factorByType(1);
#endif 
}

void Verdict::showDataDialog(DialogType type, const QVariantMap& map)
{
	QDialog* dialog = new QDialog;
	dialog->setFixedSize(800,600);
	dialog->setWindowFlags(dialog->windowFlags() & ~Qt::WindowContextHelpButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);
	dialog->setAttribute(Qt::WA_DeleteOnClose);
	QHBoxLayout* hlayout = new QHBoxLayout(dialog);
	dialog->setLayout(hlayout);
	switch (type)
	{
	case EntityAlertDialog:
		{
			QString titleName = QString::fromLocal8Bit("实体预警判断");
			EntityAlert* entityAlert = new EntityAlert();
			if(!map.isEmpty())
			{
				QString EntityName = map.value("EntityName").toString();
				entityAlert->showEntityName(EntityName);
				titleName = QString("%1 %2").arg(titleName).arg(EntityName);
			}
			dialog->setWindowTitle(titleName);
			//entityAlert->getEntityAlert(&myApp->core().remoteObjectManager().entityObjects());
			hlayout->addWidget(entityAlert);
			//v->resetEntityAlert();
		}
		break;
	case ExercisePartyAlertDialog:
		{
			dialog->setWindowTitle(QString::fromLocal8Bit("演练方预警判断"));
			ExercisePartyAlert* exercisePartyAlert = new ExercisePartyAlert();
			hlayout->addWidget(exercisePartyAlert);
			//v->getAirEntity(&myApp.core().remoteObjectManager().entityObjects());
		}
		break;
	case AircraftTargetTrackingDialog:
		{
			QString titleName = QString::fromLocal8Bit("飞机目标跟踪");
			AircraftTargetTracking* aircraftTargetTracking = new AircraftTargetTracking();
			if(!map.isEmpty())
			{
				QString EntityName = map.value("EntityName").toString();
				aircraftTargetTracking->showEntityName(EntityName);
				titleName = QString("%1 %2").arg(titleName).arg(EntityName);
			}
			hlayout->addWidget(aircraftTargetTracking);
			dialog->setWindowTitle(titleName);
			//v->getAircraftTargetTrackingEntity(&myApp.core().remoteObjectManager().entityObjects());
			//v->resetAircraftTargetTracking();
		}
		break;
	case MissileTargetTrackingDialog:
		{
			QString titleName = QString::fromLocal8Bit("导弹目标跟踪");
			MissileTargetTracking* missileTargetTracking = new MissileTargetTracking();
			if(!map.isEmpty())
			{
				QString EntityName = map.value("EntityName").toString();
				missileTargetTracking->showEntityName(EntityName);
				titleName = QString("%1 %2").arg(titleName).arg(EntityName);
			}
			hlayout->addWidget(missileTargetTracking);
			dialog->setWindowTitle(titleName);
			//v->getMissileTargetTrackingEntity(&myApp.core().remoteObjectManager().entityObjects());
			//v->resetMissileTargetTracking();
		}
		break;
	case AirCombatAdvantageCalculationDialog:
		{
			dialog->setFixedSize(800,1000);
			dialog->setWindowTitle(QString::fromLocal8Bit("空战优势计算"));
			AirCombatAdvantageCalculation* airCombatAdvantageCalculation = new AirCombatAdvantageCalculation();
			hlayout->addWidget(airCombatAdvantageCalculation);
			//v->getAirEntity(&myApp.core().remoteObjectManager().entityObjects());
			//v->resetAirCombatAdvantage();
		}
		break;
	case FireControlLockingCalculationDialog:
		{
			dialog->setWindowTitle(QString::fromLocal8Bit("火控锁定计算"));
			FireControlLockingCalculation* fireControlLockingCalculation = new FireControlLockingCalculation();
			hlayout->addWidget(fireControlLockingCalculation);
			//v->getDamageEntity(&myApp.core().remoteObjectManager().entityObjects());
		}
		break;
	case SensorAlarmCalculationDialog:
		{
			QString titleName = QString::fromLocal8Bit("传感器告警计算");
			SensorAlarmCalculation* sensorAlarmCalculation = new SensorAlarmCalculation();
			if(!map.isEmpty())
			{
				QString EntityName = map.value("EntityName").toString();
				sensorAlarmCalculation->showEntityName(EntityName);
				titleName = QString("%1 %2").arg(titleName).arg(EntityName);
			}
			hlayout->addWidget(sensorAlarmCalculation);
			dialog->setWindowTitle(titleName);
			//v->getDamageEntity(&myApp.core().remoteObjectManager().entityObjects());
		}
		break;
	case MissileHitProbabilityCalculationDialog:
		{
			dialog->setWindowTitle(QString::fromLocal8Bit("导弹命中概率计算"));
			MissileHitProbabilityCalculation* missileHitProbabilityCalculation = new MissileHitProbabilityCalculation();
			hlayout->addWidget(missileHitProbabilityCalculation);
			//v->getEntityAlert(&myApp.core().remoteObjectManager().entityObjects());
		}
		break;
	case CollisionDetectionDialog:
		{
			dialog->setWindowTitle(QString::fromLocal8Bit("碰撞检测计算"));
			CollisionDetection* collisionDetection = new CollisionDetection();
			hlayout->addWidget(collisionDetection);
			//v->getCollisionEntity(&myApp.core().remoteObjectManager().entityObjects());
			//v->resetCollisionDetection();
		}
		break;
	case TorpedoVsSubmarineDamageCalculationDialog:
		{
			dialog->setWindowTitle(QString::fromLocal8Bit("鱼雷对潜艇毁伤计算"));
			TorpedoVsSubmarineDamageCalculation* torpedoVsSubmarineDamageCalculation = new TorpedoVsSubmarineDamageCalculation();
			hlayout->addWidget(torpedoVsSubmarineDamageCalculation);
			//v->getSubmarineDamageEntity(&myApp.core().remoteObjectManager().entityObjects());
		}
		break;
	case TorpedoVsShipDamageCalculationDialog:
		{
			dialog->setWindowTitle(QString::fromLocal8Bit("鱼雷对舰艇毁伤计算"));
			TorpedoVsShipDamageCalculation* torpedoVsShipDamageCalculation = new TorpedoVsShipDamageCalculation();
			hlayout->addWidget(torpedoVsShipDamageCalculation);
			//v->getShipDamageEntity(&myApp.core().remoteObjectManager().entityObjects());
		}
		break;
	case DepthChargeVsSubmarineDamageCalculationDialog:
		{
			dialog->setWindowTitle(QString::fromLocal8Bit("深水炸弹对潜艇毁伤计算"));
			DepthChargeVsSubmarineDamageCalculation* depthChargeVsSubmarineDamageCalculation = new DepthChargeVsSubmarineDamageCalculation();
			hlayout->addWidget(depthChargeVsSubmarineDamageCalculation);
			//v->getSubmarineDamageEntity(&myApp.core().remoteObjectManager().entityObjects());
		}
		break;
	case MissileVsShipDamageCalculationDialog:
		{
			dialog->setWindowTitle(QString::fromLocal8Bit("导弹对舰艇毁伤计算"));
			MissileVsShipDamageCalculation* missileVsShipDamageCalculation = new MissileVsShipDamageCalculation();
			hlayout->addWidget(missileVsShipDamageCalculation);
			//v->getShipDamageEntity(&myApp.core().remoteObjectManager().entityObjects());
		}
		break;
	case NavalEquipmentStatusJudgmentDialog:
		{
			dialog->setWindowTitle(QString::fromLocal8Bit("舰艇装备状态裁决"));
			NavalEquipmentStatusJudgment* navalEquipmentStatusJudgment = new NavalEquipmentStatusJudgment();
			hlayout->addWidget(navalEquipmentStatusJudgment);
			//v->getShipEntity(&myApp.core().remoteObjectManager().entityObjects());
		}
		break;
	case SubmarineEquipmentStatusJudgmentDialog:
		{
			dialog->setWindowTitle(QString::fromLocal8Bit("潜艇装备状态裁决"));
			SubmarineEquipmentStatusJudgment* submarineEquipmentStatusJudgment = new SubmarineEquipmentStatusJudgment();
			hlayout->addWidget(submarineEquipmentStatusJudgment);
			//v->getSubmarineEntity(&myApp.core().remoteObjectManager().entityObjects());
		}
		break;
	case ExitJudgmentDialog:
		{
			QString titleName = QString::fromLocal8Bit("退出裁决");
			ExitJudgment* exitJudgment = new ExitJudgment();
			if(!map.isEmpty())
			{
				QString EntityName = map.value("EntityName").toString();
				exitJudgment->showEntityName(EntityName);
				titleName = QString("%1 %2").arg(titleName).arg(EntityName);
			}
			dialog->setWindowTitle(titleName);
			hlayout->addWidget(exitJudgment);
			//v->getEntityAlert(&myApp.core().remoteObjectManager().entityObjects());
			//v->resetExitJudgment();
		}
		break;
	case WarDamageStatisticsDialog:
		{
			dialog->setWindowTitle(QString::fromLocal8Bit("战损统计"));
			WarDamageStatistics* warDamageStatistics = new WarDamageStatistics();
			hlayout->addWidget(warDamageStatistics);
			//v->getCollisionEntity(&myApp.core().remoteObjectManager().entityObjects());
			//v->resetWarDamageStatistics();
		}
		break;
	}
	if(dialog)
	{
		if(dialog->windowTitle().isEmpty())
		{
			delete dialog;
		}
		else 
		{
			dialog->show();
		}
	}
}

QAction* Verdict::registerDockWidget(DialogType type, QWidget* widget, const QVariantMap& map)
{
	QDockWidget* dialog = new QDockWidget(widget, Qt::Window);
	dialog->setVisible(false);
	switch (type)
	{
	case EntityAlertDialog:
		{
			QString titleName = QString::fromLocal8Bit("实体预警判断");
			EntityAlert* entityAlert = new EntityAlert();
			if(!map.isEmpty())
			{
				QString EntityName = map.value("EntityName").toString();
				entityAlert->showEntityName(EntityName);
				titleName = QString("%1 %2").arg(titleName).arg(EntityName);
			}
			dialog->setWindowTitle(titleName);
			//entityAlert->getEntityAlert(&myApp->core().remoteObjectManager().entityObjects());
			dialog->setWidget(entityAlert);
			m_verdictWidgets.insert(EntityAlertDialog, entityAlert);
			//v->resetEntityAlert();
		}
		break;
	case ExercisePartyAlertDialog:
		{
			dialog->setWindowTitle(QString::fromLocal8Bit("演练方预警判断"));
			ExercisePartyAlert* exercisePartyAlert = new ExercisePartyAlert();
			dialog->setWidget(exercisePartyAlert);
			m_verdictWidgets.insert(ExercisePartyAlertDialog, exercisePartyAlert);
			//v->getAirEntity(&myApp.core().remoteObjectManager().entityObjects());
		}
		break;
	case AircraftTargetTrackingDialog:
		{
			QString titleName = QString::fromLocal8Bit("飞机目标跟踪");
			AircraftTargetTracking* aircraftTargetTracking = new AircraftTargetTracking();
			if(!map.isEmpty())
			{
				QString EntityName = map.value("EntityName").toString();
				aircraftTargetTracking->showEntityName(EntityName);
				titleName = QString("%1 %2").arg(titleName).arg(EntityName);
			}
			dialog->setWidget(aircraftTargetTracking);
			dialog->setWindowTitle(titleName);
			m_verdictWidgets.insert(AircraftTargetTrackingDialog, aircraftTargetTracking);
			//v->getAircraftTargetTrackingEntity(&myApp.core().remoteObjectManager().entityObjects());
			//v->resetAircraftTargetTracking();
		}
		break;
	case MissileTargetTrackingDialog:
		{
			QString titleName = QString::fromLocal8Bit("导弹目标跟踪");
			MissileTargetTracking* missileTargetTracking = new MissileTargetTracking();
			if(!map.isEmpty())
			{
				QString EntityName = map.value("EntityName").toString();
				missileTargetTracking->showEntityName(EntityName);
				titleName = QString("%1 %2").arg(titleName).arg(EntityName);
			}
			dialog->setWidget(missileTargetTracking);
			dialog->setWindowTitle(titleName);
			m_verdictWidgets.insert(MissileTargetTrackingDialog, missileTargetTracking);
			//v->getMissileTargetTrackingEntity(&myApp.core().remoteObjectManager().entityObjects());
			//v->resetMissileTargetTracking();
		}
		break;
	case AirCombatAdvantageCalculationDialog:
		{
			//dialog->setFixedSize(800,1000);
			dialog->setWindowTitle(QString::fromLocal8Bit("空战优势计算"));
			AirCombatAdvantageCalculation* airCombatAdvantageCalculation = new AirCombatAdvantageCalculation();
			dialog->setWidget(airCombatAdvantageCalculation);
			m_verdictWidgets.insert(AirCombatAdvantageCalculationDialog, airCombatAdvantageCalculation);
			//v->getAirEntity(&myApp.core().remoteObjectManager().entityObjects());
			//v->resetAirCombatAdvantage();
		}
		break;
	case FireControlLockingCalculationDialog:
		{
			dialog->setWindowTitle(QString::fromLocal8Bit("火控锁定计算"));
			FireControlLockingCalculation* fireControlLockingCalculation = new FireControlLockingCalculation();
			dialog->setWidget(fireControlLockingCalculation);
			m_verdictWidgets.insert(FireControlLockingCalculationDialog, fireControlLockingCalculation);
			//v->getDamageEntity(&myApp.core().remoteObjectManager().entityObjects());
		}
		break;
	case SensorAlarmCalculationDialog:
		{
			QString titleName = QString::fromLocal8Bit("传感器告警计算");
			SensorAlarmCalculation* sensorAlarmCalculation = new SensorAlarmCalculation();
			if(!map.isEmpty())
			{
				QString EntityName = map.value("EntityName").toString();
				sensorAlarmCalculation->showEntityName(EntityName);
				titleName = QString("%1 %2").arg(titleName).arg(EntityName);
			}
			dialog->setWidget(sensorAlarmCalculation);
			dialog->setWindowTitle(titleName);
			m_verdictWidgets.insert(SensorAlarmCalculationDialog, sensorAlarmCalculation);
			//v->getDamageEntity(&myApp.core().remoteObjectManager().entityObjects());
		}
		break;
	case MissileHitProbabilityCalculationDialog:
		{
			dialog->setWindowTitle(QString::fromLocal8Bit("导弹命中概率计算"));
			MissileHitProbabilityCalculation* missileHitProbabilityCalculation = new MissileHitProbabilityCalculation();
			dialog->setWidget(missileHitProbabilityCalculation);
			m_verdictWidgets.insert(MissileHitProbabilityCalculationDialog, missileHitProbabilityCalculation);
			//v->getEntityAlert(&myApp.core().remoteObjectManager().entityObjects());
		}
		break;
	case CollisionDetectionDialog:
		{
			dialog->setWindowTitle(QString::fromLocal8Bit("碰撞检测计算"));
			CollisionDetection* collisionDetection = new CollisionDetection();
			dialog->setWidget(collisionDetection);
			m_verdictWidgets.insert(CollisionDetectionDialog, collisionDetection);
			m_collisionDetectionAction = new QAction(QString::fromLocal8Bit("碰撞检测计算"), nullptr);;
			m_collisionDetectionAction->setCheckable(true);
			connect(m_collisionDetectionAction, SIGNAL(triggered()), this, SLOT(slot_collisionDetectionCalculation()));
			return m_collisionDetectionAction;
			//v->getCollisionEntity(&myApp.core().remoteObjectManager().entityObjects());
			//v->resetCollisionDetection();
		}
		break;
	case TorpedoVsSubmarineDamageCalculationDialog:
		{
			dialog->setWindowTitle(QString::fromLocal8Bit("鱼雷对潜艇毁伤计算"));
			TorpedoVsSubmarineDamageCalculation* torpedoVsSubmarineDamageCalculation = new TorpedoVsSubmarineDamageCalculation();
			dialog->setWidget(torpedoVsSubmarineDamageCalculation);
			m_verdictWidgets.insert(TorpedoVsSubmarineDamageCalculationDialog, torpedoVsSubmarineDamageCalculation);
			//v->getSubmarineDamageEntity(&myApp.core().remoteObjectManager().entityObjects());
		}
		break;
	case TorpedoVsShipDamageCalculationDialog:
		{
			dialog->setWindowTitle(QString::fromLocal8Bit("鱼雷对舰艇毁伤计算"));
			TorpedoVsShipDamageCalculation* torpedoVsShipDamageCalculation = new TorpedoVsShipDamageCalculation();
			dialog->setWidget(torpedoVsShipDamageCalculation);
			m_verdictWidgets.insert(TorpedoVsShipDamageCalculationDialog, torpedoVsShipDamageCalculation);
			//v->getShipDamageEntity(&myApp.core().remoteObjectManager().entityObjects());
		}
		break;
	case DepthChargeVsSubmarineDamageCalculationDialog:
		{
			dialog->setWindowTitle(QString::fromLocal8Bit("深水炸弹对潜艇毁伤计算"));
			DepthChargeVsSubmarineDamageCalculation* depthChargeVsSubmarineDamageCalculation = new DepthChargeVsSubmarineDamageCalculation();
			dialog->setWidget(depthChargeVsSubmarineDamageCalculation);
			m_verdictWidgets.insert(DepthChargeVsSubmarineDamageCalculationDialog, depthChargeVsSubmarineDamageCalculation);
			//v->getSubmarineDamageEntity(&myApp.core().remoteObjectManager().entityObjects());
		}
		break;
	case MissileVsShipDamageCalculationDialog:
		{
			dialog->setWindowTitle(QString::fromLocal8Bit("导弹对舰艇毁伤计算"));
			MissileVsShipDamageCalculation* missileVsShipDamageCalculation = new MissileVsShipDamageCalculation();
			dialog->setWidget(missileVsShipDamageCalculation);
			m_verdictWidgets.insert(MissileVsShipDamageCalculationDialog, missileVsShipDamageCalculation);
			//v->getShipDamageEntity(&myApp.core().remoteObjectManager().entityObjects());
		}
		break;
	case NavalEquipmentStatusJudgmentDialog:
		{
			dialog->setWindowTitle(QString::fromLocal8Bit("舰艇装备状态裁决"));
			NavalEquipmentStatusJudgment* navalEquipmentStatusJudgment = new NavalEquipmentStatusJudgment();
			dialog->setWidget(navalEquipmentStatusJudgment);
			m_verdictWidgets.insert(NavalEquipmentStatusJudgmentDialog, navalEquipmentStatusJudgment);
			//v->getShipEntity(&myApp.core().remoteObjectManager().entityObjects());
		}
		break;
	case SubmarineEquipmentStatusJudgmentDialog:
		{
			dialog->setWindowTitle(QString::fromLocal8Bit("潜艇装备状态裁决"));
			SubmarineEquipmentStatusJudgment* submarineEquipmentStatusJudgment = new SubmarineEquipmentStatusJudgment();
			dialog->setWidget(submarineEquipmentStatusJudgment);
			m_verdictWidgets.insert(SubmarineEquipmentStatusJudgmentDialog, submarineEquipmentStatusJudgment);
			//v->getSubmarineEntity(&myApp.core().remoteObjectManager().entityObjects());
		}
		break;
	case ExitJudgmentDialog:
		{
			QString titleName = QString::fromLocal8Bit("退出裁决");
			ExitJudgment* exitJudgment = new ExitJudgment();
			if(!map.isEmpty())
			{
				QString EntityName = map.value("EntityName").toString();
				exitJudgment->showEntityName(EntityName);
				titleName = QString("%1 %2").arg(titleName).arg(EntityName);
			}
			dialog->setWindowTitle(titleName);
			dialog->setWidget(exitJudgment);
			m_verdictWidgets.insert(ExitJudgmentDialog, exitJudgment);
			//v->getEntityAlert(&myApp.core().remoteObjectManager().entityObjects());
			//v->resetExitJudgment();
		}
		break;
	case WarDamageStatisticsDialog:
		{
			dialog->setWindowTitle(QString::fromLocal8Bit("战损统计"));
			WarDamageStatistics* warDamageStatistics = new WarDamageStatistics();
			dialog->setWidget(warDamageStatistics);
			m_verdictWidgets.insert(WarDamageStatisticsDialog, warDamageStatistics);
			//v->getCollisionEntity(&myApp.core().remoteObjectManager().entityObjects());
			//v->resetWarDamageStatistics();
		}
		break;
	}
	
	return dialog->toggleViewAction();
}

EventInfos Verdict::getExplodeEvenfs(EntitySimpleObjectType attckType, EntitySimpleObjectType targetType)
{
	EventInfos infos;
#if 0
	EventInfo info;	
	info.happenTime = QDateTime::currentDateTime().toString("yyyy_MM_dd mm:hh:ss.zzz");
	info.explodeEntityName = "test1";
	info.hitTarget = "test2";
	info.detailInfo.distance = 100.0;
	info.detailInfo.orientation = 0.02;
	info.detailInfo.r = 100.02;
	info.detailInfo.height = 0.02;
	info.detailInfo.damageNumber =10.2;
	info.detailInfo.currentHealth = 89;
	info.detailInfo.damageState = ByyDamageNone;
	infos.append(info);
#else
	// 2 7 2  -> 1 4
	if((DepthChargeObject == attckType) && (SubmarineObject == targetType))
	{
		QMutexLocker locker(&m_mutexStatics);
		infos = m_DcVsSub;
	}
	// 2 6/11 -> 1 3
	else if((MissileObject == attckType) && (ShipEntityObject == targetType)) 
	{
		QMutexLocker locker(&m_mutexStatics);
		infos = m_MisVsShip;
	}
	// 2 6 1 < 0/2 7 1  -> 1 4
	else if((TorpedoObject == attckType) && (SubmarineObject == targetType)) 
	{
		QMutexLocker locker(&m_mutexStatics);
		infos = m_TorVsSub;
	}
	// 2 6 1 < 0 /2 7 1  -> 1 3
	else if((TorpedoObject == attckType) && (ShipEntityObject == targetType))
	{
		QMutexLocker locker(&m_mutexStatics);
		infos = m_TorVsShip;
	}
#endif
	return infos;
}

EntityTypeExpendInfos Verdict::getAmmunitionExpendInfos(const QString& side)
{
	QMap<QString,EntityTypeExpendInfo> statisticsHelper;
	if(RED_SIDE == side)
	{
		QMutexLocker locker(&m_mutexStatics);
		//获取
		foreach(QString entityName, m_redAmmunitionExpendInfos.keys())
		{
			EntityStatusUnit unit = m_redAmmunitionExpendInfos.value(entityName);
			if(!statisticsHelper.contains(unit.typeName))
			{
				statisticsHelper[unit.typeName].entityTypeName = unit.typeName;
				statisticsHelper[unit.typeName].entityNumber = 1;
			}
			else
			{
				statisticsHelper[unit.typeName].entityNumber++;
			}
		}
	}
	else if(BLUE_SIDE == side)
	{
		QMutexLocker locker(&m_mutexStatics);
		//获取
		foreach(QString entityName, m_blueAmmunitionExpendInfos.keys())
		{
			EntityStatusUnit unit = m_blueAmmunitionExpendInfos.value(entityName);
			if(!statisticsHelper.contains(unit.typeName))
			{
				statisticsHelper[unit.typeName].entityTypeName = unit.typeName;
				statisticsHelper[unit.typeName].entityNumber = 1;
			}
			else
			{
				statisticsHelper[unit.typeName].entityNumber++;
			}
		}
	}
	return statisticsHelper.values();
}

EntityTypeExpendInfos Verdict::getEntityExpendInfos(const QString& side, int status)
{
	QMap<int, QMap<QString,EntityTypeExpendInfo>> statisticsHelper;
	//更新 现有的资源
	QList<ByyEntityObject*> remoteentitiesList = myApp->core().remoteObjectManager().entityObjects();
	foreach(ByyEntityObject* object, remoteentitiesList)
	{
		if(MissileObject == typeJudgement(object->entityType(), object->position().z()))
		{
			continue;
		}
		if(RED_SIDE == side && !isRed(object))
		{
			continue;
		}
		else if(BLUE_SIDE == side && !isBlue(object))
		{
			continue;
		}
		//ByyDescribeEntry::ComponentList cList = object->describeEntry()->getComponent("ESR");
		//if(cList.isEmpty())
		//{
		//	continue;
		//}
		//QString typeName = cList.first().getTag();
		QString typeName = object->itemClass();
		int status = object->damageState();
		if(!statisticsHelper[status].contains(typeName))
		{
			statisticsHelper[status][typeName].entityTypeName = typeName;
			statisticsHelper[status][typeName].entityNumber = 1;
		}
		else
		{
			statisticsHelper[status][typeName].entityNumber++;
		}
	}
	if(RED_SIDE == side)
	{
		if(ByyDamageDestroyed == status)
		{
			QMutexLocker locker(&m_mutexStatics);
			//获取
			foreach(QString entityName, m_redEntityExitInfos.keys())
			{
				EntityStatusUnit unit = m_redEntityExitInfos.value(entityName);
				if(!statisticsHelper[unit.status].contains(unit.typeName))
				{
					statisticsHelper[unit.status][unit.typeName].entityTypeName = unit.typeName;
					statisticsHelper[unit.status][unit.typeName].entityNumber = 1;
				}
				else
				{
					statisticsHelper[unit.status][unit.typeName].entityNumber++;
				}
			}
		}
	}
	else if(BLUE_SIDE == side)
	{
		if(ByyDamageDestroyed == status)
		{
			QMutexLocker locker(&m_mutexStatics);
			//获取
			foreach(QString entityName, m_blueEntityExitInfos.keys())
			{
				EntityStatusUnit unit = m_blueEntityExitInfos.value(entityName);
				if(!statisticsHelper[unit.status].contains(unit.typeName))
				{
					statisticsHelper[unit.status][unit.typeName].entityTypeName = unit.typeName;
					statisticsHelper[unit.status][unit.typeName].entityNumber = 1;
				}
				else
				{
					statisticsHelper[unit.status][unit.typeName].entityNumber++;
				}
			}
		}
	}
	return statisticsHelper.value(status).values();
}

void Verdict::slot_started()
{
	QMutexLocker locker(&m_mutexStatics);
	//重新计数
	//损伤统计 onExplosion
	m_DcVsSub.clear();
	m_MisVsShip.clear();
	m_TorVsShip.clear();
	m_TorVsSub.clear();

	//兵力损耗统计 ByyForeStationProcessor
	m_redAmmunitionExpendInfos.clear();	//弹药损耗  onWeaponLaunchEvent
	m_redEntityExitInfos.clear();		//兵力损耗统计
	m_blueAmmunitionExpendInfos.clear();	//弹药损耗  onExplosion ByyHdose::removeEntity
	m_blueEntityExitInfos.clear();		//兵力损耗统计
}

void Verdict::slot_resetEntity()
{
	foreach(VerdictBaseWidget* widget, m_verdictWidgets)
	{
		widget->resetUi();
	}
}

void Verdict::slot_stopHandle()
{
	foreach(VerdictBaseWidget* widget, m_verdictWidgets.values())
	{
		if(CollisionDetectionDialog != m_verdictWidgets.key(widget))
		{
			widget->stopHandle();
			widget->resetUi();
		}
	}
}

void Verdict::slot_entityDm(int side, const QString& objectName, const QString& typeName)
{
	//red
	if(ByyForceFriendly == side)
	{
		QMutexLocker locker(&m_mutexStatics);
		m_redEntityExitInfos[objectName].typeName = typeName;
		m_redEntityExitInfos[objectName].status	  = ByyDamageDestroyed;
	}
	//blue
	else if(ByyForceOpposing == side)
	{
		QMutexLocker locker(&m_mutexStatics);
		m_blueEntityExitInfos[objectName].typeName = typeName;
		m_blueEntityExitInfos[objectName].status   = ByyDamageDestroyed;
	}
}

void Verdict::slot_collisionDetectionCalculation()
{
	if(m_verdictWidgets.contains(CollisionDetectionDialog))
	{
		VerdictBaseWidget* verdict= m_verdictWidgets.value(CollisionDetectionDialog);
		if(verdict->isDataUpdata())
		{
			verdict->stopDataHandle();
		}
		else
		{
			verdict->startDataHandle(1000);
		}
	}
}

void Verdict::slot_OnHit(const QVariantMap& hitEventInfo)
{
	EventInfo info;
	JGeoCal::TopocentricPolar tcp;
	GeoPt	refrencePosition, hitedObjectPosition;
	QString	refrenceName, hitedObjectName;
	ByyEntityType refrenceObjectType, hitedObjectType; //攻击和被攻击目标
	refrenceName			= hitEventInfo.value("Refrence.name").toString();
	refrenceObjectType.kind		= hitEventInfo.value("Refrence.kind").toInt();
	refrenceObjectType.domain	= hitEventInfo.value("Refrence.domain").toInt();
	refrenceObjectType.category	= hitEventInfo.value("Refrence.category").toInt();
	refrencePosition.x		= hitEventInfo.value("Location.x").toDouble();
	refrencePosition.y		= hitEventInfo.value("Location.y").toDouble();
	refrencePosition.z		= hitEventInfo.value("Location.z").toDouble();

	hitedObjectName			= hitEventInfo.value("Hitted.name").toString();
	hitedObjectType.kind		= hitEventInfo.value("Hitted.kind").toInt();
	hitedObjectType.domain		= hitEventInfo.value("Hitted.domain").toInt();
	hitedObjectType.category	= hitEventInfo.value("Hitted.category").toInt();
	hitedObjectPosition.x	= hitEventInfo.value("HittedObjPos.x").toDouble();
	hitedObjectPosition.y	= hitEventInfo.value("HittedObjPos.y").toDouble();
	hitedObjectPosition.z	= hitEventInfo.value("HittedObjPos.z").toDouble();

	info.happenTime = hitEventInfo.value("Event.time").toString();
	info.explodeEntityName = refrenceName;
	info.hitTarget = hitedObjectName;
	info.detailInfo.currentHealth = hitEventInfo.value("Hitted.currentHealth").toDouble();
	info.detailInfo.damageState = static_cast<ByyDamageState>(hitEventInfo.value("Hitted.damageState").toInt());
	//计算相对位置关系
	JGeoCal::GeodeticInDegreeToTopocentricPolar(tcp, refrencePosition, hitedObjectPosition);
	info.detailInfo.distance = tcp.r;
	info.detailInfo.r = tcp.r;
	info.detailInfo.height = tcp.El;
	info.detailInfo.orientation = JGeoCal::Math::zeroTo360(tcp.Az);

	// 20240529,xks
	// 查表获取毁伤点数
	QString tgtItemcls = hitEventInfo.value("HittedObjItemCls").toString(); // 目标实体类名
	double tntkg=0;
	info.detailInfo.damageNumber = m_verdictBaseData.getEntityDamageProbItems().value(hitEventInfo.value("HittedObjItemCls").toString()).warheadProbTables.value("1001").impactProbTables.value("1001").getImpactProb(info.detailInfo.distance, info.detailInfo.orientation, info.detailInfo.height, tntkg);

	// 2 7 2  -> 1 4
	if((DepthChargeObject == typeJudgement(refrenceObjectType, refrencePosition.z)) && (SubmarineObject == typeJudgement(hitedObjectType, hitedObjectPosition.z)))
	{
		QMutexLocker locker(&m_mutexStatics);
		m_DcVsSub.append(info);
	}
	// 2 6/11 -> 1 3
	else if((MissileObject == typeJudgement(refrenceObjectType, refrencePosition.z)) && (ShipEntityObject == typeJudgement(hitedObjectType, hitedObjectPosition.z))) 
	{
		QMutexLocker locker(&m_mutexStatics);
		m_MisVsShip.append(info);
	}
	// 2 6 1 < 0/2 7 1  -> 1 4
	else if((TorpedoObject == typeJudgement(refrenceObjectType, refrencePosition.z)) && (SubmarineObject == typeJudgement(hitedObjectType, hitedObjectPosition.z))) 
	{
		QMutexLocker locker(&m_mutexStatics);
		m_TorVsSub.append(info);
	}
	// 2 6 1 < 0 /2 7 1  -> 1 3
	else if((TorpedoObject == typeJudgement(refrenceObjectType, refrencePosition.z)) && (ShipEntityObject == typeJudgement(hitedObjectType, hitedObjectPosition.z)))
	{
		QMutexLocker locker(&m_mutexStatics);
		m_TorVsShip.append(info);
	}
}

void Verdict::slot_missileLaunch(int side, const QString& objectName, const QString& typeName)
{
	//red
	if(ByyForceFriendly == side)
	{
		QMutexLocker locker(&m_mutexStatics);
		m_redAmmunitionExpendInfos[objectName].typeName = typeName;
		m_redAmmunitionExpendInfos[objectName].status   = ByyDamageDestroyed;
	}
	//blue
	else if(ByyForceOpposing == side)
	{
		QMutexLocker locker(&m_mutexStatics);
		m_blueAmmunitionExpendInfos[objectName].typeName = typeName;
		m_blueAmmunitionExpendInfos[objectName].status	 = ByyDamageDestroyed;
	}
}