#include "StdAfx.h"
#include "ByyEntityPlotWidget.h"
#include "ByyApp.h"
#include "ByyHdoseEntity.h"
#include "plot/ByyPlot.h"
#include "plot/signaldata.h"

#define AttributeName Qt::UserRole+1
#define AttributeIndex Qt::UserRole+2

ByyEntityPlotWidget::ByyEntityPlotWidget( ByyApp& app,ByyEntityObject* entity/*=0*/,QWidget* parent/*=0*/,Qt::WindowFlags f /*= 0*/ ) 
	: ByyEntityObserver(app.core(),entity,parent,f)
	,myApp(app)
{
	ui.setupUi(this);
	ui.yAttributeComboBox->addItem(QString::fromLocal8Bit("Ê±¼ä"));
	ui.xAttributeComboBox->setModel(ui.yAttributeComboBox->model());

	if (entity)
	{
		setWindowTitle(entity->name());
	}

	myHdoseEntity=dynamic_cast<ByyHdoseEntity*>(myCurrentEntity);

	myHdoseEntity->generateAttributeInfo();

	initAttribute();

	connect(ui.addAttrButton,SIGNAL(clicked()),this,SLOT(showAttributeCurve()));
	connect(ui.tabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(onTabCloseRequested(int)));

	connect(&myApp.core(),SIGNAL(postTick()),this,SLOT(tick()));

	myIntervalTimer.start();
}

ByyEntityPlotWidget::~ByyEntityPlotWidget()
{
	for (int i=0;i!=myPlotDataList.size();++i)
	{
		delete myPlotDataList[i].signalData;
	}
}

void ByyEntityPlotWidget::initAttribute()
{
	if (!myHdoseEntity)
		return;

	ByyHdoseEntity::AttributeInfoList& attrInfoList=myHdoseEntity->attributeInfoList();

	ByyHdoseEntity::AttributeInfoList::iterator it=attrInfoList.begin();

	int index=-1;

	for (;it!=attrInfoList.end();++it)
	{
		switch(it->myType)
		{
		case ByyHdoseEntity::AttributeInfo_st::t_double:
		case ByyHdoseEntity::AttributeInfo_st::t_float:
			ui.yAttributeComboBox->addItem(it.key(),it->myType);
			index=ui.yAttributeComboBox->count()-1;
			ui.yAttributeComboBox->setItemData(index,it.key(),AttributeName);
			break;
		case ByyHdoseEntity::AttributeInfo_st::t_GeoPt:
			ui.yAttributeComboBox->addItem(it.key()+"_Lat",it->myType);
			index=ui.yAttributeComboBox->count()-1;
			ui.yAttributeComboBox->setItemData(index,it.key(),AttributeName);
			ui.yAttributeComboBox->setItemData(index,0,AttributeIndex);

			ui.yAttributeComboBox->addItem(it.key()+"_Long",it->myType);
			index=ui.yAttributeComboBox->count()-1;
			ui.yAttributeComboBox->setItemData(index,it.key(),AttributeName);
			ui.yAttributeComboBox->setItemData(index,1,AttributeIndex);

			ui.yAttributeComboBox->addItem(it.key()+"_Alt",it->myType);
			index=ui.yAttributeComboBox->count()-1;
			ui.yAttributeComboBox->setItemData(index,it.key(),AttributeName);
			ui.yAttributeComboBox->setItemData(index,2,AttributeIndex);

			break;
		case ByyHdoseEntity::AttributeInfo_st::t_Vec3:
			ui.yAttributeComboBox->addItem(it.key()+"[0]",it->myType);
			index=ui.yAttributeComboBox->count()-1;
			ui.yAttributeComboBox->setItemData(index,it.key(),AttributeName);
			ui.yAttributeComboBox->setItemData(index,0,AttributeIndex);

			ui.yAttributeComboBox->addItem(it.key()+"[1]",it->myType);
			index=ui.yAttributeComboBox->count()-1;
			ui.yAttributeComboBox->setItemData(index,it.key(),AttributeName);
			ui.yAttributeComboBox->setItemData(index,1,AttributeIndex);

			ui.yAttributeComboBox->addItem(it.key()+"[2]",it->myType);
			index=ui.yAttributeComboBox->count()-1;
			ui.yAttributeComboBox->setItemData(index,it.key(),AttributeName);
			ui.yAttributeComboBox->setItemData(index,2,AttributeIndex);
			break;
		}
	}

	ui.addAttrButton->setEnabled(ui.yAttributeComboBox->count()>1);

	if (ui.yAttributeComboBox->count()>1)
	{
		ui.yAttributeComboBox->setCurrentIndex(1);
	}
}

void updateAxisInfo(ByyEntityPlotWidget::PlotData_st::Axis_st& axis,QComboBox* comboBox)
{
	int index=comboBox->currentIndex();

	if (index==0)
	{
		axis.isTime=true;
		axis.attribute=comboBox->currentText();
	}
	else
	{
		axis.isTime=false;
		axis.attribute=comboBox->itemData(index,AttributeName).toString();

		axis.type=comboBox->itemData(index).toInt();

		if (axis.type==ByyHdoseEntity::AttributeInfo_st::t_GeoPt||
			axis.type==ByyHdoseEntity::AttributeInfo_st::t_Vec3)
		{
			axis.index=comboBox->itemData(index,AttributeIndex).toInt();
		}
		else
			axis.index=-1;
	}
}

void ByyEntityPlotWidget::showAttributeCurve()
{
	
	PlotData_st pd_st;

	pd_st.plot=new ByyPlot(this);
	pd_st.signalData=new SignalData;

	updateAxisInfo(pd_st.x_Axis,ui.xAttributeComboBox);
	updateAxisInfo(pd_st.y_Axis,ui.yAttributeComboBox);
	

	QString curveName=ui.xAttributeComboBox->currentText()+"-"+ui.yAttributeComboBox->currentText();

	pd_st.plot->addCurve(curveName,*pd_st.signalData,QPen(Qt::green,3));

	myPlotDataList.append(pd_st);

	ui.tabWidget->addTab(pd_st.plot,curveName);

	ui.tabWidget->setCurrentIndex(ui.tabWidget->count()-1);

	pd_st.plot->setAxisTitle(QwtPlot::xBottom,ui.xAttributeComboBox->currentText());
	pd_st.plot->setAxisTitle(QwtPlot::yLeft,ui.yAttributeComboBox->currentText());
}

void ByyEntityPlotWidget::onTabCloseRequested( int index )
{
	PlotData_st& pd_st=myPlotDataList[index];

	ui.tabWidget->removeTab(index);

	delete pd_st.signalData;
	delete pd_st.plot;
	
	myPlotDataList.removeAt(index);
}

double axisValue(ByyEntityPlotWidget::PlotData_st::Axis_st& axis,ByyHdoseEntity* myHdoseEntity)
{
	double rel=0;

	switch(axis.type)
	{
	case ByyHdoseEntity::AttributeInfo_st::t_GeoPt:
		rel= myHdoseEntity->getGeoPtValue(axis.attribute,axis.index);
		break;
	case ByyHdoseEntity::AttributeInfo_st::t_Vec3:
		rel= myHdoseEntity->getVec3Value(axis.attribute,axis.index);
		break;
	default:
		rel= myHdoseEntity->getAttributeValue(axis.attribute);
	}

	return rel;
}

void ByyEntityPlotWidget::tick()
{
	if (!myHdoseEntity||myIntervalTimer.elapsed()<200)
		return;

	double simTime=myApp.hdose().simTime();

	for (int i=0;i!=myPlotDataList.size();++i)
	{
		PlotData_st& pd_st=myPlotDataList[i];

		QPointF pt;

		pt.setX(pd_st.x_Axis.isTime?simTime:axisValue(pd_st.x_Axis,myHdoseEntity));

		pt.setY(pd_st.y_Axis.isTime?simTime:axisValue(pd_st.y_Axis,myHdoseEntity));
		

		pd_st.signalData->append(pt);

		pd_st.plot->tick(simTime);
	}

	myIntervalTimer.restart();
}

void ByyEntityPlotWidget::setEntity( ByyEntityObject *entity )
{
	if (entity==0)
	{
		myHdoseEntity=0;
		disconnect(&myApp.core(),SIGNAL(postTick()),this,SLOT(tick()));
		ui.addAttrButton->setEnabled(false);
	}
}
