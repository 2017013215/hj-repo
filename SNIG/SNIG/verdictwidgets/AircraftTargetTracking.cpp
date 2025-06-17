#include "stdafx.h"
#include "AircraftTargetTracking.h" 
#include "ByySensorSR.h"
#include "ByyDetectObject.h"

AircraftTargetTracking::AircraftTargetTracking(VerdictBaseWidget* parent /*= nullptr*/)
	: VerdictBaseWidget(parent)
{
	setSingleFlage(false);
	createWidget();
	setupLayout();
	updateEntities();
	resetUi();
}

void AircraftTargetTracking::showEntityName(const QString& entityName)
{
	ByyEntityObject* entityObject = getEntityByName(entityName);
	if(entityObject)
	{
		setSingleFlage(true);
		ComboBox_AircraftTargetTracking_Side->setEnabled(true);
		ComboBox_AircraftTargetTracking_Self->setEnabled(true);
		ComboBox_AircraftTargetTracking_Side->hide();
		ComboBox_AircraftTargetTracking_Side->setCurrentIndex(getPartyNameIndex(getPartyName(entityObject)));
		ComboBox_AircraftTargetTracking_Self->setCurrentIndex(ComboBox_AircraftTargetTracking_Self->findText(entityName));
		slotBtnAircraftTargetTracking();
	}
}

void AircraftTargetTracking::createButton()
{	
	// 飞机目标跟踪
	Button_AircraftTargetTracking = new QPushButton;
	Button_AircraftTargetTracking->setText(START_QUERY);
	connect(Button_AircraftTargetTracking, SIGNAL(clicked()), this, SLOT(slotBtnAircraftTargetTracking()));
}

void AircraftTargetTracking::createChart()
{

}

// 飞机目标跟踪
void AircraftTargetTracking::createComBox()
{
	ComboBox_AircraftTargetTracking_Side = new QComboBox;
	ComboBox_AircraftTargetTracking_Side->addItem(RED_SIDE);
	ComboBox_AircraftTargetTracking_Side->addItem(BLUE_SIDE);
	ComboBox_AircraftTargetTracking_Side->setStyleSheet("QComboBox {combobox-popup: 1;}");
	ComboBox_AircraftTargetTracking_Side->setEditable(true);
	ComboBox_AircraftTargetTracking_Side->lineEdit()->setAlignment(Qt::AlignCenter);
	ComboBox_AircraftTargetTracking_Side->lineEdit()->setReadOnly(true);
	connect(ComboBox_AircraftTargetTracking_Side,SIGNAL(currentIndexChanged(int)),this,SLOT(slotAircraftTargetTrackingSide(int)));

	ComboBox_AircraftTargetTracking_Self = new QComboBox;
	ComboBox_AircraftTargetTracking_Self->addItem(QString::fromLocal8Bit("主动追踪者"));
	ComboBox_AircraftTargetTracking_Self->setStyleSheet("QComboBox {combobox-popup: 1;}");
	ComboBox_AircraftTargetTracking_Self->setEditable(true);
	ComboBox_AircraftTargetTracking_Self->lineEdit()->setAlignment(Qt::AlignCenter);
	ComboBox_AircraftTargetTracking_Self->lineEdit()->setReadOnly(true);
	//connect(ComboBox_AircraftTargetTracking_Self,SIGNAL(currentIndexChanged(int)),this,SLOT(slotAircraftTargetTrackingSelf(int)));
}

// 飞机目标跟踪
void AircraftTargetTracking::createTableView()
{
	QStringList HeaderList_AircraftTargetTracking;
	HeaderList_AircraftTargetTracking<<QString::fromLocal8Bit("目标名称")<<QString::fromLocal8Bit("目标距离")<<QString::fromLocal8Bit("跟踪状态")<<QString::fromLocal8Bit("持续时间");
	Model_AircraftTargetTracking = new QStandardItemModel;
	Table_AircraftTargetTracking  = new QTableView;
	Table_AircraftTargetTracking ->setModel(Model_AircraftTargetTracking);
	Model_AircraftTargetTracking ->setHorizontalHeaderLabels(HeaderList_AircraftTargetTracking);
	Table_AircraftTargetTracking ->setFocusPolicy(Qt::NoFocus);
	Table_AircraftTargetTracking ->setEditTriggers(QAbstractItemView::NoEditTriggers);
	Table_AircraftTargetTracking ->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	Table_AircraftTargetTracking ->horizontalHeader()->setStyleSheet("QHeaderView::section{background:white;color: black;}");
	Table_AircraftTargetTracking ->verticalHeader()->hide();
	Table_AircraftTargetTracking ->verticalHeader()->setDefaultSectionSize(30);
}

void AircraftTargetTracking::setupLayout()
{
	Layout_AircraftTargetTracking = new QVBoxLayout ;
	QHBoxLayout* tmpLayout = new QHBoxLayout ;
	QWidget * tmpWidget= new QWidget;
	tmpLayout->addWidget(ComboBox_AircraftTargetTracking_Side);
	tmpLayout->addWidget(ComboBox_AircraftTargetTracking_Self);
	tmpLayout->addWidget(Button_AircraftTargetTracking);
	tmpWidget->setLayout( tmpLayout);

	Layout_AircraftTargetTracking->addWidget(tmpWidget);
	Layout_AircraftTargetTracking->addWidget(Table_AircraftTargetTracking);
	setLayout(Layout_AircraftTargetTracking);
}

void AircraftTargetTracking::resetUi()
{
	slotAircraftTargetTrackingSide(ComboBox_AircraftTargetTracking_Side->currentIndex());
}

void AircraftTargetTracking::slotBtnAircraftTargetTracking()
{
	if(isDataUpdata())
	{
		stopDataHandle();
		if(!getSingleFlage())
		{
			ComboBox_AircraftTargetTracking_Side->setEnabled(true);
			ComboBox_AircraftTargetTracking_Self->setEnabled(true);
		}
		Button_AircraftTargetTracking->setText(START_QUERY);
	}
	else 
	{
		startDataHandle(1000);
		ComboBox_AircraftTargetTracking_Side->setEnabled(false);
		ComboBox_AircraftTargetTracking_Self->setEnabled(false);
		Button_AircraftTargetTracking->setText(STOP_QUERY);
	}
}

void AircraftTargetTracking::handleDataFrame()
{
	//判断所有的飞行目标ode监控情况
	QString partyName = ComboBox_AircraftTargetTracking_Side->currentText();
	QString selfEntityName = ComboBox_AircraftTargetTracking_Self->currentText();
	//获取sensor
	ByySensorSR* myCurrentSensorSR = getSensorByName(partyName, selfEntityName);
	if(!myCurrentSensorSR)
	{
		return;
	}
	ByySensorSR::DeviceStateList * deviceStateList = myCurrentSensorSR->getDeviceList();
	ByySensorSR::DeviceStateList::const_iterator it0;
	//本次发现的目标
	QStringList trackedTargets; 
	for(it0 = deviceStateList->begin(); it0 != deviceStateList->end(); it0++)
	{
		ByySensorSR::DetectObjectMap::const_iterator it1;
		for (it1 = it0.value().myDetectObjList.begin(); it1 != it0.value().myDetectObjList.end(); it1++)
		{
			QString trackedTarget = it1.value()->name(); 
			if(!isAircraft(getEntityByName(trackedTarget)))
			{
				continue;
			}
			if(!trackedTargets.contains(trackedTarget))
			{
				trackedTargets.append(trackedTarget);
			}
			if(!m_trackedTargetRecord.contains(trackedTarget))
			{
				TrackinfInfo info;
				info.targetName = trackedTarget;
				info.distance = it1.value()->distance();
				info.continueTime = it1.value()->getContinueTime();
				m_trackedTargetRecord.insert(trackedTarget, info);
			}
			else
			{
				m_trackedTargetRecord[trackedTarget].distance = it1.value()->distance();
				m_trackedTargetRecord[trackedTarget].continueTime = it1.value()->getContinueTime();
				m_trackedTargetRecord[trackedTarget].addCount();
			}
		}
	}

	{
		QMap<QString, TrackinfInfo>::iterator it;
		for (it = m_trackedTargetRecord.begin(); it != m_trackedTargetRecord.end(); it++)
		{
			if(!trackedTargets.contains(it.key()))
			{
				it.value().reduceCount();
			}
		}
	}
	
	//更新数据
	updataModelData(m_trackedTargetRecord.values());
}

void AircraftTargetTracking::preHandleDataFrame(void)
{
	m_trackedTargetRecord.clear();
}

void AircraftTargetTracking::updataModelData(const TrackinfInfos& infos)
{
	Model_AircraftTargetTracking->removeRows(0, Model_AircraftTargetTracking->rowCount()); //清空所有的数据

	foreach(TrackinfInfo info, infos) 
	{
		QList<QStandardItem*> items;
		items.append(new QStandardItem(info.targetName));
		items.append(new QStandardItem(QString("%1").arg(info.distance)));
		items.append(new QStandardItem(QString("%1").arg(info.getStatusString())));
		items.append(new QStandardItem(QString("%1").arg(info.continueTime)));
		Model_AircraftTargetTracking->appendRow(items);
	}
}

void AircraftTargetTracking::stopHandle()
{
	slotBtnAircraftTargetTracking();
}

void AircraftTargetTracking::slotAircraftTargetTrackingSide(int index)
{
	updateEntities();
	ComboBox_AircraftTargetTracking_Self->clear();
	ComboBox_AircraftTargetTracking_Self->addItems(getLocalEntitiesContainSensor(ComboBox_AircraftTargetTracking_Side->currentText(), AirEntityObject));
}
