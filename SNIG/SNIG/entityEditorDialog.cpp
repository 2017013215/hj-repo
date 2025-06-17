#include "stdafx.h"
#include "entityEditorDialog.h"
#include <modelMapping.h>
#include <entityMapping.h>
#include "entityDefineAddDialog.h"
#include "entityDefineDelegate.h"

ByyEntityEditorDialog::ByyEntityEditorDialog(QWidget *parent)
	: QDialog(parent,Qt::WindowSystemMenuHint)
{
	ui.setupUi(this);

	setWindowIcon(QIcon("../data/icons/Configuration.svg"));
	ui.tabWidget->setTabIcon(0,QIcon("../data/icons/ConfigurationDefinitions.svg"));
	ui.tabWidget->setTabIcon(1,QIcon("../data/icons/ConfigurationMappingEntity.svg"));
	ui.pushButton_1->setIcon(QIcon("../data/icons/UtilityAdd.png"));
	ui.pushButton_2->setIcon(QIcon("../data/icons/UtilityMinus.png"));
	ui.pushButton_3->setIcon(QIcon("../data/icons/UtilityRename.png"));
	ui.pushButton_4->setIcon(QIcon("../data/icons/UtilityAdd.png"));
	ui.pushButton_5->setIcon(QIcon("../data/icons/UtilityMinus.png"));
	ui.pushButton_6->setIcon(QIcon("../data/icons/UtilityRename.png"));


	init();

	connect(ui.lineEdit,SIGNAL(textChanged(const QString&)),this,SLOT(modelFilter(const QString&)));
	connect(ui.lineEdit_1,SIGNAL(textChanged(const QString&)),this,SLOT(entityFilter(const QString&)));
	connect(ui.listWidget,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),this,SLOT(currentModelDefineChanged(QListWidgetItem*,QListWidgetItem*)));
	connect(ui.pushButton_3,SIGNAL(clicked()),this,SLOT(modelRename()));
	connect(ui.pushButton_1,SIGNAL(clicked()),this,SLOT(addModelDefine()));
	connect(ui.pushButton_2,SIGNAL(clicked()),this,SLOT(removeModelDefine()));
	connect(ui.tableWidget,SIGNAL(itemDoubleClicked(QTableWidgetItem*)),SLOT(editModelUrl(QTableWidgetItem*)));
	connect(ui.pushButton_4,SIGNAL(clicked()),this,SLOT(addEntityDefine()));
	connect(ui.pushButton_5,SIGNAL(clicked()),this,SLOT(removeEntityDefine()));
	connect(ui.pushButton_6,SIGNAL(clicked()),this,SLOT(entityRename()));
	connect(ui.listWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(modelRename()));
	connect(ui.tableWidget_1,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(entityDefineChanged(QTableWidgetItem*)));

	ui.tableWidget_1->setItemDelegate(new ByyEntityDefineDelegate(ui.listWidget,this));
}

ByyEntityEditorDialog::~ByyEntityEditorDialog()
{

}

void ByyEntityEditorDialog::init()
{
	const QHash<QString,QString>& modelMappingData=ByyModelMapping::instance()->getMappingData();

	QHash<QString,QString>::const_iterator it=modelMappingData.begin();

	for (;it!=modelMappingData.end();++it)
	{
		QListWidgetItem *item=new QListWidgetItem(it.key());
		item->setData(Qt::UserRole,it.value());
		ui.listWidget->addItem(item);
	}

	const QHash<QString,QString>& entityMappingData=ByyEntityMapping::instance()->getMappingData();

	it=entityMappingData.begin();

	for (int i=0;it!=entityMappingData.end();++it)
	{
		ui.tableWidget_1->insertRow(i);
		QTableWidgetItem *item=new QTableWidgetItem(it.key());
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
		ui.tableWidget_1->setItem(i,0,item);
		item=new QTableWidgetItem(it.value());
		ui.tableWidget_1->setItem(i,1,item);

		++i;
	}

	ui.tableWidget_1->resizeColumnsToContents();
	ui.tableWidget_1->resizeRowsToContents();
	ui.tableWidget_1->sortItems(0);
}

void ByyEntityEditorDialog::modelFilter( const QString& text )
{
	for (int i=0;i!=ui.listWidget->count();++i)
	{
		ui.listWidget->setRowHidden(i,text.isEmpty()?false:!ui.listWidget->item(i)->text().toLower().contains(text.toLower()));
	}
}

void ByyEntityEditorDialog::entityFilter( const QString& text )
{
	for (int i=0;i!=ui.tableWidget_1->rowCount();++i)
	{
		ui.tableWidget_1->setRowHidden(i,text.isEmpty()?false:!ui.tableWidget_1->item(i,0)->text().toLower().contains(text.toLower()));
	}
}

void ByyEntityEditorDialog::currentModelDefineChanged( QListWidgetItem * current, QListWidgetItem * previous )
{
	if (current)
	{
		ui.tableWidget->item(0,1)->setText(current->data(Qt::UserRole).toString());
	}
}

void ByyEntityEditorDialog::modelRename()
{
	if (ui.listWidget->currentRow()==-1)
	{
		return;
	}
	QListWidgetItem *current=ui.listWidget->currentItem();

	bool ok=false;

	QString name=QInputDialog::getText(this,QString::fromLocal8Bit("重命名模型定义"),
		QString::fromLocal8Bit("新模型定义名称:"),QLineEdit::Normal,
		current->text(),&ok);

	if (!ok||name==current->text())
	{
		return;
	}

	if (ByyModelMapping::instance()->find(name))
	{
		QMessageBox::warning(this,QString::fromLocal8Bit("模型定义"),
			QString::fromLocal8Bit("该名称已经存在！"));
		return;
	}

	ByyModelMapping::instance()->rekey(current->text(),name);
	ByyEntityMapping::instance()->updateValue(current->text(),name);

	QTableWidgetItem* item;

	for (int i=0;i!=ui.tableWidget_1->rowCount();++i)
	{
		item=ui.tableWidget_1->item(i,1);
		if (current->text()==item->text())
		{
			item->setText(name);
		}
	}

	current->setText(name);
}

void ByyEntityEditorDialog::addModelDefine()
{
	bool ok=false;
	QString name=QInputDialog::getText(this,QString::fromLocal8Bit("添加模型定义"),
		QString::fromLocal8Bit("模型定义名称:"),QLineEdit::Normal,QString(),&ok);

	if (!ok)
	{
		return;
	}

	if (ByyModelMapping::instance()->find(name))
	{
		QMessageBox::warning(this,QString::fromLocal8Bit("模型定义"),
			QString::fromLocal8Bit("该名称已经存在！"));
		return;
	}

	ByyModelMapping::instance()->add(name,QString());

	QListWidgetItem *item=new QListWidgetItem(name);
	item->setData(Qt::UserRole,QString());
	ui.listWidget->addItem(item);
	ui.listWidget->sortItems();
	ui.listWidget->setCurrentItem(item);
}

void ByyEntityEditorDialog::removeModelDefine()
{
	QListWidgetItem *current=ui.listWidget->currentItem();

	if (!current)
	{
		return;
	}

	int r=QMessageBox::warning(
		this,QString::fromLocal8Bit("确认"),QString::fromLocal8Bit(
		"你确认删除\"%1\"吗?").arg(current->text()),QMessageBox::Yes|QMessageBox::No);

	if (QMessageBox::Yes==r)
	{

		ByyModelMapping::instance()->remove(current->text());
		ByyEntityMapping::instance()->updateValue(current->text(),QString());

		QTableWidgetItem* item;

		for (int i=0;i!=ui.tableWidget_1->rowCount();++i)
		{
			item=ui.tableWidget_1->item(i,1);
			if (current->text()==item->text())
			{
				item->setText(QString());
			}
		}

		delete ui.listWidget->takeItem(ui.listWidget->row(current));
	}
}

void ByyEntityEditorDialog::editModelUrl( QTableWidgetItem * item )
{
	if (item->column()==0)
	{
		return;
	}

	QListWidgetItem* current=ui.listWidget->currentItem();

	if (!current)
	{
		return;
	}


	QString fileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择文件"),
		item->text().isEmpty()?"../data/models":item->text(),
		tr("Model File (*.osg *.ive *.flt *.3ds)"));

	if (fileName.isEmpty())
	{
		return;
	}
	
	fileName=QDir().relativeFilePath(fileName);

	item->setText(fileName);
	current->setData(Qt::UserRole,fileName);

	ByyModelMapping::instance()->revalue(current->text(),fileName);
}

void ByyEntityEditorDialog::addEntityDefine()
{
	ByyEntityDefineAddDialog dlg(ui.listWidget);
	if (dlg.exec()!=QDialog::Accepted)
	{
		return;
	}

	QString entityType=dlg.getEntityType();

	if (ByyEntityMapping::instance()->find(entityType))
	{
		QMessageBox::warning(this,QString::fromLocal8Bit("实体定义"),
			QString::fromLocal8Bit("该实体类型已经存在！"));
		return;
	}

	ByyEntityMapping::instance()->add(entityType,dlg.getEntityDefine());

	int row=ui.tableWidget_1->rowCount();
	ui.tableWidget_1->insertRow(row);
	QTableWidgetItem *item=new QTableWidgetItem(entityType);
	item->setFlags(item->flags()&~Qt::ItemIsEditable);
	ui.tableWidget_1->setItem(row,0,item);
	item=new QTableWidgetItem(dlg.getEntityDefine());
	ui.tableWidget_1->setItem(row,1,item);
	ui.tableWidget_1->sortItems(0);
}

void ByyEntityEditorDialog::removeEntityDefine()
{
	int row=ui.tableWidget_1->currentRow();

	if (row==-1)
	{
		return;
	}
	QTableWidgetItem *typeItem=ui.tableWidget_1->item(row,0);

	if (QMessageBox::Yes==QMessageBox::warning(
		this,QString::fromLocal8Bit("确认"),QString::fromLocal8Bit(
		"你确认删除\"%1\"吗?").arg(typeItem->text()),QMessageBox::Yes|QMessageBox::No))
	{
		ByyEntityMapping::instance()->remove(typeItem->text());
		ui.tableWidget_1->removeRow(row);
	}
}

void ByyEntityEditorDialog::entityRename()
{
	int row=ui.tableWidget_1->currentRow();
	if (row==-1)
	{
		return;
	}
	QTableWidgetItem* current=ui.tableWidget_1->item(row,0);

	bool ok=false;

	QString name=QInputDialog::getText(this,QString::fromLocal8Bit("修改实体类型"),
		QString::fromLocal8Bit("新实体类型:"),QLineEdit::Normal,
		current->text(),&ok);

	if (!ok||name==current->text())
	{
		return;
	}

	if (ByyEntityMapping::instance()->find(name))
	{
		QMessageBox::warning(this,QString::fromLocal8Bit("实体定义"),
			QString::fromLocal8Bit("该类型已经存在！"));
		return;
	}

	ByyEntityMapping::instance()->rekey(current->text(),name);

	current->setText(name);
}

void ByyEntityEditorDialog::entityDefineChanged( QTableWidgetItem* item )
{
	if (item->column()!=1)
	{
		return;
	}

	QString name=ui.tableWidget_1->item(item->row(),0)->text();

	ByyEntityMapping::instance()->revalue(name,item->text());
}
