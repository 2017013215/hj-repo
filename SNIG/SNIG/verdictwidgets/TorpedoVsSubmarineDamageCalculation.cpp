#include "stdafx.h"
#include "TorpedoVsSubmarineDamageCalculation.h" 

TorpedoVsSubmarineDamageCalculation::TorpedoVsSubmarineDamageCalculation(VerdictBaseWidget* parent /*= nullptr*/)
	: VerdictBaseWidget(parent)
{
	createWidget();
	setupLayout();
	updateEntities();
	resetUi();
	startDataHandle(1000);
}

// 毁伤裁决(鱼雷对潜艇)
void TorpedoVsSubmarineDamageCalculation::createButton()
{
	Button_TorpedoVsSubmarineDamageCalculation = new QPushButton;
	Button_TorpedoVsSubmarineDamageCalculation->setText(QString::fromLocal8Bit("确认修改"));
	Button_TorpedoVsSubmarineDamageCalculation->setEnabled(false);
	connect(Button_TorpedoVsSubmarineDamageCalculation, SIGNAL(clicked()), this, SLOT(slotBtnTvsSubmarineDC()));
}

// 毁伤裁决(鱼雷对潜艇)
void TorpedoVsSubmarineDamageCalculation::createChart()
{
	m_axisA_TvsSubmaineDC = new QValueAxis;
	m_axisR_TvsSubmaineDC = new QValueAxis;
	m_axisA_TvsSubmaineDC->setTickCount(9);
	m_axisA_TvsSubmaineDC->setRange(0,360);//角度范围
	//m_axisA->setLabelsAngle(180);
	m_axisA_TvsSubmaineDC->setLabelFormat("%d");
	m_axisR_TvsSubmaineDC->setTickCount(9);
	m_axisR_TvsSubmaineDC->setRange(0,400);//距离范围
	//m_axisR->setLabelsAngle(180);
	m_axisR_TvsSubmaineDC->setLabelFormat("%d");

	chartView_TorpedoVsSubmarineDamageCalculation = new QChartView;
	ScatterSeries_TvsSubmarineDC = new QScatterSeries;
	ScatterSeries_TvsSubmarineDC->setName(QString::fromLocal8Bit("目标实体"));
	ScatterSeries_TvsSubmarineDC->setPen(QPen(QColor(34,34,34)));

	PolarChart_TorpedoVsSubmarineDamageCalculation = new QPolarChart();           // 创建一个极坐标系图表
	PolarChart_TorpedoVsSubmarineDamageCalculation->legend()->setVisible(false);               // 隐藏图例
	PolarChart_TorpedoVsSubmarineDamageCalculation->addSeries(ScatterSeries_TvsSubmarineDC);                         // 将创建的series添加进图表中
	PolarChart_TorpedoVsSubmarineDamageCalculation->addAxis(m_axisA_TvsSubmaineDC,QPolarChart::PolarOrientationAngular);
	PolarChart_TorpedoVsSubmarineDamageCalculation->addAxis(m_axisR_TvsSubmaineDC,QPolarChart::PolarOrientationRadial);
	ScatterSeries_TvsSubmarineDC->attachAxis(m_axisA_TvsSubmaineDC);
	ScatterSeries_TvsSubmarineDC->attachAxis(m_axisR_TvsSubmaineDC);

	PolarChart_TorpedoVsSubmarineDamageCalculation->setBackgroundVisible(false);
	chartView_TorpedoVsSubmarineDamageCalculation->setChart(PolarChart_TorpedoVsSubmarineDamageCalculation);
}

// 毁伤裁决(鱼雷对潜艇)
void TorpedoVsSubmarineDamageCalculation::createComBox()
{
	ComboBox_TorpedoVsSubmarineDamageCalculation= new QDoubleSpinBox;
	ComboBox_TorpedoVsSubmarineDamageCalculation->setEnabled(true);
	ComboBox_TorpedoVsSubmarineDamageCalculation->setValue(0.0);
	ComboBox_TorpedoVsSubmarineDamageCalculation->setMaximum(100.0);
	ComboBox_TorpedoVsSubmarineDamageCalculation->setMinimum(0.0);
	//connect(ComboBox_TorpedoVsSubmarineDamageCalculation,SIGNAL(currentIndexChanged(int)),this,SLOT(slotComCDEntity()));
}

// 毁伤裁决(鱼雷对潜艇)
void TorpedoVsSubmarineDamageCalculation::createTableView()
{
	QStringList HeaderList_TorpedoVsSubmarineDamageCalculation;
	HeaderList_TorpedoVsSubmarineDamageCalculation<<QString::fromLocal8Bit("命中时刻")<<QString::fromLocal8Bit("爆炸实体")<<QString::fromLocal8Bit("命中实体")<<QString::fromLocal8Bit("详情");

	Table_TorpedoVsSubmarineDamageCalculation = new QTableWidget;
	Table_TorpedoVsSubmarineDamageCalculation->setColumnCount(HeaderList_TorpedoVsSubmarineDamageCalculation.size());
	Table_TorpedoVsSubmarineDamageCalculation->setFocusPolicy(Qt::NoFocus);
	Table_TorpedoVsSubmarineDamageCalculation->setEditTriggers(QAbstractItemView::NoEditTriggers);
	Table_TorpedoVsSubmarineDamageCalculation->setSelectionMode(QAbstractItemView::NoSelection);
	Table_TorpedoVsSubmarineDamageCalculation->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	Table_TorpedoVsSubmarineDamageCalculation->horizontalHeader()->setStyleSheet("QHeaderView::section{background:white;color: black;}");
	Table_TorpedoVsSubmarineDamageCalculation->verticalHeader()->hide();
	for (int a=0;a<HeaderList_TorpedoVsSubmarineDamageCalculation.size();a++) {
		Table_TorpedoVsSubmarineDamageCalculation->setHorizontalHeaderItem(a,new QTableWidgetItem(HeaderList_TorpedoVsSubmarineDamageCalculation.at(a)));
	}
	Table_TorpedoVsSubmarineDamageCalculation->verticalHeader()->setDefaultSectionSize(30);
}

void TorpedoVsSubmarineDamageCalculation::setupLayout()
{
	Layout_TorpedoVsSubmarineDamageCalculation = new QGridLayout ;
	Layout_TorpedoVsSubmarineDamageCalculation->setColumnStretch(1,2);
	Layout_TorpedoVsSubmarineDamageCalculation->setColumnStretch(2,1);
	QVBoxLayout* tmpLayout = new QVBoxLayout ;
	QWidget * tmpWidget= new QWidget;

	QLabel * tmpstr = new QLabel(QString::fromLocal8Bit("爆炸事件"));
	tmpstr->setAlignment(Qt::AlignVCenter);
	tmpLayout->addWidget(tmpstr);
	tmpLayout->addWidget(Table_TorpedoVsSubmarineDamageCalculation);
	tmpWidget->setLayout(tmpLayout);

	Layout_TorpedoVsSubmarineDamageCalculation->addWidget(tmpWidget,1,1);

	tmpWidget= new QWidget;
	tmpLayout = new QVBoxLayout;
	tmpLayout->addWidget(chartView_TorpedoVsSubmarineDamageCalculation,2);

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
	tmpgridLayout->addWidget(Label_TorpedoVsSubmarineDamageCalculation_TargetName,rowIndex,2 );

	rowIndex++;
	tmpgridLayout->addWidget(strDistance,rowIndex,1 );
	tmpgridLayout->addWidget(Label_TorpedoVsSubmarineDamageCalculation_Distance,rowIndex,2 );

	rowIndex++;
	tmpgridLayout->addWidget(strPosition,rowIndex,1 );
	tmpgridLayout->addWidget(Label_TorpedoVsSubmarineDamageCalculation_Position,rowIndex,2 );

	rowIndex++;
	tmpgridLayout->addWidget(strHeight,rowIndex,1 );
	tmpgridLayout->addWidget(Label_TorpedoVsSubmarineDamageCalculation_Height,rowIndex,2 );

	//rowIndex++;
	//tmpgridLayout->addWidget(strPower,rowIndex,1 );
	//tmpgridLayout->addWidget(Label_TorpedoVsSubmarineDamageCalculation_Power,rowIndex,2 );

	rowIndex++;
	tmpgridLayout->addWidget(strDamageLevel,rowIndex,1 );
	tmpgridLayout->addWidget(Label_TorpedoVsSubmarineDamageCalculation_DamageLevel,rowIndex,2 );

	rowIndex++;
	tmpgridLayout->addWidget(strChangeDamageLevel,rowIndex,1 );
	tmpgridLayout->addWidget(ComboBox_TorpedoVsSubmarineDamageCalculation,rowIndex,2 );

	rowIndex++;
	tmpgridLayout->addWidget(Button_TorpedoVsSubmarineDamageCalculation,rowIndex,2 );
	tmpGridWidget->setLayout(tmpgridLayout);

	tmpLayout->addWidget(tmpGridWidget);
	tmpWidget->setLayout(tmpLayout);

	Layout_TorpedoVsSubmarineDamageCalculation->addWidget(tmpWidget,1,2);
	setLayout(Layout_TorpedoVsSubmarineDamageCalculation);
}

void TorpedoVsSubmarineDamageCalculation::resetUi()
{
	ScatterSeries_TvsSubmarineDC->clear();
	m_currentRow = -1;
	Table_TorpedoVsSubmarineDamageCalculation->clearContents();
	Table_TorpedoVsSubmarineDamageCalculation->setRowCount(0);
}

void TorpedoVsSubmarineDamageCalculation::preHandleDataFrame()
{
	ScatterSeries_TvsSubmarineDC->clear();
	m_currentRow = -1;
	Table_TorpedoVsSubmarineDamageCalculation->clearContents();
	Table_TorpedoVsSubmarineDamageCalculation->setRowCount(0);
}

void TorpedoVsSubmarineDamageCalculation::handleDataFrame()
{
	//需要更新
	updataDamageCalculationData(getExplodeEvenfs(TorpedoObject, SubmarineObject));
}

void TorpedoVsSubmarineDamageCalculation::updataDamageCalculationData(const EventInfos& infos)
{
	//只追加，不全局更新
	foreach(EventInfo info, infos)
	{
		//没有包含此事件，需要添加
		if(indexOfEquipmentName(Table_TorpedoVsSubmarineDamageCalculation, info.explodeEntityName, 1) < 0)
		{
			int rowCount = Table_TorpedoVsSubmarineDamageCalculation->rowCount();
			Table_TorpedoVsSubmarineDamageCalculation->setRowCount(rowCount+1);
			QTableWidgetItem * timeItem= new QTableWidgetItem(info.happenTime);
			timeItem->setData(Qt::ToolTip, info.happenTime);
			Table_TorpedoVsSubmarineDamageCalculation->setItem(rowCount,0, timeItem);
			Table_TorpedoVsSubmarineDamageCalculation->setItem(rowCount,1, new QTableWidgetItem(info.explodeEntityName));
			Table_TorpedoVsSubmarineDamageCalculation->setItem(rowCount,2, new QTableWidgetItem(info.hitTarget));
			QPushButton* pbt_detail = new QPushButton;
			pbt_detail->setText(QString::fromLocal8Bit("查看"));

			QTableWidgetItem * detailItem= new QTableWidgetItem();
			detailItem->setData(Qt::UserRole, QVariant::fromValue<EventDetail>(info.detailInfo));
			Table_TorpedoVsSubmarineDamageCalculation->setItem(rowCount, 3, detailItem);
			Table_TorpedoVsSubmarineDamageCalculation->setCellWidget(rowCount,3, pbt_detail);
			connect(pbt_detail, SIGNAL(clicked()), this, SLOT(slotBtnCheckoutDetail()));
		}
	}
}

void TorpedoVsSubmarineDamageCalculation::stopHandle()
{

}

void TorpedoVsSubmarineDamageCalculation::slotBtnTvsSubmarineDC()
{
	QString entityName = Label_TorpedoVsSubmarineDamageCalculation_TargetName->text();
	int status;// = ComboBox_TorpedoVsSubmarineDamageCalculation->currentIndex();
	sendStatusChangeCommand(entityName, status);
}

void TorpedoVsSubmarineDamageCalculation::slotBtnCheckoutDetail()
{
	Button_TorpedoVsSubmarineDamageCalculation->setEnabled(true);
	QWidget* cellwidget = qobject_cast<QWidget*>(sender());
	int currentRow = indexOfCellWidget(Table_TorpedoVsSubmarineDamageCalculation, cellwidget, 3);
	if(currentRow >= 0)
	{
		m_currentRow = currentRow;
		QTableWidgetItem * item = Table_TorpedoVsSubmarineDamageCalculation->item(currentRow, 3);
		EventDetail detail = item->data(Qt::UserRole).value<EventDetail>();
		Label_TorpedoVsSubmarineDamageCalculation_Distance->setText(QString::number(detail.distance,'f',2) );
		Label_TorpedoVsSubmarineDamageCalculation_Position->setText(QString::number(detail.orientation,'f',2) );
		Label_TorpedoVsSubmarineDamageCalculation_Height->setText(QString::number(detail.height,'f',2));
		Label_TorpedoVsSubmarineDamageCalculation_DamageLevel->setText(QString::number(detail.damageNumber,'f',2));
		Label_TorpedoVsSubmarineDamageCalculation_Power->setText(QString::number(detail.currentHealth,'f',2));
		Label_TorpedoVsSubmarineDamageCalculation_TargetName->setText(Table_TorpedoVsSubmarineDamageCalculation->item(currentRow, 2)->text());
		ScatterSeries_TvsSubmarineDC->clear();
		ScatterSeries_TvsSubmarineDC->append(detail.orientation, detail.r);
		if(detail.r > 400)
		{
			m_axisR_TvsSubmaineDC->setRange(0,detail.r);//距离范围
		}
		else
		{
			m_axisR_TvsSubmaineDC->setRange(0,400);//距离范围
		}
	}
}

void TorpedoVsSubmarineDamageCalculation::createLabel()
{
	Label_TorpedoVsSubmarineDamageCalculation_Distance = new QLabel(QString::fromLocal8Bit("0"));
	Label_TorpedoVsSubmarineDamageCalculation_Position= new QLabel(QString::fromLocal8Bit("0"));
	Label_TorpedoVsSubmarineDamageCalculation_Height= new QLabel(QString::fromLocal8Bit("0"));
	Label_TorpedoVsSubmarineDamageCalculation_DamageLevel= new QLabel(QString::fromLocal8Bit("0"));
	Label_TorpedoVsSubmarineDamageCalculation_Power= new QLabel(QString::fromLocal8Bit("0"));
	Label_TorpedoVsSubmarineDamageCalculation_TargetName = new QLabel(QString::fromLocal8Bit(""));
}

