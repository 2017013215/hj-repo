#include "StdAfx.h"
#include "behaviortreetriggernudlg.h"

BehaviorTreeTriggerNuDlg::BehaviorTreeTriggerNuDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->setWindowTitle(QString::fromLocal8Bit("设置触发次数"));
	this->setWindowFlags(Qt::Window/* | Qt::WindowTitleHint | Qt::CustomizeWindowHint*/);
	this->setWindowFlags(this->windowFlags()&~Qt::WindowMinMaxButtonsHint);
}

BehaviorTreeTriggerNuDlg::~BehaviorTreeTriggerNuDlg()
{

}

int BehaviorTreeTriggerNuDlg::getTriggerNumber()
{
	return ui.spinBox->value();
}

void BehaviorTreeTriggerNuDlg::on_pushButton_ok_clicked()
{
	this->accept();
}

void BehaviorTreeTriggerNuDlg::on_pushButton_cancel_clicked()
{
	this->reject();
}
