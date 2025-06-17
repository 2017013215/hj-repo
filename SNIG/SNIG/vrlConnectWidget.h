#ifndef VRLCONNECTWIDGET_H
#define VRLCONNECTWIDGET_H

#include <QWidget>
#include "ui_vrlConnectWidget.h"

class ByyCore;
class ByyDriver;

class ByyVRLConnectWidget : public QWidget
{
	Q_OBJECT

public:
	ByyVRLConnectWidget(ByyCore& core,QWidget *parent = 0);
	~ByyVRLConnectWidget();

protected slots:
	void onCurrentItemChanged(QTreeWidgetItem *current,QTreeWidgetItem *previous);

	void onConnect();
	void onDisconnect();

protected:
	void updateButton(ByyDriver* driver);
	void updateItem(ByyDriver* driver,QTreeWidgetItem* item);

private:
	Ui::ByyVRLConnectWidget ui;
	ByyCore& myCore;
};

#endif // VRLCONNECTWIDGET_H
