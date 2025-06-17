#pragma once
#include <QtCharts/QValueAxis>
#include <QtCharts/QChartView>
#include <QtCharts/QPolarChart>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include "VerdictBaseWidget.h" 

class QStandardItemModel;
class QTableWidget;

//实体预警判断
class MissileVsShipDamageCalculation : public VerdictBaseWidget
{
	Q_OBJECT

public:
	MissileVsShipDamageCalculation(VerdictBaseWidget* parent = nullptr);

protected:
	virtual void createButton();
	virtual void createChart();
	virtual void createComBox();
	virtual void createTableView();
	virtual void createLabel();
	virtual void setupLayout();
	virtual void resetUi();
	virtual void preHandleDataFrame();
	virtual void handleDataFrame();
	void updataDamageCalculationData(const EventInfos& infos);
	virtual void stopHandle();

protected Q_SLOTS:
	//修改毁伤等级
	void slotBtnMvsShipDC();
	//详情
	void slotBtnCheckoutDetail();

private:
	QGridLayout* Layout_MissileVsShipDamageCalculation;
	QTableWidget*	Table_MissileVsShipDamageCalculation;

	QStandardItemModel *Model_MissileVsShipDamageCalculation;
	QPushButton *Button_MissileVsShipDamageCalculation;
	QDoubleSpinBox *ComboBox_MissileVsShipDamageCalculation;
	QChartView *chartView_MissileVsShipDamageCalculation;
	QLabel* Label_MissileVsShipDamageCalculation_Distance;
	QLabel* Label_MissileVsShipDamageCalculation_Position;
	QLabel* Label_MissileVsShipDamageCalculation_Height;
	QLabel* Label_MissileVsShipDamageCalculation_DamageLevel;
	QLabel* Label_MissileVsShipDamageCalculation_Power;
	QLabel* Label_MissileVsShipDamageCalculation_TargetName;

	QPolarChart* PolarChart_MissileVsShipDamageCalculation;
	QScatterSeries* ScatterSeries_MvsShipDC;
	QValueAxis *m_axisR_MvsShipDC, *m_axisA_MvsShipDC;//极坐标轴对象

	int				m_currentRow;
};