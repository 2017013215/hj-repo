#ifndef TACTICALGRAPHICSWIDGET_H
#define TACTICALGRAPHICSWIDGET_H

#include <QWidget>
#include "ui_tacticalGraphicsWidget.h"

class ByyApp;

class ByyTacticalGraphicsWidget : public QWidget
{
	Q_OBJECT

public:
	ByyTacticalGraphicsWidget(ByyApp& app,QWidget *parent = 0);
	~ByyTacticalGraphicsWidget();

public slots:
	void checkEnabled();
	void onGraphicsCreated(ByyControlObject* obj);

protected slots:
	void onItemClicked(QTreeWidgetItem *item,int column);
	void filter();

private:
	Ui::ByyTacticalGraphicsWidget ui;
	ByyApp& myApp;

	ByyForceType myCreatingType;
};

#endif // TACTICALGRAPHICSWIDGET_H
