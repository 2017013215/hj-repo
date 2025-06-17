#include "stdafx.h"
#include "CollisionDetection.h" 
#include "JGeoCal/JGeoCalExp.h"
#include "remoteEntity.h"
#include "ByyApp.h"
#include "ByyHdose.h"

#include <QStandardItemModel>
#include <QTimer>

CollisionDetection::CollisionDetection(VerdictBaseWidget* parent /*= nullptr*/)
	: VerdictBaseWidget(parent)
{
	m_recordSimtime = 0.0;
	createWidget();
	setupLayout();
	updateEntities();
	resetUi();
}

// 碰撞检测
void CollisionDetection::createButton()
{
	Button_CollisionDetection = new QPushButton;
	Button_CollisionDetection->setText(QString::fromLocal8Bit("查看"));
	//Button_CollisionDetection->setEnabled(false);
	connect(Button_CollisionDetection, SIGNAL(clicked()), this, SLOT(slotBtnCollisionDetection()));

	SpinBox_CollisionDetection = new QSpinBox;
	SpinBox_CollisionDetection->setMinimumSize(QSize(200, 0));
	SpinBox_CollisionDetection->setMaximum(5000);
	SpinBox_CollisionDetection->setMinimum(1);
	SpinBox_CollisionDetection->setAlignment(Qt::AlignCenter);
	SpinBox_CollisionDetection->setSuffix(QString::fromLocal8Bit("千米(阈值)"));
	SpinBox_CollisionDetection->setValue(100);
}

void CollisionDetection::createChart()
{

}

// 碰撞检测
void CollisionDetection::createComBox()
{
	QStringList EntityList;
	EntityList<<QString::fromLocal8Bit("空中碰撞检测")<<QString::fromLocal8Bit("水面碰撞检测")<<QString::fromLocal8Bit("地面碰撞检测")<<QString::fromLocal8Bit("水下碰撞检测");
	ComboBox_CollisionDetection_Type = new QComboBox;
	ComboBox_CollisionDetection_Type->setEditable(true);
	ComboBox_CollisionDetection_Type->lineEdit()->setAlignment(Qt::AlignCenter);
	ComboBox_CollisionDetection_Type->lineEdit()->setReadOnly(true);
	ComboBox_CollisionDetection_Type->addItems(EntityList);
	connect(ComboBox_CollisionDetection_Type,SIGNAL(currentIndexChanged(int)),this,SLOT(slotCollisionDetectionType(int)));

	ComboBox_CollisionDetection_Entity = new QComboBox;
	ComboBox_CollisionDetection_Entity->addItem(QString::fromLocal8Bit("All"));
	ComboBox_CollisionDetection_Entity->setStyleSheet("QComboBox {combobox-popup: 1;}");
	ComboBox_CollisionDetection_Entity->setEditable(true);
	ComboBox_CollisionDetection_Entity->lineEdit()->setAlignment(Qt::AlignCenter);
	ComboBox_CollisionDetection_Entity->lineEdit()->setReadOnly(true);
	//connect(ComboBox_CollisionDetection_Entity,SIGNAL(currentIndexChanged(int)),this,SLOT(slotCollisionDetection_Entity(int)));
}

// 碰撞检测
void CollisionDetection::createTableView()
{
	QStringList HeaderList_CollisionDetection;
	HeaderList_CollisionDetection<<QString::fromLocal8Bit("实体名称A")<<QString::fromLocal8Bit("实体名称B")<<QString::fromLocal8Bit("距离")<<QString::fromLocal8Bit("方位")<<QString::fromLocal8Bit("预计最小碰撞距离")<<QString::fromLocal8Bit("预计碰撞剩余时长");
	Model_CollisionDetection = new QStandardItemModel;
	Table_CollisionDetection = new QTableView;
	Table_CollisionDetection->setModel(Model_CollisionDetection);
	Model_CollisionDetection->setHorizontalHeaderLabels(HeaderList_CollisionDetection);
	Table_CollisionDetection->setFocusPolicy(Qt::NoFocus);
	Table_CollisionDetection->setEditTriggers(QAbstractItemView::NoEditTriggers);
	Table_CollisionDetection->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	Table_CollisionDetection->horizontalHeader()->setStyleSheet("QHeaderView::section{background:white;color: black;}");
	Table_CollisionDetection->verticalHeader()->hide();
	Table_CollisionDetection->verticalHeader()->setDefaultSectionSize(30);
}

void CollisionDetection::setupLayout()
{		
	Layout_CollisionDetection = new QVBoxLayout ;
	QHBoxLayout* tmpLayout = new QHBoxLayout ;
	QWidget * tmpWidget= new QWidget;

	tmpLayout->addWidget(ComboBox_CollisionDetection_Type);
	tmpLayout->addWidget(ComboBox_CollisionDetection_Entity);
	tmpLayout->addWidget(SpinBox_CollisionDetection);
	tmpLayout->addWidget(Button_CollisionDetection);
	tmpWidget->setLayout( tmpLayout);
	Layout_CollisionDetection->addWidget(tmpWidget);

	Layout_CollisionDetection->addWidget(Table_CollisionDetection);

	Button_CollisionDetection->setEnabled(true);
	setLayout(Layout_CollisionDetection);
}

void CollisionDetection::resetUi()
{
	slotCollisionDetectionType(ComboBox_CollisionDetection_Type->currentIndex());
	//emit slotCollisionDetection_Type(0);
}

void CollisionDetection::preHandleDataFrame()
{
	Model_CollisionDetection->removeRows(0, Model_CollisionDetection->rowCount());
}

void calculationCollisionInfo(JGeoCal::GeoPt selfPos, JGeoCal::GeoPt tgtPos, CollisionDetectionInfo& info)
{
	JGeoCal::TopocentricPolar tcp;
	//计算当前
	JGeoCal::GeodeticInDegreeToTopocentricPolar(tcp,tgtPos,selfPos);
	info.distance = tcp.r;
	info.orientation = tcp.Az;

	//推演之后
	info.preMinDistance = 50.03;
	info.preMinCollisionTime = 100.03;
}

//TODO: 需要实现相关的算法 2024.05.24
bool calculationCollisionInfoIter(ByyEntityObject* entityA, ByyEntityObject* entityB, CollisionDetectionInfo& info)
{
	info.preMinDistance = 50.03;
	info.preMinCollisionTime = 100.03;
	//需要输入 A和B的位置，方向，速度，输出是否需要计算，如果计算了需要填写 预计最小碰撞距离、预计碰撞剩余时长

	//需要计算需要根据两者的距离，方向，速度进行快速判断，
	//进过了快速判断再进行计算
	//calGridLength
	//计算网格 通过网格过滤 获取直线距离，判断位置 10公里

	//经纬度 体坐标 极坐标
	//计算dr 高低角  方位角
	//调用总公式计算结果

	GeoPt pos_llaA(entityA->position().y(),entityA->position().x(),entityA->position().z());
	GeoPt pos_llaB(entityB->position().y(),entityB->position().x(),entityB->position().z());

	JGeoCal::TopocentricPolar tcp;
	//计算当前
	JGeoCal::GeodeticInDegreeToTopocentricPolar(tcp,pos_llaA,pos_llaB);
	info.distance = tcp.r;
	info.orientation = tcp.Az;

	// 20240527,xks
	// 大地系相关位置速度转ECEF系相关位置速度
	JGeoCal::Vec3 pos_ecefA,vel_ecefA,pos_ecefB,vel_ecefB;
	JGeoCal::GeodeticInDegreeToGeocentric(pos_ecefA, pos_llaA);
	JGeoCal::GeodeticInDegreeToGeocentric(pos_ecefB, pos_llaB);

	Vec3 vel_enuA(entityA->speed()*cos(entityA->orientation().y()*Deg2Rad)* sin(entityA->orientation().x()*Deg2Rad), 
				entityA->speed()*cos(entityA->orientation().y()*Deg2Rad)* cos(entityA->orientation().x()*Deg2Rad), 
				entityA->speed()*sin(entityA->orientation().y()*Deg2Rad) );
	vel_ecefA = getM_ecf2enu(entityA->position().y(), entityA->position().x()).Inverse() * vel_enuA;
	Vec3 vel_enuB(entityB->speed()*cos(entityB->orientation().y()*Deg2Rad)* sin(entityB->orientation().x()*Deg2Rad), 
		entityB->speed()*cos(entityB->orientation().y()*Deg2Rad)* cos(entityB->orientation().x()*Deg2Rad), 
		entityB->speed()*sin(entityB->orientation().y()*Deg2Rad) );
	vel_ecefB = getM_ecf2enu(entityB->position().y(), entityB->position().x()).Inverse() * vel_enuB;
	
	// 通过DR递推出在当前运动状态下的最小距离
	double durTime=0;
	double dT = 1.0; // 递推步长
	double dV = (vel_ecefA-vel_ecefB).length(); // 相对速度
	if(dV>340)
		dT=0.5;
	if(dV>800)
		dT=0.2;
	double minDist = 1.0e15;
	double dist_AB = (pos_ecefA-pos_ecefB).length();
	//long dis_increase_count=0;
	while (dist_AB < minDist)
	{
		minDist = dist_AB; // 记录前一次的距离

		// 暂时认为两个目标都做匀速直线运动
		pos_ecefA += vel_ecefA * dT;
		pos_ecefB += vel_ecefB * dT;

		dist_AB = (pos_ecefA-pos_ecefB).length(); // 记录最新的距离

		durTime += dT; // 记录持续时长
	}

	info.preMinDistance = minDist;
	info.preMinCollisionTime = durTime;

	if(minDist < 1000)
		return true;
	else
		return false;
}

void CollisionDetection::handleDataFrame()
{
	//CollisionDetectionInfos infos;
	//CollisionDetectionInfo info;
	//ByyEntityObject* entityA = getEntityByName(ComboBox_CollisionDetection_Entity->currentText());
	//if(!entityA) return;
	//QStringList entities = getEntitiesByParty(getAntiPartyName(getPartyName(entityA)));
	//
	//foreach(QString entityBName, entities)
	//{
	//	ByyEntityObject* entityB = getEntityByName(entityBName);
	//	info.entityA = entityA->name();
	//	info.entityB = entityBName;
	//	JGeoCal::GeoPt selfPos,tgtPos;
	//	selfPos.x = entityA->position().y();
	//	selfPos.y = entityA->position().x();
	//	selfPos.z = entityA->position().z();
	//	tgtPos.x = entityB->position().y();
	//	tgtPos.y = entityB->position().x();
	//	tgtPos.z = entityB->position().z();
	//	calculationCollisionInfo(selfPos, tgtPos, info);
	//	infos.append(info);
	//}
	//red 红方遍历
	QStringList redEntities = getEntitiesByParty(RED_SIDE);
	QStringList blueEntities = getEntitiesByParty(BLUE_SIDE);
	CollisionDetectionInfo info;
	if(qAbs(m_byyApp->hdose().simTime() - m_recordSimtime) > 0.5)
	{
		//需要处理重启或者或运行冲日志的情况
		m_recordSimtime = m_byyApp->hdose().simTime();
	}
	else
	{
		//可能界面暂停或者测试速度很慢冲日志
		return;
	}
	foreach(QString redEntity, redEntities)
	{
		ByyEntityObject* entityA = getEntityByName(redEntity);
		if(!entityA) continue;
		//blude 蓝方遍历
		foreach(QString blueEntity, blueEntities)
		{
			ByyEntityObject* entityB = getEntityByName(blueEntity);
			if(!entityB) continue;
			if(calculationCollisionInfoIter(entityA, entityB, info))
			{
				QString log = QString::fromLocal8Bit("碰撞告警（模拟时间%1）  红方目标：%2 与蓝方目标： %3 ，在 %4 秒后，最少距离 %5 m，可能发生碰撞，请注意！").arg(m_byyApp->hdose().simTime()).arg(redEntity).arg(blueEntity).arg(info.preMinCollisionTime).arg(info.preMinDistance);
				ByyHdoseCollisionDetectionLogDatabase::instance().appendCollisionDetectionLog(log);
			}
		}
	}
	//updataCollisionDetectionData(infos);
}

void CollisionDetection::updataCollisionDetectionData(const CollisionDetectionInfos& infos)
{
	ByyEntityObject* entityA = getEntityByName(ComboBox_CollisionDetection_Entity->currentText());
	QStringList entityObjects = getEntitiesByParty(getAntiPartyName(getPartyName(entityA)));
	QList<int> remoteRowRecords;
	int rowCount = Model_CollisionDetection->rowCount();
	for(int index = 0; index < rowCount; index++)
	{
		if(!entityObjects.contains(Model_CollisionDetection->item(index, 1)->text()))
		{
			remoteRowRecords.append(index);
		}
	}
	for(int index = remoteRowRecords.size() - 1; index >= 0 ; index--)
	{
		Model_CollisionDetection->removeRow(remoteRowRecords.at(index));
	}
	foreach(CollisionDetectionInfo info, infos)
	{
		QList<QStandardItem*> items;
		int indexCount = indexOfEquipmentNamesModel(Model_CollisionDetection, info.entityA, info.entityB, 0, 1);
		if(indexCount >= 0)
		{
			Model_CollisionDetection->item(indexCount,2)->setText(QString::number(info.distance, 'f', 2));
			Model_CollisionDetection->item(indexCount,3)->setText(QString::number(info.orientation, 'f', 2));
			Model_CollisionDetection->item(indexCount,4)->setText(QString::number(info.preMinDistance, 'f', 2));
			Model_CollisionDetection->item(indexCount,5)->setText(QString::number(info.preMinCollisionTime, 'f', 2));
		}
		else
		{
			items.append(new QStandardItem(info.entityA));
			items.append(new QStandardItem(info.entityB));
			items.append(new QStandardItem(QString::number(info.distance, 'f', 2)));
			items.append(new QStandardItem(QString::number(info.orientation, 'f', 2)));
			items.append(new QStandardItem(QString::number(info.preMinDistance, 'f', 2)));
			items.append(new QStandardItem(QString::number(info.preMinCollisionTime, 'f', 2)));
			Model_CollisionDetection->appendRow(items);
		}
	}
}

void CollisionDetection::stopHandle()
{
	slotBtnCollisionDetection();
}

void CollisionDetection::slotBtnCollisionDetection()
{
	if(isDataUpdata())
	{
		stopDataHandle();
		ComboBox_CollisionDetection_Type->setEnabled(true);
		ComboBox_CollisionDetection_Entity->setEnabled(true);
		SpinBox_CollisionDetection->setEnabled(true);
		Button_CollisionDetection->setText(START_QUERY);
	}
	else
	{
		startDataHandle(1000);
		ComboBox_CollisionDetection_Type->setEnabled(false);
		ComboBox_CollisionDetection_Entity->setEnabled(false);
		SpinBox_CollisionDetection->setEnabled(false);
		Button_CollisionDetection->setText(STOP_QUERY);
	}
	m_recordSimtime = 0.0;
}

void CollisionDetection::slotCollisionDetectionType(int index)
{
	//QString::fromLocal8Bit("空中碰撞检测")
	QStringList entityNames;
	if(0 == index)
	{
		entityNames.append(getEntitiesByTypes(RED_SIDE, AirEntityObject));
		entityNames.append(getEntitiesByTypes(BLUE_SIDE, AirEntityObject));
	}
	//QString::fromLocal8Bit("水面碰撞检测")
	else if(1 == index)
	{
		entityNames.append(getEntitiesByTypes(RED_SIDE, ShipEntityObject));
		entityNames.append(getEntitiesByTypes(BLUE_SIDE, ShipEntityObject));
	}
	//QString::fromLocal8Bit("地面碰撞检测")
	else if(2 == index)
	{
		entityNames.append(getEntitiesByTypes(RED_SIDE, LandcraftObject));
		entityNames.append(getEntitiesByTypes(BLUE_SIDE, LandcraftObject));
	}
	//QString::fromLocal8Bit("水下碰撞检测")
	else if(3 == index)
	{
		entityNames.append(getEntitiesByTypes(RED_SIDE, SubmarineObject));
		entityNames.append(getEntitiesByTypes(BLUE_SIDE, SubmarineObject));
	}
	ComboBox_CollisionDetection_Entity->clear();
	ComboBox_CollisionDetection_Entity->addItems(entityNames);
}
