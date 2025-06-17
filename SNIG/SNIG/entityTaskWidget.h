#ifndef ENTITYTASKWIDGET_H
#define ENTITYTASKWIDGET_H

#include "ByyEntityObserver.h"
#include "ui_entityTaskWidget.h"

class ByyApp;
class ByyTask;
class ByyEntityObject;

class ByyEntityTaskWidget : public ByyEntityObserver
{
	Q_OBJECT

public:
	ByyEntityTaskWidget(ByyApp& app,ByyEntityObject* entObj,QWidget *parent = 0);
	~ByyEntityTaskWidget();

protected slots:
	void createTask();
	void taskEditAccepted();
	void taskEditRejected();
	void edit();

	void remove();
	void accept();
	void reject();

	void taskSelected(QTreeWidgetItem*,QTreeWidgetItem*);
	void currentTaskToPrev();
	void currentTaskToNext();
	

	void onEntityRemove(ByyEntityObject* entObj);

protected:
	void updateTaskTree(ByyTask* curTask=0);
	void appendTaskToTree(ByyTask* task,bool focused );
	void updateTreeItem(ByyTask* task);
	void editTask( ByyTask* task );
	void updateUIs();
private:
	Ui::ByyEntityTaskWidget ui;
	ByyApp& myApp;

	ByyTask* myCreatedTask;
};

#endif // ENTITYTASKWIDGET_H
