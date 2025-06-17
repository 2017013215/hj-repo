#include "StdAfx.h"
#include "behaviortreetriggerdlg.h"
#include "btntrigger.h"

BehaviorTreeTriggerDlg::BehaviorTreeTriggerDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->setWindowTitle(QString::fromLocal8Bit("Ñ¡ÔñÈÎÎñ"));
	this->setWindowFlags(Qt::Window/* | Qt::WindowTitleHint | Qt::CustomizeWindowHint*/);
	this->setWindowFlags(this->windowFlags()&~Qt::WindowMinMaxButtonsHint);
	initTriggerList();
}

BehaviorTreeTriggerDlg::~BehaviorTreeTriggerDlg()
{

}

void BehaviorTreeTriggerDlg::initTriggerList()
{
	QStringList list = BTNTriggerManager::instance()->getTriggerNodeList();
	ui.listWidget->addItems(list);
}

QString BehaviorTreeTriggerDlg::getSelectedTrigger()
{
	return m_CurSelectedTrigger;
}

void BehaviorTreeTriggerDlg::on_pushButton_ok_clicked()
{
	this->accept();
}

void BehaviorTreeTriggerDlg::on_pushButton_cancel_clicked()
{
	this->reject();
}

void BehaviorTreeTriggerDlg::on_listWidget_itemClicked(QListWidgetItem * item)
{
	if (item)
	{
		m_CurSelectedTrigger = item->text();
	}
}
