#ifndef DATAPLOTWIDGET_H
#define DATAPLOTWIDGET_H

#include "ByyEntityObserver.h"
#include "plot/signaldata.h"

class ByyPlot;
class ByyApp;
class QwtPlotCurve;

class ByyEntityPlotWidget1 : public ByyEntityObserver
{
	Q_OBJECT

public:
	ByyEntityPlotWidget1(ByyApp& app,ByyEntityObject* entity=0,QWidget* parent=0,Qt::WindowFlags f = 0);
	~ByyEntityPlotWidget1();

protected slots:
	void tick();
	void exportDocument();
	void onCurrentCurveChanged(int index);

protected:
	 void addCurve(const QString& name,SignalData& sd);

private:
	ByyApp& myApp;

	QTabBar *myTabBar;

	ByyPlot *myPlot;

	SignalData myAlt;
	SignalData mySpeed;

	QElapsedTimer myElapsedTimer;

	double myMaxXAxis;
	double myMaxYAxis;

	QList<QwtPlotCurve*> myCurveList;
};

#endif // DATAPLOTWIDGET_H
