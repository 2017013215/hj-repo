#include "StdAfx.h"
#include "ByyDescribeWidget.h"
#include "entityObject.h"
#include "ByyDescribeEntry.h"
#include "ByyDescComponentWidget.h"

ByyDescribeWidget::ByyDescribeWidget( ByyCore& core,ByyEntityObject* entity/*=0*/,QWidget* parent/*=0*/,Qt::WindowFlags f /*= 0*/ )
	:ByyEntityObserver(core,entity,parent,f)
{
	ui.setupUi(this);

	setEntity(entity);
}

ByyDescribeWidget::~ByyDescribeWidget()
{

}

void ByyDescribeWidget::setEntity( ByyEntityObject *entity )
{
	ByyEntityObserver::setEntity(entity);

	while(ui.tabWidget->count()>0)
	{
		QWidget *tabItem=ui.tabWidget->widget(0);
		ui.tabWidget->removeTab(0);
		delete tabItem;
	}

	if (!entity)
		return;

	ByyDescribeEntry* entry=myCurrentEntity->describeEntry();

	setWindowTitle(myCurrentEntity->name()+"-"+entry->getItemClass());

	const ByyDescribeEntry::ComponentList& components=entry->getComponent();

	for (int i=0;i!=components.size();++i)
	{
		ui.tabWidget->addTab(new ByyDescComponentWidget(components[i],this),components[i].getTag());
	}
}
