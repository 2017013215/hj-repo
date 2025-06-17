#include "stdafx.h"
#include "entityalert.h"

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QTableView>
#include <QComboBox>
#include "JudgeMathFun.h"
#include "ByyDetectObject.h"
#include "ByySensorSR.h"
#include "remoteEntity.h"

#include <QDebug>

EntityAlert::EntityAlert(VerdictBaseWidget* parent /*= nullptr*/)
	: VerdictBaseWidget(parent)
{
	setSingleFlage(false);
	createWidget();
	setupLayout();
	updateEntities();
	resetUi();
}

void EntityAlert::showEntityName(const QString& entityName)
{
	ByyEntityObject* entityObject = getEntityByName(entityName);
	if(entityObject)
	{
		setSingleFlage(true);
		ComboBox_EntityAlert_Sides->setEnabled(false);
		ComboBox_EntityAlert_Sides->hide();
		ComboBox_EntityAlert_Sides->setCurrentIndex(getPartyNameIndex(getPartyName(entityObject)));
		ComboBox_EntityAlert_Entity->setCurrentIndex(ComboBox_EntityAlert_Entity->findText(entityName));
		slotBtnEntityAlert();
	}
}

void EntityAlert::slotSides(int index)
{
	Q_UNUSED(index)
	updateEntities();
	ComboBox_EntityAlert_Entity->clear();
	ComboBox_EntityAlert_Entity->addItems(getEntitiesByParty(ComboBox_EntityAlert_Sides->currentText()));
}

void EntityAlert::slotEntity(int index)
{
	Q_UNUSED(index)
	updateEntities();
	ComboBox_EntityAlert_Sensor->clear();
	ComboBox_EntityAlert_Sensor->addItems(getSensorsByEntity(ComboBox_EntityAlert_Sides->currentText(), ComboBox_EntityAlert_Entity->currentText()));
}

void EntityAlert::slotSensor(int index)
{
	//do nothing
}

void EntityAlert::handleDataFrame()
{
	updateEntities();
	QString selfEntityName = ComboBox_EntityAlert_Entity->currentText();
	QString deviceName = ComboBox_EntityAlert_Sensor->currentText();
	//获取sensor
	ByySensorSR* myCurrentSensorSR = getSensorByName(ComboBox_EntityAlert_Sides->currentText(), selfEntityName);
	if(!myCurrentSensorSR)
	{
		return;
	}

	//更新状态
	ByySensorSR::DeviceState *devState = myCurrentSensorSR->getDeviceState(deviceName);
	if( !devState )
		return;
	if(devState->myPowerOn)
	{
		Label_TurnOff->setText(QString::fromLocal8Bit("开机"));
	}
	else
	{
		Label_TurnOff->setText(QString::fromLocal8Bit("关机"));
	}

	EntityAlertInfos infos;
	//获取敌对全局目标,判断是否在视场范围内	
	//获取传感器视场参数
	JGeoCal::FoVDS fov;
	double distResolution = 0.0;
	ByyDescribeEntry::ComponentList componentList = getEntityByName(selfEntityName)->describeEntry()->getComponent("sensor");	//切换不同实体，同时会切换对应传感器
	for (int i = 0; i < componentList.count(); i++)
	{
		if (deviceName == componentList[i].getTag())
		{
			const ByyDescParamList&  selfSensorParamList = componentList[i].getParamList();
			distResolution = selfSensorParamList.value("DistResolution").toDouble();
			if(0 == distResolution)
			{
				distResolution = selfSensorParamList.value("RangeErr").toDouble();
			}

			//distResolution *= genRandom();
			
			//-1到1
			fov.m_FoVType	= selfSensorParamList.value("FOVType").toInt();/*视场类型（0-null、1-简单锥形、2-复杂锥形、3-矩形）*/
			fov.m_ConeAngle = selfSensorParamList.value("ConeAngle").toInt();/*简单锥形半角（deg）*/
			// 复杂锥形参数
			fov.m_InnerHalfAngle = selfSensorParamList.value("InnerHalfAngle").toDouble();
			fov.m_OuterHalfAngle = selfSensorParamList.value("OuterHalfAngle").toDouble();
			fov.m_MinClockAngle	 = selfSensorParamList.value("MinClockAngle").toDouble();
			fov.m_MaxClockAngle  = selfSensorParamList.value("MaxClockAngle").toDouble();
			//fov.m_VertHalfAngle = paramList.value("").toDouble();/*矩形垂直方向半角,deg*/
			//fov.m_HoriHalfAngle = paramList.value("").toDouble();/*矩形水平方向半角,deg*/
			// 视场指向（瞄准轴）参数
			fov.m_PointingType = selfSensorParamList.value("PointingType").toLong(); /*0-null、1-fixed in Topocentric Axes、2-fixed in Body Axes、3-target*/
			fov.m_BoresightAz = selfSensorParamList.value("FixedBoresightAz").toDouble();
			fov.m_BoresightEl = selfSensorParamList.value("FixedBoresightEl").toDouble();
			// 距离范围参数
			fov.m_MinRange = selfSensorParamList.value("MinDetectDistance").toDouble();/*最小探测距离(km)*/
			fov.m_MaxRange = selfSensorParamList.value("MaxDetectDistance").toDouble();/*最大探测距离(km)*/
			// 无视场指向（方位角以正北为0、高低角以水平为0）或无明确视场类型时，用下述描述参数
			fov.m_MinAz = selfSensorParamList.value("MinAz").toDouble();/*探测方位角小值（以传感器瞄准轴坐标系的x轴为基准，绕z正向旋转时x正向的夹角为正，0-360或者-180～180可以为负值，表示从负到正的范围）*/;
			fov.m_MaxAz = selfSensorParamList.value("MaxAz").toDouble();/*探测方位角大值（定义同前）*/
			fov.m_MinEl = selfSensorParamList.value("MinEl").toDouble();/*探测高低角小值（与传感器轴系的xy平面的夹角，偏向z轴正向，则>0，-90～+90）*/
			fov.m_MaxEl = selfSensorParamList.value("MaxEl").toDouble();/*探测高低角大值（定义同前）*/
			//fov.m_MinAltitude/*最小海拔高度,km*/
			//fov.m_MaxAltitude/*最大海拔高度,km*/
			m_axisR_EntityAlert->setRange(fov.m_MinRange, fov.m_MaxRange);
			break;
		}
		//没有找到则退出
		if(i == (componentList.count() - 1))
		{
			return; 
		}
	}

	QStringList antiEntities = getEntitiesByParty(getAntiPartyName(getEntityByName(selfEntityName)));
	foreach(QString antiEntity, antiEntities)
	{
		JGeoCal::TopocentricPolar tcp;
		if(entityAlertBusiness(getEntityByName(selfEntityName), getEntityByName(antiEntity), fov, tcp))
		{
			EntityAlertInfo info;
			info.entityName = antiEntity;
			info.isAlerted = false;
			info.resolution = 0; //TODO
			info.sitePoint = QPointF(JGeoCal::Math::zeroTo360(tcp.Az), (tcp.r/1000));
			infos.append(info);
		}
	}

	qSort(infos.begin(), infos.end(), infoLessThan);

	//对观测目标进行显示
	int detectCount = 0;
	foreach(ByyDetectObject* detObject, devState->myDetectObjList.values())
	{
		EntityAlertInfo info;
		info.entityName = detObject->name();
		int index = infos.indexOf(info);
		if(index >= 0)
		{
			infos[index].isAlerted = true;
			infos[index].resolution = distResolution * genRandom();
			detectCount++;
		}
	}

	//计算覆盖率
	double percentage = 0.0;
	if(!infos.isEmpty())
	{
		percentage = static_cast<double>(detectCount) / static_cast<double>(infos.size()) * 100.0; 
		Label_Percent->setText(QString::number(percentage,'f',4));
	}

	//刷新数据
	updateEntityAlertInfos(infos);
}

void EntityAlert::updateEntityAlertInfos(const EntityAlertInfos& infos)
{
	Model_EntityAlert->removeRows(0, Model_EntityAlert->rowCount()); //清空所有的数据

	foreach(EntityAlertInfo info, infos) 
	{
		QList<QStandardItem*> items;
		items.append(new QStandardItem(info.entityName));
		if(info.isAlerted)
			items.append(new QStandardItem(QString("%1").arg(info.resolution)));
		else 
			items.append(new QStandardItem(QString("%1").arg("INF")));
		items.append(new QStandardItem(QString("%1").arg(info.isAlerted ? QString::fromLocal8Bit("是"):QString::fromLocal8Bit("否"))));
		Model_EntityAlert->appendRow(items);
		if(ScatterSeries_EntityAlerts.contains(info.entityName))
		{
			ScatterSeries_EntityAlerts.value(info.entityName)->append(info.sitePoint.x(), info.sitePoint.y());
		}
		else
		{
			QScatterSeries *ScatterSeries_EntityAlert = new QScatterSeries;
			PolarChart_EntityAlert->addSeries(ScatterSeries_EntityAlert);                         // 将创建的series添加进图表中
			ScatterSeries_EntityAlert->setName(info.entityName);
			ScatterSeries_EntityAlert->setMarkerSize(5);
			ScatterSeries_EntityAlert->setPen(QPen(QColor(34,34,34), 2));
			ScatterSeries_EntityAlert->attachAxis(m_axisA_EntityAlert);
			ScatterSeries_EntityAlert->attachAxis(m_axisR_EntityAlert);
			ScatterSeries_EntityAlert->append(info.sitePoint.x(), info.sitePoint.y());
			ScatterSeries_EntityAlerts.insert(info.entityName, ScatterSeries_EntityAlert);
		}
	}
}

void EntityAlert::stopHandle()
{
	slotBtnEntityAlert();
}

void EntityAlert::slotBtnEntityAlert()
{
	if(isDataUpdata())
	{
		stopDataHandle();
		if(!getSingleFlage())
		{
			ComboBox_EntityAlert_Sides->setEnabled(true);
			ComboBox_EntityAlert_Entity->setEnabled(true);
		}
		ComboBox_EntityAlert_Sensor->setEnabled(true);
		Button_EntityAlert->setText(START_QUERY);
	}
	else
	{
		Label_TurnOff->setText("");
		Label_Percent->setText("0%");
		PolarChart_EntityAlert->removeAllSeries();
		ScatterSeries_EntityAlerts.clear();
		startDataHandle(1000);
		ComboBox_EntityAlert_Sides->setEnabled(false);
		ComboBox_EntityAlert_Entity->setEnabled(false);
		ComboBox_EntityAlert_Sensor->setEnabled(false);
		Button_EntityAlert->setText(STOP_QUERY);
	}
}

void EntityAlert::createButton()
{
	//实体预警判断
	Button_EntityAlert = new QPushButton;
	Button_EntityAlert->setText(START_QUERY);
	connect(Button_EntityAlert, SIGNAL(clicked()), this, SLOT(slotBtnEntityAlert()));
}

void EntityAlert::createChart()
{
	m_axisA_EntityAlert = new QValueAxis;
	m_axisR_EntityAlert = new QValueAxis;
	m_axisA_EntityAlert->setTickCount(9);
	m_axisA_EntityAlert->setRange(0,360);//角度范围
	//m_axisA->setLabelsAngle(180);
	m_axisA_EntityAlert->setLabelFormat("%d");
	m_axisR_EntityAlert->setTickCount(9);
	m_axisR_EntityAlert->setRange(0,400);//距离范围
	//m_axisR->setLabelsAngle(180);
	m_axisR_EntityAlert->setLabelFormat("%d");

	chartView_EntityAlert = new QChartView;

	PolarChart_EntityAlert = new QPolarChart();           // 创建一个极坐标系图表
	PolarChart_EntityAlert->legend()->setVisible(true);  // 隐藏图例
	PolarChart_EntityAlert->addAxis(m_axisA_EntityAlert,QPolarChart::PolarOrientationAngular);
	PolarChart_EntityAlert->addAxis(m_axisR_EntityAlert,QPolarChart::PolarOrientationRadial);

	PolarChart_EntityAlert->setBackgroundVisible(false);
	chartView_EntityAlert->setChart(PolarChart_EntityAlert);
}

void EntityAlert::createComBox()
{
	//实体预警判断
	ComboBox_EntityAlert_Sides = new QComboBox;
	ComboBox_EntityAlert_Sides->addItem(RED_SIDE);
	ComboBox_EntityAlert_Sides->addItem(BLUE_SIDE);
	ComboBox_EntityAlert_Sides->setStyleSheet("QComboBox {combobox-popup: 1;}");
	ComboBox_EntityAlert_Sides->setEditable(true);
	ComboBox_EntityAlert_Sides->lineEdit()->setAlignment(Qt::AlignCenter);
	ComboBox_EntityAlert_Sides->lineEdit()->setReadOnly(true);
	connect(ComboBox_EntityAlert_Sides,SIGNAL(currentIndexChanged(int)),this,SLOT(slotSides(int)));

	ComboBox_EntityAlert_Entity = new QComboBox;
	ComboBox_EntityAlert_Entity->setStyleSheet("QComboBox {combobox-popup: 1;}");
	ComboBox_EntityAlert_Entity->setEditable(true);
	ComboBox_EntityAlert_Entity->lineEdit()->setAlignment(Qt::AlignCenter);
	ComboBox_EntityAlert_Entity->lineEdit()->setReadOnly(true);
	ComboBox_EntityAlert_Entity->addItem(QString::fromLocal8Bit("实体"));
	connect(ComboBox_EntityAlert_Entity,SIGNAL(currentIndexChanged(int)),this,SLOT(slotEntity(int)));

	ComboBox_EntityAlert_Sensor = new QComboBox;
	ComboBox_EntityAlert_Sensor->setStyleSheet("QComboBox {combobox-popup: 1;}");
	ComboBox_EntityAlert_Sensor->setEditable(true);
	ComboBox_EntityAlert_Sensor->lineEdit()->setAlignment(Qt::AlignCenter);
	ComboBox_EntityAlert_Sensor->lineEdit()->setReadOnly(true);
	ComboBox_EntityAlert_Sensor->addItem(QString::fromLocal8Bit("传感器"));
	connect(ComboBox_EntityAlert_Sensor,SIGNAL(currentIndexChanged(int)),this,SLOT(slotSensor(int)));
}

void EntityAlert::createTableView()
{
	/*水平表头*/
	QStringList HeaderList_EntityAlert;
	HeaderList_EntityAlert<<QString::fromLocal8Bit("目标名称")<<QString::fromLocal8Bit("误差/精度(m)")<<QString::fromLocal8Bit("是否预警");
	Model_EntityAlert = new QStandardItemModel;

	Table_EntityAlert = new QTableView;
	Table_EntityAlert->setModel(Model_EntityAlert);
	Model_EntityAlert->setHorizontalHeaderLabels(HeaderList_EntityAlert);
	Table_EntityAlert->setFocusPolicy(Qt::NoFocus);
	Table_EntityAlert->setEditTriggers(QAbstractItemView::NoEditTriggers);
	Table_EntityAlert->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	Table_EntityAlert->horizontalHeader()->setStyleSheet("QHeaderView::section{background:white;color: black;}");
	Table_EntityAlert->verticalHeader()->hide();
	Table_EntityAlert->verticalHeader()->setDefaultSectionSize(30);
}

void EntityAlert::setupLayout() 
{
	Layout_EntityAlert = new QGridLayout ;
	//Label_Empty_1 = new QLabel;
	//Label_Empty_2 = new QLabel;
	Label_TurnOff_State = new QLabel(QString::fromLocal8Bit("开关机状态："));
	Label_TurnOff = new QLabel(QString::fromLocal8Bit("关机"));
	Label_TurnOff->setAlignment(Qt::AlignCenter);
	Label_Coverage = new QLabel(QString::fromLocal8Bit("预警覆盖率："));
	Label_Percent =new QLabel(QString::fromLocal8Bit("0%"));
	Label_Percent->setAlignment(Qt::AlignCenter);

	Layout_EntityAlert->addWidget(ComboBox_EntityAlert_Sides,0,0,1,1);
	//Layout_EntityAlert->addWidget(Label_Empty_1,0,1,1,1);
	Layout_EntityAlert->addWidget(ComboBox_EntityAlert_Entity,0,1,1,1);
	//Layout_EntityAlert->addWidget(Label_Empty_1,0,3,1,1);
	Layout_EntityAlert->addWidget(ComboBox_EntityAlert_Sensor,0,2,1,1);
	//Layout_EntityAlert->addWidget(Label_Empty_1,0,5,1,1);
	Layout_EntityAlert->addWidget(Button_EntityAlert,0,3,1,1);

	Layout_EntityAlert->addWidget(chartView_EntityAlert,1,0,4,4);
	Layout_EntityAlert->addWidget(Label_TurnOff_State,5,0);
	Layout_EntityAlert->addWidget(Label_TurnOff,5,1);
	Layout_EntityAlert->addWidget(Label_Coverage,5,2);
	Layout_EntityAlert->addWidget(Label_Percent,5,3);
	Layout_EntityAlert->addWidget(Table_EntityAlert,1,4,4,4);
	setLayout(Layout_EntityAlert);
}

void EntityAlert::resetUi()
{
	slotSides(ComboBox_EntityAlert_Sides->currentIndex());
	//ComboBox_EntityAlert_Entity->clear();
	//ComboBox_EntityAlert_Entity->addItems(redList);
}
