#pragma once
#include <QWidget>
#include <QMap>

#include <QtCharts/QValueAxis>
#include <QtCharts/QChartView>
#include <QtCharts/QPolarChart>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>

#include "ByySensorSR.h"
#include "remoteObjectManager.h"
#include "entityObject.h"
#include "VerdictBaseWidget.h" 

class QGridLayout;
class QComboBox;
class QPushButton;
class QTableView;
class QLabel;
class ByySensorSR;
class QChartView;
class QValueAxis;
class QPolarChart;
class QScatterSeries;
class QTimer;

//通用告警数据
struct EntityAlertInfo {
	QString entityName;
	double  resolution;
	bool	isAlerted;
	QPointF	sitePoint;
};
typedef QList<EntityAlertInfo> EntityAlertInfos;

inline bool operator==(const EntityAlertInfo& info1, const EntityAlertInfo& info2)
{
	return info1.entityName == info2.entityName;
}
inline bool infoLessThan(const EntityAlertInfo& info1, const EntityAlertInfo& info2)
{
	return info1.entityName <= info2.entityName;
}

//实体预警判断
class EntityAlert : public VerdictBaseWidget
{
	Q_OBJECT

public:
	EntityAlert(VerdictBaseWidget* parent = nullptr);
	//void getEntityAlert(QList<ByyEntityObject*>* entObjList);
	void showEntityName(const QString& entityName);
	
protected Q_SLOTS:
	void slotBtnEntityAlert();
	void slotSides(int index);
	void slotEntity(int index);
	void slotSensor(int index);

protected:
	virtual void createButton();
	virtual void createChart();
	virtual void createComBox();
	virtual void createTableView();
	virtual void setupLayout();
	virtual void resetUi();
	void handleDataFrame();
	void updateEntityAlertInfos(const EntityAlertInfos& infos);
	virtual void stopHandle();

private:
	//QTimer *tr_EntityAlert;
	QGridLayout *Layout_EntityAlert;		//实体预警

	QStringList redList,blueList;
	QComboBox *ComboBox_EntityAlert_Sides;	//实体预警-红蓝双方
	QComboBox *ComboBox_EntityAlert_Entity;	//实体预警-实体
	QComboBox *ComboBox_EntityAlert_Sensor;	//实体预警-传感器

	QPushButton *Button_EntityAlert;

	QTableView  *Table_EntityAlert;			//目标预警裁决
	QStandardItemModel *Model_EntityAlert;

	QMap<QString,QScatterSeries*> ScatterSeries_EntityAlerts;
	QPolarChart* PolarChart_EntityAlert;
	QValueAxis *m_axisR_EntityAlert, *m_axisA_EntityAlert;	//极坐标轴对象
	QChartView *chartView_EntityAlert;

	//内部的对象管理
	long selectEntID;
	ByyEntityObject* myCurrentEntity;
	ByyDescribeEntry::ComponentList v_componentList;
	QList<ByyEntityObject*>* v_entObjList;
	

	//QLabel *Label_Empty_1;
	//QLabel *Label_Empty_2;
	//QComboBox *ComboBox_TA_Sensor;			//目标预警裁决-传感器
	QLabel *Label_TurnOff_State;	//开关机状态
	QLabel *Label_TurnOff;			//开机/关机
	QLabel *Label_Coverage;			//预警覆盖率
	QLabel *Label_Percent;			//百分比
};