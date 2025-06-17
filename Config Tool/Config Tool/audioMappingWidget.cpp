#include "audioMappingWidget.h"
#include "audioAddDialog.h"
#include "mappingQuery.h"
#include <QMessageBox>
ByyAudioMappingWidget::ByyAudioMappingWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.pushButton->setIcon(QIcon("../data/icons/UtilityAdd.png"));
	ui.pushButton_2->setIcon(QIcon("../data/icons/UtilityMinus.png"));
	ui.pushButton_3->setIcon(QIcon("../data/icons/UtilityRename.png"));

	connect(ui.pushButton,SIGNAL(clicked()),SLOT(add()));
	connect(ui.pushButton_2,SIGNAL(clicked()),SLOT(remove()));
	connect(ui.pushButton_3,SIGNAL(clicked()),SLOT(edit()));
}

ByyAudioMappingWidget::~ByyAudioMappingWidget()
{

}

void ByyAudioMappingWidget::init( const QString& table,const QString& label )
{
	d_table=table;

	QVector<AudioMapping> mappings=ByyMappingQuery::instance()->getAudioMappings(table);

	for (int i=0;i!=mappings.size();++i)
	{
		int row=ui.tableWidget->rowCount();
		ui.tableWidget->insertRow(row);
		QTableWidgetItem *item=new QTableWidgetItem(mappings[i].entityType.toString());
		item->setData(Qt::UserRole,mappings[i].id);

		ui.tableWidget->setItem(row,0,item);
		item=new QTableWidgetItem(mappings[i].modelFilepath);
		ui.tableWidget->setItem(row,1,item);
	}

	ui.label->setText(QString("<span style=\"font-weight:bold\">")+label+"</span>");
}

void ByyAudioMappingWidget::add()
{
	ByyAudioAddDialog dlg;

	if (dlg.exec()!=QDialog::Accepted)
	{
		return;
	}
	
	if (ByyMappingQuery::instance()->findAudioMapping(EntityType(dlg.getEntityType()),d_table))
	{
		QMessageBox::warning(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("该实体类型已经存在！"));
		return;
	}

	int id=ByyMappingQuery::instance()->addAudioMapping(EntityType(dlg.getEntityType()),dlg.getFileName(),d_table);

	int row=ui.tableWidget->rowCount();
	ui.tableWidget->insertRow(row);

	QTableWidgetItem *item=new QTableWidgetItem(dlg.getEntityType());
	item->setData(Qt::UserRole,id);
	ui.tableWidget->setItem(row,0,item);
	item=new QTableWidgetItem(dlg.getFileName());
	ui.tableWidget->setItem(row,1,item);

	
	//emit modified();
}

void ByyAudioMappingWidget::remove()
{
	int row=ui.tableWidget->currentRow();

	if (row==-1)
	{
		return;
	}

	int r=QMessageBox::warning(this,QString::fromLocal8Bit("提示"),
		QString::fromLocal8Bit("确认要删除\"%1\"吗？").arg(ui.tableWidget->item(row,0)->text()),QMessageBox::Yes|QMessageBox::No);

	if (r!=QMessageBox::Yes)
	{
		return;
	}

	ByyMappingQuery::instance()->removeAudioMapping(ui.tableWidget->item(row,0)->data(Qt::UserRole).toInt(),d_table);

	ui.tableWidget->removeRow(row);
	//emit modified();
}

void ByyAudioMappingWidget::edit()
{
	int row=ui.tableWidget->currentRow();

	if (row==-1)
	{
		return;
	}

	QTableWidgetItem *typeItem=ui.tableWidget->item(row,0);
	QTableWidgetItem *fileItem=ui.tableWidget->item(row,1);

	ByyAudioAddDialog dlg(typeItem->text(),fileItem->text());

	if (dlg.exec()==QDialog::Accepted)
	{
		if (typeItem->text()!=dlg.getEntityType())
		{
			if (ByyMappingQuery::instance()->findAudioMapping(EntityType(dlg.getEntityType()),d_table))
			{
				QMessageBox::warning(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("该实体类型已经存在！"));
				return;
			}
			ByyMappingQuery::instance()->reAudioType(typeItem->data(Qt::UserRole).toInt(),EntityType(dlg.getEntityType()),d_table);

			typeItem->setText(dlg.getEntityType());
		}

		if (fileItem->text()!=dlg.getFileName())
		{
			ByyMappingQuery::instance()->reAudioFile(typeItem->data(Qt::UserRole).toInt(),dlg.getFileName(),d_table);

			fileItem->setText(dlg.getFileName());
		}
		//emit modified();
	}
}
