#ifndef DISPLAYMANAGEWIDGET_H
#define DISPLAYMANAGEWIDGET_H

#include <QWidget>
#include "ui_displayManageWidget.h"

class ByyApp;

class ByyDisplayManageWidget : public QWidget
{
	Q_OBJECT

public:
	ByyDisplayManageWidget(ByyApp& app,QWidget *parent = 0);

	~ByyDisplayManageWidget();

protected:
	void init();
	void initAction();

	bool eventFilter(QObject *obj, QEvent *ev);

	QTreeWidgetItem* createWindowItem(ByyOsgWindow* window);
	QTreeWidgetItem* createChannelItem(ByyChannel* channel);
	QTreeWidgetItem* findWindowItem(ByyOsgWindow* window);
	QTreeWidgetItem* findChannelItem(ByyChannel* channel);

protected slots:
	void addWindow();
	void removeWindow();
	void addChannel();
	void removeChannel();

	void onWindowCreated(ByyOsgWindow* window);
	void onWindowToBeDestroyed(ByyOsgWindow* window);
	void onChannelCreated(ByyChannel* channel);
	void onChannelToBeDestroyed(ByyChannel* channel);

private:
	Ui::ByyDisplayManageWidget ui;
	ByyApp& myApp;

	QAction* addWindowAction;
	QAction* removeWindowAction;
	QAction* addChannelAction;
	QAction* removeChannelAction;

	QTreeWidgetItem* myRoot;
};

#endif // DISPLAYMANAGEWIDGET_H
