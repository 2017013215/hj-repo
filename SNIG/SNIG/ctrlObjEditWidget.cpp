#include "StdAfx.h"
#include "ctrlObjEditWidget.h"
#include "localizedCtrlObject.h"

ByyCtrlObjEditWidget::ByyCtrlObjEditWidget( ByyControlObject* ctrlObj,QWidget *parent /*= 0*/ ) 
	: QWidget(parent)
	,myCtrlObj(ctrlObj)
{
	ui.setupUi(this);
	ui.tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

	ui.nameEdit->setText(myCtrlObj->name());
	ui.sideComboBox->setCurrentIndex(myCtrlObj->forceType());
	ui.checkBox->setChecked(myCtrlObj->clampToTerrain());

	//setPointsToTableWidget(ctrlObj->points());

	{
		ByyPointSet* pointSetObj = dynamic_cast<ByyPointSet*>(ctrlObj);
		if (pointSetObj != NULL)
		{
			setPointsToTableWidget(pointSetObj->points(),pointSetObj->speedS(),pointSetObj->bCrossS(),pointSetObj->waitTimeS(), pointSetObj->bTimeCtrlS(), pointSetObj->timeArriveS(), pointSetObj->iRoundS(), pointSetObj->rSurroundS());
		}
	}
	{
		ByyLocalizedCtrlObject* pointSetObj = dynamic_cast<ByyLocalizedCtrlObject*>(ctrlObj);
		if (pointSetObj != NULL)
		{
			setPointsToTableWidget(pointSetObj->points(),pointSetObj->speedS(),pointSetObj->bCrossS(),pointSetObj->waitTimeS(), pointSetObj->bTimeCtrlS(), pointSetObj->timeArriveS(), pointSetObj->iRoundS(), pointSetObj->rSurroundS());
		}
	}

	QObject::connect(ui.updateButton, SIGNAL(clicked()), this, SLOT(updateObj()));
	QObject::connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(close()));
}

ByyCtrlObjEditWidget::~ByyCtrlObjEditWidget()
{

}

void ByyCtrlObjEditWidget::updateObj()
{
	myCtrlObj->setName(ui.nameEdit->text());
	myCtrlObj->setForceType((ByyForceType)ui.sideComboBox->currentIndex());
	myCtrlObj->setPoints(tableWidgetToPoints());
	myCtrlObj->setClampToTerrain(ui.checkBox->isChecked());
	//ByyRoute* routeCtrlObj = dynamic_cast<ByyRoute*>(myCtrlObj);
	{
		ByyPointSet* routeCtrlObj = dynamic_cast<ByyPointSet*>(myCtrlObj);
		if (routeCtrlObj != NULL)
		{
			routeCtrlObj->setSpeedS(tableWidetToDoubleArray(3));
			routeCtrlObj->setBCrossS(tableWidetToDoubleArray(4));
			routeCtrlObj->setWaitTimeS(tableWidetToDoubleArray(5));
			routeCtrlObj->setBTimeCtrlS(tableWidetToDoubleArray(6));
			routeCtrlObj->setTimeArriveS(tableWidetToDoubleArray(7));
			routeCtrlObj->setIRoundS(tableWidetToDoubleArray(8));
			routeCtrlObj->setRSurroundS(tableWidetToDoubleArray(9));
		}
	}
	{
		ByyLocalizedCtrlObject* routeCtrlObj = dynamic_cast<ByyLocalizedCtrlObject*>(myCtrlObj);
		if (routeCtrlObj != NULL)
		{
			routeCtrlObj->setSpeedS(tableWidetToDoubleArray(3));
			routeCtrlObj->setBCrossS(tableWidetToDoubleArray(4));
			routeCtrlObj->setWaitTimeS(tableWidetToDoubleArray(5));
			routeCtrlObj->setBTimeCtrlS(tableWidetToDoubleArray(6));
			routeCtrlObj->setTimeArriveS(tableWidetToDoubleArray(7));
			routeCtrlObj->setIRoundS(tableWidetToDoubleArray(8));
			routeCtrlObj->setRSurroundS(tableWidetToDoubleArray(9));
		}
	}
	close();
}

void ByyCtrlObjEditWidget::setPointsToTableWidget( const ByyVec3dArray& points ,const ByyDoubleArray& speedS, const ByyDoubleArray& BCrossS, const ByyDoubleArray& WaitTimeS, const ByyDoubleArray& BTimeCtrlS, const ByyDoubleArray& TimeArriveS,  const ByyDoubleArray& IRound, const ByyDoubleArray& RSurround )
{
	//ui.tableWidget->clear();
	ui.tableWidget->clearContents();

	for (int i=0;i!=points.size();++i)
	{
		int row=ui.tableWidget->rowCount();
		int colum=ui.tableWidget->columnCount();

		ui.tableWidget->insertRow(row);

		for(int j=0;j!=3;++j)
		{
			QTableWidgetItem *item=new QTableWidgetItem(QString::number(points[i][j],'f',j<2?10:2));

			ui.tableWidget->setItem(row,j,item);
		}

		// 速度(m/s)
		QTableWidgetItem *item=new QTableWidgetItem(QString::number(speedS[i],'f',3));
		ui.tableWidget->setItem(row,3,item);
		// 过点类型(0-压点；1-旁切；2-左绕飞；3-右绕飞)
		item=new QTableWidgetItem(QString::number(BCrossS[i]));
		ui.tableWidget->setItem(row,4,item);
		// 停留时间(s)
		item=new QTableWidgetItem(QString::number(WaitTimeS[i],'f',3));
		ui.tableWidget->setItem(row,5,item);
		// 到达用时控制(0/1)
		item=new QTableWidgetItem(QString::number(BTimeCtrlS[i]));
		ui.tableWidget->setItem(row,6,item);
		// 到达用时(s)
		item=new QTableWidgetItem(QString::number(TimeArriveS[i],'f',3));
		ui.tableWidget->setItem(row,7,item);
		// 到绕飞圈数
		item=new QTableWidgetItem(QString::number(IRound[i],'f',1));
		ui.tableWidget->setItem(row,8,item);
		// 绕飞半径
		item=new QTableWidgetItem(QString::number(RSurround[i],'f',3));
		ui.tableWidget->setItem(row,9,item);
	}
}

void ByyCtrlObjEditWidget::setPointsToTableWidgetComm( const ByyVec3dArray& points)
{
	ui.tableWidget->clearContents();

	for (int i=0;i!=points.size();++i)
	{
		int row=ui.tableWidget->rowCount();

		ui.tableWidget->insertRow(row);

		for(int j=0;j!=3;++j)
		{
			QTableWidgetItem *item=new QTableWidgetItem(QString::number(points[i][j],'f',j<2?10:2));

			ui.tableWidget->setItem(row,j,item);
		}
	}
}
ByyVec3dArray ByyCtrlObjEditWidget::tableWidgetToPoints()
{
	ByyVec3dArray result;
	result.resize(ui.tableWidget->rowCount());

	for (int i=0;i!=ui.tableWidget->rowCount();++i)
	{
		for (int j=0;j!=3;++j)
		{
			result[i][j]=ui.tableWidget->item(i,j)->text().toDouble();
		}
	}

	return result;
}

ByyDoubleArray ByyCtrlObjEditWidget::tableWidetToSpeedS()
{
	ByyDoubleArray result;
	result.resize(ui.tableWidget->rowCount());

	for (int i=0;i!=ui.tableWidget->rowCount();++i)
	{
		result[i]=ui.tableWidget->item(i,3)->text().toDouble();
	}

	return result;
}

ByyDoubleArray ByyCtrlObjEditWidget::tableWidetToDoubleArray(int column)
{
	ByyDoubleArray result;
	result.resize(ui.tableWidget->rowCount());

	for (int i=0;i!=ui.tableWidget->rowCount();++i)
	{
		result[i]=ui.tableWidget->item(i,column)->text().toDouble();
	}

	return result;
}
ByyDoubleArray ByyCtrlObjEditWidget::tableWidetToBCrossS()
{
	ByyDoubleArray result;
	result.resize(ui.tableWidget->rowCount());

	for (int i=0;i!=ui.tableWidget->rowCount();++i)
	{
		//result[i]=ui.tableWidget->item(i,4)->text().toDouble();
		QComboBox* box =dynamic_cast<QComboBox*>(ui.tableWidget->cellWidget(i,4));
		if(box)
		{
			result[i]= box->currentIndex();
		}
	}

	return result;
}
