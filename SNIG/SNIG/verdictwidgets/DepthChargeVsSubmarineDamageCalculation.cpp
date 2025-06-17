#include "stdafx.h"
#include "DepthChargeVsSubmarineDamageCalculation.h" 

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QDoubleSpinBox>

DepthChargeVsSubmarineDamageCalculation::DepthChargeVsSubmarineDamageCalculation(VerdictBaseWidget* parent /*= nullptr*/)
	: VerdictBaseWidget(parent)
{
	createWidget();
	setupLayout();
	updateEntities();
	resetUi();
	startDataHandle(1000);
}

// 毁伤裁决(深水炸弹对潜艇)
void DepthChargeVsSubmarineDamageCalculation::createButton()
{
	Button_DepthChargeVsSubmarineDamageCalculation = new QPushButton;
	Button_DepthChargeVsSubmarineDamageCalculation->setText(QString::fromLocal8Bit("确认修改"));
	Button_DepthChargeVsSubmarineDamageCalculation->setEnabled(false);
	connect(Button_DepthChargeVsSubmarineDamageCalculation, SIGNAL(clicked()), this, SLOT(slotBtnDCvsSubmaineDC()));
}

// 毁伤裁决(深水炸弹对潜艇)
void DepthChargeVsSubmarineDamageCalculation::createChart()
{
	m_axisR_DCvsSubmaineDC = new QValueAxis;
	m_axisA_DCvsSubmaineDC = new QValueAxis;
	m_axisA_DCvsSubmaineDC->setTickCount(9);
	m_axisA_DCvsSubmaineDC->setRange(0,360);//角度范围
	//m_axisA->setLabelsAngle(180);
	m_axisA_DCvsSubmaineDC->setLabelFormat("%d");
	m_axisR_DCvsSubmaineDC->setTickCount(9);
	m_axisR_DCvsSubmaineDC->setRange(0,400);//距离范围
	//m_axisR->setLabelsAngle(180);
	m_axisR_DCvsSubmaineDC->setLabelFormat("%d");

	chartView_DepthChargeVsSubmarineDamageCalculation = new QChartView;
	ScatterSeries_DCvsSubmaineDC = new QScatterSeries;
	ScatterSeries_DCvsSubmaineDC->setName(QString::fromLocal8Bit("目标实体"));
	ScatterSeries_DCvsSubmaineDC->setPen(QPen(QColor(34,34,34)));

	PolarChart_DepthChargeVsSubmarineDamageCalculation = new QPolarChart();           // 创建一个极坐标系图表
	PolarChart_DepthChargeVsSubmarineDamageCalculation->legend()->setVisible(false);               // 隐藏图例
	PolarChart_DepthChargeVsSubmarineDamageCalculation->addSeries(ScatterSeries_DCvsSubmaineDC);                         // 将创建的series添加进图表中
	PolarChart_DepthChargeVsSubmarineDamageCalculation->addAxis(m_axisA_DCvsSubmaineDC,QPolarChart::PolarOrientationAngular);
	PolarChart_DepthChargeVsSubmarineDamageCalculation->addAxis(m_axisR_DCvsSubmaineDC,QPolarChart::PolarOrientationRadial);
	ScatterSeries_DCvsSubmaineDC->attachAxis(m_axisA_DCvsSubmaineDC);
	ScatterSeries_DCvsSubmaineDC->attachAxis(m_axisR_DCvsSubmaineDC);
	// 根据已有的series生成默认的坐标轴
	PolarChart_DepthChargeVsSubmarineDamageCalculation->setBackgroundVisible(false);
	chartView_DepthChargeVsSubmarineDamageCalculation->setChart(PolarChart_DepthChargeVsSubmarineDamageCalculation);
}

// 毁伤裁决(深水炸弹对潜艇)
void DepthChargeVsSubmarineDamageCalculation::createComBox()
{
	ComboBox_DepthChargeVsSubmarineDamageCalculation = new QDoubleSpinBox;
	ComboBox_DepthChargeVsSubmarineDamageCalculation->setEnabled(true);
	ComboBox_DepthChargeVsSubmarineDamageCalculation->setValue(0.0);
	ComboBox_DepthChargeVsSubmarineDamageCalculation->setMaximum(100.0);
	ComboBox_DepthChargeVsSubmarineDamageCalculation->setMinimum(0.0);
	//ComboBox_DepthChargeVsSubmarineDamageCalculation->lineEdit()->setAlignment(Qt::AlignCenter);
	//ComboBox_DepthChargeVsSubmarineDamageCalculation->lineEdit()->setReadOnly(true);
	//ComboBox_DepthChargeVsSubmarineDamageCalculation->addItems(getJudgmentList());
	//connect(ComboBox_DepthChargeVsSubmarineDamageCalculation,SIGNAL(currentIndexChanged(int)),this,SLOT(slotComCDEntity()));
}

// 毁伤裁决(深水炸弹对潜艇)
void DepthChargeVsSubmarineDamageCalculation::createTableView()
{
	QStringList HeaderList_DepthChargeVsSubmarineDamageCalculation;
	HeaderList_DepthChargeVsSubmarineDamageCalculation<<QString::fromLocal8Bit("命中时刻")<<QString::fromLocal8Bit("爆炸实体")<<QString::fromLocal8Bit("命中实体")<<QString::fromLocal8Bit("详情");
	//Model_DepthChargeVsSubmarineDamageCalculation = new QStandardItemModel;
	Table_DepthChargeVsSubmarineDamageCalculation = new QTableWidget;
	Table_DepthChargeVsSubmarineDamageCalculation->setColumnCount(HeaderList_DepthChargeVsSubmarineDamageCalculation.size());
	Table_DepthChargeVsSubmarineDamageCalculation->setFocusPolicy(Qt::NoFocus);
	Table_DepthChargeVsSubmarineDamageCalculation->setEditTriggers(QAbstractItemView::NoEditTriggers);
	Table_DepthChargeVsSubmarineDamageCalculation->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	Table_DepthChargeVsSubmarineDamageCalculation->horizontalHeader()->setStyleSheet("QHeaderView::section{background:white;color: black;}");
	Table_DepthChargeVsSubmarineDamageCalculation->verticalHeader()->hide();
	for (int a=0;a<HeaderList_DepthChargeVsSubmarineDamageCalculation.size();a++) {
		Table_DepthChargeVsSubmarineDamageCalculation->setHorizontalHeaderItem(a,new QTableWidgetItem(HeaderList_DepthChargeVsSubmarineDamageCalculation.at(a)));
	}
	Table_DepthChargeVsSubmarineDamageCalculation->verticalHeader()->setDefaultSectionSize(30);
}

void DepthChargeVsSubmarineDamageCalculation::createLabel()
{
	Label_DepthChargeVsSubmarineDamageCalculation_Distance = new QLabel(QString::fromLocal8Bit("0"));
	Label_DepthChargeVsSubmarineDamageCalculation_Position = new QLabel(QString::fromLocal8Bit("0"));
	Label_DepthChargeVsSubmarineDamageCalculation_Height  = new QLabel(QString::fromLocal8Bit("0"));
	Label_DepthChargeVsSubmarineDamageCalculation_DamageLevel = new QLabel(QString::fromLocal8Bit("0"));
	Label_DepthChargeVsSubmarineDamageCalculation_Power = new QLabel(QString::fromLocal8Bit("0"));
	Label_DepthChargeVsSubmarineDamageCalculation_TargetName = new QLabel(QString::fromLocal8Bit(""));
}

void DepthChargeVsSubmarineDamageCalculation::setupLayout()
{
	Layout_DepthChargeVsSubmarineDamageCalculation = new QGridLayout ;
	Layout_DepthChargeVsSubmarineDamageCalculation->setColumnStretch(1,2);
	Layout_DepthChargeVsSubmarineDamageCalculation->setColumnStretch(2,1);
	QVBoxLayout* tmpLayout = new QVBoxLayout ;
	QWidget * tmpWidget= new QWidget;

	QLabel * tmpstr = new QLabel(QString::fromLocal8Bit("爆炸事件"));
	tmpstr->setAlignment(Qt::AlignVCenter);
	tmpLayout->addWidget(tmpstr);
	tmpLayout->addWidget(Table_DepthChargeVsSubmarineDamageCalculation);
	tmpWidget->setLayout(tmpLayout);

	Layout_DepthChargeVsSubmarineDamageCalculation->addWidget(tmpWidget,1,1);

	tmpWidget= new QWidget;
	tmpLayout = new QVBoxLayout;
	tmpLayout->addWidget(chartView_DepthChargeVsSubmarineDamageCalculation,2);

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
	tmpgridLayout->addWidget(Label_DepthChargeVsSubmarineDamageCalculation_TargetName,rowIndex,2 );

	rowIndex++;
	tmpgridLayout->addWidget(strDistance,rowIndex,1 );
	tmpgridLayout->addWidget(Label_DepthChargeVsSubmarineDamageCalculation_Distance,rowIndex,2 );

	rowIndex++;
	tmpgridLayout->addWidget(strPosition,rowIndex,1 );
	tmpgridLayout->addWidget(Label_DepthChargeVsSubmarineDamageCalculation_Position,rowIndex,2 );

	rowIndex++;
	tmpgridLayout->addWidget(strHeight,rowIndex,1 );
	tmpgridLayout->addWidget(Label_DepthChargeVsSubmarineDamageCalculation_Height,rowIndex,2 );

	//rowIndex++;
	//tmpgridLayout->addWidget(strPower,rowIndex,1 );
	//tmpgridLayout->addWidget(Label_DepthChargeVsSubmarineDamageCalculation_Power,rowIndex,2 );

	rowIndex++;
	tmpgridLayout->addWidget(strDamageLevel,rowIndex,1 );
	tmpgridLayout->addWidget(Label_DepthChargeVsSubmarineDamageCalculation_DamageLevel,rowIndex,2 );

	rowIndex++;
	tmpgridLayout->addWidget(strChangeDamageLevel,rowIndex,1 );
	tmpgridLayout->addWidget(ComboBox_DepthChargeVsSubmarineDamageCalculation,rowIndex,2 );

	rowIndex++;
	tmpgridLayout->addWidget(Button_DepthChargeVsSubmarineDamageCalculation,rowIndex,2 );
	tmpGridWidget->setLayout(tmpgridLayout);

	tmpLayout->addWidget(tmpGridWidget);
	tmpWidget->setLayout(tmpLayout);

	Layout_DepthChargeVsSubmarineDamageCalculation->addWidget(tmpWidget,1,2);
	setLayout(Layout_DepthChargeVsSubmarineDamageCalculation);
}

void DepthChargeVsSubmarineDamageCalculation::resetUi()
{
	ScatterSeries_DCvsSubmaineDC->clear();
	m_currentRow = -1;
	Table_DepthChargeVsSubmarineDamageCalculation->clearContents();
	Table_DepthChargeVsSubmarineDamageCalculation->setRowCount(0);
}

void DepthChargeVsSubmarineDamageCalculation::preHandleDataFrame()
{
	ScatterSeries_DCvsSubmaineDC->clear();
	m_currentRow = -1;
	Table_DepthChargeVsSubmarineDamageCalculation->clearContents();
	Table_DepthChargeVsSubmarineDamageCalculation->setRowCount(0);
}

void DepthChargeVsSubmarineDamageCalculation::handleDataFrame()
{
	//需要更新
	updataDamageCalculationData(getExplodeEvenfs(DepthChargeObject, SubmarineObject));
}

void DepthChargeVsSubmarineDamageCalculation::updataDamageCalculationData(const EventInfos& infos)
{
	//只追加，不全局更新
	foreach(EventInfo info, infos)
	{
		//没有包含此事件，需要添加
		if(indexOfEquipmentName(Table_DepthChargeVsSubmarineDamageCalculation, info.explodeEntityName, 1) < 0)
		{
			int rowCount = Table_DepthChargeVsSubmarineDamageCalculation->rowCount();
			Table_DepthChargeVsSubmarineDamageCalculation->setRowCount(rowCount+1);
			QTableWidgetItem * timeItem= new QTableWidgetItem(info.happenTime);
			timeItem->setData(Qt::ToolTip, info.happenTime);
			Table_DepthChargeVsSubmarineDamageCalculation->setItem(rowCount,0, timeItem);
			Table_DepthChargeVsSubmarineDamageCalculation->setItem(rowCount,1, new QTableWidgetItem(info.explodeEntityName));
			Table_DepthChargeVsSubmarineDamageCalculation->setItem(rowCount,2, new QTableWidgetItem(info.hitTarget));
			QPushButton* pbt_detail = new QPushButton;
			pbt_detail->setText(QString::fromLocal8Bit("查看"));

			QTableWidgetItem * detailItem= new QTableWidgetItem();
			detailItem->setData(Qt::UserRole, QVariant::fromValue<EventDetail>(info.detailInfo));
			Table_DepthChargeVsSubmarineDamageCalculation->setItem(rowCount, 3, detailItem);
			Table_DepthChargeVsSubmarineDamageCalculation->setCellWidget(rowCount,3, pbt_detail);
			connect(pbt_detail, SIGNAL(clicked()), this, SLOT(slotBtnCheckoutDetail()));
		}
	}
}

void DepthChargeVsSubmarineDamageCalculation::stopHandle()
{

}

void DepthChargeVsSubmarineDamageCalculation::slotBtnDCvsSubmaineDC()
{
	QString entityName = Label_DepthChargeVsSubmarineDamageCalculation_TargetName->text();
	int status;// = ComboBox_DepthChargeVsSubmarineDamageCalculation->currentIndex();
	sendStatusChangeCommand(entityName, status);
}

void DepthChargeVsSubmarineDamageCalculation::slotBtnCheckoutDetail()
{
	Button_DepthChargeVsSubmarineDamageCalculation->setEnabled(true);
	QWidget* cellwidget = qobject_cast<QWidget*>(sender());
	int currentRow = indexOfCellWidget(Table_DepthChargeVsSubmarineDamageCalculation, cellwidget, 3);
	if(currentRow >= 0)
	{
		m_currentRow = currentRow;
		QTableWidgetItem * item = Table_DepthChargeVsSubmarineDamageCalculation->item(currentRow, 3);
		EventDetail detail = item->data(Qt::UserRole).value<EventDetail>();
		Label_DepthChargeVsSubmarineDamageCalculation_Distance->setText(QString::number(detail.distance,'f',2) );
		Label_DepthChargeVsSubmarineDamageCalculation_Position->setText(QString::number(detail.orientation,'f',2) );
		Label_DepthChargeVsSubmarineDamageCalculation_Height->setText(QString::number(detail.height,'f',2));
		Label_DepthChargeVsSubmarineDamageCalculation_DamageLevel->setText(QString::number(detail.damageNumber,'f',2));
		Label_DepthChargeVsSubmarineDamageCalculation_Power->setText(QString::number(detail.currentHealth,'f',2));
		Label_DepthChargeVsSubmarineDamageCalculation_TargetName->setText(Table_DepthChargeVsSubmarineDamageCalculation->item(currentRow, 2)->text());
		ScatterSeries_DCvsSubmaineDC->clear();
		ScatterSeries_DCvsSubmaineDC->append(detail.orientation, detail.r);
		if(detail.r > 400)
		{
			m_axisR_DCvsSubmaineDC->setRange(0,detail.r);//距离范围
		}
		else
		{
			m_axisR_DCvsSubmaineDC->setRange(0,400);//距离范围
		}
	}
}