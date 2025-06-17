#pragma once
#include <QtCharts/QValueAxis>
#include <QtCharts/QChartView>
#include <QtCharts/QPolarChart>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include "VerdictBaseWidget.h" 

//实体预警判断
class TorpedoVsSubmarineDamageCalculation : public VerdictBaseWidget
{
	Q_OBJECT

public:
	TorpedoVsSubmarineDamageCalculation(VerdictBaseWidget* parent = nullptr);

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
	void slotBtnTvsSubmarineDC();
	//详情
	void slotBtnCheckoutDetail();

private:
	QGridLayout		*Layout_TorpedoVsSubmarineDamageCalculation;
	QTableWidget	*Table_TorpedoVsSubmarineDamageCalculation;

	QStandardItemModel	*Model_TorpedoVsSubmarineDamageCalculation;
	QPushButton			*Button_TorpedoVsSubmarineDamageCalculation;
	QDoubleSpinBox		*ComboBox_TorpedoVsSubmarineDamageCalculation;
	QChartView			*chartView_TorpedoVsSubmarineDamageCalculation;
	QLabel				*Label_TorpedoVsSubmarineDamageCalculation_Distance;
	QLabel				*Label_TorpedoVsSubmarineDamageCalculation_Position;
	QLabel				*Label_TorpedoVsSubmarineDamageCalculation_Height;
	QLabel				*Label_TorpedoVsSubmarineDamageCalculation_DamageLevel;
	QLabel				*Label_TorpedoVsSubmarineDamageCalculation_Power;
	QLabel				*Label_TorpedoVsSubmarineDamageCalculation_TargetName;

	QValueAxis			*m_axisR_TvsSubmaineDC, *m_axisA_TvsSubmaineDC;//极坐标轴对象
	QPolarChart			*PolarChart_TorpedoVsSubmarineDamageCalculation;
	QScatterSeries		*ScatterSeries_TvsSubmarineDC;

	int					m_currentRow;
};