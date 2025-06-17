#include "StdAfx.h"
#include "autoHidePanel.h"
#include "autoHidePanelItem.h"
#include "ByyApp.h"
#include "mainWindow.h"
#include "autoHideDock.h"

ByyAutoHidePanel::ByyAutoHidePanel( QWidget* centralWidget,QObject *parent ) 
	: QObject(parent)
,myCentralWidget(centralWidget)
{
	myCentralWidget->installEventFilter(this);
}

ByyAutoHidePanel::~ByyAutoHidePanel()
{

}

bool ByyAutoHidePanel::eventFilter( QObject *obj, QEvent *ev )
{
	if (ev->type()==QEvent::Resize)
	{
		refreshPos();
	}
	return __super::eventFilter(obj,ev);
}

void ByyAutoHidePanel::refreshPos()
{
	int x=myCentralWidget->width()-30;

	int y=0;

	for (int i=0;i!=myPannelItems.size();++i)
	{
		y+=2;

		myPannelItems[i]->move(x,y);

		y+=myPannelItems[i]->size().height();
	}

	for (int i=0;i!=myDockWidgets.size();++i)
	{
		myDockWidgets[i]->move(myCentralWidget->width()-30-5-myDockWidgets[i]->width(),5);
	}
}

void ByyAutoHidePanel::addDockWidget( ByyAutoHideDock* dockWidget,const QString& text,const QIcon& icon )
{
	dockWidget->setParent(myCentralWidget);

	myDockWidgets.append(dockWidget);

	ByyAutoHidePanelItem *item=new ByyAutoHidePanelItem(myCentralWidget);

	item->setIcon(icon);
	item->setText(text);

	connect(item,SIGNAL(clicked(bool)),dockWidget,SLOT(setVisible(bool)));
	connect(dockWidget,SIGNAL(closed()),item,SLOT(setChecked()));

	myPannelItems.push_back(item);
}
