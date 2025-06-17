#ifndef BYYDESCRIBEWIDGET_H
#define BYYDESCRIBEWIDGET_H

#include "ByyEntityObserver.h"
#include "ui_ByyDescribeWidget.h"

class ByyEntityObject;

class ByyDescribeWidget :public ByyEntityObserver
{
	Q_OBJECT

public:
	ByyDescribeWidget(ByyCore& core,ByyEntityObject* entity=0,QWidget* parent=0,Qt::WindowFlags f = 0);
	~ByyDescribeWidget();

	void setEntity(ByyEntityObject *entity);

private:
	Ui::ByyDescribeWidget ui;
};

#endif // BYYDESCRIBEWIDGET_H
