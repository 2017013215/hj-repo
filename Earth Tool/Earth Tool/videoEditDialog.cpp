#include "stdafx.h"
#include "videoEditDialog.h"
#include "model.h"

ByyVideoEditDialog::ByyVideoEditDialog( QWidget *parent /*= 0*/ ) : QDialog(parent)
{
	ui.setupUi(this);
	ui.pushButton_4->setIcon(QIcon("../data/icons/UtilityAdd.png"));
	ui.pushButton_5->setIcon(QIcon("../data/icons/UtilityMinus.png"));

	connect(ui.pushButton_4,SIGNAL(clicked()),SLOT(addVideo()));
	connect(ui.pushButton_5,SIGNAL(clicked()),SLOT(removeVideo()));

	connect(ui.tableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),SLOT(itemChanged(QTableWidgetItem*)));
}

ByyVideoEditDialog::~ByyVideoEditDialog()
{

}

void ByyVideoEditDialog::setCurrentModel( ByyModel* model )
{
	myModel=model;

	ui.tableWidget->blockSignals(true);

	while (ui.tableWidget->rowCount())
	{
		ui.tableWidget->removeRow(0);
	}

	QVector<VideoInfo>& infos=model->getVideoInfos();

	for (int i=0;i!=infos.size();++i)
	{
		ui.tableWidget->insertRow(i);
		data2Row(i,infos[i]);
	}

	ui.tableWidget->blockSignals(false);
}

void ByyVideoEditDialog::addVideo()
{
	VideoInfo newVideoInfo;

	int row=ui.tableWidget->rowCount();
	newVideoInfo.name=QString("Video %1").arg(row);
	newVideoInfo.url="189.125.117.10";
	newVideoInfo.port=8000;
	newVideoInfo.accout="admin";
	newVideoInfo.password="12345";

	ui.tableWidget->insertRow(row);

	myModel->getVideoInfos().push_back(newVideoInfo);

	data2Row(row,newVideoInfo);

	emit modified();
}

void ByyVideoEditDialog::removeVideo()
{
	int row=ui.tableWidget->currentRow();

	if (row==-1)
	{
		return;
	}

// 	int r=QMessageBox::warning(this,QString::fromLocal8Bit("提示"),
// 		QString::fromLocal8Bit("确认删除吗？"),QMessageBox::Yes|QMessageBox::No);

	ui.tableWidget->removeRow(row);

	myModel->getVideoInfos().remove(row);

	emit modified();
}

void ByyVideoEditDialog::row2Data( int row,VideoInfo& vi )
{
	ui.tableWidget->blockSignals(true);
	if (row>=0&&row<ui.tableWidget->rowCount())
	{
		vi.name=ui.tableWidget->item(row,0)->text();
		vi.url=ui.tableWidget->item(row,1)->text();
		vi.port=ui.tableWidget->item(row,2)->text().toInt();
		vi.accout=ui.tableWidget->item(row,3)->text();
		vi.password=ui.tableWidget->item(row,4)->text();
		vi.allCamera=ui.tableWidget->item(row,5)->text().isEmpty();
		vi.cameras=ui.tableWidget->item(row,5)->text();
	}
	ui.tableWidget->blockSignals(false);
}

void ByyVideoEditDialog::data2Row( int row,VideoInfo& vi )
{
	if (row>=0&&row<ui.tableWidget->rowCount())
	{
		for (int i=0;i!=6;++i)
		{
			if (ui.tableWidget->item(row,i)==NULL)
			{
				ui.tableWidget->setItem(row,i,new QTableWidgetItem);
			}
		}

		ui.tableWidget->item(row,0)->setText(vi.name);
		ui.tableWidget->item(row,1)->setText(vi.url);
		ui.tableWidget->item(row,2)->setText(QString::number(vi.port));
		ui.tableWidget->item(row,3)->setText(vi.accout);
		ui.tableWidget->item(row,4)->setText(vi.password);
		ui.tableWidget->item(row,5)->setText(vi.cameras);
	}
}

void ByyVideoEditDialog::itemChanged( QTableWidgetItem * item )
{
	int row=item->row();

	VideoInfo& videoInfo=myModel->getVideoInfos()[row];

	switch(item->column())
	{
	case 0:
		videoInfo.name=item->text();
		break;
	case 1:
		videoInfo.url=item->text();
		break;
	case 2:
		videoInfo.port=item->text().toInt();
		break;
	case 3:
		videoInfo.accout=item->text();
		break;
	case 4:
		videoInfo.password=item->text();
		break;
	case 5:
		videoInfo.cameras=item->text();
		break;
	}

	emit modified();
}
