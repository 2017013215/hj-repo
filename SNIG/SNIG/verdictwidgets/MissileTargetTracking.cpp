#include "stdafx.h"
#include "MissileTargetTracking.h"  
#include "ByySensorSR.h"
#include "ByyDetectObject.h"

MissileTargetTracking::MissileTargetTracking(VerdictBaseWidget* parent /*= nullptr*/)
	: VerdictBaseWidget(parent)
{
	setSingleFlage(false);
	createWidget();
	setupLayout();
	updateEntities();
	resetUi();
}

void MissileTargetTracking::showEntityName(const QString& entityName)
{
	ByyEntityObject* entityObject = getEntityByName(entityName);
	if(entityObject)
	{
		setSingleFlage(true);
		ComboBox_MissileTargetTracking_Side->setEnabled(false);
		ComboBox_MissileTargetTracking_Self->setEnabled(false);
		ComboBox_MissileTargetTracking_Side->hide();
		ComboBox_MissileTargetTracking_Side->setCurrentIndex(getPartyNameIndex(getPartyName(entityObject)));
		ComboBox_MissileTargetTracking_Self->setCurrentIndex(ComboBox_MissileTargetTracking_Self->findText(entityName));
		slotBtnMissileTargetTracking();
	}
}

// 导弹目标跟踪
void MissileTargetTracking::createButton()
{
	Button_MissileTargetTracking = new QPushButton;
	Button_MissileTargetTracking->setText(START_QUERY);
	//Button_MissileTargetTracking->setEnabled(false);
	connect(Button_MissileTargetTracking, SIGNAL(clicked()), this, SLOT(slotBtnMissileTargetTracking()));
}

void MissileTargetTracking::createChart()
{

}

// 导弹目标跟踪
void MissileTargetTracking::createComBox()
{
	ComboBox_MissileTargetTracking_Side = new QComboBox;
	ComboBox_MissileTargetTracking_Side->addItem(RED_SIDE);
	ComboBox_MissileTargetTracking_Side->addItem(BLUE_SIDE);
	ComboBox_MissileTargetTracking_Side->setStyleSheet("QComboBox {combobox-popup: 1;}");
	ComboBox_MissileTargetTracking_Side->setEditable(true);
	ComboBox_MissileTargetTracking_Side->lineEdit()->setAlignment(Qt::AlignCenter);
	ComboBox_MissileTargetTracking_Side->lineEdit()->setReadOnly(true);
	connect(ComboBox_MissileTargetTracking_Side,SIGNAL(currentIndexChanged(int)),this,SLOT(slotMissileTargetTrackingSide(int)));

	ComboBox_MissileTargetTracking_Self = new QComboBox;
	ComboBox_MissileTargetTracking_Self->addItem(QString::fromLocal8Bit("追踪者"));
	ComboBox_MissileTargetTracking_Self->setStyleSheet("QComboBox {combobox-popup: 1;}");
	ComboBox_MissileTargetTracking_Self->setEditable(true);
	ComboBox_MissileTargetTracking_Self->lineEdit()->setAlignment(Qt::AlignCenter);
	ComboBox_MissileTargetTracking_Self->lineEdit()->setReadOnly(true);
	//connect(ComboBox_MissileTargetTracking_Self,SIGNAL(currentIndexChanged(int)),this,SLOT(slotMissileTargetTrackingSelf(int)));
}

// 导弹目标跟踪
void MissileTargetTracking::createTableView()
	
{
	QStringList HeaderList_MissileTargetTracking;
	HeaderList_MissileTargetTracking<<QString::fromLocal8Bit("目标名称")<<QString::fromLocal8Bit("目标距离")<<QString::fromLocal8Bit("跟踪状态")<<QString::fromLocal8Bit("持续时间");
	Model_MissileTargetTracking = new QStandardItemModel;
	Table_MissileTargetTracking  = new QTableView;
	Table_MissileTargetTracking ->setModel(Model_MissileTargetTracking);
	Model_MissileTargetTracking ->setHorizontalHeaderLabels(HeaderList_MissileTargetTracking);
	Table_MissileTargetTracking ->setFocusPolicy(Qt::NoFocus);
	Table_MissileTargetTracking ->setEditTriggers(QAbstractItemView::NoEditTriggers);
	Table_MissileTargetTracking ->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	Table_MissileTargetTracking ->horizontalHeader()->setStyleSheet("QHeaderView::section{background:white;color: black;}");
	Table_MissileTargetTracking ->verticalHeader()->hide();
	Table_MissileTargetTracking ->verticalHeader()->setDefaultSectionSize(30);
}

void MissileTargetTracking::setupLayout()
{
	Layout_MissileTargetTracking = new QVBoxLayout ;
	QHBoxLayout* tmpLayout = new QHBoxLayout ;
	QWidget * tmpWidget= new QWidget;

	tmpLayout->addWidget(ComboBox_MissileTargetTracking_Side);
	tmpLayout->addWidget(ComboBox_MissileTargetTracking_Self);
	tmpLayout->addWidget(Button_MissileTargetTracking);
	tmpWidget->setLayout( tmpLayout);

	Layout_MissileTargetTracking->addWidget(tmpWidget);
	Layout_MissileTargetTracking->addWidget(Table_MissileTargetTracking);
	setLayout(Layout_MissileTargetTracking);
}

void MissileTargetTracking::resetUi()
{
	slotMissileTargetTrackingSide(ComboBox_MissileTargetTracking_Side->currentIndex());
}

void MissileTargetTracking::slotBtnMissileTargetTracking()
{
	if(isDataUpdata())
	{
		stopDataHandle();
		if(!getSingleFlage())
		{
			ComboBox_MissileTargetTracking_Side->setEnabled(true);
			ComboBox_MissileTargetTracking_Self->setEnabled(true);
		}
		Button_MissileTargetTracking->setText(START_QUERY);
	}
	else 
	{
		startDataHandle(1000);
		ComboBox_MissileTargetTracking_Side->setEnabled(false);
		ComboBox_MissileTargetTracking_Self->setEnabled(false);
		Button_MissileTargetTracking->setText(STOP_QUERY);
	}
}

void MissileTargetTracking::handleDataFrame()
{
	//判断所有的飞行目标ode监控情况
	QString partyName = ComboBox_MissileTargetTracking_Side->currentText();
	QString selfEntityName = ComboBox_MissileTargetTracking_Self->currentText();
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
			if(!isMissile(getEntityByName(trackedTarget)))
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

void MissileTargetTracking::preHandleDataFrame(void)
{
	m_trackedTargetRecord.clear();
}

void MissileTargetTracking::updataModelData(const TrackinfInfos& infos)
{
	Model_MissileTargetTracking->removeRows(0, Model_MissileTargetTracking->rowCount()); //清空所有的数据

	foreach(TrackinfInfo info, infos) 
	{
		QList<QStandardItem*> items;
		items.append(new QStandardItem(info.targetName));
		items.append(new QStandardItem(QString("%1").arg(info.distance)));
		items.append(new QStandardItem(QString("%1").arg(info.getStatusString())));
		items.append(new QStandardItem(QString("%1").arg(info.continueTime)));
		Model_MissileTargetTracking->appendRow(items);
	}
}

void MissileTargetTracking::stopHandle()
{
	slotBtnMissileTargetTracking();
}

void MissileTargetTracking::slotMissileTargetTrackingSide(int index)
{
	updateEntities();
	ComboBox_MissileTargetTracking_Self->clear();
	ComboBox_MissileTargetTracking_Self->addItems(getLocalEntitiesContainSensor(ComboBox_MissileTargetTracking_Side->currentText(), AirEntityObject));
}
