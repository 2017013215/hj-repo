#include "StdAfx.h"
#include "entityTaskWidget.h"
#include "taskFactory.h"
#include "task.h"
#include "taskEditorWidget.h"
#include "taskWidgetFactory.h"
#include "taskManager.h"
#include "ByyApp.h"

Q_DECLARE_METATYPE(ByyTaskCreator*)
Q_DECLARE_METATYPE(ByyTask*)

QMenu* findMenu(QString id,QMenu* parent)
{
	foreach(QObject* item,parent->children())
	{
		QMenu* menu=qobject_cast<QMenu*>(item);
		if(menu==0)
			continue;
		if(menu->title()==id)
			return menu;
	}
	QMenu* newmenu=parent->addMenu(id);
	return newmenu;
}

ByyEntityTaskWidget::ByyEntityTaskWidget( ByyApp& app,ByyEntityObject* entObj,QWidget *parent /*= 0*/ ) 
: ByyEntityObserver(app.core(),entObj,parent)
,myApp(app)
,myCreatedTask(NULL)
{
	ui.setupUi(this);

	setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

	QMenuBar *menuBar=new QMenuBar(this);

	QMenu* appendMenu=menuBar->addMenu(QString::fromLocal8Bit("Ìí¼Ó"));

	ByyTaskFactory::TaskCreatorList& creators=myApp.core().taskFactory().creators();

	for (int i=0;i!=creators.size();++i)
	{
		QMenu* menu=0;
		if(creators[i]->typeInfo().clsify.isEmpty())
			menu=appendMenu;
		else
			menu=findMenu(creators[i]->typeInfo().clsify,appendMenu);
		QAction* taskAction=menu->addAction(creators[i]->typeInfo().name,this,SLOT(createTask()));
		taskAction->setData(QVariant::fromValue<ByyTaskCreator*>(creators[i]));
	}

	ui.verticalLayout_2->insertWidget(0,menuBar);

	updateTaskTree();
	updateUIs();

	connect(ui.treeWidget,SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),this,SLOT(taskSelected(QTreeWidgetItem*,QTreeWidgetItem*)));
	connect(ui.btnBack,SIGNAL(clicked(bool)),this,SLOT(currentTaskToPrev()));
	connect(ui.btnForward,SIGNAL(clicked(bool)),this,SLOT(currentTaskToNext()));
}

ByyEntityTaskWidget::~ByyEntityTaskWidget()
{

}

void ByyEntityTaskWidget::updateTaskTree(ByyTask* curTask)
{
	ui.treeWidget->clear();
	ByyTaskManager::TaskList& taskList=myCurrentEntity->taskManager().taskList();

	for (int i=0;i!=taskList.size();++i)
	{
		appendTaskToTree(taskList[i],taskList[i]==curTask);
	}
}

void ByyEntityTaskWidget::appendTaskToTree( ByyTask* task,bool focused )
{
	QTreeWidgetItem *item=new QTreeWidgetItem;
	item->setText(0,task->string());
	item->setData(0,Qt::UserRole,QVariant::fromValue<ByyTask*>(task));
	ui.treeWidget->addTopLevelItem(item);
	if(focused)
		ui.treeWidget->setCurrentItem(item);
}

void ByyEntityTaskWidget::updateTreeItem( ByyTask* task )
{
	if (!task)
		return;

	QTreeWidgetItem *item=NULL;

	for (int i=0;i!=ui.treeWidget->topLevelItemCount();++i)
	{
		item=ui.treeWidget->topLevelItem(i);

		if (item->data(0,Qt::UserRole).value<ByyTask*>()==task)
		{
			item->setText(0,task->string());
		}
 	}
}

void ByyEntityTaskWidget::createTask()
{
	QAction* taskAction=dynamic_cast<QAction*>(sender());

	if (!taskAction)
		return;

	ByyTaskCreator* taskCreator=taskAction->data().value<ByyTaskCreator*>();

	if (!taskCreator)
		return;

	myCreatedTask=taskCreator->createTask(*myCurrentEntity);

	editTask(myCreatedTask);
}

void ByyEntityTaskWidget::taskEditAccepted()
{
	ui.widget->setEnabled(true);

	ByyTaskEditorWidget* taskEditor=dynamic_cast<ByyTaskEditorWidget*>(sender());

	if (taskEditor)
	{
		if (myCreatedTask==taskEditor->task())
		{
			myCurrentEntity->taskManager().appendTask(myCreatedTask);
			myApp.hdose().sendTask(myCreatedTask);
			appendTaskToTree(myCreatedTask,true);
			myCreatedTask=NULL;
		}
		else
		{
			updateTreeItem(taskEditor->task());
		}

		taskEditor->deleteLater();
	}
}

void ByyEntityTaskWidget::taskEditRejected()
{
	ui.widget->setEnabled(true);

	ByyTaskEditorWidget* taskEditor=dynamic_cast<ByyTaskEditorWidget*>(sender());

	if (taskEditor)
	{
		if (myCreatedTask==taskEditor->task())
		{
			delete myCreatedTask;
			myCreatedTask=NULL;
		}
		taskEditor->deleteLater();
	}
}

void ByyEntityTaskWidget::edit()
{
	QTreeWidgetItem *item=ui.treeWidget->currentItem();

	if (!item)
		return;

	ByyTask* task=item->data(0,Qt::UserRole).value<ByyTask*>();

	editTask(task);
}

void ByyEntityTaskWidget::remove()
{
	QTreeWidgetItem *item=ui.treeWidget->currentItem();

	if (item)
	{
		ByyTask* task=item->data(0,Qt::UserRole).value<ByyTask*>();

		ui.treeWidget->takeTopLevelItem(ui.treeWidget->indexOfTopLevelItem(item));

		myCurrentEntity->taskManager().removeTask(task);
	}
}

void ByyEntityTaskWidget::accept()
{
	close();
}

void ByyEntityTaskWidget::reject()
{
	close();
}

void ByyEntityTaskWidget::editTask( ByyTask* task )
{
	if (!task)
		return;

	ByyTaskEditorWidget* taskEditor=ByyTaskWidgetFactory::instance().createTaskEditor(task->typeInfo().type,myApp.ig(),this);

	if (taskEditor)
	{
		connect(taskEditor,SIGNAL(accepted()),SLOT(taskEditAccepted()));
		connect(taskEditor,SIGNAL(rejected()),SLOT(taskEditRejected()));

		ui.widget->setEnabled(false);
		taskEditor->createWidget();
		taskEditor->init(task);

		taskEditor->show();
	}
}

void ByyEntityTaskWidget::onEntityRemove( ByyEntityObject* entObj )
{
	if (entObj==myCurrentEntity)
	{
		close();
		deleteLater();
	}
}

void ByyEntityTaskWidget::taskSelected(QTreeWidgetItem*,QTreeWidgetItem*)
{
	updateUIs();
}

void ByyEntityTaskWidget::currentTaskToPrev()
{
	QTreeWidgetItem *item=ui.treeWidget->currentItem();
	ByyTask* task=item->data(0,Qt::UserRole).value<ByyTask*>();
	myCurrentEntity->taskManager().taskToPrev(task);
	updateTaskTree(task);
}
void ByyEntityTaskWidget::currentTaskToNext()
{
	QTreeWidgetItem *item=ui.treeWidget->currentItem();
	ByyTask* task=item->data(0,Qt::UserRole).value<ByyTask*>();
	myCurrentEntity->taskManager().taskToNext(task);
	updateTaskTree(task);
}



void ByyEntityTaskWidget::updateUIs()
{
	QTreeWidgetItem *item=ui.treeWidget->currentItem();
	ui.pushButton_3->setEnabled(item!=0);
	ui.pushButton_4->setEnabled(item!=0);
	if(item==0)
	{
		ui.btnBack->setEnabled(false);
		ui.btnForward->setEnabled(false);
		return;
	}
	ByyTask* task=item->data(0,Qt::UserRole).value<ByyTask*>();
	ui.btnBack->setEnabled(myCurrentEntity->taskManager().taskCanToPrev(task));
	ui.btnForward->setEnabled(myCurrentEntity->taskManager().taskCanToNext(task));
}