#include "StdAfx.h"
#include "sensorOperTaskWidget.h"
#include "sensorOperTask.h"

ByySensorOperTaskWidget::ByySensorOperTaskWidget( ByyIG& ig,QWidget *parent /*= 0*/ )
	: ByyTaskEditorWidget(ig,parent),myTask(NULL)
{
	ui.setupUi(this);

	connect(ui.pushButton,SIGNAL(clicked()),SLOT(accept()));
	connect(ui.pushButton_2,SIGNAL(clicked()),SLOT(reject()));
}

ByySensorOperTaskWidget::~ByySensorOperTaskWidget()
{

}

void ByySensorOperTaskWidget::init( ByyTask* task )
{
	myTask=dynamic_cast<ByySensorOperTask*>(task);

	ByyDescribeEntry *descEntry=myTask->entity().describeEntry();


	if (descEntry)
	{
		const ByyDescribeEntry::ComponentList& components=descEntry->getComponent("sensor");

		for (int i=0;i!=components.size();++i)
		{
			ui.comboBox->addItem(components[i].getTag());
		}
	}

	if (ui.comboBox->count()==0)
	{
		ui.pushButton->setEnabled(false);
	}

	ui.nameEdit->setText(myTask->name());
	ui.startTimeSpinBox->setValue(myTask->startTime());

	ui.comboBox->setCurrentIndex(ui.comboBox->findText(myTask->getDeviceName()));
	ui.checkBox->setChecked(myTask->getPowerOn());
}

ByyTask* ByySensorOperTaskWidget::task()
{
	return myTask;
}

void ByySensorOperTaskWidget::accept()
{
	myTask->setDeviceName(ui.comboBox->currentText());
	myTask->setPowerOn(ui.checkBox->isChecked());

	myTask->setName(ui.nameEdit->text());
	myTask->setStartTime(ui.startTimeSpinBox->value());

	close();
	emit accepted();
}

void ByySensorOperTaskWidget::reject()
{
	close();
	emit rejected();
}
