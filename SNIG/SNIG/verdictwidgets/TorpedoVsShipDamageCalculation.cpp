#include "stdafx.h"
#include "TorpedoVsShipDamageCalculation.h" 

#include <QTableWidget>
#include <QTableWidgetItem>

TorpedoVsShipDamageCalculation::TorpedoVsShipDamageCalculation(VerdictBaseWidget* parent /*= nullptr*/)
	: VerdictBaseWidget(parent)
{
	createWidget();
	setupLayout();
	updateEntities();
	resetUi();
	startDataHandle(1000);
}

// 毁伤裁决(鱼雷对水面舰艇)
void TorpedoVsShipDamageCalculation::createButton()
{
	Button_TorpedoVsShipDamageCalculation = new QPushButton;
	Button_TorpedoVsShipDamageCalculation->setText(QString::fromLocal8Bit("确认修改"));
	Button_TorpedoVsShipDamageCalculation->setEnabled(false);
	connect(Button_TorpedoVsShipDamageCalculation, SIGNAL(clicked()), this, SLOT(slotBtnTvsShipDC()));
}

// 毁伤裁决(鱼雷对水面舰艇)
void TorpedoVsShipDamageCalculation::createChart()
{
	m_axisA_TvsShipDC = new QValueAxis;
	m_axisR_TvsShipDC = new QValueAxis;
	m_axisA_TvsShipDC->setTickCount(9);
	m_axisA_TvsShipDC->setRange(0,360);//角度范围
	//m_axisA->setLabelsAngle(180);
	m_axisA_TvsShipDC->setLabelFormat("%d");
	m_axisR_TvsShipDC->setTickCount(9);
	m_axisR_TvsShipDC->setRange(0,400);//距离范围
	//m_axisR->setLabelsAngle(180);
	m_axisR_TvsShipDC->setLabelFormat("%d");

	chartView_TorpedoVsShipDamageCalculation = new QChartView;
	ScatterSeries_TvsShipDC = new QScatterSeries;
	ScatterSeries_TvsShipDC->setName(QString::fromLocal8Bit("目标实体"));
	ScatterSeries_TvsShipDC->setPen(QPen(QColor(34,34,34)));

	PolarChart_TorpedoVsShipDamageCalculation = new QPolarChart();           // 创建一个极坐标系图表
	PolarChart_TorpedoVsShipDamageCalculation->legend()->setVisible(false);               // 隐藏图例
	PolarChart_TorpedoVsShipDamageCalculation->addSeries(ScatterSeries_TvsShipDC);                         // 将创建的series添加进图表中
	PolarChart_TorpedoVsShipDamageCalculation->addAxis(m_axisA_TvsShipDC,QPolarChart::PolarOrientationAngular);
	PolarChart_TorpedoVsShipDamageCalculation->addAxis(m_axisR_TvsShipDC,QPolarChart::PolarOrientationRadial);
	ScatterSeries_TvsShipDC->attachAxis(m_axisA_TvsShipDC);
	ScatterSeries_TvsShipDC->attachAxis(m_axisR_TvsShipDC);
	// 根据已有的series生成默认的坐标轴
	PolarChart_TorpedoVsShipDamageCalculation->setBackgroundVisible(false);
	chartView_TorpedoVsShipDamageCalculation->setChart(PolarChart_TorpedoVsShipDamageCalculation);
}


// 毁伤裁决(鱼雷对水面舰艇)
void TorpedoVsShipDamageCalculation::createComBox()
{
	ComboBox_TorpedoVsShipDamageCalculation = new QDoubleSpinBox;
	ComboBox_TorpedoVsShipDamageCalculation->setEnabled(true);
	ComboBox_TorpedoVsShipDamageCalculation->setValue(0.0);
	ComboBox_TorpedoVsShipDamageCalculation->setMaximum(100.0);
	ComboBox_TorpedoVsShipDamageCalculation->setMinimum(0.0);
	//connect(ComboBox_TorpedoVsShipDamageCalculation,SIGNAL(currentIndexChanged(int)),this,SLOT(slotComCDEntity()));
}

// 毁伤裁决(鱼雷对水面舰艇)
void TorpedoVsShipDamageCalculation::createTableView()
{
	QStringList HeaderList_TorpedoVsShipDamageCalculation;
	HeaderList_TorpedoVsShipDamageCalculation<<QString::fromLocal8Bit("命中时刻")<<QString::fromLocal8Bit("爆炸实体")<<QString::fromLocal8Bit("命中实体")<<QString::fromLocal8Bit("详情");
	// Model_TorpedoVsShipDamageCalculation = new QStandardItemModel;

	Table_TorpedoVsShipDamageCalculation = new QTableWidget;
	Table_TorpedoVsShipDamageCalculation->setColumnCount(HeaderList_TorpedoVsShipDamageCalculation.size());
	Table_TorpedoVsShipDamageCalculation->setFocusPolicy(Qt::NoFocus);
	Table_TorpedoVsShipDamageCalculation->setEditTriggers(QAbstractItemView::NoEditTriggers);
	Table_TorpedoVsShipDamageCalculation->setSelectionMode(QAbstractItemView::NoSelection);
	Table_TorpedoVsShipDamageCalculation->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	Table_TorpedoVsShipDamageCalculation->horizontalHeader()->setStyleSheet("QHeaderView::section{background:white;color: black;}");
	Table_TorpedoVsShipDamageCalculation->verticalHeader()->hide();
	for (int a=0;a<HeaderList_TorpedoVsShipDamageCalculation.size();a++) {
		Table_TorpedoVsShipDamageCalculation->setHorizontalHeaderItem(a,new QTableWidgetItem(HeaderList_TorpedoVsShipDamageCalculation.at(a)));
	}
	Table_TorpedoVsShipDamageCalculation->verticalHeader()->setDefaultSectionSize(30);
}

void TorpedoVsShipDamageCalculation::createLabel()
{
	Label_TorpedoVsShipDamageCalculation_Distance = new QLabel(QString::fromLocal8Bit("0"));
	Label_TorpedoVsShipDamageCalculation_Position = new QLabel(QString::fromLocal8Bit("0"));
	Label_TorpedoVsShipDamageCalculation_Height = new QLabel(QString::fromLocal8Bit("0"));
	Label_TorpedoVsShipDamageCalculation_DamageLevel = new QLabel(QString::fromLocal8Bit("0"));
	Label_TorpedoVsShipDamageCalculation_Power = new QLabel(QString::fromLocal8Bit("0"));
	Label_TorpedoVsShipDamageCalculation_TargetName = new QLabel(QString::fromLocal8Bit(""));
}

void TorpedoVsShipDamageCalculation::setupLayout()
{
	Layout_TorpedoVsShipDamageCalculation = new QGridLayout ;
	Layout_TorpedoVsShipDamageCalculation->setColumnStretch(1,2);
	Layout_TorpedoVsShipDamageCalculation->setColumnStretch(2,1);
	QVBoxLayout* tmpLayout = new QVBoxLayout ;
	QWidget * tmpWidget= new QWidget;

	QLabel * tmpstr = new QLabel(QString::fromLocal8Bit("爆炸事件"));
	tmpstr->setAlignment(Qt::AlignVCenter);
	tmpLayout->addWidget(tmpstr);
	tmpLayout->addWidget(Table_TorpedoVsShipDamageCalculation);
	tmpWidget->setLayout(tmpLayout);

	Layout_TorpedoVsShipDamageCalculation->addWidget(tmpWidget,1,1);

	tmpWidget= new QWidget;
	tmpLayout = new QVBoxLayout;
	tmpLayout->addWidget(chartView_TorpedoVsShipDamageCalculation,2);

	QGridLayout *tmpgridLayout = new QGridLayout;
	QWidget *  tmpGridWidget= new QWidget;
	QLabel * strDistance = new QLabel(QString::fromLocal8Bit("距离"));
	strDistance->setAlignment(Qt::AlignVCenter);
	QLabel * strPosition = new QLabel(QString::fromLocal8Bit("方位"));
	strPosition->setAlignment(Qt::AlignVCenter);
	QLabel * strHeight = new QLabel(QString::fromLocal8Bit("高低"));
	strHeight->setAlignment(Qt::AlignVCenter);
	QLabel * strPower = new QLabel(QString::fromLocal8Bit("当前生命值"));
	strPower->setAlignment(Qt::AlignVCenter);
	QLabel * strDamageLevel = new QLabel(QString::fromLocal8Bit("毁伤点数"));
	strDamageLevel->setAlignment(Qt::AlignVCenter);
	QLabel * strTargetName = new QLabel(QString::fromLocal8Bit("攻击目标"));
	strTargetName->setAlignment(Qt::AlignVCenter);
	QLabel * strChangeDamageLevel = new QLabel(QString::fromLocal8Bit("调整毁伤点数："));
	strChangeDamageLevel->setAlignment(Qt::AlignVCenter);

	int rowIndex = 0;
	tmpgridLayout->addWidget(strTargetName,rowIndex,1 );
	tmpgridLayout->addWidget(Label_TorpedoVsShipDamageCalculation_TargetName,rowIndex,2 );

	rowIndex++;
	tmpgridLayout->addWidget(strDistance,rowIndex,1 );
	tmpgridLayout->addWidget(Label_TorpedoVsShipDamageCalculation_Distance,rowIndex,2 );

	rowIndex++;
	tmpgridLayout->addWidget(strPosition,rowIndex,1 );
	tmpgridLayout->addWidget(Label_TorpedoVsShipDamageCalculation_Position,rowIndex,2 );

	rowIndex++;
	tmpgridLayout->addWidget(strHeight,rowIndex,1 );
	tmpgridLayout->addWidget(Label_TorpedoVsShipDamageCalculation_Height,rowIndex,2 );

	//rowIndex++;
	//tmpgridLayout->addWidget(strPower,rowIndex,1 );
	//tmpgridLayout->addWidget(Label_TorpedoVsShipDamageCalculation_Power,rowIndex,2 );

	rowIndex++;
	tmpgridLayout->addWidget(strDamageLevel,rowIndex,1 );
	tmpgridLayout->addWidget(Label_TorpedoVsShipDamageCalculation_DamageLevel,rowIndex,2 );

	rowIndex++;
	tmpgridLayout->addWidget(strChangeDamageLevel,rowIndex,1 );
	tmpgridLayout->addWidget(ComboBox_TorpedoVsShipDamageCalculation,rowIndex,2 );

	rowIndex++;
	tmpgridLayout->addWidget(Button_TorpedoVsShipDamageCalculation,rowIndex,2 );
	tmpGridWidget->setLayout(tmpgridLayout);

	tmpLayout->addWidget(tmpGridWidget);
	tmpWidget->setLayout(tmpLayout);

	Layout_TorpedoVsShipDamageCalculation->addWidget(tmpWidget,1,2);
	setLayout(Layout_TorpedoVsShipDamageCalculation);
}

void TorpedoVsShipDamageCalculation::resetUi()
{
	ScatterSeries_TvsShipDC->clear();
	m_currentRow = -1;
	Table_TorpedoVsShipDamageCalculation->clearContents();
	Table_TorpedoVsShipDamageCalculation->setRowCount(0);
}

void TorpedoVsShipDamageCalculation::preHandleDataFrame()
{
	ScatterSeries_TvsShipDC->clear();
	m_currentRow = -1;
	Table_TorpedoVsShipDamageCalculation->clearContents();
	Table_TorpedoVsShipDamageCalculation->setRowCount(0);
}

void TorpedoVsShipDamageCalculation::handleDataFrame()
{
	//需要更新
	updataDamageCalculationData(getExplodeEvenfs(TorpedoObject, ShipEntityObject));
}

void TorpedoVsShipDamageCalculation::updataDamageCalculationData(const EventInfos& infos)
{
	//只追加，不全局更新
	foreach(EventInfo info, infos)
	{
		//没有包含此事件，需要添加
		if(indexOfEquipmentName(Table_TorpedoVsShipDamageCalculation, info.explodeEntityName, 1) < 0)
		{
			int rowCount = Table_TorpedoVsShipDamageCalculation->rowCount();
			Table_TorpedoVsShipDamageCalculation->setRowCount(rowCount+1);
			QTableWidgetItem * timeItem= new QTableWidgetItem(info.happenTime);
			timeItem->setData(Qt::ToolTip, info.happenTime);
			Table_TorpedoVsShipDamageCalculation->setItem(rowCount,0, timeItem);
			Table_TorpedoVsShipDamageCalculation->setItem(rowCount,1, new QTableWidgetItem(info.explodeEntityName));
			Table_TorpedoVsShipDamageCalculation->setItem(rowCount,2, new QTableWidgetItem(info.hitTarget));
			QPushButton* pbt_detail = new QPushButton;
			pbt_detail->setText(QString::fromLocal8Bit("查看"));

			QTableWidgetItem * detailItem= new QTableWidgetItem();
			detailItem->setData(Qt::UserRole, QVariant::fromValue<EventDetail>(info.detailInfo));
			Table_TorpedoVsShipDamageCalculation->setItem(rowCount, 3, detailItem);
			Table_TorpedoVsShipDamageCalculation->setCellWidget(rowCount,3, pbt_detail);
			connect(pbt_detail, SIGNAL(clicked()), this, SLOT(slotBtnCheckoutDetail()));
		}
	}
}

void TorpedoVsShipDamageCalculation::stopHandle()
{

}

void TorpedoVsShipDamageCalculation::slotBtnTvsShipDC()
{
	QString entityName = Label_TorpedoVsShipDamageCalculation_TargetName->text();
	int status;// = ComboBox_TorpedoVsShipDamageCalculation->currentIndex();
	sendStatusChangeCommand(entityName, status);
}

void TorpedoVsShipDamageCalculation::slotBtnCheckoutDetail()
{
	Button_TorpedoVsShipDamageCalculation->setEnabled(true);
	QWidget* cellwidget = qobject_cast<QWidget*>(sender());
	int currentRow = indexOfCellWidget(Table_TorpedoVsShipDamageCalculation, cellwidget, 3);
	if(currentRow >= 0)
	{
		m_currentRow = currentRow;
		QTableWidgetItem * item = Table_TorpedoVsShipDamageCalculation->item(currentRow, 3);
		EventDetail detail = item->data(Qt::UserRole).value<EventDetail>();
		Label_TorpedoVsShipDamageCalculation_Distance->setText(QString::number(detail.distance,'f',2) );
		Label_TorpedoVsShipDamageCalculation_Position->setText(QString::number(detail.orientation,'f',2) );
		Label_TorpedoVsShipDamageCalculation_Height->setText(QString::number(detail.height,'f',2));
		Label_TorpedoVsShipDamageCalculation_DamageLevel->setText(QString::number(detail.damageNumber,'f',2));
		Label_TorpedoVsShipDamageCalculation_Power->setText(QString::number(detail.currentHealth,'f',2));
		Label_TorpedoVsShipDamageCalculation_TargetName->setText(Table_TorpedoVsShipDamageCalculation->item(currentRow, 2)->text());
		ScatterSeries_TvsShipDC->clear();
		ScatterSeries_TvsShipDC->append(detail.orientation, detail.r);
		if(detail.r > 400)
		{
			m_axisR_TvsShipDC->setRange(0,detail.r);//距离范围
		}
		else
		{
			m_axisR_TvsShipDC->setRange(0,400);//距离范围
		}
	}
}