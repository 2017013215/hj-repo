#include "StdAfx.h"
#include "simulationActions.h"
#include "ByyHdose.h"
#include "ByyApp.h"
#include "simuController.h"
#include "scenarioManager.h"
#include "scenarioV2.h"
#include "scnProfileWidget.h"
#include "scnEnvironmentSettingsWidget.h"
#include "mainWindow.h"
#include "scenarioFileUtil.h"

//enum WidgetType
//{
//	EntityAlert,
//	ExercisePartyAlert,
//	AircraftTargetTracking,
//	MissileTargetTracking,
//	AirCombatAdvantageCalculation,
//	FireControlLockingCalculation,
//	SensorAlarmCalculation,
//	MissileHitProbabilityCalculation,
//	CollisionDetection,
//	TorpedoVsSubmarineDamageCalculation,
//	TorpedoVsShipDamageCalculation,
//	DepthChargeVsSubmarineDamageCalculation,
//	MissileVsShipDamageCalculation,
//	NavalEquipmentStatusJudgment,
//	SubmarineEquipmentStatusJudgment,
//	ExitJudgment,
//	WarDamageStatistics,
//};

ByyRewindAction::ByyRewindAction( ByyApp& app,QWidget* parent/*=0*/ ) 
	: ByyAction(app,"ByyRewindAction",parent)
{
	setText( QString::fromLocal8Bit("复位(&R)") );
	setIcon( QIcon("../data/icons/rewind.png") );

	connect(&myApp.hdose(),SIGNAL(signal_runningChanged()),SLOT(checkEnabled()));
	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
	connect(&myApp.hdose().simuController(),SIGNAL(signal_StatusChanged(int)),SLOT(checkEnabled()));
}

ByyRewindAction::~ByyRewindAction()
{

}

void ByyRewindAction::on_triggered()
{
	myApp.hdose().simuController().pause();
	myApp.hdose().simuController().restart();

	if (myApp.hdose().scenarioManager().scenario()&&myApp.hdose().scenarioManager().scenario()->modified())
	{
		myApp.hdose().scenarioManager().reload();
	}
}

void ByyRewindAction::checkEnabled()
{
	if(myApp.hdose().mode() == ByyHdose::Join)
	{
		setEnabled(myApp.hdose().started()&&myApp.hdose().simuController().scnLoaded());
	}
	else
	{
		setEnabled(myApp.hdose().started()&&myApp.hdose().scenarioManager().scenario()&&myApp.hdose().simuController().scnLoaded());
	}
}

ByyStartAction::ByyStartAction( ByyApp& app,QWidget* parent/*=0*/ ) 
	: ByyAction(app,"ByyStartAction",parent)
{
	setText( QString::fromLocal8Bit("开始(&S)") );
	setIcon( QIcon("../data/icons/play.png") );

	connect(&myApp.hdose(),SIGNAL(signal_runningChanged()),SLOT(checkEnabled()));
	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
	connect(&myApp.hdose().simuController(),SIGNAL(signal_StatusChanged(int)),SLOT(checkEnabled()));
}

ByyStartAction::~ByyStartAction()
{

}

void ByyStartAction::on_triggered()
{
	if (myApp.hdose().simuController().isPaused())
	{
		if (!myApp.hdose().simuController().scnLoaded())
		{
			if (!ByyScenarioFileUtil::okToContinue(myApp))
			{
				return;
			}

			if(myApp.hdose().scenarioManager().scenario())
			{
				if (myApp.hdose().scenarioManager().scenario()->filename().isEmpty())
				{
					ByyScenarioFileUtil::save(myApp);
				}

				myApp.hdose().simuController().loadScenario(myApp.hdose().scenarioManager().scenario()->filename().toLocal8Bit().data());
			}
		}

		myApp.hdose().simuController().start();
	}
}

void ByyStartAction::checkEnabled()
{
	if(myApp.hdose().mode() == ByyHdose::Join)
	{
		setEnabled(myApp.hdose().started()&&
			myApp.hdose().simuController().isPaused());
	}
	else
	{
		setEnabled(myApp.hdose().started()&&
			myApp.hdose().scenarioManager().scenario()&&
			myApp.hdose().simuController().isPaused());
	}
}

ByyPauseAction::ByyPauseAction( ByyApp& app,QWidget* parent/*=0*/ )
	: ByyAction(app,"ByyPauseAction",parent)
{
	setText( QString::fromLocal8Bit("暂停(&P)") );
	setIcon( QIcon("../data/icons/pause.png") );

	connect(&myApp.hdose(),SIGNAL(signal_runningChanged()),SLOT(checkEnabled()));
	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
	connect(&myApp.hdose().simuController(),SIGNAL(signal_StatusChanged(int)),SLOT(checkEnabled()));
}

ByyPauseAction::~ByyPauseAction()
{

}

void ByyPauseAction::on_triggered()
{
	myApp.hdose().simuController().pause();
}

void ByyPauseAction::checkEnabled()
{
	if(myApp.hdose().mode() == ByyHdose::Join)
	{
		setEnabled(myApp.hdose().started()&&
			myApp.hdose().simuController().isRunning());
	}
	else
	{
		setEnabled(myApp.hdose().started()&&
		myApp.hdose().scenarioManager().scenario()&&
		myApp.hdose().simuController().isRunning());
	}
}

ByyFastBestAction::ByyFastBestAction( ByyApp& app,QWidget* parent/*=0*/ )
	: ByyCheckableAction(app,"ByyFastBestAction",parent)
{
	setText( QString::fromLocal8Bit("尽量快(&F)") );
	setIcon( QIcon("../data/icons/speedup.png") );

	connect(&myApp.hdose(),SIGNAL(signal_runningChanged()),SLOT(checkEnabled()));
	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
	connect(&myApp.hdose().simuController(),SIGNAL(signal_StatusChanged(int)),SLOT(checkEnabled()));
	connect(&myApp.hdose().simuController(),SIGNAL(signal_fastBestChanged(bool)),SLOT(checkEnabled()));
	
}

ByyFastBestAction::~ByyFastBestAction()
{

}

void ByyFastBestAction::on_triggered()
{
	if (isChecked())
		myApp.hdose().simuController().fastBest();
	else
		myApp.hdose().simuController().normal();
}

void ByyFastBestAction::checkEnabled()
{
	if(myApp.hdose().mode() == ByyHdose::Join)
	{
		setEnabled(myApp.hdose().started()&&
			(myApp.hdose().simuController().isRunning()||myApp.hdose().simuController().isPaused()));
	}
	else
	{
		setEnabled(myApp.hdose().started()&&
		myApp.hdose().scenarioManager().scenario()&&
			(myApp.hdose().simuController().isRunning()||myApp.hdose().simuController().isPaused()));
	}

	blockSignals(true);
	setChecked(myApp.hdose().simuController().isFastBest());
	blockSignals(false);
}

ByyScnProfileSettingsAction::ByyScnProfileSettingsAction( ByyApp& app,QWidget* parent/*=0*/ )
	: ByyAction(app,"ByyScnProfileSettingsAction",parent)
{
	setText( QString::fromLocal8Bit("仿真配置(&R)") );
	setIcon( QIcon("../data/icons/directory_schema.png") );

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioCreated(ByyScenarioV2*)),SLOT(on_triggered()));
	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioLoaded(ByyScenarioV2*)),SLOT(on_triggered()));
}

ByyScnProfileSettingsAction::~ByyScnProfileSettingsAction()
{

}

void ByyScnProfileSettingsAction::on_triggered()
{
	ByyScnProfileWidget *w=new ByyScnProfileWidget(myApp,&myApp.mainWindow());
	w->setWindowFlags(Qt::Window);
	w->setAttribute(Qt::WA_DeleteOnClose);
	w->show();
}

void ByyScnProfileSettingsAction::checkEnabled()
{
	setEnabled(myApp.hdose().scenarioManager().scenario());
}

ByyScnEnvironmentSettingsAction::ByyScnEnvironmentSettingsAction( ByyApp& app,QWidget* parent/*=0*/ )
	: ByyAction(app,"ByyScnEnvironmentSettingsAction",parent)
{
	setText( QString::fromLocal8Bit("环境信息(&R)") );
	setIcon( QIcon("../data/icons/weather_few_clouds.svg") );

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
}

ByyScnEnvironmentSettingsAction::~ByyScnEnvironmentSettingsAction()
{

}

void ByyScnEnvironmentSettingsAction::on_triggered()
{
	ByyScnEnvironmentSettingsWidget *w=new ByyScnEnvironmentSettingsWidget(myApp,&myApp.mainWindow());
	w->setWindowFlags(Qt::Window);
	w->setAttribute(Qt::WA_DeleteOnClose);
	w->show();
}

void ByyScnEnvironmentSettingsAction::checkEnabled()
{
	setEnabled(myApp.hdose().scenarioManager().scenario());
}



ByySnapShotAction::ByySnapShotAction( ByyApp& app,QWidget* parent/*=0*/ )
	: ByyAction(app,"ByySnapShotAction",parent)
{
	setText( QString::fromLocal8Bit("仿真快照(&S)") );
	setIcon( QIcon("../data/icons/snapshoot.png") );

	connect(&myApp.hdose(),SIGNAL(signal_runningChanged()),SLOT(checkEnabled()));
	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
	connect(&myApp.hdose().simuController(),SIGNAL(signal_StatusChanged(int)),SLOT(checkEnabled()));
}

ByySnapShotAction::~ByySnapShotAction()
{

}

void ByySnapShotAction::on_triggered()
{
	myApp.hdose().snapShot();
}

void ByySnapShotAction::checkEnabled()
{
	if(myApp.hdose().mode() == ByyHdose::Join)
	{
		setEnabled(myApp.hdose().started()&&
			myApp.hdose().simuController().isRunning());
	}
	else
	{
		setEnabled(myApp.hdose().started()&&
			myApp.hdose().scenarioManager().scenario()&&
			myApp.hdose().simuController().isRunning());
	}
}

#include "Formation/formationWidget.h"

ByyFormationEditAction::ByyFormationEditAction(ByyApp& app,QWidget* parent/*=0*/)
	: ByyAction(app,"ByyFormationEditAction",parent)
{
	setText( QString::fromLocal8Bit("编队阵型(&F)"));
	setIcon( QIcon("../data/icons/Formation/ft.png") );
	setChecked(false);
	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
	m_formationWidget = NULL;
}

ByyFormationEditAction::~ByyFormationEditAction()
{

}

void ByyFormationEditAction::on_triggered()
{
	m_formationWidget = new formationWidget(&myApp,&myApp.mainWindow());
	m_formationWidget->setWindowFlags(Qt::Window);
	m_formationWidget->setAttribute(Qt::WA_DeleteOnClose);
	m_formationWidget->getScnFormation();
	m_formationWidget->show();
	m_formationWidget->closeFlag = 0;
}

void ByyFormationEditAction::checkEnabled()
{
	setEnabled(myApp.hdose().scenarioManager().scenario());
	// 关闭窗口保存想定时有问题，用closeFlag修正问题
	if(myApp.hdose().scenarioManager().scenario() && m_formationWidget && !m_formationWidget->closeFlag)
	{
		m_formationWidget->clearSceneItem();
		m_formationWidget->getScnFormation();
	}
}

//空战优势裁决 9/7
#include "Verdict.h"
ByyAirCombatSuperiorityAction::ByyAirCombatSuperiorityAction(ByyApp& app,QWidget* parent/* =0 */)
	: ByyAction(app,"ByyAirCombatSuperiorityAction",parent)
{
	setText(QString::fromLocal8Bit("空战优势裁决"));
	setChecked(false);

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
}
ByyAirCombatSuperiorityAction::~ByyAirCombatSuperiorityAction()
{

}
void ByyAirCombatSuperiorityAction::on_triggered()
{
	//QString::fromLocal8Bit("空战优势裁决")
}

void ByyAirCombatSuperiorityAction::checkEnabled()
{
	setEnabled(myApp.hdose().scenarioManager().scenario());
}

//目标预警裁决 9/7
ByyTargetAlertAction::ByyTargetAlertAction(ByyApp& app,QWidget* parent/* =0 */)
	: ByyAction(app,"ByyTargetAlertAction",parent)
{
	setText(QString::fromLocal8Bit("目标预警裁决"));
	setChecked(false);

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
}
ByyTargetAlertAction::~ByyTargetAlertAction()
{

}
void ByyTargetAlertAction::on_triggered()
{
	//QString::fromLocal8Bit("目标预警裁决");
}

void ByyTargetAlertAction::checkEnabled()
{
	setEnabled(myApp.hdose().scenarioManager().scenario());
}

//碰撞检测 9/7
ByyCollisionDetectionAction::ByyCollisionDetectionAction(ByyApp& app,QWidget* parent/* =0 */)
	: ByyAction(app,"ByyCollisionDetectionAction",parent)
{
	setText(QString::fromLocal8Bit("碰撞检测计算"));
	setChecked(false);

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
}
ByyCollisionDetectionAction::~ByyCollisionDetectionAction()
{

}
void ByyCollisionDetectionAction::on_triggered()
{
	Verdict::getInstance()->showDataDialog(CollisionDetectionDialog);
}
void ByyCollisionDetectionAction::checkEnabled()
{
	setEnabled(myApp.hdose().scenarioManager().scenario());
	setEnabled(true);
}

//毁伤裁决 9/7
ByyDamageAction::ByyDamageAction(ByyApp& app,QWidget* parent/* =0 */)
	: ByyAction(app,"ByyCollisionDetectionAction",parent)
{
	setText(QString::fromLocal8Bit("毁伤裁决"));
	setChecked(false);

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
}

ByyDamageAction::~ByyDamageAction()
{

}
void ByyDamageAction::on_triggered()
{
	//QString::fromLocal8Bit("毁伤裁决")
}

void ByyDamageAction::checkEnabled()
{
	setEnabled(myApp.hdose().scenarioManager().scenario());
}

//实体预警判断 2024/04/22
ByyEntityAlertAction::ByyEntityAlertAction(ByyApp& app,QWidget* parent/*=0*/)
	: ByyAction(app,"ByyEntityAlertAction",parent)
{
	setText(QString::fromLocal8Bit("实体预警判断"));
	setChecked(false);

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
}

ByyEntityAlertAction::~ByyEntityAlertAction()
{

}

void ByyEntityAlertAction::on_triggered()
{
	Verdict::getInstance()->showDataDialog(EntityAlertDialog);
}

void ByyEntityAlertAction::checkEnabled()
{
	setEnabled(myApp.hdose().scenarioManager().scenario());
	setEnabled(true);
}

//演练方预警判断  2024/4/22 
ByyExercisePartyAlertAction::ByyExercisePartyAlertAction(ByyApp& app,QWidget* parent/*=0*/)
	: ByyAction(app,"ByyExercisePartyAlertAction",parent)
{
	setText(QString::fromLocal8Bit("演练方预警判断"));
	setChecked(false);

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
}

ByyExercisePartyAlertAction::~ByyExercisePartyAlertAction()
{

}

void ByyExercisePartyAlertAction::on_triggered()
{
	Verdict::getInstance()->showDataDialog(ExercisePartyAlertDialog);
}

void ByyExercisePartyAlertAction::checkEnabled()
{
	//setEnabled(myApp.hdose().scenarioManager().scenario());
	setEnabled(true);
}

//飞机目标跟踪  2024/4/22 
ByyAircraftTargetTrackingAction::ByyAircraftTargetTrackingAction(ByyApp& app,QWidget* parent/*=0*/)
	: ByyAction(app,"ByyAircraftTargetTrackingAction",parent)
{
	setText(QString::fromLocal8Bit("飞机目标跟踪"));
	setChecked(false);

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
}

ByyAircraftTargetTrackingAction::~ByyAircraftTargetTrackingAction()
{

}

void ByyAircraftTargetTrackingAction::on_triggered()
{
	Verdict::getInstance()->showDataDialog(AircraftTargetTrackingDialog);
}

void ByyAircraftTargetTrackingAction::checkEnabled()
{
	setEnabled(myApp.hdose().scenarioManager().scenario());
	setEnabled(true);
}

// ByyMissileTargetTrackingAction 导弹目标跟踪  2024/4/22 
ByyMissileTargetTrackingAction::ByyMissileTargetTrackingAction(ByyApp& app,QWidget* parent/*=0*/)
	: ByyAction(app,"ByyMissileTargetTrackingAction",parent)
{
	setText(QString::fromLocal8Bit("导弹目标跟踪"));
	setChecked(false);

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
}

ByyMissileTargetTrackingAction::~ByyMissileTargetTrackingAction()
{

}

void ByyMissileTargetTrackingAction::on_triggered()
{
	Verdict::getInstance()->showDataDialog(MissileTargetTrackingDialog);
}

void ByyMissileTargetTrackingAction::checkEnabled()
{
	setEnabled(myApp.hdose().scenarioManager().scenario());
	setEnabled(true);
}

// ByyAirCombatAdvantageCalculationAction 空战优势计算  2024/4/22 
ByyAirCombatAdvantageCalculationAction::ByyAirCombatAdvantageCalculationAction(ByyApp& app,QWidget* parent/*=0*/)
	: ByyAction(app,"ByyAirCombatAdvantageCalculationAction",parent)
{
	setText(QString::fromLocal8Bit("空战优势计算"));
	setChecked(false);

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
}

ByyAirCombatAdvantageCalculationAction::~ByyAirCombatAdvantageCalculationAction()
{

}

void ByyAirCombatAdvantageCalculationAction::on_triggered()
{
	Verdict::getInstance()->showDataDialog(AirCombatAdvantageCalculationDialog);
}

void ByyAirCombatAdvantageCalculationAction::checkEnabled()
{
	setEnabled(myApp.hdose().scenarioManager().scenario());
}

// ByyFireControlLockingCalculationAction 火控锁定计算  2024/4/22 
ByyFireControlLockingCalculationAction::ByyFireControlLockingCalculationAction(ByyApp& app,QWidget* parent/*=0*/)
	: ByyAction(app,"ByyFireControlLockingCalculationAction",parent)
{
	setText(QString::fromLocal8Bit("火控锁定计算"));
	setChecked(false);

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
}

ByyFireControlLockingCalculationAction::~ByyFireControlLockingCalculationAction()
{

}

void ByyFireControlLockingCalculationAction::on_triggered()
{
	Verdict::getInstance()->showDataDialog(FireControlLockingCalculationDialog);
}

void ByyFireControlLockingCalculationAction::checkEnabled()
{
	setEnabled(myApp.hdose().scenarioManager().scenario());
	setEnabled(true);
}

// ByySensorAlarmCalculationAction 传感器告警计算  2024/4/22 
ByySensorAlarmCalculationAction::ByySensorAlarmCalculationAction(ByyApp& app,QWidget* parent/*=0*/)
	: ByyAction(app,"ByySensorAlarmCalculationAction",parent)
{
	setText(QString::fromLocal8Bit("传感器告警计算"));
	setChecked(false);

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
}

ByySensorAlarmCalculationAction::~ByySensorAlarmCalculationAction()
{

}

void ByySensorAlarmCalculationAction::on_triggered()
{
	Verdict::getInstance()->showDataDialog(SensorAlarmCalculationDialog);
}

void ByySensorAlarmCalculationAction::checkEnabled()
{
	setEnabled(myApp.hdose().scenarioManager().scenario());
	setEnabled(true);
}

// ByyMissileHitProbabilityCalculationAction 导弹命中概率计算  2024/4/22 
ByyMissileHitProbabilityCalculationAction::ByyMissileHitProbabilityCalculationAction(ByyApp& app,QWidget* parent/*=0*/)
	: ByyAction(app,"ByyMissileHitProbabilityCalculationAction",parent)
{
	setText(QString::fromLocal8Bit("导弹命中概率计算"));
	setChecked(false);

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
}

ByyMissileHitProbabilityCalculationAction::~ByyMissileHitProbabilityCalculationAction()
{

}

void ByyMissileHitProbabilityCalculationAction::on_triggered()
{
	Verdict::getInstance()->showDataDialog(MissileHitProbabilityCalculationDialog);
}

void ByyMissileHitProbabilityCalculationAction::checkEnabled()
{
	setEnabled(myApp.hdose().scenarioManager().scenario());
	setEnabled(true);
}

// ByyTorpedoVsSubmarineDamageCalculationAction 鱼雷对潜艇毁伤计算  2024/4/22 
ByyTorpedoVsSubmarineDamageCalculationAction::ByyTorpedoVsSubmarineDamageCalculationAction(ByyApp& app,QWidget* parent/*=0*/)
	: ByyAction(app,"ByyTorpedoVsSubmarineDamageCalculationAction",parent)
{
	setText(QString::fromLocal8Bit("鱼雷对潜艇毁伤计算"));
	setChecked(false);

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
}

ByyTorpedoVsSubmarineDamageCalculationAction::~ByyTorpedoVsSubmarineDamageCalculationAction()
{

}

void ByyTorpedoVsSubmarineDamageCalculationAction::on_triggered()
{
	Verdict::getInstance()->showDataDialog(TorpedoVsSubmarineDamageCalculationDialog);
}

void ByyTorpedoVsSubmarineDamageCalculationAction::checkEnabled()
{
	setEnabled(myApp.hdose().scenarioManager().scenario());
	setEnabled(true);
}

// ByyTorpedoVsShipDamageCalculationAction 鱼雷对舰艇毁伤计算  2024/4/22 
ByyTorpedoVsShipDamageCalculationAction::ByyTorpedoVsShipDamageCalculationAction(ByyApp& app,QWidget* parent/*=0*/)
	: ByyAction(app,"ByyTorpedoVsShipDamageCalculationAction",parent)
{
	setText(QString::fromLocal8Bit("鱼雷对舰艇毁伤计算"));
	setChecked(false);

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
}

ByyTorpedoVsShipDamageCalculationAction::~ByyTorpedoVsShipDamageCalculationAction()
{

}

void ByyTorpedoVsShipDamageCalculationAction::on_triggered()
{
	Verdict::getInstance()->showDataDialog(TorpedoVsShipDamageCalculationDialog);
}

void ByyTorpedoVsShipDamageCalculationAction::checkEnabled()
{
	setEnabled(myApp.hdose().scenarioManager().scenario());
	setEnabled(true);
}

// ByyDepthChargeVsSubmarineDamageCalculationAction 深水炸弹对潜艇毁伤计算  2024/4/22 
ByyDepthChargeVsSubmarineDamageCalculationAction::ByyDepthChargeVsSubmarineDamageCalculationAction(ByyApp& app,QWidget* parent/*=0*/)
		: ByyAction(app,"ByyDepthChargeVsSubmarineDamageCalculationAction",parent)
{
	setText(QString::fromLocal8Bit("深水炸弹对潜艇毁伤计算"));
	setChecked(false);

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
}

ByyDepthChargeVsSubmarineDamageCalculationAction::~ByyDepthChargeVsSubmarineDamageCalculationAction()
{

}

void ByyDepthChargeVsSubmarineDamageCalculationAction::on_triggered()
{
	Verdict::getInstance()->showDataDialog(DepthChargeVsSubmarineDamageCalculationDialog);
}

void ByyDepthChargeVsSubmarineDamageCalculationAction::checkEnabled()
{
	setEnabled(myApp.hdose().scenarioManager().scenario());
	setEnabled(true);
}

// ByyMissileVsShipDamageCalculationAction 导弹对舰艇毁伤计算  2024/4/22 
ByyMissileVsShipDamageCalculationAction::ByyMissileVsShipDamageCalculationAction(ByyApp& app,QWidget* parent/*=0*/)
			: ByyAction(app,"ByyMissileVsShipDamageCalculationAction",parent)
{
	setText(QString::fromLocal8Bit("导弹对舰艇毁伤计算"));
	setChecked(false);

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
}

ByyMissileVsShipDamageCalculationAction::~ByyMissileVsShipDamageCalculationAction()
{

}

void ByyMissileVsShipDamageCalculationAction::on_triggered()
{
	Verdict::getInstance()->showDataDialog(MissileVsShipDamageCalculationDialog);
}

void ByyMissileVsShipDamageCalculationAction::checkEnabled()
{
	setEnabled(myApp.hdose().scenarioManager().scenario());
	setEnabled(true);
}

// ByyNavalEquipmentStatusJudgmentAction 舰艇装备状态裁决  2024/4/22 
ByyNavalEquipmentStatusJudgmentAction::ByyNavalEquipmentStatusJudgmentAction(ByyApp& app,QWidget* parent/*=0*/)
	: ByyAction(app,"ByyNavalEquipmentStatusJudgmentAction",parent)
{
	setText(QString::fromLocal8Bit("舰艇装备状态裁决"));
	setChecked(false);

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
}

ByyNavalEquipmentStatusJudgmentAction::~ByyNavalEquipmentStatusJudgmentAction()
{

}

void ByyNavalEquipmentStatusJudgmentAction::on_triggered()
{
	Verdict::getInstance()->showDataDialog(NavalEquipmentStatusJudgmentDialog);
}

void ByyNavalEquipmentStatusJudgmentAction::checkEnabled()
{
	setEnabled(myApp.hdose().scenarioManager().scenario());
	setEnabled(true);
}

// ByySubmarineEquipmentStatusJudgmentAction 潜艇装备状态裁决  2024/4/22 
ByySubmarineEquipmentStatusJudgmentAction::ByySubmarineEquipmentStatusJudgmentAction(ByyApp& app,QWidget* parent/*=0*/)
	: ByyAction(app,"ByySubmarineEquipmentStatusJudgmentAction",parent)
{
	setText(QString::fromLocal8Bit("潜艇装备状态裁决"));
	setChecked(false);

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
}

ByySubmarineEquipmentStatusJudgmentAction::~ByySubmarineEquipmentStatusJudgmentAction()
{

}

void ByySubmarineEquipmentStatusJudgmentAction::on_triggered()
{
	Verdict::getInstance()->showDataDialog(SubmarineEquipmentStatusJudgmentDialog);
}

void ByySubmarineEquipmentStatusJudgmentAction::checkEnabled()
{
	setEnabled(myApp.hdose().scenarioManager().scenario());
	setEnabled(true);
}

// ByyWarDamageStatisticsAction 战损统计  2024/4/22 
ByyWarDamageStatisticsAction::ByyWarDamageStatisticsAction(ByyApp& app,QWidget* parent/*=0*/)
	: ByyAction(app,"ByyWarDamageStatisticsAction",parent)
{
	setText(QString::fromLocal8Bit("战损统计"));
	setChecked(false);

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
}

ByyWarDamageStatisticsAction::~ByyWarDamageStatisticsAction()
{

}

void ByyWarDamageStatisticsAction::on_triggered()
{
	Verdict::getInstance()->showDataDialog(WarDamageStatisticsDialog);
}

void ByyWarDamageStatisticsAction::checkEnabled()
{
	setEnabled(myApp.hdose().scenarioManager().scenario());
	setEnabled(true);
}

ByyExitJudgmentAction::ByyExitJudgmentAction(ByyApp& app,QWidget* parent/*=0*/)
	: ByyAction(app,"ByyExitJudgmentAction",parent)
{
	setText(QString::fromLocal8Bit("退出裁决"));
	setChecked(false);

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
}

ByyExitJudgmentAction::~ByyExitJudgmentAction()
{

}

void ByyExitJudgmentAction::on_triggered()
{
	Verdict::getInstance()->showDataDialog(ExitJudgmentDialog);
}

void ByyExitJudgmentAction::checkEnabled()
{
	setEnabled(myApp.hdose().scenarioManager().scenario());
	setEnabled(true);
}
