#ifndef VERDICTBASEWIDGET_H
#define VERDICTBASEWIDGET_H

/*** 决策提供的界面的统计 名称 类名 开发状态
 * 1.		实体预警判断		EntityAlert 
 * 2.		演练方预警判断		ExercisePartyAlert 
 * 3.		飞机目标跟踪		AircraftTargetTracking
 * 4.		导弹目标跟踪		MissileTargetTracking
 * 5.		空战优势计算		AirCombatAdvantageCalculation
 * 6.		火控锁定计算		FireControlLockingCalculation
 * 7.		传感器告警计算		SensorAlarmCalculation
 * 8.		导弹命中概率计算	MissileHitProbabilityCalculation
 * 9.		碰撞检测计算		CollisionDetection
 * 10.		鱼雷对潜艇毁伤计算			TorpedoVsSubmarineDamageCalculation
 * 11.		鱼雷对舰艇毁伤计算			TorpedoVsShipDamageCalculation
 * 12.		深水炸弹对潜艇毁伤计算		DepthChargeVsSubmarineDamageCalculation
 * 13.		导弹对舰艇毁伤计算			MissileVsShipDamageCalculation
 * 14.		舰艇装备状态裁决			NavalEquipmentStatusJudgment
 * 15.		潜艇装备状态裁决			SubmarineEquipmentStatusJudgment
 * 16.		退出裁决					ExitJudgment
 * 17.		战损统计					WarDamageStatistics
 * 18.
 */
#include <QWidget>
#include <QList>
#include <QStringList>
#include <QMap>

class QTimer;
class ByySensorSR;
class ByyEntityObject;
class ByyRemoteEntity;
class ByyApp;
namespace JGeoCal {
	struct FoVDS;
	class  TopocentricPolar;
};

QStringList getJudgmentList();
QStringList getDamageJudgmentList();
QStringList getExitJudgmentList();
QStringList getStateJudgmentList();

double genRandom();

//查看情况
#define START_QUERY QString::fromLocal8Bit("查看")
#define STOP_QUERY	QString::fromLocal8Bit("停止")

struct EventDetail {
	double distance;	//距离
	double orientation;	//方位
	double r;			//与站心的直线距离,km
	double height;		//高低
	double damageNumber;//毁伤点数 //TODO: 获取损伤点数需要查表
	double currentHealth;//当前生命值
	ByyDamageState damageState;//当前损伤等级
};
Q_DECLARE_METATYPE(EventDetail)

struct EventInfo
{
	QString happenTime;			//发生时间
	QString explodeEntityName;	//爆炸实体
	QString hitTarget;			//命中目标
	EventDetail detailInfo;		//详情
};
typedef QList<EventInfo> EventInfos;
int indexOfEquipmentName(QTableWidget* tableWidget, const QString& equipmentName, int col);
int indexOfEquipmentNames(QTableWidget* tableWidget, const QString& equipmentName1, const QString& equipmentName2, int col1, int col2);
int indexOfEquipmentNamesModel(QStandardItemModel* model, const QString& equipmentName1, const QString& equipmentName2, int col1, int col2);
int indexOfCellWidget(QTableWidget* tableWidget, QWidget* cellwidget, int col);
//目标是否在辐照范围方范围内
bool entityAlertBusiness(ByyEntityObject* self, ByyEntityObject* tgt, JGeoCal::FoVDS fov, JGeoCal::TopocentricPolar&tcp);

//损伤统计
struct EntityTypeExpendInfo{
	QString entityTypeName;
	int		entityNumber;
};
typedef QList<EntityTypeExpendInfo> EntityTypeExpendInfos;

//提供统一的时间处理方式，以及事件记录本
class VerdictBaseWidget : public QWidget
{
	Q_OBJECT

public:
	VerdictBaseWidget(QWidget *parent = nullptr);
	~VerdictBaseWidget();
	virtual void resetUi();
	virtual void stopHandle();


	//获取实体和数据统计
	int getShipCount(QList<ByyEntityObject*>* entObjList);
	int getSubmarineCount(QList<ByyEntityObject*>* entObjList);
	int getEntityCount(QList<ByyEntityObject*>* entObjList ,int kind,int domain );
	void getAirEntity(QList<ByyEntityObject*>* entObjList);
	void getTargetEntity(QList<ByyEntityObject*>* entObjList);
	void getCollisionEntity(QList<ByyEntityObject*>* entObjList);
	void getDamageEntity(QList<ByyEntityObject*>* entObjList);

	void startDataHandle(int updateTime); //开始更新数据 毫秒
	void stopDataHandle(); //开始更新数据 毫秒
	bool isDataUpdata();

protected Q_SLOTS:
	virtual void onUpdateRealTimeData(); //运行实时数据

protected:
	//界面数据处理
	virtual void createButton();
	virtual void createChart();
	virtual void createComBox();
	virtual void createTableView();
	virtual void createLabel();
	virtual void createLog();
	virtual void setupLayout();
	virtual void createWidget();

	void setSingleFlage(bool isSingle);
	bool getSingleFlage(void);

	//通用数据判断
	ByyEntityObject* getEntityByName(const QString& entityName);
	QString getTypeName(ByyEntityObject* entity);

	//界面单帧数据处理
	virtual void preHandleDataFrame(void);
	virtual void handleDataFrame(void);

	//对象查询需求，需要进行方式的统一
	QStringList findObjects(const QString& srcname, EntitySimpleObjectType type);
	
	//获取实体对象
	void updateEntities(void);
	QStringList getEntitiesByParty(const QString& partyName);
	QStringList getSensorsByEntity(const QString& partyName, const QString& entityName);
	QStringList getLocalEntitiesContainSensor(const QString& partyName);
	QStringList getLocalEntitiesContainSensor(const QString& partyName, EntitySimpleObjectType objectType);
	QStringList getEntitiesByType(const QString& partyName, EntitySimpleObjectType objectType);
	QStringList getEntitiesByTypes(const QString& partyName, int objectTypes);
	int getPartyNameIndex(const QString& partyName);
	ByySensorSR* getSensorByName(const QString& partyName, const QString& entityName);
	ByySensorSR* getSensorByName(const QString& entityName);

	//TODO: 下发指令
	void sendStatusChangeCommand(const QString& entityname, const QString& equipmentname, int status);

	//获取爆炸相关事件
	EventInfos getExplodeEvenfs(EntitySimpleObjectType attckType, EntitySimpleObjectType targetType);
	void sendStatusChangeCommand(const QString& entityname, int status);

protected:
	ByyApp* m_byyApp;
	QMap<QString, ByyEntityObject*> m_redRemoteEntities; 
	QMap<QString, ByyEntityObject*> m_blueRemoteEntities; 
	QTimer* tr_dataHandleTimer;

	QStringList redList,blueList;	//实体对象列表
	QStringList redList_AircraftTargetTracking, blueList_AircraftTargetTracking;	
	QStringList redList_MissileTargetTracking, blueList_MissileTargetTracking;
	QStringList blueList_ShipEntity,redList_ShipEntity;
	QStringList blueList_SubmarineEntity,redList_SubmarineEntity;

	ByyDescribeEntry::ComponentList v_componentList;
	QList<ByyEntityObject*>* v_entObjList;
	ByyEntityObject* myCurrentEntity;
	ByyEntityObject* redCurrentEntity;
	ByyEntityObject* blueCurrentEntity;
	ByySensorSR* myCurrentSensorSR;

	long redID;
	long blueID;
	long selectEntID;

	// 单一显示
	bool m_isSingle;
};

#endif // VERDICTBASEWIDGET_H
