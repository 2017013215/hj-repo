#pragma once

#include <QtCharts/QValueAxis>
#include <QtCharts/QChartView>
#include <QtCharts/QPolarChart>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include "VerdictBaseWidget.h" 

//实体预警判断
class TorpedoVsShipDamageCalculation : public VerdictBaseWidget
{
	Q_OBJECT

public:
	TorpedoVsShipDamageCalculation(VerdictBaseWidget* parent = nullptr);

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
	void slotBtnTvsShipDC();
	//详情
	void slotBtnCheckoutDetail();

private:
	QGridLayout		*Layout_TorpedoVsShipDamageCalculation;
	QTableWidget	*Table_TorpedoVsShipDamageCalculation;

	QPushButton *Button_TorpedoVsShipDamageCalculation;
	QDoubleSpinBox	*ComboBox_TorpedoVsShipDamageCalculation;
	QChartView	*chartView_TorpedoVsShipDamageCalculation;
	QLabel		*Label_TorpedoVsShipDamageCalculation_Distance;
	QLabel		*Label_TorpedoVsShipDamageCalculation_Position;
	QLabel		*Label_TorpedoVsShipDamageCalculation_Height;
	QLabel		*Label_TorpedoVsShipDamageCalculation_DamageLevel;
	QLabel		*Label_TorpedoVsShipDamageCalculation_Power;
	QLabel		*Label_TorpedoVsShipDamageCalculation_TargetName;

	QPolarChart		*PolarChart_TorpedoVsShipDamageCalculation;
	QScatterSeries	*ScatterSeries_TvsShipDC;
	QValueAxis		*m_axisR_TvsShipDC, *m_axisA_TvsShipDC;//极坐标轴对象

	int				m_currentRow;
};