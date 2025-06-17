#ifndef SIMTIMEWIDGET_H
#define SIMTIMEWIDGET_H

#include <QWidget>
#include <QTimer>
#include "ui_simTimeWidget.h"

class ByyApp;

class ByySimTimeWidget : public QWidget
{
	Q_OBJECT

public:
	ByySimTimeWidget(ByyApp &app,QWidget *parent = 0);
	~ByySimTimeWidget();

protected slots:
	void update();
	void onSimStatusChanged();

private:
	Ui::ByySimTimeWidget ui;
	ByyApp &myApp;
	QTimer myTimer;

	double myLastSimTime;
	double myLastRealTime;
};

#endif // SIMTIMEWIDGET_H
