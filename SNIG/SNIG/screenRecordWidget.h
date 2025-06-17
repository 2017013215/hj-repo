#ifndef SCREENRECORDWIDGET_H
#define SCREENRECORDWIDGET_H

#include <QWidget>
#include "ui_screenRecordWidget.h"

class ByyIG;

class ByyScreenRecordWidget : public QWidget
{
	Q_OBJECT

public:
	ByyScreenRecordWidget(ByyIG& ig,QMainWindow* m,QWidget *parent = 0);
	~ByyScreenRecordWidget();

public slots:
	void start();
	void pauseOrCoutiue();
	void stop();

	void timeout();

private:
	Ui::ByyScreenRecordWidget ui;
	ByyIG& myIG;

	qint64 elapsedTime;
	bool _started;

	QTimer timer;

	QMainWindow *mainWindow;
};

#endif // SCREENRECORDWIDGET_H
