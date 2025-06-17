#ifndef ENTOBJINFOWIDGET_H
#define ENTOBJINFOWIDGET_H

#include <QTimer>

#include "ByyEntityObserver.h"
#include "ui_ByyEntityInfoWidget.h"

class ByyEntityObject;

class ByyEntityInfoWidget :public ByyEntityObserver
{
	Q_OBJECT
public:
	ByyEntityInfoWidget(ByyCore& core,ByyEntityObject* entity=0,QWidget* parent=0,Qt::WindowFlags f = 0);
	~ByyEntityInfoWidget();

	void setEntity(ByyEntityObject *entity);

public slots:
	void updateInfo();

private:
	Ui::ByyEntityInfoWidget ui;

	QTimer myTimer;
};

#endif // ENTOBJINFOWIDGET_H
