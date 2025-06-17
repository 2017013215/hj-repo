#ifndef BEHAVIORTREETRIGGERDLG_H
#define BEHAVIORTREETRIGGERDLG_H

#include <QDialog>
#include "ui_behaviortreetriggerdlg.h"

class BehaviorTreeTriggerDlg : public QDialog
{
	Q_OBJECT

public:
	BehaviorTreeTriggerDlg(QWidget *parent = 0);
	~BehaviorTreeTriggerDlg();
	void initTriggerList();
	QString getSelectedTrigger();

public slots:
	void on_pushButton_ok_clicked();
	void on_pushButton_cancel_clicked();
	void on_listWidget_itemClicked(QListWidgetItem * item);

private:
	Ui::BehaviorTreeTriggerDlg ui;
	QString m_CurSelectedTrigger;
};

#endif // BEHAVIORTREETRIGGERDLG_H
