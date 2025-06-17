#ifndef SCNPROFILEWIDGET_H
#define SCNPROFILEWIDGET_H

#include <QWidget>
#include "ui_scnProfileWidget.h"

class ByyApp;

class ByyScnProfileWidget : public QWidget
{
	Q_OBJECT

public:
	ByyScnProfileWidget(ByyApp& app,QWidget *parent = 0);
	~ByyScnProfileWidget();

protected slots:
	void onOkClicked();
	void onValueChanged();

private:
	Ui::ByyScnProfileWidget ui;
	ByyApp& myApp;
};

#endif // SCNPROFILEWIDGET_H
