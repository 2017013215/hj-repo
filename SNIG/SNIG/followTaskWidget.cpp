#include "StdAfx.h"
#include "followTaskWidget.h"
#include "followTask.h"
#include "ByyIG.h"
#include "ByyCore.h"
#include "localObjectManager.h"

#include "comApp.h"
#include "ByyApp.h"
#include "ByyHdose.h"

ByyFollowTaskWidget::ByyFollowTaskWidget( ByyIG& ig,QWidget *parent /*= 0*/ ) 
	: ByyTaskEditorWidget(ig,parent),myTask(0)
{
	ui.setupUi(this);


	/*if(comApp::instance()->getApp()->hdose().mode() == ByyHdose::Join)
	{
		ByyLocalObjectManager::EntityObjectMap& entitys=myIG.core().remoteObjectManager().entityObjects();

		ByyLocalObjectManager::EntityObjectMap::iterator it=entitys.begin();

		for (;it!=entitys.end();++it)
		{
			ui.waypointComboBox->addItem((*it)->name());
		}
	}
	else
	{
		ByyLocalObjectManager::EntityObjectMap& entitys=myIG.core().localObjectManager().entityObjects();

		ByyLocalObjectManager::EntityObjectMap::iterator it=entitys.begin();

		for (;it!=entitys.end();++it)
		{
			ui.waypointComboBox->addItem((*it)->name());
		}
	}*/
	
	ByyLocalObjectManager::EntityObjectMap& entitys=myIG.core().localObjectManager().entityObjects();

	ByyLocalObjectManager::EntityObjectMap::iterator it=entitys.begin();

	for (;it!=entitys.end();++it)
	{
		ui.waypointComboBox->addItem((*it)->name());
	}


	connect(ui.pushButton,SIGNAL(clicked()),SLOT(accept()));
	connect(ui.pushButton_2,SIGNAL(clicked()),SLOT(reject()));
}

ByyFollowTaskWidget::~ByyFollowTaskWidget()
{

}

void ByyFollowTaskWidget::init( ByyTask* task )
{
	myTask=dynamic_cast<ByyFollowTask*>(task);

	ui.nameEdit->setText(myTask->name());
	ui.startTimeSpinBox->setValue(myTask->startTime());

	ui.nameEdit_2->setText(QString::number(myTask->distance()));
	ui.nameEdit_3->setText(QString::number(myTask->relativeOrientation()));
	ui.nameEdit_4->setText(QString::number(myTask->highOffset()));

	int index=ui.waypointComboBox->findText(myTask->followedObj());
	ui.waypointComboBox->setCurrentIndex(index);
}

ByyTask* ByyFollowTaskWidget::task()
{
	return myTask;
}

void ByyFollowTaskWidget::accept()
{
	myTask->setFollowedObj(ui.waypointComboBox->currentText());

	myTask->setName(ui.nameEdit->text());
	myTask->setStartTime(ui.startTimeSpinBox->value());

	myTask->setDistance(ui.nameEdit_2->text().toInt());
	myTask->setRelativeOrientation(ui.nameEdit_3->text().toInt());
	myTask->setHighOffset(ui.nameEdit_4->text().toInt());

	close();
	emit accepted();
}

void ByyFollowTaskWidget::reject()
{
	close();
	emit rejected();
}
