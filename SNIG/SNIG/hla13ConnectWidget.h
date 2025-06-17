#ifndef HLA13CONNECTWIDGET_H
#define HLA13CONNECTWIDGET_H

#include <QWidget>
#include "ui_hla13ConnectWidget.h"

class ByyConnectManager;
class ByyHLA13Connect;

class ByyHLA13ConnectWidget : public QWidget
{
	Q_OBJECT

public:
	ByyHLA13ConnectWidget(QWidget *parent = 0);
	~ByyHLA13ConnectWidget();

	void setManager(ByyConnectManager *cm);

protected:
	void showEvent(QShowEvent *event);
	void addConnect(ByyHLA13Connect* conn);

public slots:
	void updateWidget();

protected slots:
	void renameClicked();
	void addClicked();
	void removeClicked();
	void ConnectClicked();

	void infoChanged();

private:
	Ui::ByyHLA13ConnectWidget ui;
	ByyConnectManager* manager;
};

#endif // HLA13CONNECTWIDGET_H
