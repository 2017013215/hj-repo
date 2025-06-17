#include "entityMappingWidget.h"
#include "entityDefineAddDialog.h"
#include "entityDefineDelegate.h"

#include <QMessageBox>
#include <QInputDialog>

ByyEntityMappingWidget::ByyEntityMappingWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.pushButton_4->setIcon(QIcon("../data/icons/UtilityAdd.png"));
	ui.pushButton_5->setIcon(QIcon("../data/icons/UtilityMinus.png"));
	ui.pushButton_6->setIcon(QIcon("../data/icons/UtilityRename.png"));

	connect(ui.lineEdit_1,SIGNAL(textChanged(const QString&)),this,SLOT(entityFilter(const QString&)));
	connect(ui.pushButton_4,SIGNAL(clicked()),this,SLOT(addEntityDefine()));
	connect(ui.pushButton_5,SIGNAL(clicked()),this,SLOT(removeEntityDefine()));
	connect(ui.pushButton_6,SIGNAL(clicked()),this,SLOT(entityRename()));
	connect(ui.tableWidget_1,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(entityDefineChanged(QTableWidgetItem*)));

	ui.tableWidget_1->setItemDelegate(new ByyEntityDefineDelegate(this));
}

ByyEntityMappingWidget::~ByyEntityMappingWidget()
{

}

void ByyEntityMappingWidget::init()
{
	ui.tableWidget_1->blockSignals(true);

	while (ui.tableWidget_1->rowCount())
	{
		ui.tableWidget_1->removeRow(0);
	}

	QVector<EntityModelMapping> entityMappings=ByyMappingQuery::instance()->getEntityMappings();

	for (int i=0;i!=entityMappings.size();++i)
	{
		ui.tableWidget_1->insertRow(i);
		QTableWidgetItem *item=new QTableWidgetItem(entityMappings[i].entityType.toString());
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
		item->setData(Qt::UserRole,entityMappings[i].id);
		ui.tableWidget_1->setItem(i,0,item);
		item=new QTableWidgetItem(entityMappings[i].modelFilepath);
		item->setData(Qt::UserRole,entityMappings[i].modelId);
		ui.tableWidget_1->setItem(i,1,item);
		item=new QTableWidgetItem();
		item->setText(entityMappings[i].junBiaoId.toString());
		item->setData(Qt::UserRole,entityMappings[i].junBiaoId.libId);
		item->setData(Qt::UserRole+1,entityMappings[i].junBiaoId.markId);
		ui.tableWidget_1->setItem(i,2,item);
	}

	ui.tableWidget_1->resizeColumnsToContents();
	ui.tableWidget_1->resizeRowsToContents();
	ui.tableWidget_1->sortItems(0);
	ui.tableWidget_1->blockSignals(false);
}

void ByyEntityMappingWidget::entityFilter( const QString& text )
{
	for (int i=0;i!=ui.tableWidget_1->rowCount();++i)
	{
		ui.tableWidget_1->setRowHidden(i,text.isEmpty()?false:!ui.tableWidget_1->item(i,0)->text().toLower().contains(text.toLower()));
	}
}

void ByyEntityMappingWidget::addEntityDefine()
{
	ByyEntityDefineAddDialog dlg;
	if (dlg.exec()!=QDialog::Accepted)
	{
		return;
	}

	EntityType type(dlg.getEntityType());
	int modelId=dlg.getModelId();

	if (ByyMappingQuery::instance()->findEntityType(type))
	{
		QMessageBox::warning(this,QString::fromLocal8Bit("实体定义"),
			QString::fromLocal8Bit("该实体类型已经存在！"));
		return;
	}

	int id=ByyMappingQuery::instance()->addEntityType(type,modelId);

	int row=ui.tableWidget_1->rowCount();
	ui.tableWidget_1->insertRow(row);
	QTableWidgetItem *item=new QTableWidgetItem(dlg.getEntityType());
	item->setFlags(item->flags()&~Qt::ItemIsEditable);
	item->setData(Qt::UserRole,id);
	ui.tableWidget_1->setItem(row,0,item);
	item=new QTableWidgetItem(dlg.getModelName());
	item->setData(Qt::UserRole,modelId);
	ui.tableWidget_1->setItem(row,1,item);
	ui.tableWidget_1->sortItems(0);
	//emit modified();
}

void ByyEntityMappingWidget::removeEntityDefine()
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
		ByyMappingQuery::instance()->removeEntityType(typeItem->data(Qt::UserRole).toInt());
		ui.tableWidget_1->removeRow(row);
		//emit modified();
	}
}

void ByyEntityMappingWidget::entityRename()
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

	if (ByyMappingQuery::instance()->findEntityType(EntityType(name)))
	{
		QMessageBox::warning(this,QString::fromLocal8Bit("实体定义"),
			QString::fromLocal8Bit("该类型已经存在！"));
		return;
	}

	ByyMappingQuery::instance()->reEntityType(current->data(Qt::UserRole).toInt(),EntityType(name));

	current->setText(name);

	//emit modified();
}

void ByyEntityMappingWidget::entityDefineChanged( QTableWidgetItem* item )
{
	if (item->column()==1)
	{
		QString name=ui.tableWidget_1->item(item->row(),0)->text();

		
		//emit modified();
	}
	else if (item->column()==2)
	{
		JunBiaoId junbiaoId;
		junbiaoId.libId=item->data(Qt::UserRole).toInt();
		junbiaoId.markId=item->data(Qt::UserRole+1).toInt();
		ByyMappingQuery::instance()->reEntityJunBiao(ui.tableWidget_1->item(item->row(),0)->data(Qt::UserRole).toInt(),junbiaoId);
	}

	
}

void ByyEntityMappingWidget::showEvent( QShowEvent *e )
{
	init();
	QWidget::showEvent(e);
}