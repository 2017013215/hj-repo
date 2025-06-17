#ifndef CONTROLOBJECTWIDGET_H
#define CONTROLOBJECTWIDGET_H

#include <QWidget>
#include "ui_controlObjectWidget.h"

class ByyApp;
class ByyControlObject;

class ByyControlObjectWidget : public QWidget
{
	Q_OBJECT

public:
	ByyControlObjectWidget(ByyApp& app,QWidget *parent = 0);
	~ByyControlObjectWidget();

public slots:
	void appendContrlObject(ByyControlObject* obj);
	void removeContrlObject(ByyControlObject* obj);

protected slots:
	void itemDoubleClicked(QTreeWidgetItem *item,int column);
	void onNameChanged(const QString& name);

	void onVisibleChanged(bool b);
	void onForceTypeChanged(ByyForceType type);

	void onItemClicked(QTreeWidgetItem *current, int column );

protected:
	bool eventFilter(QObject *obj, QEvent *ev);

private:
	Ui::ByyControlObjectWidget ui;
	ByyApp& myApp;

	typedef QMap<ByyControlObject*,QTreeWidgetItem*> ControlObjectMap;

	ControlObjectMap myControlObjects;
};

#endif // CONTROLOBJECTWIDGET_H
