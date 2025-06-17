#include "StdAfx.h"
#include "tacticalGraphicsWidget.h"
#include "ByyRoute.h"
#include "ByyPolygon.h"
#include "ByyEllipse.h"
#include "ByyWaypoint.h"
#include "ByyPhaseLine.h"
#include "ByyRectangle.h"
#include "ByyCircle.h"
#include "ByyFineArrow.h"
#include "ByyDoubleArrow.h"
#include "ByyGatheringPlace.h"
#include "ByyAttackArrow.h"
#include "ByyStraightArrow.h"
#include "ByyApp.h"
#include "ByyHdose.h"
#include "tacticalGraphicsProcesser.h"
#include "scenarioManager.h"
#include "ByyArc.h"

ByyTacticalGraphicsWidget::ByyTacticalGraphicsWidget( ByyApp& app,QWidget *parent /*= 0*/ ) 
	: QWidget(parent)
	,myApp(app)
{
	ui.setupUi(this);

	ui.forceComboBox->addItem(QString::fromLocal8Bit("红方"),ByyForceFriendly);
	ui.forceComboBox->addItem(QString::fromLocal8Bit("蓝方"),ByyForceOpposing);
	ui.forceComboBox->addItem(QString::fromLocal8Bit("中立"),ByyForceNeutral);

	QTreeWidgetItem *item=0;

	{
		item=new QTreeWidgetItem;
		item->setText(0,ByyWaypoint::theType());
		item->setIcon(0,QIcon("../data/icons/placemark32.png"));
		ui.treeWidget->addTopLevelItem(item);
	}
	{
		item=new QTreeWidgetItem;
		item->setText(0,ByyRoute::theType());
		item->setIcon(0,QIcon("../data/icons/path.png"));
		ui.treeWidget->addTopLevelItem(item);
	}
	{
		item=new QTreeWidgetItem;
		item->setText(0,ByyPolygon::theType());
		item->setIcon(0,QIcon("../data/icons/area.png"));
		ui.treeWidget->addTopLevelItem(item);
	}
	{
		item=new QTreeWidgetItem;
		item->setText(0,ByyEllipse::theType());
		item->setIcon(0,QIcon("../data/icons/ellipse.png"));
		ui.treeWidget->addTopLevelItem(item);
	}
	//{
	//	item=new QTreeWidgetItem;
	//	item->setText(0,ByyPhaseLine::theType());
	//	item->setIcon(0,QIcon("../data/icons/path.png"));
	//	ui.treeWidget->addTopLevelItem(item);
	//}
	{
		item=new QTreeWidgetItem;
		item->setText(0,ByyRectangle::theType());
		item->setIcon(0,QIcon("../data/icons/area.png"));
		ui.treeWidget->addTopLevelItem(item);
	}
	{
		item=new QTreeWidgetItem;
		item->setText(0,ByyCircle::theType());
		item->setIcon(0,QIcon("../data/icons/circle.png"));
		ui.treeWidget->addTopLevelItem(item);
	}
	{
		item=new QTreeWidgetItem;
		item->setText(0,ByyArc::theType());
		item->setIcon(0,QIcon("../data/icons/huxing.png"));
		ui.treeWidget->addTopLevelItem(item);
	}
	{
		item=new QTreeWidgetItem;
		item->setText(0,ByyFineArrow::theType());
		item->setIcon(0,QIcon("../data/icons/FineArrow.png"));
		ui.treeWidget->addTopLevelItem(item);
	}
	{
		item=new QTreeWidgetItem;
		item->setText(0,ByyDoubleArrow::theType());
		item->setIcon(0,QIcon("../data/icons/DoubleArrow.png"));
		ui.treeWidget->addTopLevelItem(item);
	}
	{
		item=new QTreeWidgetItem;
		item->setText(0,ByyGatheringPlace::theType());
		item->setIcon(0,QIcon("../data/icons/jujiqu.png"));
		ui.treeWidget->addTopLevelItem(item);
	}
	{
		item=new QTreeWidgetItem;
		item->setText(0,ByyAttackArrow::theType());
		item->setIcon(0,QIcon("../data/icons/jingongfangxiang.png"));
		ui.treeWidget->addTopLevelItem(item);
	}
	{
		item=new QTreeWidgetItem;
		item->setText(0,ByyStraightArrow::theType());
		item->setIcon(0,QIcon("../data/icons/zhijiantou.png"));
		ui.treeWidget->addTopLevelItem(item);
	}

	connect(ui.treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(onItemClicked(QTreeWidgetItem*,int)));
	connect(ui.searchEdit,SIGNAL(textChanged(const QString&)),SLOT(filter()));

	checkEnabled();

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
	connect(&myApp.hdose(),SIGNAL(signal_runningChanged()),SLOT(checkEnabled()));

	connect(&myApp.ig().tacticalGraphicsProcesser(),SIGNAL(graphicsCreated(ByyControlObject*)),SLOT(onGraphicsCreated(ByyControlObject*)));
}

ByyTacticalGraphicsWidget::~ByyTacticalGraphicsWidget()
{

}

void ByyTacticalGraphicsWidget::onItemClicked( QTreeWidgetItem *item,int column )
{
	QString type=item->text(0);

	myCreatingType=(ByyForceType)ui.forceComboBox->itemData(ui.forceComboBox->currentIndex()).toInt();

	myApp.ig().tacticalGraphicsProcesser().createCreator(type);
}

void ByyTacticalGraphicsWidget::checkEnabled()
{
	bool enabled=false;
	if(myApp.hdose().started())
	{
		enabled=true;
	}
	else
		enabled=myApp.hdose().scenarioManager().scenario();
	ui.treeWidget->setEnabled(enabled);
}

void ByyTacticalGraphicsWidget::filter()
{
	QTreeWidgetItem *item=0;

	for (int i=0;i!=ui.treeWidget->topLevelItemCount();++i)
	{
		bool _show=true;

		item=ui.treeWidget->topLevelItem(i);

		if (_show&&!ui.searchEdit->text().isEmpty())
		{
			_show=item->text(0).toLower().contains(ui.searchEdit->text().toLower());
		}

		item->setHidden(!_show);
	}
}

void ByyTacticalGraphicsWidget::onGraphicsCreated( ByyControlObject* obj )
{
	myApp.hdose().appendControlObject(obj,myCreatingType);
}
