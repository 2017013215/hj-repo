#include "stdafx.h"
#include "SensorAlarmCalculation.h" 
#include "ByyDetectObject.h"
#include "ByySensorSR.h"
#include "remoteEntity.h"
#include "JGeoCal/JGeoCalExp.h"

static const int AXIS_MAX_X = 10, AXIS_MAX_Y = 1;/* 横纵坐标最大显示范围 */

SensorAlarmCalculation::SensorAlarmCalculation(VerdictBaseWidget* parent /*= nullptr*/)
	: VerdictBaseWidget(parent)
{
	setSingleFlage(false);
	createWidget();
	setupLayout();
	updateEntities();
	resetUi();
}

void SensorAlarmCalculation::showEntityName(const QString& entityName)
{
	ByyEntityObject* entityObject = getEntityByName(entityName);
	if(entityObject)
	{
		setSingleFlage(true);
		ComboBox_SensorAlarmCalculation_Red->setEditable(false);
		ComboBox_SensorAlarmCalculation_Red->setCurrentIndex(ComboBox_SensorAlarmCalculation_Red->findText(entityName));
		slotBtnSensorAlarmCalculation();
	}
}

//传感器是否告警
void SensorAlarmCalculation::createButton()
{
	Button_SensorAlarmCalculation = new QPushButton(this);
	Button_SensorAlarmCalculation->setText(QString::fromLocal8Bit("查看"));
	//Button_SensorAlarmCalculation->setEnabled(false);
	connect(Button_SensorAlarmCalculation, SIGNAL(clicked()), this, SLOT(slotBtnSensorAlarmCalculation()));
}

//传感器是否告警
void SensorAlarmCalculation::createChart()
{
	m_axisA_SensorAlarm = new QValueAxis;
	m_axisR_SensorAlarm = new QValueAxis;
	m_axisA_SensorAlarm->setTickCount(9);
	m_axisA_SensorAlarm->setRange(0,360);//角度范围
	//m_axisA->setLabelsAngle(180);
	m_axisA_SensorAlarm->setLabelFormat("%d");
	m_axisR_SensorAlarm->setTickCount(9);
	m_axisR_SensorAlarm->setRange(0,400);//距离范围
	//m_axisR->setLabelsAngle(180);
	m_axisR_SensorAlarm->setLabelFormat("%d");

	chartView_SensorAlarm = new QChartView;

	PolarChart_SensorAlarm = new QPolarChart();           // 创建一个极坐标系图表
	PolarChart_SensorAlarm->legend()->setVisible(true);  // 隐藏图例
	PolarChart_SensorAlarm->addAxis(m_axisA_SensorAlarm, QPolarChart::PolarOrientationAngular);
	PolarChart_SensorAlarm->addAxis(m_axisR_SensorAlarm, QPolarChart::PolarOrientationRadial);

	PolarChart_SensorAlarm->setBackgroundVisible(false);
	chartView_SensorAlarm->setChart(PolarChart_SensorAlarm);
}

//传感器是否告警
void SensorAlarmCalculation::createComBox()
{
	ComboBox_SensorAlarmCalculation_Red = new QComboBox;
	ComboBox_SensorAlarmCalculation_Red->addItem(QString::fromLocal8Bit("红方"));
	ComboBox_SensorAlarmCalculation_Red->setStyleSheet("QComboBox {combobox-popup: 1;}");
	ComboBox_SensorAlarmCalculation_Red->setEditable(true);
	ComboBox_SensorAlarmCalculation_Red->lineEdit()->setAlignment(Qt::AlignCenter);
	ComboBox_SensorAlarmCalculation_Red->lineEdit()->setReadOnly(true);
	connect(ComboBox_SensorAlarmCalculation_Red,SIGNAL(currentIndexChanged(int)),this,SLOT(slotSensorAlarmCalculationRed()));
}

void SensorAlarmCalculation::createTableView()
{
	/*水平表头*/
	QStringList HeaderList_SensorAlarm;
	HeaderList_SensorAlarm<<QString::fromLocal8Bit("目标名称")<<QString::fromLocal8Bit("告警距离")<<QString::fromLocal8Bit("是否告警");
	Model_SensorAlarm = new QStandardItemModel;

	Table_SensorAlarm = new QTableView;
	Table_SensorAlarm->setModel(Model_SensorAlarm);
	Model_SensorAlarm->setHorizontalHeaderLabels(HeaderList_SensorAlarm);
	Table_SensorAlarm->setFocusPolicy(Qt::NoFocus);
	Table_SensorAlarm->setEditTriggers(QAbstractItemView::NoEditTriggers);
	Table_SensorAlarm->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	Table_SensorAlarm->horizontalHeader()->setStyleSheet("QHeaderView::section{background:white;color: black;}");
	Table_SensorAlarm->verticalHeader()->hide();
	Table_SensorAlarm->verticalHeader()->setDefaultSectionSize(30);
}

void SensorAlarmCalculation::createLog()
{
}

void SensorAlarmCalculation::setupLayout()
{
	Layout_SensorAlarmCalculation = new QVBoxLayout ;
	QHBoxLayout* tmpLayout = new QHBoxLayout ;
	QWidget * tmpWidget= new QWidget;

	tmpLayout->addWidget(ComboBox_SensorAlarmCalculation_Red);
	//tmpLayout->addWidget(ComboBox_SensorAlarmCalculation_Sensor);
	tmpLayout->addWidget(Button_SensorAlarmCalculation);
	tmpWidget->setLayout( tmpLayout);
	Layout_SensorAlarmCalculation->addWidget( tmpWidget);

	tmpLayout = new QHBoxLayout ;
	tmpWidget= new QWidget;
	tmpLayout->addWidget(chartView_SensorAlarm);
	tmpLayout->addWidget(Table_SensorAlarm);
	tmpWidget->setLayout(tmpLayout);
	Layout_SensorAlarmCalculation->addWidget( tmpWidget);

	setLayout(Layout_SensorAlarmCalculation);
}

void SensorAlarmCalculation::resetUi()
{
	ComboBox_SensorAlarmCalculation_Red->clear();
	QStringList airobjects;
	airobjects.append(getEntitiesByType(RED_SIDE, AirEntityObject));
	airobjects.append(getEntitiesByType(BLUE_SIDE, AirEntityObject));
	ComboBox_SensorAlarmCalculation_Red->addItems(airobjects);
}

void SensorAlarmCalculation::preHandleDataFrame()
{
	Model_SensorAlarm->removeRows(0, Model_SensorAlarm->rowCount()); //清空所有的数据
	PolarChart_SensorAlarm->removeAllSeries();
	ScatterSeries_SensorAlarms.clear();
}

void SensorAlarmCalculation::handleDataFrame()
{
	SensorAlarmDatas infos;
	updateEntities();
	QString selfEntityName = ComboBox_SensorAlarmCalculation_Red->currentText();
	ByyEntityObject* selfEntity = getEntityByName(selfEntityName);
	//我方是否在地方的辐照范围内
	ByySensorSR* selfSensorSR = getSensorByName(selfEntityName);
	if(!selfSensorSR)
	{
		return;
	}
	if(!selfEntity) return;
	QStringList antiEntitites = getEntitiesByParty(getAntiPartyName(getPartyName(selfEntity)));

	//查找 ESMTAG
	QString sensorEsmTag;
	foreach(ByyDescribeComponent component, selfEntity->describeEntry()->getComponent("sensor"))
	{
		const ByyDescParamList&  selfSensorParamList = component.getParamList();
		if(QString::number(1004) == selfSensorParamList.value("SensorType"))
		{
			sensorEsmTag = component.getTag();
			break;
		}
	}

	foreach(QString antiEntityName, antiEntitites)
	{
		//单个敌方的计算
		ByyEntityObject* antiEnt = getEntityByName(antiEntityName);
		//我方是否在地方的辐照范围内
		ByySensorSR* myCurrentSensorSR = getSensorByName(antiEntityName);
		if(!myCurrentSensorSR)
		{
			continue;
		}

		//获取传感器视场参数
		JGeoCal::FoVDS fov;
		JGeoCal::TopocentricPolar tcp;
		QStringList sensorList = myCurrentSensorSR->getDeviceList()->keys();
		ByyDescribeEntry::ComponentList componentList = antiEnt->describeEntry()->getComponent("sensor");	//切换不同实体，同时会切换对应传感器
		foreach(QString sensorDevice, sensorList)
		{
			//如果没有开机不用计算
			if(!myCurrentSensorSR->getDeviceState(sensorDevice)->myPowerOn)
			{
				continue;
			}
			for (int i = 0; i < componentList.count(); i++)
			{
				if (sensorDevice == componentList[i].getTag())
				{
					const ByyDescParamList&  selfSensorParamList = componentList[i].getParamList();
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

					//我方在敌方的辐照范围内
					if(entityAlertBusiness(antiEnt, selfEntity, fov, tcp))
					{
						SensorAlarmData info;
						info.entityName = antiEntityName;
						info.distance = tcp.r /1000;
						info.r = tcp.r/1000;
						info.orientation = JGeoCal::Math::zeroTo360(tcp.Az);
						//是否发现了敌方
						info.isAlarmed = selfSensorSR->isTracked(antiEntityName, sensorEsmTag);
						infos.append(info);
					}
				}
			}
		}
	}

	updataSensorAlarmCalculationData(infos);
}

void SensorAlarmCalculation::updataSensorAlarmCalculationData(const SensorAlarmDatas& infos)
{
	Model_SensorAlarm->removeRows(0, Model_SensorAlarm->rowCount()); //清空所有的数据
	foreach(SensorAlarmData info, infos)
	{
		QList<QStandardItem*> items;
		items.append(new QStandardItem(info.entityName));
		items.append(new QStandardItem(QString::number(info.distance, 'f', 2)));
		items.append(new QStandardItem(QString("%1").arg(info.isAlarmed ? QString::fromLocal8Bit("是"):QString::fromLocal8Bit("否"))));
		Model_SensorAlarm->appendRow(items);
		if(ScatterSeries_SensorAlarms.contains(info.entityName))
		{
			ScatterSeries_SensorAlarms.value(info.entityName)->append(info.orientation, info.r);
		}
		else
		{
			QScatterSeries *ScatterSeries_SensorAlarm = new QScatterSeries;
			PolarChart_SensorAlarm->addSeries(ScatterSeries_SensorAlarm);                         // 将创建的series添加进图表中
			ScatterSeries_SensorAlarm->setName(info.entityName);
			ScatterSeries_SensorAlarm->setMarkerSize(5);
			ScatterSeries_SensorAlarm->setPen(QPen(QColor(34,34,34), 2));
			ScatterSeries_SensorAlarm->attachAxis(m_axisA_SensorAlarm);
			ScatterSeries_SensorAlarm->attachAxis(m_axisR_SensorAlarm);
			ScatterSeries_SensorAlarm->append(info.orientation, info.r);
			ScatterSeries_SensorAlarms.insert(info.entityName, ScatterSeries_SensorAlarm);
		}
	}
}

void SensorAlarmCalculation::stopHandle()
{
	slotBtnSensorAlarmCalculation();
}

void SensorAlarmCalculation::slotBtnSensorAlarmCalculation()
{
	if(isDataUpdata())
	{
		stopDataHandle();
		if(!getSingleFlage())
		{
			ComboBox_SensorAlarmCalculation_Red->setEnabled(true);
		}
		Button_SensorAlarmCalculation->setText(START_QUERY);
	}
	else
	{
		startDataHandle(1000);
		ComboBox_SensorAlarmCalculation_Red->setEnabled(false);
		Button_SensorAlarmCalculation->setText(STOP_QUERY);
	}
}

void SensorAlarmCalculation::slotSensorAlarmCalculationRed(int index)
{

}

