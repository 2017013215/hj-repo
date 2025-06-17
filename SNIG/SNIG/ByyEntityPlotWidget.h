#ifndef BYYENTITYPLOTWIDGET_H
#define BYYENTITYPLOTWIDGET_H

#include "ByyEntityObserver.h"
#include "ui_ByyEntityPlotWidget.h"

class ByyApp;
class ByyHdoseEntity;
class ByyPlot;
class SignalData;

class ByyEntityPlotWidget : public ByyEntityObserver
{
	Q_OBJECT

public:
	ByyEntityPlotWidget(ByyApp& app,ByyEntityObject* entity=0,QWidget* parent=0,Qt::WindowFlags f = 0);
	~ByyEntityPlotWidget();

	struct PlotData_st
	{
		ByyPlot* plot;
		SignalData*	signalData;

		struct Axis_st
		{
			QString attribute;
			int type;
			int index;

			bool isTime;
		};

		Axis_st x_Axis,y_Axis;
	};

	void setEntity(ByyEntityObject *entity);

protected slots:
	void showAttributeCurve();
	void onTabCloseRequested(int index);
	void tick();

protected:
	void initAttribute();
	

private:
	Ui::ByyEntityPlotWidget ui;

	ByyApp& myApp;

	

	QList<PlotData_st> myPlotDataList;

	QElapsedTimer myIntervalTimer;

	ByyHdoseEntity* myHdoseEntity;
};

#endif // BYYENTITYPLOTWIDGET_H
