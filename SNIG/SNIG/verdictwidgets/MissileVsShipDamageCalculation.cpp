#include "stdafx.h"
#include "MissileVsShipDamageCalculation.h" 

MissileVsShipDamageCalculation::MissileVsShipDamageCalculation(VerdictBaseWidget* parent /*= nullptr*/)
	: VerdictBaseWidget(parent)
{
	createWidget();
	setupLayout();
	updateEntities();
	resetUi();
	startDataHandle(1000);
}

// 毁伤裁决(导弹对水面舰艇)
void MissileVsShipDamageCalculation::createButton()
{
	Button_MissileVsShipDamageCalculation = new QPushButton;
	Button_MissileVsShipDamageCalculation->setText(QString::fromLocal8Bit("确认修改"));
	Button_MissileVsShipDamageCalculation->setEnabled(false);
	connect(Button_MissileVsShipDamageCalculation, SIGNAL(clicked()), this, SLOT(slotBtnMvsShipDC()));
}

// 毁伤裁决(导弹对水面舰艇)
void MissileVsShipDamageCalculation::createChart()
{
	m_axisA_MvsShipDC = new QValueAxis;
	m_axisR_MvsShipDC = new QValueAxis;
	m_axisA_MvsShipDC->setTickCount(9);
	m_axisA_MvsShipDC->setRange(0,360);//角度范围
	//m_axisA->setLabelsAngle(180);
	m_axisA_MvsShipDC->setLabelFormat("%d");
	m_axisR_MvsShipDC->setTickCount(9);
	m_axisR_MvsShipDC->setRange(0,400);//距离范围
	//m_axisR->setLabelsAngle(180);
	m_axisR_MvsShipDC->setLabelFormat("%d");

	chartView_MissileVsShipDamageCalculation = new QChartView;
	ScatterSeries_MvsShipDC = new QScatterSeries;
	ScatterSeries_MvsShipDC->setName(QString::fromLocal8Bit("目标实体"));
	ScatterSeries_MvsShipDC->setPen(QPen(QColor(34,34,34)));

	PolarChart_MissileVsShipDamageCalculation = new QPolarChart();           // 创建一个极坐标系图表
	PolarChart_MissileVsShipDamageCalculation->legend()->setVisible(false);               // 隐藏图例
	PolarChart_MissileVsShipDamageCalculation->addSeries(ScatterSeries_MvsShipDC);                         // 将创建的series添加进图表中
	PolarChart_MissileVsShipDamageCalculation->addAxis(m_axisA_MvsShipDC,QPolarChart::PolarOrientationAngular);
	PolarChart_MissileVsShipDamageCalculation->addAxis(m_axisR_MvsShipDC,QPolarChart::PolarOrientationRadial);
	ScatterSeries_MvsShipDC->attachAxis(m_axisA_MvsShipDC);
	ScatterSeries_MvsShipDC->attachAxis(m_axisR_MvsShipDC);
	// 根据已有的series生成默认的坐标轴
	PolarChart_MissileVsShipDamageCalculation->setBackgroundVisible(false);
	chartView_MissileVsShipDamageCalculation->setChart(PolarChart_MissileVsShipDamageCalculation);
}

// 毁伤裁决(导弹对水面舰艇)
void MissileVsShipDamageCalculation::createComBox()
{
	ComboBox_MissileVsShipDamageCalculation = new QDoubleSpinBox;
	ComboBox_MissileVsShipDamageCalculation->setEnabled(true);
	ComboBox_MissileVsShipDamageCalculation->setValue(0.0);
	ComboBox_MissileVsShipDamageCalculation->setMaximum(100.0);
	ComboBox_MissileVsShipDamageCalculation->setMinimum(0.0);
	//connect(ComboBox_MissileVsShipDamageCalculation,SIGNAL(currentIndexChanged(int)),this,SLOT(slotComCDEntity()));
}

// 毁伤裁决(导弹对水面舰艇)
void MissileVsShipDamageCalculation::createTableView()
{
	QStringList HeaderList_MissileVsShipDamageCalculation;
	HeaderList_MissileVsShipDamageCalculation<<QString::fromLocal8Bit("命中时刻")<<QString::fromLocal8Bit("爆炸实体")<<QString::fromLocal8Bit("命中实体")<<QString::fromLocal8Bit("详情");
	//Model_MissileVsShipDamageCalculation = new QStandardItemModel;
	Table_MissileVsShipDamageCalculation = new QTableWidget;
	Table_MissileVsShipDamageCalculation->setColumnCount(HeaderList_MissileVsShipDamageCalculation.size());
	Table_MissileVsShipDamageCalculation->setFocusPolicy(Qt::NoFocus);
	Table_MissileVsShipDamageCalculation->setEditTriggers(QAbstractItemView::NoEditTriggers);
	Table_MissileVsShipDamageCalculation->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	Table_MissileVsShipDamageCalculation->horizontalHeader()->setStyleSheet("QHeaderView::section{background:white;color: black;}");
	Table_MissileVsShipDamageCalculation->verticalHeader()->hide();
	for (int a=0;a<HeaderList_MissileVsShipDamageCalculation.size();a++) {
		Table_MissileVsShipDamageCalculation->setHorizontalHeaderItem(a,new QTableWidgetItem(HeaderList_MissileVsShipDamageCalculation.at(a)));
	}
	Table_MissileVsShipDamageCalculation->verticalHeader()->setDefaultSectionSize(30);
}

void MissileVsShipDamageCalculation::createLabel()
{
	Label_MissileVsShipDamageCalculation_Distance = new QLabel(QString::fromLocal8Bit("0"));
	Label_MissileVsShipDamageCalculation_Position = new QLabel(QString::fromLocal8Bit("0"));
	Label_MissileVsShipDamageCalculation_Height = new QLabel(QString::fromLocal8Bit("0"));
	Label_MissileVsShipDamageCalculation_DamageLevel = new QLabel(QString::fromLocal8Bit("0"));
	Label_MissileVsShipDamageCalculation_Power = new QLabel(QString::fromLocal8Bit("0"));
	Label_MissileVsShipDamageCalculation_TargetName = new QLabel(QString::fromLocal8Bit(""));
}

void MissileVsShipDamageCalculation::setupLayout()
{
	Layout_MissileVsShipDamageCalculation = new QGridLayout ;
	Layout_MissileVsShipDamageCalculation->setColumnStretch(1,2);
	Layout_MissileVsShipDamageCalculation->setColumnStretch(2,1);
	QVBoxLayout* tmpLayout = new QVBoxLayout ;
	QWidget * tmpWidget= new QWidget;

	QLabel * tmpstr = new QLabel(QString::fromLocal8Bit("爆炸事件"));
	tmpstr->setAlignment(Qt::AlignVCenter);
	tmpLayout->addWidget(tmpstr);
	tmpLayout->addWidget(Table_MissileVsShipDamageCalculation);
	tmpWidget->setLayout(tmpLayout);

	Layout_MissileVsShipDamageCalculation->addWidget(tmpWidget,1,1);

	tmpWidget= new QWidget;
	tmpLayout = new QVBoxLayout;
	tmpLayout->addWidget(chartView_MissileVsShipDamageCalculation,2);

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
	tmpgridLayout->addWidget(Label_MissileVsShipDamageCalculation_TargetName,rowIndex,2 );

	rowIndex++;
	tmpgridLayout->addWidget(strDistance,rowIndex,1 );
	tmpgridLayout->addWidget(Label_MissileVsShipDamageCalculation_Distance,rowIndex,2 );

	rowIndex++;
	tmpgridLayout->addWidget(strPosition,rowIndex,1 );
	tmpgridLayout->addWidget(Label_MissileVsShipDamageCalculation_Position,rowIndex,2 );

	rowIndex++;
	tmpgridLayout->addWidget(strHeight,rowIndex,1 );
	tmpgridLayout->addWidget(Label_MissileVsShipDamageCalculation_Height,rowIndex,2 );

	//rowIndex++;
	//tmpgridLayout->addWidget(strPower,rowIndex,1 );
	//tmpgridLayout->addWidget(Label_MissileVsShipDamageCalculation_Power,rowIndex,2 );

	rowIndex++;
	tmpgridLayout->addWidget(strDamageLevel,rowIndex,1 );
	tmpgridLayout->addWidget(Label_MissileVsShipDamageCalculation_DamageLevel,rowIndex,2 );

	rowIndex++;
	tmpgridLayout->addWidget(strChangeDamageLevel,rowIndex,1 );
	tmpgridLayout->addWidget(ComboBox_MissileVsShipDamageCalculation,rowIndex,2 );

	rowIndex++;
	tmpgridLayout->addWidget(Button_MissileVsShipDamageCalculation,rowIndex,2 );
	tmpGridWidget->setLayout(tmpgridLayout);

	tmpLayout->addWidget(tmpGridWidget);
	tmpWidget->setLayout(tmpLayout);

	Layout_MissileVsShipDamageCalculation->addWidget(tmpWidget,1,2);
	setLayout(Layout_MissileVsShipDamageCalculation);
}

void MissileVsShipDamageCalculation::resetUi()
{
	ScatterSeries_MvsShipDC->clear();
	m_currentRow = -1;
	Table_MissileVsShipDamageCalculation->clearContents();
	Table_MissileVsShipDamageCalculation->setRowCount(0);
}

void MissileVsShipDamageCalculation::preHandleDataFrame()
{
	ScatterSeries_MvsShipDC->clear();
	m_currentRow = -1;
	Table_MissileVsShipDamageCalculation->clearContents();
	Table_MissileVsShipDamageCalculation->setRowCount(0);
}

void MissileVsShipDamageCalculation::slotBtnMvsShipDC()
{
	QString entityName = Label_MissileVsShipDamageCalculation_TargetName->text();
	int status ;//= ComboBox_MissileVsShipDamageCalculation->currentIndex();
	sendStatusChangeCommand(entityName, status);
}

void MissileVsShipDamageCalculation::slotBtnCheckoutDetail()
{
	Button_MissileVsShipDamageCalculation->setEnabled(true);
	QWidget* cellwidget = qobject_cast<QWidget*>(sender());
	int currentRow = indexOfCellWidget(Table_MissileVsShipDamageCalculation, cellwidget, 3);
	if(currentRow >= 0)
	{
		m_currentRow = currentRow;
		QTableWidgetItem * item = Table_MissileVsShipDamageCalculation->item(currentRow, 3);
		EventDetail detail = item->data(Qt::UserRole).value<EventDetail>();
		Label_MissileVsShipDamageCalculation_Distance->setText(QString::number(detail.distance,'f',2) );
		Label_MissileVsShipDamageCalculation_Position->setText(QString::number(detail.orientation,'f',2) );
		Label_MissileVsShipDamageCalculation_Height->setText(QString::number(detail.height,'f',2));
		Label_MissileVsShipDamageCalculation_DamageLevel->setText(QString::number(detail.damageNumber,'f',2));
		Label_MissileVsShipDamageCalculation_Power->setText(QString::number(detail.currentHealth,'f',2));
		Label_MissileVsShipDamageCalculation_TargetName->setText(Table_MissileVsShipDamageCalculation->item(currentRow, 2)->text());
		ScatterSeries_MvsShipDC->clear();
		ScatterSeries_MvsShipDC->append(detail.orientation, detail.r);	
		if(detail.r > 400)
		{
			m_axisR_MvsShipDC->setRange(0,detail.r);//距离范围
		}
		else
		{
			m_axisR_MvsShipDC->setRange(0,400);//距离范围
		}
	}
}

void MissileVsShipDamageCalculation::handleDataFrame()
{
	//需要更新
	updataDamageCalculationData(getExplodeEvenfs(MissileObject, ShipEntityObject));
}

void MissileVsShipDamageCalculation::updataDamageCalculationData(const EventInfos& infos)
{
	//只追加，不全局更新
	foreach(EventInfo info, infos)
	{
		//没有包含此事件，需要添加
		if(indexOfEquipmentName(Table_MissileVsShipDamageCalculation, info.explodeEntityName, 1) < 0)
		{
			int rowCount = Table_MissileVsShipDamageCalculation->rowCount();
			Table_MissileVsShipDamageCalculation->setRowCount(rowCount+1);
			QTableWidgetItem * timeItem= new QTableWidgetItem(info.happenTime);
			timeItem->setData(Qt::ToolTip, info.happenTime);
			Table_MissileVsShipDamageCalculation->setItem(rowCount,0, timeItem);
			Table_MissileVsShipDamageCalculation->setItem(rowCount,1, new QTableWidgetItem(info.explodeEntityName));
			Table_MissileVsShipDamageCalculation->setItem(rowCount,2, new QTableWidgetItem(info.hitTarget));
			QPushButton* pbt_detail = new QPushButton;
			pbt_detail->setText(QString::fromLocal8Bit("查看"));

			QTableWidgetItem * detailItem= new QTableWidgetItem();
			detailItem->setData(Qt::UserRole, QVariant::fromValue<EventDetail>(info.detailInfo));
			Table_MissileVsShipDamageCalculation->setItem(rowCount, 3, detailItem);
			Table_MissileVsShipDamageCalculation->setCellWidget(rowCount,3, pbt_detail);
			connect(pbt_detail, SIGNAL(clicked()), this, SLOT(slotBtnCheckoutDetail()));
		}
	}
}

void MissileVsShipDamageCalculation::stopHandle()
{

}
