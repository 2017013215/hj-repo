#include "StdAfx.h"
#include "ByyWeaponControlWidget.h"
#include "ByyWeaponSysSR.h"

ByyWeaponControlWidget::ByyWeaponControlWidget( ByyCore& core,ByyEntityObject* entity/*=0*/,QWidget* parent/*=0*/,Qt::WindowFlags f /*= 0*/ ) 
	: ByyEntityObserver(core,entity,parent,f)
	,myWeponSysSR(0)
{
	ui.setupUi(this);

	setEntity(entity);
}

ByyWeaponControlWidget::~ByyWeaponControlWidget()
{

}

void ByyWeaponControlWidget::setEntity( ByyEntityObject *entity )
{
	ByyEntityObserver::setEntity(entity);

	ui.treeWidget->clear();

	mySystemItems.clear();
	myWeaponItems.clear();

	if (!entity)
		return;

	ByyDescribeEntry *descEntry=myCurrentEntity->describeEntry();


	if (!descEntry)
		return;
	
	const ByyDescribeEntry::ComponentList& components=descEntry->getComponent("WeaponMount");

	for (int i=0;i!=components.size();++i)
	{
		const ByyDescParamList& descParamList=components[i].getParamList();

		QString DeviceName=descParamList.value("DeviceName");

		QTreeWidgetItem* item=new QTreeWidgetItem(ui.treeWidget,0);
		item->setText(0,DeviceName);

		mySystemItems.insert(DeviceName,item);

		

		QString weaponCountMax=descParamList.value("WeaponCountMax");

		item->setText(1,QString("(%1/%2)").arg(descParamList.value("WeaponCountNormal")).arg(weaponCountMax));

		item->setData(1,Qt::UserRole,weaponCountMax);

		QStringList weaponRecData=descParamList.value("WeaponRecData").split(";");

		for (int i=0;i!=weaponRecData.size();++i)
		{
			QStringList weponParam=weaponRecData[i].split(",");

			if (weponParam.size()<2)
				continue;

			QTreeWidgetItem *weaponItem=new QTreeWidgetItem(item,1);

			weaponItem->setText(0,weponParam[1]);

			QString maxCount=weponParam.value(3);

			weaponItem->setText(1,QString("(%1/%2)").arg(weponParam.value(4)).arg(maxCount));
			weaponItem->setData(1,Qt::UserRole,maxCount);

			weaponItem->setText(2,QString::fromLocal8Bit(weponParam.value(4).toInt()?"可用":"不可用"));

			myWeaponItems.insert(weponParam[1],weaponItem);
		}
	}

	if (!entity->isLocal())
	{
		myWeponSysSR=dynamic_cast<ByyWeaponSysSR*>(entity->getExtendEsr(ByyWeaponSysSR::theType()));

		if(myWeponSysSR)
		{
			connect(myWeponSysSR,SIGNAL(dataChanged()),this,SLOT(onSRDataChanged()),Qt::QueuedConnection);

			updateFromSR();
		}
	}

	ui.treeWidget->expandAll();
}

void ByyWeaponControlWidget::updateFromSR()
{
	if (!myCurrentEntity||!myWeponSysSR)
		return;

	const ByyWeaponSysSR::WeaponSystemMap& weponSystemList=myWeponSysSR->getSystemList();

	ByyWeaponSysSR::WeaponSystemMap::iterator it=weponSystemList.begin();

	for (;it!=weponSystemList.end();++it)
	{
		if (!mySystemItems.contains(it.key()))
			continue;

		QTreeWidgetItem *item=mySystemItems[it.key()];

		item->setText(1,QString("%1/%2").arg(it.value().m_WeaponCount).arg(item->data(1,Qt::UserRole).toString()));

		const ByyWeaponSysSR::WeaponRecSmpDataList& recSmpDataList=it.value().getWeaponRecSmpData();

		for (int i=0;i!=recSmpDataList.size();++i)
		{
			QMultiMap<QString,QTreeWidgetItem*>::iterator it1=myWeaponItems.find(recSmpDataList[i].m_Name);

			while (it1 != myWeaponItems.end() && it1.key() ==recSmpDataList[i].m_Name)
			{
				if (it1.value()->parent()==item)
				{
					QTreeWidgetItem* weaponItem=it1.value();

					weaponItem->setText(1,QString("%1/%2").arg(recSmpDataList[i].m_Count).arg(weaponItem->data(1,Qt::UserRole).toString()));
					weaponItem->setText(2,QString::fromLocal8Bit(recSmpDataList[i].m_Available?"可用":"不可用"));
					break;
				}
				
				++it1;
			}
		}
	}
}

void ByyWeaponControlWidget::onSRDataChanged()
{
	if (sender()==myWeponSysSR)
	{
		updateFromSR();
	}
}
