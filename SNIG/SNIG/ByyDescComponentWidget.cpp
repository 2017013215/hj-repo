#include "StdAfx.h"
#include "ByyDescComponentWidget.h"
#include "ByyDescribeComponent.h"
#include "ByyDescParamList.h"

ByyDescComponentWidget::ByyDescComponentWidget(const ByyDescribeComponent& component,QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	const ByyDescParamList& paramList=component.getParamList();

	for (int i=0;i!=paramList.count();++i)
	{
		int row=i+1;
		ui.tableWidget->insertRow(row);

		QTableWidgetItem *item=new QTableWidgetItem;
		item->setText(paramList.name(i));
		item->setTextAlignment(Qt::AlignCenter);

		if (row%2!=0)
		{
			item->setBackground(Qt::SolidPattern);
			item->setBackgroundColor(QColor(249,249,249));
		}

		ui.tableWidget->setItem(row,0,item);

		item=new QTableWidgetItem;
		item->setText(paramList.value(i));
		item->setTextAlignment(Qt::AlignCenter);

		if (row%2!=0)
		{
			item->setBackground(Qt::SolidPattern);
			item->setBackgroundColor(QColor(249,249,249));
		}

		ui.tableWidget->setItem(row,1,item);
	}
}

ByyDescComponentWidget::~ByyDescComponentWidget()
{

}
