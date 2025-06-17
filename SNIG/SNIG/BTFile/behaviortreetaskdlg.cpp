#include "StdAfx.h"
#include "behaviortreetaskdlg.h"
#include "forcetask.h"

BehaviorTreeTaskDlg::BehaviorTreeTaskDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->setWindowTitle(QString::fromLocal8Bit("Ñ¡ÔñÈÎÎñ"));
	this->setWindowFlags(Qt::Window/* | Qt::WindowTitleHint | Qt::CustomizeWindowHint*/);
	this->setWindowFlags(this->windowFlags()&~Qt::WindowMinMaxButtonsHint);
	initTaskList();
}

BehaviorTreeTaskDlg::~BehaviorTreeTaskDlg()
{

}

void BehaviorTreeTaskDlg::initTaskList()
{
	QStringList list = ForceTaskManager::instance()->getTaskNodeList();
	ui.listWidget->addItems(list);
}

QString BehaviorTreeTaskDlg::getSelectedTask()
{
	return m_CurSelectedTask;
}

void BehaviorTreeTaskDlg::on_pushButton_ok_clicked()
{
	this->accept();
}

void BehaviorTreeTaskDlg::on_pushButton_cancel_clicked()
{
	this->reject();
}

void BehaviorTreeTaskDlg::on_listWidget_itemClicked(QListWidgetItem * item)
{
	if (item)
	{
		m_CurSelectedTask = item->text();
	}
}
