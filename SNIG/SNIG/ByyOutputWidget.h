#ifndef BYYOUTPUTWIDGET_H
#define BYYOUTPUTWIDGET_H

#include <QWidget>
#include "ui_ByyOutputWidget.h"

class ByyOutputWidget : public QWidget
{
	Q_OBJECT

public:
	ByyOutputWidget(ByyHdose& hdose,QWidget *parent = 0);
	~ByyOutputWidget();

	QSize sizeHint()const;

protected slots:
	void appendHdoseLog(QString log);
	void appendSimEventLog(double simTime,QString log);
	void appendUserLog(QTime time,QString log);
	void appendCollisionDetectionLog(QString log);
	void cleatSimEventLog();

protected:
	void showEvent(QShowEvent *e);
	void hideEvent(QHideEvent *e);

private:
	Ui::ByyOutputWidget ui;
	ByyHdose& myHdose;
};

#endif // BYYOUTPUTWIDGET_H
