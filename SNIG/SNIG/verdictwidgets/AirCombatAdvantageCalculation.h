#pragma once
#include <QtCharts/QValueAxis>
#include <QtCharts/QChartView>
#include <QtCharts/QPolarChart>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include "VerdictBaseWidget.h" 

class QGridLayout;
class QComboBox;
class QPushButton;
class QChartView;
class QTimer;
class ByyEntityObject;

struct AirCombatAdvantageData {
	AirCombatAdvantageData()
	{
		redAdvantage = 0.0;
		blueAdvantage = 0.0;
	}
	double redAdvantage;
	double blueAdvantage;
};

struct AirCombatAdvantageChart {
	QChartView		*graphicsView_AirCombatAdvantageCalculation;
	QSplineSeries	*B_lineSeries_ACAC;//曲线图对象-蓝
	QSplineSeries	*R_lineSeries_ACAC;//曲线图对象-红
	QChart			*chart_ACAC;//图表对象
	QValueAxis		*axisX_ACAC, *axisY_ACAC;// 横纵坐标轴对象 

	int				pointCount;
	double			localYMin;
	double		    localYMax;
	void reset();
	void updataAirCombatAdvantageData(const AirCombatAdvantageData& data);
};

//实体预警判断
class AirCombatAdvantageCalculation : public VerdictBaseWidget
{
	Q_OBJECT

public:
	AirCombatAdvantageCalculation(VerdictBaseWidget* parent = nullptr);

protected:
	virtual void createButton();
	virtual void createChart();
	virtual void createComBox();
	virtual void createTableView();
	virtual void setupLayout();
	virtual void resetUi();
	virtual void preHandleDataFrame();
	virtual void handleDataFrame();
	void updataAirCombatAdvantageData(const QString& AirCombatAdvantageType, const AirCombatAdvantageData& data);
	virtual void stopHandle();

protected Q_SLOTS:
	void slotBtnAirCombatAdvantageCalculation();
	void slotAirCombatAdvantageCalculationRed(int index);
	void slotAirCombatAdvantageCalculationBlue(int index);
	void slotAirCombatAdvantageCalculationAdvantage(int index);
	double switchAirCombatAdvantage(ByyEntityObject* selfEntObject,ByyEntityObject* tgtEntObject);

private:
	QGridLayout *Layout_AirCombatAdvantageCalculation;  
	QComboBox	*ComboBox_AirCombatAdvantageCalculation_Red;
	QComboBox	*ComboBox_AirCombatAdvantageCalculation_Blue;
	QComboBox	*ComboBox_AirCombatAdvantageCalculation_Advantage;
	QPushButton	*Button_AirCombatAdvantageCalculation;

	QMap<QString, AirCombatAdvantageChart> m_AirCombatAdvantages;
	//QChartView	*graphicsView_AirCombatAdvantageCalculation;

	////QTimer			*tr_AirCombatAdvantageCalculation;
	////ByyEntityObject	*redCurrentEntity_AirCombatAdvantageCalculation;
	////ByyEntityObject	*blueCurrentEntity_AirCombatAdvantageCalculation;
	//QSplineSeries	*B_lineSeries_ACAC;//曲线图对象-蓝
	//QSplineSeries	*R_lineSeries_ACAC;//曲线图对象-红
	//QChart			*m_chart_ACAC;//图表对象
	//QValueAxis		*m_axisX_ACAC, *m_axisY_ACAC;// 横纵坐标轴对象 
	int				m_currentAdvantage;
	int				m_pointCount;
	double			m_yMin;
	double		    m_yMax;
};