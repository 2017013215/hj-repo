#include "StdAfx.h"
#include "cruiseTaskWidget.h"
#include "cruiseTask.h"

ByyCruiseTaskWidget::ByyCruiseTaskWidget( ByyIG& ig,QWidget *parent /*= 0*/ ) 
	: ByyTaskEditorWidget(ig,parent),myTask(NULL)
{
	ui.setupUi(this);
	connect(ui.pushButton,SIGNAL(clicked()),SLOT(accept()));
	connect(ui.pushButton_2,SIGNAL(clicked()),SLOT(reject()));
}

ByyCruiseTaskWidget::~ByyCruiseTaskWidget()
{

}

void ByyCruiseTaskWidget::init( ByyTask* task )
{
	myTask=dynamic_cast<ByyCruiseTask*>(task);

	ui.nameEdit->setText(myTask->name());
	ui.startTimeSpinBox->setValue(myTask->startTime());
}

ByyTask* ByyCruiseTaskWidget::task()
{
	return myTask;
}

void ByyCruiseTaskWidget::accept()
{
	myTask->setName(ui.nameEdit->text());
	myTask->setStartTime(ui.startTimeSpinBox->value());

	close();
	emit accepted();
}

void ByyCruiseTaskWidget::reject()
{
	close();
	emit rejected();
}
