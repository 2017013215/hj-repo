#include "stdafx.h"
#include "MissileHitProbabilityCalculation.h" 
#include <QDoubleSpinBox>
#include "remoteEntity.h"
#include "ByySensorSR.h"
#include "ByyDetectObject.h"
#include "JGeoCal/JGeoCalExp.h"

MissileHitProbabilityCalculation::MissileHitProbabilityCalculation(VerdictBaseWidget* parent /*= nullptr*/)
	: VerdictBaseWidget(parent)
{
	createWidget();
	setupLayout();
	resetUi();
}

//导弹命中概率
void MissileHitProbabilityCalculation::createButton()
{
	Button_MissileHitProbabilityCalculation = new QPushButton;
	Button_MissileHitProbabilityCalculation->setText(QString::fromLocal8Bit("查看"));
	//Button_MissileHitProbabilityCalculation->setEnabled(false);
	connect(Button_MissileHitProbabilityCalculation, SIGNAL(clicked()), this, SLOT(slotBtnMissileHitProbabilityCalculation()));
}

void MissileHitProbabilityCalculation::createChart()
{

}

//导弹命中概率
void MissileHitProbabilityCalculation::createComBox()
{
	QStringList EntityList;
	EntityList<<RED_SIDE<<BLUE_SIDE;
	ComboBox_MissileHitProbabilityCalculation = new QComboBox;
	ComboBox_MissileHitProbabilityCalculation->setEditable(true);
	ComboBox_MissileHitProbabilityCalculation->lineEdit()->setAlignment(Qt::AlignCenter);
	ComboBox_MissileHitProbabilityCalculation->lineEdit()->setReadOnly(true);
	ComboBox_MissileHitProbabilityCalculation->addItems(EntityList);
	connect(ComboBox_MissileHitProbabilityCalculation,SIGNAL(currentIndexChanged(int)),this,SLOT(slotMissileHitProbabilityCalculation(int)));
}

//导弹命中概率
void MissileHitProbabilityCalculation::createTableView()
{
	QStringList HeaderList_MissileHitProbabilityCalculation;
	HeaderList_MissileHitProbabilityCalculation<<QString::fromLocal8Bit("导弹名称")<<QString::fromLocal8Bit("目标名称")<<QString::fromLocal8Bit("目标距离")<<QString::fromLocal8Bit("命中概率")<<QString::fromLocal8Bit("人工设置（%）")<<QString::fromLocal8Bit("");

	Table_MissileHitProbabilityCalculation = new QTableWidget;

	Table_MissileHitProbabilityCalculation->setColumnCount(HeaderList_MissileHitProbabilityCalculation.size());
	Table_MissileHitProbabilityCalculation->setFocusPolicy(Qt::NoFocus);
	Table_MissileHitProbabilityCalculation->setEditTriggers(QAbstractItemView::NoEditTriggers);
	Table_MissileHitProbabilityCalculation->setSelectionMode(QAbstractItemView::NoSelection);
	Table_MissileHitProbabilityCalculation->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	Table_MissileHitProbabilityCalculation->horizontalHeader()->setStyleSheet("QHeaderView::section{background:white;color: black;}");
	Table_MissileHitProbabilityCalculation->verticalHeader()->hide();
	for (int a=0;a<HeaderList_MissileHitProbabilityCalculation.size();a++) {
		Table_MissileHitProbabilityCalculation->setHorizontalHeaderItem(a,new QTableWidgetItem(HeaderList_MissileHitProbabilityCalculation.at(a)));
	}
	Table_MissileHitProbabilityCalculation->verticalHeader()->setDefaultSectionSize(30);
}

void MissileHitProbabilityCalculation::setupLayout()
{
	Layout_MissileHitProbabilityCalculation = new QVBoxLayout ;
	QHBoxLayout* tmpLayout = new QHBoxLayout ;
	QWidget *tmpWidget= new QWidget;
	tmpLayout->addWidget(ComboBox_MissileHitProbabilityCalculation);
	tmpLayout->addWidget(Button_MissileHitProbabilityCalculation);
	tmpWidget->setLayout( tmpLayout);
	Layout_MissileHitProbabilityCalculation->addWidget( tmpWidget);
	Layout_MissileHitProbabilityCalculation->addWidget(Table_MissileHitProbabilityCalculation);

	setLayout(Layout_MissileHitProbabilityCalculation);
}

void MissileHitProbabilityCalculation::preHandleDataFrame()
{
	Table_MissileHitProbabilityCalculation->clearContents();
	Table_MissileHitProbabilityCalculation->setRowCount(0);
}

double MissileHitProbabilityCalculation::getProbablility(QString EntClsname, int kind, int domain, double lchdist_m, 
	double hitR_m, double hitH_m, double hitVel_mps, double hitTgtVel_mps)
{
	// xks, 20240528
	QString kind_domain;
	kind_domain.sprintf("%d-%d",kind,domain);
	double hitprob = Verdict::getInstance()->getVerdictBaseData().getWeaponHitProbItems().value(EntClsname).hitProbTables.value(kind_domain).attrs.value("StdProb").toDouble();
	double fac1 = Verdict::getInstance()->getVerdictBaseData().getWeaponHitProbItems().value(EntClsname).hitProbTables.value(kind_domain).corrFactors.value("LchRange").factorByDistance(lchdist_m);
	double fac2 = Verdict::getInstance()->getVerdictBaseData().getWeaponHitProbItems().value(EntClsname).hitProbTables.value(kind_domain).corrFactors.value("HitDist").factorByDistance(hitR_m);
	double fac3 = Verdict::getInstance()->getVerdictBaseData().getWeaponHitProbItems().value(EntClsname).hitProbTables.value(kind_domain).corrFactors.value("HitHight").factorByDistance(hitH_m);
	double fac4 = Verdict::getInstance()->getVerdictBaseData().getWeaponHitProbItems().value(EntClsname).hitProbTables.value(kind_domain).corrFactors.value("HitVel").factorByDistance(hitVel_mps);
	double fac5 = Verdict::getInstance()->getVerdictBaseData().getWeaponHitProbItems().value(EntClsname).hitProbTables.value(kind_domain).corrFactors.value("HitTgtVel").factorByDistance(hitTgtVel_mps);
	return hitprob*fac1*fac2*fac3*fac4*fac5;
	//return 50.0;
}

void MissileHitProbabilityCalculation::handleDataFrame()
{
	MHPCDatas infos;
	QString missilePartyName = ComboBox_MissileHitProbabilityCalculation->currentText();
	QStringList missilObjects = getEntitiesByType(missilePartyName, MissileObject);
	foreach(QString missileName, missilObjects) 
	{
#if 0
		ByyEntityObject* entityObject = getEntityByName(missileName);
		ByyRemoteEntity* selfRemoet = dynamic_cast<ByyRemoteEntity*>(entityObject);
		if(selfRemoet)
		{
			ByySensorSR* sensorSr = selfRemoet->getOrCreateSR<ByySensorSR>();
			ByySensorSR::DeviceStateList *devList = sensorSr->getDeviceList();
			foreach(QString device, devList->keys())
			{
				foreach(ByyDetectObject* detectObject, devList->value(device).myDetectObjList.values())
				{
					MHPCData info;
					info.missileName = missileName;
					info.targetName = detectObject->targetName();
					ByyEntityObject* targetObject = getEntityByName(info.targetName);
					JGeoCal::GeoPt selfPos,tgtPos; //纬经高
					JGeoCal::TopocentricPolar tcp;
					// ByyEntityObject  经纬高
					selfPos.x = selfRemoet->position().y();
					selfPos.y = selfRemoet->position().x();
					selfPos.z = selfRemoet->position().z();
					tgtPos.x  = targetObject->position().y();
					tgtPos.y  = targetObject->position().x();
					tgtPos.z  = targetObject->position().z();
					JGeoCal::GeodeticInDegreeToTopocentricPolar(tcp, tgtPos, selfPos);
					info.distance = tcp.r;
					info.probablility = getProbablility(tcp.r);
					infos.append(info);
				}
			}
		}
#else
		ByyEntityObject* entityObject = getEntityByName(missileName);
		QString targetName = entityObject->getTargetName();
		//TODO:需要查看获取目标是否正确 2025.05.24
		if(entityObject)
		{
			//targetName=QString::fromLocal8Bit("金刚_1");
			QStringList targetObjects = getEntitiesByParty(getAntiPartyName(missilePartyName));
			foreach(QString targetObjectName, targetObjects)
			{
				if(targetName != targetObjectName) continue;
				MHPCData info;
				info.missileName = missileName;
				info.targetName = targetObjectName;
				ByyEntityObject* targetObject = getEntityByName(targetObjectName);
				JGeoCal::GeoPt selfPos,tgtPos; //纬经高
				JGeoCal::TopocentricPolar tcp;
				// ByyEntityObject  经纬高
				selfPos.x = entityObject->position().y();
				selfPos.y = entityObject->position().x();
				selfPos.z = entityObject->position().z();
				tgtPos.x  = targetObject->position().y();
				tgtPos.y  = targetObject->position().x();
				tgtPos.z  = targetObject->position().z();
				JGeoCal::GeodeticInDegreeToTopocentricPolar(tcp, tgtPos, selfPos);
				info.distance = tcp.r;
				//TODO:需要获取相关的命中概念 2025.05.24
				info.probablility = getProbablility(entityObject->itemClass(), targetObject->entityType().kind, targetObject->entityType().domain, 300000.0, tcp.r, entityObject->position().z(), entityObject->speed(), targetObject->speed()); // 20240528,xks
				infos.append(info);
			}
		}
#endif
	}
	//info.missileName = "missile1";
	//info.targetName = "target1";
	//info.distance = 1000.23;
	//info.probablility = 45.23;
	//infos.append(info);
	updataMissileHitProbabilityData(infos);
}

void MissileHitProbabilityCalculation::updataMissileHitProbabilityData(const MHPCDatas& infos)
{
	//清空数据
	QString missilePartyName = ComboBox_MissileHitProbabilityCalculation->currentText();
	QStringList missilObjects = getEntitiesByType(missilePartyName, MissileObject);
	QList<int> remoteRowRecords;
	int rowCount = Table_MissileHitProbabilityCalculation->rowCount();
	for(int index = 0; index < rowCount; index++)
	{
		if(!missilObjects.contains(Table_MissileHitProbabilityCalculation->item(index, 0)->text()))
		{
			remoteRowRecords.append(index);
		}
	}
	for(int index = remoteRowRecords.size() - 1; index >= 0 ; index--)
	{
		Table_MissileHitProbabilityCalculation->removeRow(remoteRowRecords.at(index));
	}
	foreach(MHPCData info, infos)
	{
		int indexCount = indexOfEquipmentNames(Table_MissileHitProbabilityCalculation, info.missileName, info.targetName, 0, 1);
		if(indexCount >= 0)
		{
			Table_MissileHitProbabilityCalculation->item(indexCount,2)->setText(QString::number(info.distance, 'f', 2));
			Table_MissileHitProbabilityCalculation->item(indexCount,3)->setText(QString::number(info.probablility, 'f', 2));
		}
		else
		{
			int rowCount = Table_MissileHitProbabilityCalculation->rowCount();
			Table_MissileHitProbabilityCalculation->setRowCount(rowCount+1);
			Table_MissileHitProbabilityCalculation->setItem(rowCount,0,new QTableWidgetItem(info.missileName));
			Table_MissileHitProbabilityCalculation->setItem(rowCount,1,new QTableWidgetItem(info.targetName));
			Table_MissileHitProbabilityCalculation->setItem(rowCount,2,new QTableWidgetItem(QString::number(info.distance, 'f', 2)));
			Table_MissileHitProbabilityCalculation->setItem(rowCount,3,new QTableWidgetItem(QString::number(info.probablility, 'f', 2)));
			QDoubleSpinBox* setProbability = new QDoubleSpinBox();
			setProbability->setValue(0.0);
			setProbability->setMinimum(0.0);
			setProbability->setMaximum(100.0);
			Table_MissileHitProbabilityCalculation->setCellWidget(rowCount,4,setProbability);

			QPushButton* okPbt = new QPushButton();
			okPbt->setText(QString::fromLocal8Bit("设置"));
			Table_MissileHitProbabilityCalculation->setCellWidget(rowCount,5,okPbt);
			connect(okPbt, SIGNAL(clicked()), this, SLOT(slotBtnChangedMHP()));
		}
	}
}

void MissileHitProbabilityCalculation::stopHandle()
{
	slotBtnMissileHitProbabilityCalculation();
}

void MissileHitProbabilityCalculation::slotBtnMissileHitProbabilityCalculation()
{
	if(isDataUpdata())
	{
		stopDataHandle();
		ComboBox_MissileHitProbabilityCalculation->setEnabled(true);
		Button_MissileHitProbabilityCalculation->setText(START_QUERY);
	}
	else
	{
		startDataHandle(1000);
		ComboBox_MissileHitProbabilityCalculation->setEnabled(false);
		Button_MissileHitProbabilityCalculation->setText(STOP_QUERY);
	}
}

void MissileHitProbabilityCalculation::slotBtnChangedMHP()
{
	QWidget* cellwidget = qobject_cast<QWidget*>(sender());
	int currentRow = indexOfCellWidget(Table_MissileHitProbabilityCalculation, cellwidget, 5);
	if(currentRow >= 0)
	{
		//TODO: 设置命中概率
	}
}
