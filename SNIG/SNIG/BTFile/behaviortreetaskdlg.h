#ifndef BEHAVIORTREETASKDLG_H
#define BEHAVIORTREETASKDLG_H

#include <QDialog>
#include "ui_behaviortreetaskdlg.h"

class BehaviorTreeTaskDlg : public QDialog
{
	Q_OBJECT

public:
	BehaviorTreeTaskDlg(QWidget *parent = 0);
	~BehaviorTreeTaskDlg();
	void initTaskList();
	QString getSelectedTask();

public slots:
	void on_pushButton_ok_clicked();
	void on_pushButton_cancel_clicked();
	void on_listWidget_itemClicked(QListWidgetItem * item);

private:
	Ui::BehaviorTreeTaskDlg ui;
	QString m_CurSelectedTask;
};

#endif // BEHAVIORTREETASKDLG_H
