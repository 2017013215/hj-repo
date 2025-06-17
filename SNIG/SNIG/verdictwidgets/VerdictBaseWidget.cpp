#include "StdAfx.h"
#include "verdictbasewidget.h"
#include "comApp.h"
#include "ByyApp.h"
#include "ByyCore.h"
#include "remoteObjectManager.h"
#include "localObjectManager.h"
#include "remoteEntity.h"
#include "ByySensorSR.h"
#include "Verdict.h"
#include "JGeoCal/JGeoCalExp.h"

#include <QTimer>
#include <random>
#include <QTableWidget>

QStringList getJudgmentList()
{
	QStringList ret;
	ret <<QString::fromLocal8Bit("完好")<<QString::fromLocal8Bit("轻伤")<<QString::fromLocal8Bit("重伤")<<QString::fromLocal8Bit("摧毁");
	return ret;
};

QStringList getDamageJudgmentList()
{
	QStringList ret;
	ret <<QString::fromLocal8Bit("轻伤")<<QString::fromLocal8Bit("重伤")<<QString::fromLocal8Bit("摧毁");
	return ret;
};

QStringList getExitJudgmentList()
{
	QStringList ret;
	ret <<QString::fromLocal8Bit("退出")<<QString::fromLocal8Bit("不退出");
	return ret;
};

QStringList getStateJudgmentList()
{
	QStringList ret;
	ret <<QString::fromLocal8Bit("可用")<<QString::fromLocal8Bit("可用,备件维修")<<QString::fromLocal8Bit("可用,阵地维修")<<QString::fromLocal8Bit("可用,返厂维修")<<QString::fromLocal8Bit("不可用");
	return ret;
};

double genRandom()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<double> dis(-1.0, 1.0);
	return dis(gen);
}

int indexOfEquipmentName(QTableWidget* tableWidget, const QString& equipmentName, int col)
{
	int rowCount = tableWidget->rowCount();
	for(int index = 0; index < rowCount; index++)
	{
		if(equipmentName == tableWidget->item(index, col)->text())
		{
			return index;
		}
	}
	return -1;
}

int indexOfEquipmentNames(QTableWidget* tableWidget, const QString& equipmentName1, const QString& equipmentName2, int col1, int col2)
{
	int rowCount = tableWidget->rowCount();
	for(int index = 0; index < rowCount; index++)
	{
		if((equipmentName1 == tableWidget->item(index, col1)->text()) && (equipmentName2 == tableWidget->item(index, col2)->text()))
		{
			return index;
		}
	}
	return -1;
}

int indexOfEquipmentNamesModel(QStandardItemModel* model, const QString& equipmentName1, const QString& equipmentName2, int col1, int col2)
{
	int rowCount = model->rowCount();
	for(int index = 0; index < rowCount; index++)
	{
		if((equipmentName1 == model->item(index, col1)->text()) && (equipmentName2 == model->item(index, col2)->text()))
		{
			return index;
		}
	}
	return -1;
}

int indexOfCellWidget(QTableWidget* tableWidget, QWidget* cellwidget, int col)
{
	int rowCount = tableWidget->rowCount();
	for(int index = 0; index < rowCount; index++)
	{
		if(cellwidget == tableWidget->cellWidget(index, col))
		{
			return index;
		}
	}
	return -1;
}

bool entityAlertBusiness(ByyEntityObject* self, ByyEntityObject* tgt, JGeoCal::FoVDS fov, JGeoCal::TopocentricPolar&tcp)
{
	JGeoCal::GeoPt selfPos,tgtPos; //纬经高
	JGeoCal::Vec3  orientation;

	// ByyEntityObject  经纬高
	selfPos.x = self->position().y();
	selfPos.y = self->position().x();
	selfPos.z = self->position().z();
	tgtPos.x  = tgt->position().y();
	tgtPos.y  = tgt->position().x();
	tgtPos.z  = tgt->position().z();
	orientation = Vec3(self->orientation().x(), self->orientation().y(), self->orientation().z());

	//计算辐照范围
	if( JGeoCal::isInSensorFoV(selfPos, JGeoCal::Vec3(0,0,0), orientation, tgtPos, fov) )
	{
		//计算相对位置关系
		JGeoCal::GeodeticInDegreeToTopocentricPolar(tcp, tgtPos, selfPos);
		return true;
	}
	else
	{
		return false;
	}
}

VerdictBaseWidget::VerdictBaseWidget( QWidget *parent )
	:QWidget(parent)
{
	//qRegisterMetaType<EventDetail>("EventDetail");
	m_isSingle = false;
	m_byyApp = comApp::instance()->getApp();
	tr_dataHandleTimer = new QTimer;
	connect(tr_dataHandleTimer, SIGNAL(timeout()) , this, SLOT(onUpdateRealTimeData()));
}

VerdictBaseWidget::~VerdictBaseWidget()
{

}

void VerdictBaseWidget::onUpdateRealTimeData()
{
	updateEntities();
	handleDataFrame();
}

void VerdictBaseWidget::createButton()
{

}

void VerdictBaseWidget::createChart()
{

}

void VerdictBaseWidget::createComBox()
{

}

void VerdictBaseWidget::createTableView()
{

}

void VerdictBaseWidget::createLabel()
{

}

void VerdictBaseWidget::createLog()
{

}

void VerdictBaseWidget::setupLayout()
{

}

void VerdictBaseWidget::createWidget()
{
	createComBox();
	createChart();
	createButton();
	createTableView();
	createLabel();
	createLog();
}

void VerdictBaseWidget::resetUi()
{

}

void VerdictBaseWidget::stopHandle()
{
	if(isDataUpdata())
	{
		stopDataHandle();
	}
}

void VerdictBaseWidget::setSingleFlage(bool isSingle)
{
	m_isSingle = isSingle;
}

bool VerdictBaseWidget::getSingleFlage(void)
{
	return m_isSingle;
}

ByyEntityObject* VerdictBaseWidget::getEntityByName(const QString& entityName)
{
	updateEntities();
	if(m_redRemoteEntities.contains(entityName))
	{
		return m_redRemoteEntities.value(entityName);
	}
	if(m_blueRemoteEntities.contains(entityName))
	{
		return m_blueRemoteEntities.value(entityName);
	}
	return nullptr;
}

QString VerdictBaseWidget::getTypeName(ByyEntityObject* entity)
{
	QString ret;
	if(isAircraft(entity))
	{
		ret = QString::fromLocal8Bit("飞行器");
	}
	else if(isShip(entity))
	{
		ret = QString::fromLocal8Bit("舰艇");
	}
	else if(isSubmarine(entity))
	{
		ret = QString::fromLocal8Bit("潜艇");
	}
	else if(isTorpedo(entity))
	{
		ret = QString::fromLocal8Bit("鱼类");
	}
	else if(isMissile(entity))
	{
		ret = QString::fromLocal8Bit("导弹");
	}
	return ret;
}

void VerdictBaseWidget::preHandleDataFrame(void)
{

}

int VerdictBaseWidget::getShipCount(QList<ByyEntityObject*>* entObjList)
{
	return getEntityCount(entObjList, 1, 3);
}

int VerdictBaseWidget::getSubmarineCount(QList<ByyEntityObject*>* entObjList)
{
	return getEntityCount(entObjList, 1, 4);
}

int VerdictBaseWidget::getEntityCount(QList<ByyEntityObject*>* entObjList ,int kind,int domain)
{
	int count =0;
	for (int i = 0;i<entObjList->count();i++)
	{
		ByyEntityType entType = (*entObjList)[i]->entityType();

		if (entType.kind==kind && entType.domain==domain)
		{	
			count++;
		}
	}
	return count;
}

void VerdictBaseWidget::getAirEntity(QList<ByyEntityObject*>* entObjList)
{
	v_entObjList = entObjList;
	redList.clear();
	blueList.clear();
	if (!v_entObjList->isEmpty())
	{
		ByyEntityType entType;
		for (int i = 0;i<entObjList->count();i++)
		{
			entType = (*entObjList)[i]->entityType();
			ByyEntityObject* entObj = (*entObjList)[i];
			if(isAircraft(entObj))
			{
				if(isRed(entObj))
				{
					//红方
					redList << entObj->name();
				}
				else if(isBlue(entObj))
				{	
					//蓝方
					blueList << entObj->name();
				}
			}
		}
	}
}

void VerdictBaseWidget::getTargetEntity(QList<ByyEntityObject*>* entObjList)
{
	v_entObjList = entObjList;
}

void VerdictBaseWidget::getCollisionEntity(QList<ByyEntityObject*>* entObjList)
{
	v_entObjList = entObjList;

	if (!entObjList->isEmpty())
	{
		myCurrentEntity = (*v_entObjList)[0];
		selectEntID=myCurrentEntity->id();
		//switch (myCurrentEntity->damageState())
		//{
		//case ByyDamageNone:
		//	damageState = QString::fromLocal8Bit("完好");
		//	break;
		//case ByyDamageSlight:
		//	damageState = QString::fromLocal8Bit("轻伤");
		//	break;
		//case ByyDamageModerate:
		//	damageState = QString::fromLocal8Bit("重伤");
		//	break;
		//case ByyDamageDestroyed:
		//	damageState = QString::fromLocal8Bit("摧毁");
		//	break;
		//default:
		//	return;
		//}	
	}
}

void VerdictBaseWidget::getDamageEntity(QList<ByyEntityObject*>* entObjList)
{
	v_entObjList = entObjList;
}

void VerdictBaseWidget::handleDataFrame(void)
{

}

void VerdictBaseWidget::startDataHandle(int updateTime)
{
	if(tr_dataHandleTimer)
	{
		preHandleDataFrame();
		if(tr_dataHandleTimer->isActive())
		{
			tr_dataHandleTimer->stop();
		}
		tr_dataHandleTimer->start(updateTime);
	}
}

void VerdictBaseWidget::stopDataHandle()
{
	if(tr_dataHandleTimer) tr_dataHandleTimer->stop();
}

bool VerdictBaseWidget::isDataUpdata()
{
	if(tr_dataHandleTimer) return tr_dataHandleTimer->isActive();
	else return false;
}

QStringList VerdictBaseWidget::findObjects(const QString& srcname, EntitySimpleObjectType type)
{
	QStringList ret;
	return ret;
}

void VerdictBaseWidget::updateEntities(void)
{
	Q_ASSERT(m_byyApp);
	if(!m_byyApp) return;
	//m_redEntities.clear();
	//m_blueEntities.clear();
	//处理本地对象Local
	//QList<ByyEntityObject*> entitiesList = m_byyApp->core().localObjectManager().entityObjects().values();
	//foreach(ByyEntityObject* iterObject, entitiesList)
	//{
	//	if(isRed(iterObject))
	//	{
	//		m_redEntities.insert(iterObject->name(), iterObject);
	//	}
	//	else if(isBlue(iterObject))
	//	{
	//		m_blueEntities.insert(iterObject->name(), iterObject);
	//	}	
	//}
	//处理远端的对象remote
	m_redRemoteEntities.clear();
	m_blueRemoteEntities.clear();
	QList<ByyEntityObject*> remoteentitiesList = m_byyApp->core().remoteObjectManager().entityObjects();
	foreach(ByyEntityObject* iterObject, remoteentitiesList)
	{
		if(isRed(iterObject))
		{
			m_redRemoteEntities.insert(iterObject->name(), iterObject);
		}
		else if(isBlue(iterObject))
		{
			m_blueRemoteEntities.insert(iterObject->name(), iterObject);
		}	
	}
}

QStringList VerdictBaseWidget::getEntitiesByParty(const QString& partyName)
{
	QStringList ret;
	if(RED_SIDE == partyName)
	{
		ret = m_redRemoteEntities.keys();
	}
	else if(BLUE_SIDE == partyName)
	{
		ret = m_blueRemoteEntities.keys();
	}
	return ret;
}

QStringList getEntityComponentNames(ByyEntityObject* entObj, const QString& type)
{
	Q_ASSERT(entObj);
	QStringList ret;
	ByyDescribeEntry *descEntry = entObj->describeEntry();
	ByyDescribeEntry::ComponentList componentList = descEntry->getComponent(type);	//切换不同实体，同时会切换对应传感器			
	for (int j = 0; j < componentList.count(); j++)
	{
		ret.append(componentList[j].getTag());
	}
	return ret;
}

QStringList VerdictBaseWidget::getSensorsByEntity(const QString& partyName, const QString& entityName)
{
	updateEntities();
	QStringList ret;
	if(RED_SIDE == partyName)
	{
		if(m_redRemoteEntities.contains(entityName))
		{
			ret = getEntityComponentNames(m_redRemoteEntities.value(entityName, nullptr), "sensor");
		}
	}
	else if(BLUE_SIDE == partyName)
	{
		if(m_blueRemoteEntities.contains(entityName))
		{
			ret = getEntityComponentNames(m_blueRemoteEntities.value(entityName, nullptr), "sensor");
		}
	}
	return ret;
}

QStringList VerdictBaseWidget::getLocalEntitiesContainSensor(const QString& partyName)
{
	updateEntities();
	QStringList ret;
	QList<ByyEntityObject*> entitiesList;
	if(RED_SIDE == partyName)
	{
		entitiesList = m_redRemoteEntities.values();
	}
	else if(BLUE_SIDE == partyName)
	{
		entitiesList = m_blueRemoteEntities.values();
	}
	foreach(ByyEntityObject* iterObject, entitiesList)
	{
		if(!getEntityComponentNames(iterObject, "sensor").isEmpty())
		{
			ret.append(iterObject->name());
		}
	}
	return ret;
}

QStringList VerdictBaseWidget::getLocalEntitiesContainSensor(const QString& partyName, EntitySimpleObjectType objectType)
{
	updateEntities();
	QStringList ret;
	QList<ByyEntityObject*> entitiesList;
	if(RED_SIDE == partyName)
	{
		entitiesList = m_redRemoteEntities.values();
	}
	else if(BLUE_SIDE == partyName)
	{
		entitiesList = m_blueRemoteEntities.values();
	}
	foreach(ByyEntityObject* iterObject, entitiesList)
	{
		if((AirEntityObject == objectType) && isAircraft(iterObject))
		{
		}
		else if((ShipEntityObject == objectType) && isShip(iterObject))
		{
		}
		else if((SubmarineObject == objectType) && isSubmarine(iterObject))
		{
		}
		else if((MissileObject == objectType) && isMissile(iterObject))
		{
		}
		else if((TorpedoObject == objectType) && isTorpedo(iterObject))
		{
		}
		else if((LandcraftObject == objectType) && isLandcraft(iterObject))
		{
		}
		else
		{
			continue;
		}
		if(!getEntityComponentNames(iterObject, "sensor").isEmpty())
		{
			ret.append(iterObject->name());
		}
	}
	return ret;
}

QStringList VerdictBaseWidget::getEntitiesByType(const QString& partyName, EntitySimpleObjectType objectType)
{
	updateEntities();
	QStringList ret;
	//QList<ByyEntityObject*> entitiesList;
	QList<ByyEntityObject*> remoteentitiesList;
	if(RED_SIDE == partyName)
	{
		//entitiesList = m_redEntities.values();
		remoteentitiesList = m_redRemoteEntities.values();
	}
	else if(BLUE_SIDE == partyName)
	{
		//entitiesList = m_blueEntities.values();
		remoteentitiesList = m_blueRemoteEntities.values();
	}
	foreach(ByyEntityObject* iterObject, remoteentitiesList)
	{
		bool filterBool = false;
		switch (objectType)
		{
		case AirEntityObject:
			filterBool = isAircraft(iterObject);
			break;
		case ShipEntityObject:
			filterBool = isShip(iterObject);
			break;
		case SubmarineObject:
			filterBool = isSubmarine(iterObject);
			break;
		case TorpedoObject:
			filterBool = isTorpedo(iterObject);
			break;
		case MissileObject:
			filterBool = isMissile(iterObject);
			break;
		case LandcraftObject:
			filterBool = isLandcraft(iterObject);
			break;
		case InvalidObject:
			break;
		}
		if(filterBool)
		{
			ret.append(iterObject->name());
		}
	}

	//foreach(ByyEntityObject* iterObject, remoteentitiesList)
	//{
	//	bool filterBool = false;
	//	switch (objectType)
	//	{
	//	case MissileObject:
	//		filterBool = isMissile(iterObject);
	//		break;
	//	case AirEntityObject:
	//	case ShipEntityObject:
	//	case SubmarineObject:
	//	case TorpedoObject:
	//	case InvalidObject:
	//		break;
	//	}
	//	if(filterBool)
	//	{
	//		ret.append(iterObject->name());
	//	}
	//}
	return ret;
}

QStringList VerdictBaseWidget::getEntitiesByTypes(const QString& partyName, int objectTypes)
{
	QStringList ret;
	if(AirEntityObject & objectTypes)
	{
		ret.append(getEntitiesByType(partyName, AirEntityObject));
	}
	if(ShipEntityObject & objectTypes)
	{
		ret.append(getEntitiesByType(partyName, ShipEntityObject));
	}
	if(SubmarineObject & objectTypes)
	{
		ret.append(getEntitiesByType(partyName, SubmarineObject));
	}
	if(MissileObject & objectTypes)
	{
		ret.append(getEntitiesByType(partyName, MissileObject));
	}
	if(TorpedoObject & objectTypes)
	{
		ret.append(getEntitiesByType(partyName, TorpedoObject));
	}
	if(DepthChargeObject & objectTypes)
	{
		ret.append(getEntitiesByType(partyName, DepthChargeObject));
	}
	if(LandcraftObject & objectTypes)
	{
		ret.append(getEntitiesByType(partyName, LandcraftObject));
	}
	
	return ret;
}

int VerdictBaseWidget::getPartyNameIndex(const QString& partyName)
{
	int ret = -1;
	if(RED_SIDE == partyName)
	{
		return 0;
	}
	else if(BLUE_SIDE == partyName)
	{
		return 1;
	}
	return ret;
}

ByySensorSR* VerdictBaseWidget::getSensorByName(const QString& partyName, const QString& entityName)
{
	updateEntities();
	ByySensorSR* ret = nullptr;
	ByyRemoteEntity* entityObject = nullptr;
	updateEntities();
	if(RED_SIDE == partyName)
	{
		entityObject = dynamic_cast<ByyRemoteEntity*>(m_redRemoteEntities.value(entityName));
	}
	else if(BLUE_SIDE == partyName)
	{
		entityObject = dynamic_cast<ByyRemoteEntity*>(m_blueRemoteEntities.value(entityName));
	}
	if(entityObject)
	{
		ret = entityObject->getOrCreateSR<ByySensorSR>();
	}
	return ret;
}

ByySensorSR* VerdictBaseWidget::getSensorByName(const QString& entityName)
{
	ByySensorSR* ret = nullptr;
	ret = getSensorByName(RED_SIDE, entityName);
	if(!ret)
	{
		ret = getSensorByName(BLUE_SIDE, entityName);
	}
	return ret;
}

void VerdictBaseWidget::sendStatusChangeCommand(const QString& entityname, const QString& equipmentname, int status)
{
	//TODO: 下发实体装备的状态修改指令 2024.05.24
}

void VerdictBaseWidget::sendStatusChangeCommand(const QString& entityname, int status)
{
	//TODO: 下发实体的状态修改指令 2024.05.24
}

EventInfos VerdictBaseWidget::getExplodeEvenfs(EntitySimpleObjectType attckType, EntitySimpleObjectType targetType)
{
	return Verdict::getInstance()->getExplodeEvenfs(attckType, targetType);
}
