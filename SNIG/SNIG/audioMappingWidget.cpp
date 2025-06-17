#include "stdafx.h"
#include "audioMappingWidget.h"
#include "audioAddDialog.h"
#include <abstractMapping.h>

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

void ByyAudioMappingWidget::init( ByyAbstractMapping* m ,const QString& label)
{
	mapping=m;

	const QHash<QString,QString>& _data=mapping->getMappingData();

	QHash<QString,QString>::const_iterator it=_data.begin();

	int row;
	QTableWidgetItem *item;

	for (;it!=_data.end();++it)
	{
		row=ui.tableWidget->rowCount();
		ui.tableWidget->insertRow(row);
		item=new QTableWidgetItem(it.key());

		ui.tableWidget->setItem(row,0,item);
		item=new QTableWidgetItem(it.value());
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
	
	if (mapping->find(dlg.getEntityType()))
	{
		QMessageBox::warning(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("该实体类型已经存在！"));
		return;
	}

	int row=ui.tableWidget->rowCount();
	ui.tableWidget->insertRow(row);

	QTableWidgetItem *item=new QTableWidgetItem(dlg.getEntityType());

	ui.tableWidget->setItem(row,0,item);
	item=new QTableWidgetItem(dlg.getFileName());
	ui.tableWidget->setItem(row,1,item);

	mapping->add(dlg.getEntityType(),dlg.getFileName());
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

	mapping->remove(ui.tableWidget->item(row,0)->text());
	ui.tableWidget->removeRow(row);
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
			if (mapping->find(dlg.getEntityType()))
			{
				QMessageBox::warning(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("该实体类型已经存在！"));
				return;
			}
			mapping->rekey(typeItem->text(),dlg.getEntityType());

			typeItem->setText(dlg.getEntityType());
		}

		if (fileItem->text()!=dlg.getFileName())
		{
			mapping->revalue(dlg.getEntityType(),dlg.getFileName());
			fileItem->setText(dlg.getFileName());
		}
	}
}
