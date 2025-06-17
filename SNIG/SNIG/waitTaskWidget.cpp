#include "StdAfx.h"
#include "waitTaskWidget.h"
#include "waitTask.h"

ByyWaitTaskWidget::ByyWaitTaskWidget( ByyIG& ig,QWidget *parent /*= 0*/ )
	: ByyTaskEditorWidget(ig,parent),myTask(NULL)
{
	ui.setupUi(this);
	connect(ui.pushButton,SIGNAL(clicked()),SLOT(accept()));
	connect(ui.pushButton_2,SIGNAL(clicked()),SLOT(reject()));
}

ByyWaitTaskWidget::~ByyWaitTaskWidget()
{

}

void ByyWaitTaskWidget::init( ByyTask* task )
{
	myTask=dynamic_cast<ByyWaitTask*>(task);

	ui.nameEdit->setText(myTask->name());
	ui.startTimeSpinBox->setValue(myTask->startTime());

	ui.nameEdit_2->setText(QString::number(myTask->waitTime()));
}

ByyTask* ByyWaitTaskWidget::task()
{
	return myTask;
}

void ByyWaitTaskWidget::accept()
{
	myTask->setWaitTime(ui.nameEdit_2->text().toInt());

	myTask->setName(ui.nameEdit->text());
	myTask->setStartTime(ui.startTimeSpinBox->value());

	close();
	emit accepted();
}

void ByyWaitTaskWidget::reject()
{
	close();
	emit rejected();
}
