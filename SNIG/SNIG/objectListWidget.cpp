#include "StdAfx.h"
#include "objectListWidget.h"
#include "entityObjectWidget.h"
#include "controlObjectWidget.h"

ByyObjectListWidget::ByyObjectListWidget( ByyApp &app,QWidget *parent /*= 0*/ ) 
	: QWidget(parent)
{
	QTabWidget *tabWidget=new QTabWidget;

	QHBoxLayout *mainLayout=new QHBoxLayout;
	mainLayout->setContentsMargins(0,0,0,0);
	mainLayout->addWidget(tabWidget);
	setLayout(mainLayout);

	myEntObjWidget=new ByyEntityObjectWidget(app);

	tabWidget->addTab(myEntObjWidget,QString::fromLocal8Bit("实体列表"));

	myCtrlObjWidget=new ByyControlObjectWidget(app);

	tabWidget->addTab(myCtrlObjWidget,QString::fromLocal8Bit("控制对象"));
}

ByyObjectListWidget::~ByyObjectListWidget()
{

}
