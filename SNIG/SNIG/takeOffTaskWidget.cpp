#include "StdAfx.h"
#include "takeOffTaskWidget.h"
#include "takeOffTask.h"

ByyTakeOffTaskWidget::ByyTakeOffTaskWidget( ByyIG& ig,QWidget *parent /*= 0*/ ) 
	: ByyTaskEditorWidget(ig,parent)
	,myTask(NULL)
{
	ui.setupUi(this);

	connect(ui.pushButton,SIGNAL(clicked()),SLOT(accept()));
	connect(ui.pushButton_2,SIGNAL(clicked()),SLOT(reject()));
}

ByyTakeOffTaskWidget::~ByyTakeOffTaskWidget()
{

}

void ByyTakeOffTaskWidget::init( ByyTask* task )
{
	myTask=dynamic_cast<ByyTakeOffTask*>(task);

	ui.nameEdit->setText(myTask->name());
	ui.startTimeSpinBox->setValue(myTask->startTime());
}

ByyTask* ByyTakeOffTaskWidget::task()
{
	return myTask;
}

void ByyTakeOffTaskWidget::accept()
{
	myTask->setName(ui.nameEdit->text());
	myTask->setStartTime(ui.startTimeSpinBox->value());

	close();
	emit accepted();
}

void ByyTakeOffTaskWidget::reject()
{
	close();
	emit rejected();
}
