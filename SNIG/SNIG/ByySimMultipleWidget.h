#ifndef BYYSIMMULTIPLEWIDGET_H
#define BYYSIMMULTIPLEWIDGET_H

#include <QWidget>
#include "ui_ByySimMultipleWidget.h"

class ByyHdose;

class ByySimMultipleWidget : public QWidget
{
	Q_OBJECT

public:
	ByySimMultipleWidget(ByyHdose& hdose,QWidget *parent = 0);
	~ByySimMultipleWidget();

protected slots:
	void onValueChanged(double val);
	void checkEnabled();

	void resetUiValue();

private:
	Ui::ByySimMultipleWidget ui;

	ByyHdose& myHdose;
};

#endif // BYYSIMMULTIPLEWIDGET_H
