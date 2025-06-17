#include "stdafx.h"
#include "ExercisePartyAlert.h" 
#include "ByyDetectObject.h"
#include "remoteEntity.h"
#include "ByySensorSR.h"
#include "JGeoCal/JGeoCalExp.h"
#include "JudgeMathFun.h"

ExercisePartyAlert::ExercisePartyAlert(VerdictBaseWidget* parent /*= nullptr*/)
	: VerdictBaseWidget(parent)
{
	createWidget();
	setupLayout();
	updateEntities();
	resetUi();
}

void ExercisePartyAlert::createButton()
{
	//演练方预警判断
	Button_ExercisePartyAlert = new QPushButton;
	Button_ExercisePartyAlert->setText(START_QUERY);
	Button_ExercisePartyAlert->setEnabled(false);
	connect(Button_ExercisePartyAlert, SIGNAL(clicked()), this, SLOT(slotBtnExercisePartyAlert()));
}

void ExercisePartyAlert::createChart()
{

}

//演练方预警判断
void ExercisePartyAlert::createComBox()
{
	ComboBox_ExercisePartyAlert_Sides = new QComboBox;
	ComboBox_ExercisePartyAlert_Sides->addItem(RED_SIDE);
	ComboBox_ExercisePartyAlert_Sides->addItem(BLUE_SIDE);
	ComboBox_ExercisePartyAlert_Sides->setStyleSheet("QComboBox {combobox-popup: 1;}");
	ComboBox_ExercisePartyAlert_Sides->setEditable(true);
	ComboBox_ExercisePartyAlert_Sides->lineEdit()->setAlignment(Qt::AlignCenter);
	ComboBox_ExercisePartyAlert_Sides->lineEdit()->setReadOnly(true);
	//connect(ComboBox_ExercisePartyAlert_Sides,SIGNAL(currentIndexChanged(int)),this,SLOT(slotComAcs()));
}

//演练方预警判断
void ExercisePartyAlert::createTableView()
{
	QStringList HeaderList_ExercisePartyAlert;
	HeaderList_ExercisePartyAlert<<QString::fromLocal8Bit("目标名称")<<QString::fromLocal8Bit("误差/精度(m)")<<QString::fromLocal8Bit("是否预警")<< QString::fromLocal8Bit("目标种类");
	Model_ExercisePartyAlert = new QStandardItemModel;
	Table_ExercisePartyAlert  = new QTableView;
	Table_ExercisePartyAlert ->setModel(Model_ExercisePartyAlert);
	Model_ExercisePartyAlert ->setHorizontalHeaderLabels(HeaderList_ExercisePartyAlert);
	Table_ExercisePartyAlert ->setFocusPolicy(Qt::NoFocus);
	Table_ExercisePartyAlert ->setEditTriggers(QAbstractItemView::NoEditTriggers);
	Table_ExercisePartyAlert ->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	Table_ExercisePartyAlert ->horizontalHeader()->setStyleSheet("QHeaderView::section{background:white;color: black;}");
	Table_ExercisePartyAlert ->verticalHeader()->hide();
	Table_ExercisePartyAlert ->verticalHeader()->setDefaultSectionSize(30);
}

void ExercisePartyAlert::setupLayout()
{
	Layout_ExercisePartyAlert = new QVBoxLayout ;
	QHBoxLayout* tmpLayout = new QHBoxLayout ;
	QWidget * tmpWidget= new QWidget;

	tmpLayout->addWidget(ComboBox_ExercisePartyAlert_Sides);
	tmpLayout->addWidget(Button_ExercisePartyAlert);
	tmpWidget->setLayout( tmpLayout);

	Layout_ExercisePartyAlert->addWidget(tmpWidget);
	Layout_ExercisePartyAlert->addWidget(Table_ExercisePartyAlert);


	Label_StrAirCraftTotalCount = new QLabel(QString::fromLocal8Bit("飞机目标总个数："));
	Label_AirCraftTotalCount = new QLabel(QString::fromLocal8Bit("0"));
	Label_AirCraftTotalCount->setAlignment(Qt::AlignCenter);
	Label_Coverage1 = new QLabel(QString::fromLocal8Bit("预警覆盖率："));
	Label_Percent1 =new QLabel(QString::fromLocal8Bit("0%"));
	Label_Percent1->setAlignment(Qt::AlignCenter);
	tmpLayout = new QHBoxLayout ;
	tmpWidget= new QWidget;
	tmpLayout->addWidget(Label_StrAirCraftTotalCount);
	tmpLayout->addWidget(Label_AirCraftTotalCount);
	tmpLayout->addWidget(Label_Coverage1);
	tmpLayout->addWidget(Label_Percent1);
	tmpWidget->setLayout( tmpLayout);
	Layout_ExercisePartyAlert->addWidget(tmpWidget);

	Label_StrShipTotalCount = new QLabel(QString::fromLocal8Bit("舰艇目标总个数："));
	Label_ShipTotalCount = new QLabel(QString::fromLocal8Bit("0"));
	Label_ShipTotalCount->setAlignment(Qt::AlignCenter);
	Label_Coverage2 = new QLabel(QString::fromLocal8Bit("预警覆盖率："));
	Label_Percent2 =new QLabel(QString::fromLocal8Bit("0%"));
	Label_Percent2->setAlignment(Qt::AlignCenter);
	tmpLayout = new QHBoxLayout ;
	tmpWidget= new QWidget;
	tmpLayout->addWidget(Label_StrShipTotalCount);
	tmpLayout->addWidget(Label_ShipTotalCount);
	tmpLayout->addWidget(Label_Coverage2);
	tmpLayout->addWidget(Label_Percent2);
	tmpWidget->setLayout( tmpLayout);
	Layout_ExercisePartyAlert->addWidget(tmpWidget);

	Label_StrSubmarineTotalCount = new QLabel(QString::fromLocal8Bit("潜艇目标总个数："));
	Label_SubmarineTotalCount = new QLabel(QString::fromLocal8Bit("0"));
	Label_SubmarineTotalCount->setAlignment(Qt::AlignCenter);
	Label_Coverage3 = new QLabel(QString::fromLocal8Bit("预警覆盖率："));
	Label_Percent3 =new QLabel(QString::fromLocal8Bit("0%"));
	Label_Percent3->setAlignment(Qt::AlignCenter);
	tmpLayout = new QHBoxLayout ;
	tmpWidget= new QWidget;
	tmpLayout->addWidget(Label_StrSubmarineTotalCount);
	tmpLayout->addWidget(Label_SubmarineTotalCount);
	tmpLayout->addWidget(Label_Coverage3);
	tmpLayout->addWidget(Label_Percent3);
	tmpWidget->setLayout( tmpLayout);
	Layout_ExercisePartyAlert->addWidget(tmpWidget);

	Label_StrMissileTotalCount = new QLabel(QString::fromLocal8Bit("导弹目标总个数："));
	Label_MissileTotalCount = new QLabel(QString::fromLocal8Bit("0"));
	Label_MissileTotalCount->setAlignment(Qt::AlignCenter);
	Label_Coverage4 = new QLabel(QString::fromLocal8Bit("预警覆盖率："));
	Label_Percent4 =new QLabel(QString::fromLocal8Bit("0%"));
	Label_Percent4->setAlignment(Qt::AlignCenter);
	tmpLayout = new QHBoxLayout ;
	tmpWidget= new QWidget;
	tmpLayout->addWidget(Label_StrMissileTotalCount);
	tmpLayout->addWidget(Label_MissileTotalCount);
	tmpLayout->addWidget(Label_Coverage4);
	tmpLayout->addWidget(Label_Percent4);
	tmpWidget->setLayout( tmpLayout);
	Layout_ExercisePartyAlert->addWidget(tmpWidget);
	Button_ExercisePartyAlert->setEnabled(true);
	setLayout(Layout_ExercisePartyAlert);
}

void ExercisePartyAlert::resetUi()
{
	totalAirCraftCount = 0;
	totalShipCount = 0;
	totalSubmarineCount = 0;
	totalMissileCount = 0;
	totalTorpedoCount = 0;

	Label_AirCraftTotalCount->setText(QString::number(totalAirCraftCount)) ;
	Label_ShipTotalCount->setText(QString::number(totalShipCount));
	Label_SubmarineTotalCount->setText(QString::number(totalSubmarineCount));
	Label_MissileTotalCount->setText(QString::number(totalMissileCount));
	Model_ExercisePartyAlert->removeRows(0, Model_ExercisePartyAlert->rowCount());
}

void ExercisePartyAlert::stopHandle()
{
	slotBtnExercisePartyAlert();
}

void ExercisePartyAlert::slotBtnExercisePartyAlert()
{
	if(isDataUpdata())
	{
		ComboBox_ExercisePartyAlert_Sides->setEnabled(true);
		stopDataHandle();
		Button_ExercisePartyAlert->setText(START_QUERY);
	}
	else
	{
		resetUi();
		ComboBox_ExercisePartyAlert_Sides->setEnabled(false);
		startDataHandle(1000);
		Button_ExercisePartyAlert->setText(STOP_QUERY);
	}
}

void ExercisePartyAlert::handleDataFrame()
{
	updateEntities();
	//复位
	totalAirCraftCount = 0;
	totalShipCount = 0;
	totalSubmarineCount = 0;
	totalMissileCount = 0;
	totalTorpedoCount = 0;

	alertedAirCraftCount = 0;
	alertedShipCount = 0;
	alertedSubmarineCount = 0;
	alertedMissileCount = 0;
	alertedTorpedoCount = 0;

	EntityAlertInfos infos;
	//统计范围总数和告警情况
	QString currentSide = ComboBox_ExercisePartyAlert_Sides->currentText();
	QStringList antiEntities = getEntitiesByParty(getAntiPartyName(currentSide));
	foreach(QString antiEntityName, antiEntities)
	{
		bool isAlerted = false;
		double resolution = 0.0;
		ByyEntityObject* antiEntityObj = getEntityByName(antiEntityName);
		if(isInPartySensorsFoV(antiEntityObj, currentSide, isAlerted, resolution))
		{
			EntityAlertInfo info;
			info.entityName = antiEntityName;
			info.isAlerted = isAlerted;
			info.resolution = resolution;
			infos.append(info);
			//统计数据
			if(isAircraft(antiEntityObj))
			{
				totalAirCraftCount++;
				if(isAlerted)
				{
					alertedAirCraftCount++;
				}
			}
			else if(isShip(antiEntityObj))
			{
				totalShipCount++;
				if(isAlerted)
				{
					alertedShipCount++;
				}
			}
			else if(isMissile(antiEntityObj))
			{
				totalMissileCount++;
				if(isAlerted)
				{
					alertedMissileCount++;
				}
			}
			else if(isSubmarine(antiEntityObj))
			{
				totalSubmarineCount++;
				if(isAlerted)
				{
					alertedSubmarineCount++;
				}
			}
			else if(isTorpedo(antiEntityObj))
			{
				totalTorpedoCount++;
				if(isAlerted)
				{
					alertedTorpedoCount++;
				}
			}
		}
	}	
	qSort(infos.begin(), infos.end(), infoLessThan);
	//数据显示
	updateEntityAlertInfos(infos);
}

double calculateDecimal(double num1, double num2)
{
	if(qAbs(num2 - 0.0) < 0.000001)
	{
		return 0.0 ;
	}
	return num1 / num2 * 100.0;
}

void ExercisePartyAlert::updateEntityAlertInfos(const EntityAlertInfos& infos)
{
	//数据复位
	Model_ExercisePartyAlert->removeRows(0, Model_ExercisePartyAlert->rowCount()); //清空所有的数据

	foreach(EntityAlertInfo info, infos) 
	{
		QList<QStandardItem*> items;
		items.append(new QStandardItem(info.entityName));
		items.append(new QStandardItem(QString("%1").arg(info.resolution)));
		items.append(new QStandardItem(QString("%1").arg(info.isAlerted ? QString::fromLocal8Bit("是"):QString::fromLocal8Bit("否"))));
		items.append(new QStandardItem(QString("%1").arg(getTypeName(getEntityByName(info.entityName)))));
		Model_ExercisePartyAlert->appendRow(items);
	}

	Label_AirCraftTotalCount->setText(QString::number(totalAirCraftCount)) ;
	Label_ShipTotalCount->setText(QString::number(totalShipCount));
	Label_SubmarineTotalCount->setText(QString::number(totalSubmarineCount));
	Label_MissileTotalCount->setText(QString::number(totalMissileCount));
	
	Label_Percent1->setText(QString("%1%").arg(calculateDecimal(static_cast<double>(totalAirCraftCount),	static_cast<double>(alertedAirCraftCount))));
	Label_Percent2->setText(QString("%1%").arg(calculateDecimal(static_cast<double>(totalShipCount),		static_cast<double>(alertedShipCount))));
	Label_Percent3->setText(QString("%1%").arg(calculateDecimal(static_cast<double>(totalSubmarineCount),	static_cast<double>(alertedSubmarineCount))));
	Label_Percent4->setText(QString("%1%").arg(calculateDecimal(static_cast<double>(totalMissileCount),		static_cast<double>(alertedMissileCount))));
}

bool ExercisePartyAlert::isInPartySensorsFoV(ByyEntityObject* entity, const QString& partyName, bool &isAlerted, double &resolution)
{
	bool ret = false;
	QStringList defendEntities = getEntitiesByParty(partyName);
	double resolutionLocal = 1000000.0; //极大值误差
	double isAlertedLocal = false;
	foreach(QString defendEntity, defendEntities)
	{
		ByyEntityObject* currentEntity = getEntityByName(defendEntity);
		ByyDescribeEntry *descEntry = currentEntity->describeEntry();
		ByyDescribeEntry::ComponentList componentList = descEntry->getComponent("sensor");	//切换不同实体，同时会切换对应传感器	
		ByyRemoteEntity *remoteEnt = dynamic_cast<ByyRemoteEntity*>(currentEntity);
		if (!remoteEnt)
			return ret;
		ByySensorSR* CurrentSensorSR = remoteEnt->getOrCreateSR<ByySensorSR>();
		if(!CurrentSensorSR)
			return ret;
		for (int i = 0;i<componentList.count();i++)
		{
			ByySensorSR::DeviceState *devState = CurrentSensorSR->getDeviceState(componentList[i].getTag());
			//ByySensorSR::DeviceState *devState=0;
			//如果不在范围内，需要判断，已经在范围内了，不在判断
			if( devState && !ret)
			{
				JGeoCal::TopocentricPolar tcp;
				JGeoCal::GeoPt selfPos,tgtPos;
				ByyDescribeComponent component;
				component = componentList[i];
				const ByyDescParamList& paramList=component.getParamList();
	
				JGeoCal::FoVDS fov;

				double distResolution = paramList.value("DistResolution").toDouble();
				if(0 == distResolution)
				{
					distResolution = paramList.value("RangeErr").toDouble();
				}
				distResolution *= genRandom();
				if(resolutionLocal >= distResolution)
				{
					resolutionLocal = distResolution;
				}

				fov.m_FoVType= paramList.value("FOVType").toInt();/*视场类型（0-null、1-简单锥形、2-复杂锥形、3-矩形）*/
				fov.m_ConeAngle = paramList.value("ConeAngle").toInt();/*简单锥形半角（deg）*/
				// 复杂锥形参数
				fov.m_InnerHalfAngle = paramList.value("InnerHalfAngle").toDouble();
				fov.m_OuterHalfAngle = paramList.value("OuterHalfAngle").toDouble();
				fov.m_MinClockAngle = paramList.value("MinClockAngle").toDouble();
				fov.m_MaxClockAngle = paramList.value("MaxClockAngle").toDouble();
				//fov.m_VertHalfAngle = paramList.value("").toDouble();/*矩形垂直方向半角,deg*/
				//fov.m_HoriHalfAngle = paramList.value("").toDouble();/*矩形水平方向半角,deg*/
				// 视场指向（瞄准轴）参数
				fov.m_PointingType = paramList.value("PointingType").toLong(); /*0-null、1-fixed in Topocentric Axes、2-fixed in Body Axes、3-target*/
				fov.m_BoresightAz = paramList.value("FixedBoresightAz").toDouble();
				fov.m_BoresightEl = paramList.value("FixedBoresightEl").toDouble();
				// 距离范围参数
				fov.m_MinRange = paramList.value("MinDetectDistance").toDouble();/*最小探测距离(km)*/
				fov.m_MaxRange = paramList.value("MaxDetectDistance").toDouble();/*最大探测距离(km)*/
				// 无视场指向（方位角以正北为0、高低角以水平为0）或无明确视场类型时，用下述描述参数
				fov.m_MinAz = paramList.value("MinAz").toDouble();/*探测方位角小值（以传感器瞄准轴坐标系的x轴为基准，绕z正向旋转时x正向的夹角为正，0-360或者-180～180可以为负值，表示从负到正的范围）*/;
				fov.m_MaxAz = paramList.value("MaxAz").toDouble();/*探测方位角大值（定义同前）*/
				fov.m_MinEl = paramList.value("MinEl").toDouble();/*探测高低角小值（与传感器轴系的xy平面的夹角，偏向z轴正向，则>0，-90～+90）*/
				fov.m_MaxEl = paramList.value("MaxEl").toDouble();/*探测高低角大值（定义同前）*/
				//fov.m_MinAltitude/*最小海拔高度,km*/
				//fov.m_MaxAltitude/*最大海拔高度,km*/
				if(entityAlertBusiness(currentEntity, entity, fov, tcp))
				{
					ret = true;
				}
			}
			if(devState && !isAlertedLocal)
			{
				foreach(ByyDetectObject* detectObj, devState->myDetectObjList.values())
				{
					if(detectObj->name() == entity->name())
					{
						isAlertedLocal = true;
					}
				}
			}
		}
	}
	isAlerted = isAlertedLocal;
	resolution = resolutionLocal;
	return ret;
}

