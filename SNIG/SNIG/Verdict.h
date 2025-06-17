#ifndef VERDICT_H
#define  VERDICT_H
#include <QMutex>
#include "VerdictBaseWidget.h"
#include "VerdictBaseData.h"

class ByyApp;

enum DialogType
{
	EntityAlertDialog,
	ExercisePartyAlertDialog,
	AircraftTargetTrackingDialog,
	MissileTargetTrackingDialog,
	AirCombatAdvantageCalculationDialog,
	FireControlLockingCalculationDialog,
	SensorAlarmCalculationDialog,
	MissileHitProbabilityCalculationDialog,
	CollisionDetectionDialog,
	TorpedoVsSubmarineDamageCalculationDialog,
	TorpedoVsShipDamageCalculationDialog,
	DepthChargeVsSubmarineDamageCalculationDialog,
	MissileVsShipDamageCalculationDialog,
	NavalEquipmentStatusJudgmentDialog,
	SubmarineEquipmentStatusJudgmentDialog,
	ExitJudgmentDialog,
	WarDamageStatisticsDialog,
};

class QAction;
class QWidget;

struct EntityStatusUnit {
	QString typeName;
	int		status;
};
class Verdict : public QObject
{
	Q_OBJECT

public:
	static Verdict* getInstance();
	~Verdict();
	void setApp(ByyApp* app);
	void showDataDialog(DialogType type, const QVariantMap& map = QVariantMap());
	QAction* registerDockWidget(DialogType type, QWidget* widget, const QVariantMap& map = QVariantMap());
	VerdictBaseData& getVerdictBaseData() { return m_verdictBaseData; }

	//数据统计
	EventInfos getExplodeEvenfs(EntitySimpleObjectType attckType, EntitySimpleObjectType targetType);
	EntityTypeExpendInfos getAmmunitionExpendInfos(const QString& side);
	EntityTypeExpendInfos getEntityExpendInfos(const QString& side, int status);

protected Q_SLOTS:
	//事件统计
	void slot_started();
	void slot_resetEntity();
	void slot_stopHandle();
	//void slot_missileTJ(int count);
	void slot_OnHit(const QVariantMap& hitEventInfo);
	void slot_missileLaunch(int side, const QString& objectName, const QString& typeName);
	void slot_entityDm(int side, const QString& objectName, const QString& typeName);
	void slot_collisionDetectionCalculation(); //全局进行判断

private:
	explicit Verdict(QObject *parent = nullptr);
	static Verdict* m_pVerdict;
	ByyApp* myApp;
	QAction* m_collisionDetectionAction;

	QMap<int, VerdictBaseWidget*> m_verdictWidgets;
	QMutex	m_mutexStatics; //资源保护
	//损伤统计 onExplosion
	EventInfos m_DcVsSub;
	EventInfos m_MisVsShip;
	EventInfos m_TorVsShip;
	EventInfos m_TorVsSub;

	//兵力损耗统计 ByyForeStationProcessor
	QMap<QString, EntityStatusUnit>	 m_redAmmunitionExpendInfos;	//弹药损耗  onWeaponLaunchEvent
	QMap<QString, EntityStatusUnit>  m_redEntityExitInfos;			//兵力损耗统计
	QMap<QString, EntityStatusUnit>	 m_blueAmmunitionExpendInfos;	//弹药损耗  onExplosion ByyHdose::removeEntity
	QMap<QString, EntityStatusUnit>  m_blueEntityExitInfos;			//兵力损耗统计

	//决策相关的资源
	VerdictBaseData  m_verdictBaseData;
};

#endif 