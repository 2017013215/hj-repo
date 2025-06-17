#ifndef SIMULATIONMENUITEMS_H
#define SIMULATIONMENUITEMS_H

#include "ByyAction.h"

class ByyRewindAction : public ByyAction
{
public:
	ByyRewindAction(ByyApp& app,QWidget* parent=0);
	~ByyRewindAction();

protected slots:
	virtual void on_triggered();
	virtual void checkEnabled();
	
};

class ByyStartAction : public ByyAction
{
public:
	ByyStartAction(ByyApp& app,QWidget* parent=0);
	~ByyStartAction();

protected slots:
	virtual void on_triggered();
	virtual void checkEnabled();

};

class ByyPauseAction : public ByyAction
{
public:
	ByyPauseAction(ByyApp& app,QWidget* parent=0);
	~ByyPauseAction();

protected slots:
	virtual void on_triggered();
	virtual void checkEnabled();

};

class ByyFastBestAction : public ByyCheckableAction
{
public:
	ByyFastBestAction(ByyApp& app,QWidget* parent=0);
	~ByyFastBestAction();

protected slots:
	virtual void on_triggered();
	virtual void checkEnabled();

};

class ByyScnProfileSettingsAction : public ByyAction
{
public:
	ByyScnProfileSettingsAction(ByyApp& app,QWidget* parent=0);
	~ByyScnProfileSettingsAction();

protected slots:
	virtual void on_triggered();
	virtual void checkEnabled();

};

class ByyScnEnvironmentSettingsAction : public ByyAction
{
public:
	ByyScnEnvironmentSettingsAction(ByyApp& app,QWidget* parent=0);
	~ByyScnEnvironmentSettingsAction();

protected slots:
	virtual void on_triggered();
	virtual void checkEnabled();

};

class ByySnapShotAction : public ByyAction
{
public:
	ByySnapShotAction(ByyApp& app,QWidget* parent=0);
	~ByySnapShotAction();

protected slots:
	virtual void on_triggered();
	virtual void checkEnabled();

};


//编队阵型编辑菜单按钮
class formationWidget;
class ByyFormationEditAction : public ByyAction
{
public:
	ByyFormationEditAction(ByyApp& app,QWidget* parent=0);
	~ByyFormationEditAction();

	protected slots:
		virtual void on_triggered();
		virtual void checkEnabled();
private:
	formationWidget* m_formationWidget;

};

//裁决-空战优势裁决 9/7
class ByyAirCombatSuperiorityAction :public ByyAction
{
public:
	ByyAirCombatSuperiorityAction(ByyApp& app,QWidget* parent=0);
	~ByyAirCombatSuperiorityAction();

protected slots:
	virtual void on_triggered();
	virtual void checkEnabled();
};
//目标预警裁决 9/7
class ByyTargetAlertAction :public ByyAction
{
public:
	ByyTargetAlertAction(ByyApp& app,QWidget* parent=0);
	~ByyTargetAlertAction();

protected slots:
	virtual void on_triggered();
	virtual void checkEnabled();
};
//碰撞检测 9/7
class ByyCollisionDetectionAction :public ByyAction
{
public:
	ByyCollisionDetectionAction(ByyApp& app,QWidget* parent=0);
	~ByyCollisionDetectionAction();

protected slots:
	virtual void on_triggered();
	virtual void checkEnabled();
};
//毁伤裁决 9/7
class ByyDamageAction :public ByyAction
{
public:
	ByyDamageAction(ByyApp& app,QWidget* parent=0);
	~ByyDamageAction();

protected slots:
	virtual void on_triggered();
	virtual void checkEnabled();
};


//实体预警判断  2024/4/22 
class ByyEntityAlertAction :public ByyAction
{
public:
	ByyEntityAlertAction(ByyApp& app,QWidget* parent=0);
	~ByyEntityAlertAction();

	protected slots:
		virtual void on_triggered();
		virtual void checkEnabled();
};

//演练方预警  2024/4/22 
class ByyExercisePartyAlertAction :public ByyAction
{
public:
	ByyExercisePartyAlertAction(ByyApp& app,QWidget* parent=0);
	~ByyExercisePartyAlertAction();

	protected slots:
		virtual void on_triggered();
		virtual void checkEnabled();
};
//飞机目标跟踪  2024/4/22 
class ByyAircraftTargetTrackingAction :public ByyAction
{
public:
	ByyAircraftTargetTrackingAction(ByyApp& app,QWidget* parent=0);
	~ByyAircraftTargetTrackingAction();

	protected slots:
		virtual void on_triggered();
		virtual void checkEnabled();
};

// ByyMissileTargetTrackingAction 导弹目标跟踪  2024/4/22 
class ByyMissileTargetTrackingAction :public ByyAction
{
public:
	ByyMissileTargetTrackingAction(ByyApp& app,QWidget* parent=0);
	~ByyMissileTargetTrackingAction();

	protected slots:
		virtual void on_triggered();
		virtual void checkEnabled();
};

// ByyAirCombatAdvantageCalculationAction 空战优势计算  2024/4/22 
class ByyAirCombatAdvantageCalculationAction :public ByyAction
{
public:
	ByyAirCombatAdvantageCalculationAction(ByyApp& app,QWidget* parent=0);
	~ByyAirCombatAdvantageCalculationAction();

	protected slots:
		virtual void on_triggered();
		virtual void checkEnabled();
};

// ByySensorAlarmCalculationAction 传感器告警计算  2024/4/22 
class ByySensorAlarmCalculationAction :public ByyAction
{
public:
	ByySensorAlarmCalculationAction(ByyApp& app,QWidget* parent=0);
	~ByySensorAlarmCalculationAction();

	protected slots:
		virtual void on_triggered();
		virtual void checkEnabled();
};

// ByyFireControlLockingCalculationAction 火控锁定计算  2024/4/22
class ByyFireControlLockingCalculationAction :public ByyAction
{
public:
	ByyFireControlLockingCalculationAction(ByyApp& app,QWidget* parent=0);
	~ByyFireControlLockingCalculationAction();

	protected slots:
		virtual void on_triggered();
		virtual void checkEnabled();
};

// ByyMissileHitProbabilityCalculationAction 导弹命中概率计算  2024/4/22 
class ByyMissileHitProbabilityCalculationAction :public ByyAction
{
public:
	ByyMissileHitProbabilityCalculationAction(ByyApp& app,QWidget* parent=0);
	~ByyMissileHitProbabilityCalculationAction();

	protected slots:
		virtual void on_triggered();
		virtual void checkEnabled();
};

// ByyTorpedoVsSubmarineDamageCalculationAction 鱼雷对潜艇毁伤计算  2024/4/22 
class ByyTorpedoVsSubmarineDamageCalculationAction :public ByyAction
{
public:
	ByyTorpedoVsSubmarineDamageCalculationAction(ByyApp& app,QWidget* parent=0);
	~ByyTorpedoVsSubmarineDamageCalculationAction();

	protected slots:
		virtual void on_triggered();
		virtual void checkEnabled();
};
// ByyTorpedoVsShipDamageCalculationAction 鱼雷对舰艇毁伤计算  2024/4/22 
class ByyTorpedoVsShipDamageCalculationAction :public ByyAction
{
public:
	ByyTorpedoVsShipDamageCalculationAction(ByyApp& app,QWidget* parent=0);
	~ByyTorpedoVsShipDamageCalculationAction();

	protected slots:
		virtual void on_triggered();
		virtual void checkEnabled();
};

// ByyDepthChargeVsSubmarineDamageCalculationAction 深水炸弹对潜艇毁伤计算  2024/4/22 
class ByyDepthChargeVsSubmarineDamageCalculationAction :public ByyAction
{
public:
	ByyDepthChargeVsSubmarineDamageCalculationAction(ByyApp& app,QWidget* parent=0);
	~ByyDepthChargeVsSubmarineDamageCalculationAction();

	protected slots:
		virtual void on_triggered();
		virtual void checkEnabled();
};

// ByyMissileVsShipDamageCalculationAction 导弹对舰艇毁伤计算  2024/4/22 
class ByyMissileVsShipDamageCalculationAction :public ByyAction
{
public:
	ByyMissileVsShipDamageCalculationAction(ByyApp& app,QWidget* parent=0);
	~ByyMissileVsShipDamageCalculationAction();

	protected slots:
		virtual void on_triggered();
		virtual void checkEnabled();
};
// ByyNavalEquipmentStatusJudgmentAction 舰艇装备状态裁决  2024/4/22 
class ByyNavalEquipmentStatusJudgmentAction :public ByyAction
{
public:
	ByyNavalEquipmentStatusJudgmentAction(ByyApp& app,QWidget* parent=0);
	~ByyNavalEquipmentStatusJudgmentAction();

	protected slots:
		virtual void on_triggered();
		virtual void checkEnabled();
};

// ByySubmarineEquipmentStatusJudgmentAction 潜艇装备状态裁决  2024/4/22 
class ByySubmarineEquipmentStatusJudgmentAction :public ByyAction
{
public:
	ByySubmarineEquipmentStatusJudgmentAction(ByyApp& app,QWidget* parent=0);
	~ByySubmarineEquipmentStatusJudgmentAction();

	protected slots:
		virtual void on_triggered();
		virtual void checkEnabled();
};
class ByyExitJudgmentAction :public ByyAction
{
public:
	ByyExitJudgmentAction(ByyApp& app,QWidget* parent=0);
	~ByyExitJudgmentAction();

	protected slots:
		virtual void on_triggered();
		virtual void checkEnabled();
};

// ByyWarDamageStatisticsAction 战损统计  2024/4/22 
class ByyWarDamageStatisticsAction :public ByyAction
{
public:
	ByyWarDamageStatisticsAction(ByyApp& app,QWidget* parent=0);
	~ByyWarDamageStatisticsAction();

	protected slots:
		virtual void on_triggered();
		virtual void checkEnabled();
};
#endif // SIMULATIONMENUITEMS_H
