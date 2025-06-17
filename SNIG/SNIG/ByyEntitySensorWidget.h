#ifndef BYYENTITYSENSORWIDGET_H
#define BYYENTITYSENSORWIDGET_H

#include "ByyEntityObserver.h"
#include "ui_ByyEntitySensorWidget.h"

class ByyEntityObject;
class ByyApp;

class ByyEntitySensorWidget :public ByyEntityObserver
{
	Q_OBJECT

public:
	ByyEntitySensorWidget(ByyApp& app,ByyEntityObject* entity=0,QWidget* parent=0,Qt::WindowFlags f = 0);
	~ByyEntitySensorWidget();

	void setEntity(ByyEntityObject *entity);

private:
	Ui::ByyEntitySensorWidget ui;
	ByyApp&	myApp;
};

#endif // BYYENTITYSENSORWIDGET_H
