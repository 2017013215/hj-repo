#pragma once
#include <QMap>
#include <QStringList>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChartView>
#include <QtCharts/QPolarChart>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include "VerdictBaseWidget.h"

class QPushButton;
class QGridLayout;
class QTableWidget;
class QLabel;
class QDoubleSpinBox;

//实体预警判断
class DepthChargeVsSubmarineDamageCalculation : public VerdictBaseWidget
{
	Q_OBJECT

public:
	DepthChargeVsSubmarineDamageCalculation(VerdictBaseWidget* parent = nullptr);

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
	void slotBtnDCvsSubmaineDC();
	//详情
	void slotBtnCheckoutDetail();

private:
	QGridLayout		*Layout_DepthChargeVsSubmarineDamageCalculation;
	QTableWidget	*Table_DepthChargeVsSubmarineDamageCalculation;

	QPushButton		*Button_DepthChargeVsSubmarineDamageCalculation;
	QDoubleSpinBox	*ComboBox_DepthChargeVsSubmarineDamageCalculation;
	QChartView		*chartView_DepthChargeVsSubmarineDamageCalculation;
	QLabel			*Label_DepthChargeVsSubmarineDamageCalculation_Distance;
	QLabel			*Label_DepthChargeVsSubmarineDamageCalculation_Position;
	QLabel			*Label_DepthChargeVsSubmarineDamageCalculation_Height;
	QLabel			*Label_DepthChargeVsSubmarineDamageCalculation_DamageLevel;
	QLabel			*Label_DepthChargeVsSubmarineDamageCalculation_Power;
	QLabel		    *Label_DepthChargeVsSubmarineDamageCalculation_TargetName;

	QPolarChart		*PolarChart_DepthChargeVsSubmarineDamageCalculation;
	QScatterSeries	*ScatterSeries_DCvsSubmaineDC;
	QValueAxis		*m_axisR_DCvsSubmaineDC, *m_axisA_DCvsSubmaineDC;//极坐标轴对象

	int				m_currentRow;
};