#ifndef BEHAVIORTREETRIGGERNUDLG_H
#define BEHAVIORTREETRIGGERNUDLG_H

#include <QDialog>
#include "ui_behaviortreetriggernudlg.h"

class BehaviorTreeTriggerNuDlg : public QDialog
{
	Q_OBJECT

public:
	BehaviorTreeTriggerNuDlg(QWidget *parent = 0);
	~BehaviorTreeTriggerNuDlg();
	int getTriggerNumber();

public slots:
	void on_pushButton_ok_clicked();
	void on_pushButton_cancel_clicked();

private:
	Ui::BehaviorTreeTriggerNuDlg ui;
};

#endif // BEHAVIORTREETRIGGERNUDLG_H
