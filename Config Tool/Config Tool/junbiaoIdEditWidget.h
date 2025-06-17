#ifndef JUNBIAOIDEDITWIDGET_H
#define JUNBIAOIDEDITWIDGET_H

#include <QWidget>
#include "ui_junbiaoIdEditWidget.h"

#include "mappingQuery.h"

class ByyJunbiaoIdEditWidget : public QWidget
{
	Q_OBJECT

public:
	ByyJunbiaoIdEditWidget(QWidget *parent = 0);
	~ByyJunbiaoIdEditWidget();

	void setLibId(int id);
	void setMarkId(int id);

	void setId(int lib,int mark);

	int libId();
	int markId();

	QString string();

private:
	Ui::ByyJunbiaoIdEditWidget ui;
};

#endif // JUNBIAOIDEDITWIDGET_H
