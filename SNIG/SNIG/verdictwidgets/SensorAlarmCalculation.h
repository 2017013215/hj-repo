#pragma once
#include <QtCharts/QValueAxis>
#include <QtCharts/QChartView>
#include <QtCharts/QPolarChart>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include "VerdictBaseWidget.h" 

struct SensorAlarmData
{
	QString entityName;
	double  distance;
	bool	isAlarmed;
	double  r;
	double  orientation;
};
typedef QList<SensorAlarmData> SensorAlarmDatas; 

//实体预警判断
class SensorAlarmCalculation : public VerdictBaseWidget
{
	Q_OBJECT

public:
	SensorAlarmCalculation(VerdictBaseWidget* parent = nullptr);
	void showEntityName(const QString& entityName);

protected:
	virtual void createButton();
	virtual void createChart();
	virtual void createComBox();
	virtual void createTableView();
	virtual void createLog();
	virtual void setupLayout();
	virtual void resetUi();
	virtual void preHandleDataFrame();
	virtual void handleDataFrame();
	void updataSensorAlarmCalculationData(const SensorAlarmDatas& infos);
	virtual void stopHandle();

protected Q_SLOTS:
	void slotBtnSensorAlarmCalculation();
	void slotSensorAlarmCalculationRed(int index);

private:
	QVBoxLayout		*Layout_SensorAlarmCalculation;

	QPushButton		*Button_SensorAlarmCalculation;
	QComboBox		*ComboBox_SensorAlarmCalculation_Red;

	QTableView			*Table_SensorAlarm;			//目标预警裁决
	QStandardItemModel	*Model_SensorAlarm;

	QMap<QString,QScatterSeries*> ScatterSeries_SensorAlarms;
	QPolarChart	*PolarChart_SensorAlarm;
	QValueAxis	*m_axisR_SensorAlarm, *m_axisA_SensorAlarm;	//极坐标轴对象
	QChartView	*chartView_SensorAlarm;
};