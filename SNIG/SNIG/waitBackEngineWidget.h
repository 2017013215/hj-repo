#ifndef WAITBACKENGINEWIDGET_H
#define WAITBACKENGINEWIDGET_H

#include <QWidget>
#include "ui_waitBackEngineWidget.h"

class ByyHdose;

class ByyWaitBackEngineWidget : public QWidget
{
	Q_OBJECT

public:
	ByyWaitBackEngineWidget(ByyHdose& hdose,QWidget *parent = 0);
	~ByyWaitBackEngineWidget();

protected slots:
	void onBackStateChanged(int state);

private:
	Ui::ByyWaitBackEngineWidget ui;
	ByyHdose& myHdose;

};

#endif // WAITBACKENGINEWIDGET_H
