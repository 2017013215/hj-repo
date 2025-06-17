#include "StdAfx.h"
#include "ByyApp.h"
#include "displayManageWidget.h"
#include "ByyDisplaySignaler.h"
#include <osgQt/GraphicsWindowQt>
#include "mainWindow.h"

ByyDisplayManageWidget::ByyDisplayManageWidget( ByyApp& app,QWidget *parent /*= 0*/ ) 
	: QWidget(parent)
	,myApp(app)
{
	ui.setupUi(this);

	init();
	initAction();

	ui.treeWidget->installEventFilter(this);

	connect(ByyDisplaySignaler::instance(),SIGNAL(windowCreated(ByyOsgWindow*)),this,SLOT(onWindowCreated(ByyOsgWindow*)));
	connect(ByyDisplaySignaler::instance(),SIGNAL(windowToBeDestroyed(ByyOsgWindow*)),this,SLOT(onWindowToBeDestroyed(ByyOsgWindow*)));
	connect(ByyDisplaySignaler::instance(),SIGNAL(channelCreated(ByyChannel*)),this,SLOT(onChannelCreated(ByyChannel*)));
	connect(ByyDisplaySignaler::instance(),SIGNAL(channelToBeDestroyed(ByyChannel*)),this,SLOT(onChannelToBeDestroyed(ByyChannel*)));
}

ByyDisplayManageWidget::~ByyDisplayManageWidget()
{

}

void ByyDisplayManageWidget::init()
{
	myRoot=new QTreeWidgetItem(0);
	myRoot->setText(0,QString::fromLocal8Bit("本地"));

	ui.treeWidget->addTopLevelItem(myRoot);

	ByyDisplay::WindowList& windowlist=myApp.ig().display().windows();

	for (int i=0;i!=windowlist.size();++i)
	{
		onWindowCreated(windowlist[i]);
	}
}

void ByyDisplayManageWidget::initAction()
{
	addWindowAction=new QAction(QString::fromLocal8Bit("添加窗口"),this);
	connect(addWindowAction,SIGNAL(triggered()),this,SLOT(addWindow()));

	removeWindowAction=new QAction(QString::fromLocal8Bit("移除窗口"),this);
	connect(removeWindowAction,SIGNAL(triggered()),this,SLOT(removeWindow()));

	addChannelAction=new QAction(QString::fromLocal8Bit("添加通道"),this);
	connect(addChannelAction,SIGNAL(triggered()),this,SLOT(addChannel()));

	removeChannelAction=new QAction(QString::fromLocal8Bit("移除通道"),this);
	connect(removeChannelAction,SIGNAL(triggered()),this,SLOT(removeChannel()));
}

void ByyDisplayManageWidget::addWindow()
{
	QTreeWidgetItem *item=ui.treeWidget->currentItem();

	if (!item)
		return;

	ByyOsgWindow* window=myApp.ig().display().addWindow();
	window->getAdapterWidget()->setParent(&myApp.mainWindow());
	window->getAdapterWidget()->setWindowFlags(Qt::Window);
	window->getAdapterWidget()->show();

//	QDockWidget *dock=new QDockWidget();
//	dock->setWidget(window->getAdapterWidget());

	//dock->setFloating(true);
//	dock->show();
}

void ByyDisplayManageWidget::removeWindow()
{
	QTreeWidgetItem *item=ui.treeWidget->currentItem();

	if (!item)
		return;
}

void ByyDisplayManageWidget::addChannel()
{
	QTreeWidgetItem *item=ui.treeWidget->currentItem();

	if (!item)
		return;
}

void ByyDisplayManageWidget::removeChannel()
{
	QTreeWidgetItem *item=ui.treeWidget->currentItem();

	if (!item)
		return;


}

bool ByyDisplayManageWidget::eventFilter( QObject *obj, QEvent *ev )
{
	if (obj==ui.treeWidget&&ev->type()==QEvent::ContextMenu)
	{
		QTreeWidgetItem *item=ui.treeWidget->itemAt(ui.treeWidget->viewport()->mapFromGlobal(QCursor::pos()));

		if (item)
		{
			QMenu *menu=new QMenu(this);

			switch(item->type())
			{
			case 0:
				menu->addAction(addWindowAction);
				break;
			case 1:
				menu->addAction(removeWindowAction);
				menu->addAction(addChannelAction);
				break;
			case 2:
				menu->addAction(removeChannelAction);
				break;
			}

			menu->setAttribute(Qt::WA_DeleteOnClose);

			menu->popup(QCursor::pos());
		}

		return true;
	}

	return QWidget::eventFilter(obj,ev);
}

void ByyDisplayManageWidget::onWindowCreated( ByyOsgWindow* window )
{
	QTreeWidgetItem *windowItem=createWindowItem(window);
	myRoot->addChild(windowItem);

	ByyOsgWindow::ChannelList& channels=window->channels();

	ByyOsgWindow::ChannelList::iterator it=channels.begin();
	ByyOsgWindow::ChannelList::iterator itend=channels.end();

	for (;it!=itend;++it)
	{
		ByyChannel* channel=*it;

		QTreeWidgetItem *channelItem=createChannelItem(channel);
		windowItem->addChild(channelItem);
	}
}

void ByyDisplayManageWidget::onWindowToBeDestroyed( ByyOsgWindow* window )
{
	QTreeWidgetItem* windowItem=findWindowItem(window);

	if (windowItem)
	{
		myRoot->removeChild(windowItem);
		delete windowItem;
	}
}

void ByyDisplayManageWidget::onChannelCreated( ByyChannel* channel )
{
	QTreeWidgetItem* windowItem=findWindowItem(&channel->window());

	if (windowItem)
	{
		windowItem->addChild(createChannelItem(channel));
	}
}

void ByyDisplayManageWidget::onChannelToBeDestroyed( ByyChannel* channel )
{
	QTreeWidgetItem* channelItem=findChannelItem(channel);

	if (channelItem)
	{
		if (channelItem->parent())
		{
			channelItem->parent()->removeChild(channelItem);
			delete channelItem;
		}
	}
}

QTreeWidgetItem* ByyDisplayManageWidget::createWindowItem( ByyOsgWindow* window )
{
	QTreeWidgetItem* windowItem=new QTreeWidgetItem(1);
	windowItem->setText(0,window->getName());
	windowItem->setData(0,Qt::UserRole,(int)window);
	return windowItem;
}

QTreeWidgetItem* ByyDisplayManageWidget::createChannelItem( ByyChannel* channel )
{
	QTreeWidgetItem *channelItem=new QTreeWidgetItem(2);
	channelItem->setText(0,channel->getName());
	channelItem->setData(0,Qt::UserRole,(int)channel);
	return channelItem;
}

QTreeWidgetItem* ByyDisplayManageWidget::findWindowItem( ByyOsgWindow* window )
{
	QTreeWidgetItemIterator it(myRoot);
	while (*it) 
	{
		if ((*it)->type()==1)
		{
			if(window==(ByyOsgWindow*)(*it)->data(0,Qt::UserRole).toInt())
			{
				return *it;
			}
		}

		++it;
	}

	return 0;
}

QTreeWidgetItem* ByyDisplayManageWidget::findChannelItem( ByyChannel* channel )
{
	QTreeWidgetItemIterator it(myRoot);
	while (*it) 
	{
		if ((*it)->type()==2)
		{
			if(channel==(ByyChannel*)(*it)->data(0,Qt::UserRole).toInt())
			{
				return *it;
			}
		}

		++it;
	}

	return 0;
}
