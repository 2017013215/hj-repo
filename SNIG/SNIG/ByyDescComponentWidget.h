#ifndef BYYDESCCOMPONENTWIDGET_H
#define BYYDESCCOMPONENTWIDGET_H

#include <QWidget>
#include "ui_ByyDescComponentWidget.h"

class ByyDescribeComponent;

class ByyDescComponentWidget : public QWidget
{
	Q_OBJECT

public:
	ByyDescComponentWidget(const ByyDescribeComponent& component,QWidget *parent = 0);
	~ByyDescComponentWidget();

private:
	Ui::ByyDescComponentWidget ui;
};

#endif // BYYDESCCOMPONENTWIDGET_H
