#include "StdAfx.h"
#include "localEntityWidget.h"
#include "localObjectManager.h"

ByyLocalEntityWidget::ByyLocalEntityWidget( ByyLocalObjectManager& objManager,QWidget *parent /*= 0*/ ) 
	: QWidget(parent,Qt::Window)
	,myObjManager(objManager)
{
	ui.setupUi(this);

	connect(ui.lineEdit,SIGNAL(textChanged(const QString&)),SLOT(onFilter(const QString&)));

	connect(ui.okButton,SIGNAL(clicked()),SLOT(onOkClicked()));
	connect(ui.cancelButton,SIGNAL(clicked()),SLOT(onCancelClicked()));

	refresh();
}

ByyLocalEntityWidget::~ByyLocalEntityWidget()
{

}

void ByyLocalEntityWidget::refresh()
{
	ui.listWidget->clear();

	ByyLocalObjectManager::EntityObjectMap& entitys=myObjManager.entityObjects();

	ByyLocalObjectManager::EntityObjectMap::iterator it=entitys.begin();

	for (;it!=entitys.end();++it)
	{
		QListWidgetItem *item=new QListWidgetItem(it.value()->name());

		item->setData(Qt::UserRole,(int)it.value());

		ui.listWidget->addItem(item);
	}

	onFilter(ui.lineEdit->text());
}

void ByyLocalEntityWidget::onFilter( const QString& text )
{
	for (int i=0;i!=ui.listWidget->count();++i)
	{
		QListWidgetItem *item=ui.listWidget->item(i);

		item->setHidden((!text.isEmpty())&&(!item->text().contains(text)));
	}
}

void ByyLocalEntityWidget::onOkClicked()
{
	QListWidgetItem *item=ui.listWidget->currentItem();

	ByyEntityObject *ent=0;

	if (item)
	{
		ent=(ByyEntityObject*)item->data(Qt::UserRole).toInt();
	}

	emit accept(ent);

	close();
}

void ByyLocalEntityWidget::onCancelClicked()
{
	emit accept(0);

	close();
}

void ByyLocalEntityWidget::setUserEntity( ByyEntityObject* ent )
{
	myUserEntity=ent;
}

ByyEntityObject* ByyLocalEntityWidget::userEntity()
{
	return myUserEntity;
}
