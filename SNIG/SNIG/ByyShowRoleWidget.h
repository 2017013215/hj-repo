#ifndef BYYSHOWROLEWIDGET_H
#define BYYSHOWROLEWIDGET_H

#include <QWidget>
#include "ui_ByyShowRoleWidget.h"

class ByyApp;

class ByyShowRoleWidget : public QWidget
{
	Q_OBJECT

public:
	ByyShowRoleWidget(ByyApp& app,QWidget *parent = 0);
	~ByyShowRoleWidget();

protected slots:
	void onRoleChanged(int index);

private:
	Ui::ByyShowRoleWidget ui;

	ByyApp& myApp;
};

#endif // BYYSHOWROLEWIDGET_H
