#include "stdafx.h"
#include "FireControlLockingCalculation.h" 
#include "entityObject.h"
#include "remoteEntity.h"
#include "ByySensorSR.h"
#include "ByyDetectObject.h"
#include "JGeoCal/JGeoCalExp.h"

static const int AXIS_MAX_X = 10, AXIS_MAX_Y = 1;/* 横纵坐标最大显示范围 */

FireControlLockingCalculation::FireControlLockingCalculation(VerdictBaseWidget* parent /*= nullptr*/)
	: VerdictBaseWidget(parent)
{
	createWidget();
	setupLayout();
	updateEntities();
	resetUi();
}

//火控锁定裁决
void FireControlLockingCalculation::createButton()
{
	Button_FireControlLockingCalculation = new QPushButton;
	Button_FireControlLockingCalculation->setText(START_QUERY);
	//Button_FireControlLockingCalculation->setEnabled(false);
	connect(Button_FireControlLockingCalculation, SIGNAL(clicked()), this, SLOT(slotBtnFireControlLockingCalculation()));
}

//火控锁定裁决
void FireControlLockingCalculation::createChart()
{
}

//火控锁定裁决
void FireControlLockingCalculation::createComBox()
{
	ComboBox_FireControlLockingCalculation_Red = new QComboBox;
	ComboBox_FireControlLockingCalculation_Red->addItem(RED_SIDE);
	ComboBox_FireControlLockingCalculation_Red->setStyleSheet("QComboBox {combobox-popup: 1;}");
	ComboBox_FireControlLockingCalculation_Red->setEditable(true);
	ComboBox_FireControlLockingCalculation_Red->lineEdit()->setAlignment(Qt::AlignCenter);
	ComboBox_FireControlLockingCalculation_Red->lineEdit()->setReadOnly(true);
	connect(ComboBox_FireControlLockingCalculation_Red,SIGNAL(currentIndexChanged(int)),this,SLOT(slotFireControlLockingCalculationRed(int)));

	ComboBox_FireControlLockingCalculation_Blue = new QComboBox;
	ComboBox_FireControlLockingCalculation_Blue->addItem(BLUE_SIDE);
	ComboBox_FireControlLockingCalculation_Blue->setStyleSheet("QComboBox {combobox-popup: 1;}");
	ComboBox_FireControlLockingCalculation_Blue->setEditable(true);
	ComboBox_FireControlLockingCalculation_Blue->lineEdit()->setAlignment(Qt::AlignCenter);
	ComboBox_FireControlLockingCalculation_Blue->lineEdit()->setReadOnly(true);
	connect(ComboBox_FireControlLockingCalculation_Blue,SIGNAL(currentIndexChanged(int)),this,SLOT(slotFireControlLockingCalculationBlue()));
}

void FireControlLockingCalculation::createTableView()
{
	QStringList HeaderList_FireControlLockingCalculation;
	HeaderList_FireControlLockingCalculation<<QString::fromLocal8Bit("目标名称")<<QString::fromLocal8Bit("锁定状态")<<QString::fromLocal8Bit("锁定距离")<<QString::fromLocal8Bit("锁定时长");
	//Model_DepthChargeVsSubmarineDamageCalculation = new QStandardItemModel;
	Table_FireControlLockingCalculation = new QTableWidget;
	Table_FireControlLockingCalculation->setColumnCount(HeaderList_FireControlLockingCalculation.size());
	Table_FireControlLockingCalculation->setFocusPolicy(Qt::NoFocus);
	Table_FireControlLockingCalculation->setEditTriggers(QAbstractItemView::NoEditTriggers);
	Table_FireControlLockingCalculation->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	Table_FireControlLockingCalculation->horizontalHeader()->setStyleSheet("QHeaderView::section{background:white;color: black;}");
	Table_FireControlLockingCalculation->verticalHeader()->hide();
	for (int a=0;a<HeaderList_FireControlLockingCalculation.size();a++) {
		Table_FireControlLockingCalculation->setHorizontalHeaderItem(a,new QTableWidgetItem(HeaderList_FireControlLockingCalculation.at(a)));
	}
	Table_FireControlLockingCalculation->verticalHeader()->setDefaultSectionSize(30);
}

void FireControlLockingCalculation::createLog()
{
}

void FireControlLockingCalculation::setupLayout()
{
	Layout_FireControlLockingCalculation = new QVBoxLayout ;
	QHBoxLayout* tmpLayout = new QHBoxLayout ;
	QWidget * tmpWidget= new QWidget;

	tmpLayout->addWidget(ComboBox_FireControlLockingCalculation_Red);
	tmpLayout->addWidget(ComboBox_FireControlLockingCalculation_Blue);
	tmpLayout->addWidget(Button_FireControlLockingCalculation);
	tmpWidget->setLayout( tmpLayout);
	Layout_FireControlLockingCalculation->addWidget(tmpWidget);

	Layout_FireControlLockingCalculation->addWidget(Table_FireControlLockingCalculation);

	setLayout(Layout_FireControlLockingCalculation);
}

void FireControlLockingCalculation::resetUi()
{
	ComboBox_FireControlLockingCalculation_Red->clear();
	ComboBox_FireControlLockingCalculation_Blue->clear();
	QStringList airobjects;
	airobjects.append(getEntitiesByType(RED_SIDE, AirEntityObject));
	airobjects.append(getEntitiesByType(BLUE_SIDE, AirEntityObject));
	ComboBox_FireControlLockingCalculation_Red->addItems(airobjects);
}

void FireControlLockingCalculation::preHandleDataFrame()
{
	m_spendTimeRecord = 0;
	Table_FireControlLockingCalculation->clearContents();
	Table_FireControlLockingCalculation->setRowCount(0);
}

void FireControlLockingCalculation::handleDataFrame()
{
	FireControlLockingData info;
	ByyEntityObject* self = getEntityByName(ComboBox_FireControlLockingCalculation_Red->currentText());
	ByyEntityObject* target = getEntityByName(ComboBox_FireControlLockingCalculation_Blue->currentText());
	if(!self || !target) return;
	info.lockingTarget = target->name();
	bool findIt = false;
	//是否发现了
	ByyRemoteEntity* selfRemoet = dynamic_cast<ByyRemoteEntity*>(self);
	if(selfRemoet)
	{
		ByySensorSR* sensorSr = selfRemoet->getOrCreateSR<ByySensorSR>();
		ByySensorSR::DeviceStateList *devList = sensorSr->getDeviceList();
		foreach(QString device, devList->keys())
		{
			foreach(ByyDetectObject* detectObject, devList->value(device).myDetectObjList.values())
			{
				if(target->name() == detectObject->targetName())
				{
					findIt = true;
					//info.spendTime = detectObject->getContinueTime();
					break;
				}
			}
			if(findIt)
			{
				break;
			}
		}
	}
	if(findIt)
	{
		JGeoCal::GeoPt selfPos,tgtPos; //纬经高
		JGeoCal::TopocentricPolar tcp;
		// ByyEntityObject  经纬高
		selfPos.x = self->position().y();
		selfPos.y = self->position().x();
		selfPos.z = self->position().z();
		tgtPos.x  = target->position().y();
		tgtPos.y  = target->position().x();
		tgtPos.z  = target->position().z();
		JGeoCal::GeodeticInDegreeToTopocentricPolar(tcp, tgtPos, selfPos);
		//在方位 30度以内
		//TODO:对于此算法需要进行检视 2025.05.24
		double slefAz = (self->orientation().x() > 0 ?(self->orientation().x()):(360+self->orientation().x()));
		double relationAz = (tcp.Az > 0 ?(tcp.Az):(360+tcp.Az));
		if(qAbs(slefAz - relationAz) < 30.0)
		{
			m_spendTimeRecord++;
			info.lockingStatus = QString::fromLocal8Bit("火控锁定");
		}
		else
		{
			m_spendTimeRecord = 0;
			info.lockingStatus = QString::fromLocal8Bit("已跟踪未锁定");
		}
		info.distance = tcp.r;
	}
	else
	{
		m_spendTimeRecord = 0;
		info.lockingStatus = QString::fromLocal8Bit("未跟踪");
		info.distance = -1.0;
	}
	info.spendTime = m_spendTimeRecord;
	updataFireControlLockingCalculationData(info);
}

void FireControlLockingCalculation::updataFireControlLockingCalculationData(const FireControlLockingData& info)
{
	if(!info.lockingTarget.isEmpty())
	{
		Table_FireControlLockingCalculation->clearContents();
		Table_FireControlLockingCalculation->setRowCount(0);
		Table_FireControlLockingCalculation->setRowCount(1);
		Table_FireControlLockingCalculation->setItem(0, 0, new QTableWidgetItem(info.lockingTarget));
		Table_FireControlLockingCalculation->setItem(0, 1, new QTableWidgetItem(info.lockingStatus));
		Table_FireControlLockingCalculation->setItem(0, 2, new QTableWidgetItem(QString::number(info.distance,'f',2)));
		Table_FireControlLockingCalculation->setItem(0, 3, new QTableWidgetItem(QString::number(info.spendTime,'f',2)));
	}
}

void FireControlLockingCalculation::stopHandle()
{
	slotBtnFireControlLockingCalculation();
}

void FireControlLockingCalculation::slotBtnFireControlLockingCalculation()
{
	if(isDataUpdata())
	{
		stopDataHandle();
		ComboBox_FireControlLockingCalculation_Red->setEnabled(true);
		ComboBox_FireControlLockingCalculation_Blue->setEnabled(true);
		Button_FireControlLockingCalculation->setText(START_QUERY);
	}
	else
	{
		startDataHandle(1000);
		ComboBox_FireControlLockingCalculation_Red->setEnabled(false);
		ComboBox_FireControlLockingCalculation_Blue->setEnabled(false);
		Button_FireControlLockingCalculation->setText(STOP_QUERY);
	}
}

void FireControlLockingCalculation::slotFireControlLockingCalculationRed(int index)
{
	ComboBox_FireControlLockingCalculation_Blue->clear();
	ComboBox_FireControlLockingCalculation_Blue->addItems(getEntitiesByType(getAntiPartyName(getEntityByName(ComboBox_FireControlLockingCalculation_Red->currentText())), AirEntityObject));
}

void FireControlLockingCalculation::slotFireControlLockingCalculationBlue(int index)
{

}

