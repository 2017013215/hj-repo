#include "stdafx.h"
#include "NavalEquipmentStatusJudgment.h"

NavalEquipmentStatusJudgment::NavalEquipmentStatusJudgment(VerdictBaseWidget* parent /*= nullptr*/)
	: VerdictBaseWidget(parent)
{
	createWidget();
	setupLayout();
	updateEntities();
	resetUi();
}

//舰艇装备状态裁决
void NavalEquipmentStatusJudgment::createButton()
{
	Button_NavalEquipmentStatusJudgment = new QPushButton;
	Button_NavalEquipmentStatusJudgment->setText(START_QUERY);
	connect(Button_NavalEquipmentStatusJudgment, SIGNAL(clicked()), this, SLOT(slotBtnNESJ()));
}

void NavalEquipmentStatusJudgment::createChart()
{

}

//舰艇装备状态裁决
void NavalEquipmentStatusJudgment::createComBox()
{
	QStringList EntityList;
	EntityList << RED_SIDE << BLUE_SIDE;
	ComboBox_NavalEquipmentStatusJudgment_Sides = new QComboBox;
	ComboBox_NavalEquipmentStatusJudgment_Sides->setEditable(true);
	ComboBox_NavalEquipmentStatusJudgment_Sides->lineEdit()->setAlignment(Qt::AlignCenter);
	ComboBox_NavalEquipmentStatusJudgment_Sides->lineEdit()->setReadOnly(true);
	ComboBox_NavalEquipmentStatusJudgment_Sides->addItems(EntityList);
	connect(ComboBox_NavalEquipmentStatusJudgment_Sides,SIGNAL(currentIndexChanged(int)),this,SLOT(slotNESJSides(int)));

	ComboBox_NavalEquipmentStatusJudgment_Entity = new QComboBox;
	ComboBox_NavalEquipmentStatusJudgment_Entity->addItem(QString::fromLocal8Bit("All"));
	ComboBox_NavalEquipmentStatusJudgment_Entity->setStyleSheet("QComboBox {combobox-popup: 1;}");
	ComboBox_NavalEquipmentStatusJudgment_Entity->setEditable(true);
	ComboBox_NavalEquipmentStatusJudgment_Entity->lineEdit()->setAlignment(Qt::AlignCenter);
	ComboBox_NavalEquipmentStatusJudgment_Entity->lineEdit()->setReadOnly(true);
	//connect(ComboBox_NavalEquipmentStatusJudgment_Entity,SIGNAL(currentIndexChanged(int)),this,SLOT(slotNESJEntity(int)));
}

//舰艇装备状态裁决
void NavalEquipmentStatusJudgment::createTableView()
{
	QStringList HeaderList_NavalEquipmentStatusJudgment;
	HeaderList_NavalEquipmentStatusJudgment<<QString::fromLocal8Bit("装备名称")<<QString::fromLocal8Bit("可用状态")<<QString::fromLocal8Bit("损管控制")<<QString::fromLocal8Bit("");

	Table_NavalEquipmentStatusJudgment = new QTableWidget;
	Table_NavalEquipmentStatusJudgment->setColumnCount(HeaderList_NavalEquipmentStatusJudgment.size());
	Table_NavalEquipmentStatusJudgment->setFocusPolicy(Qt::NoFocus);
	Table_NavalEquipmentStatusJudgment->setEditTriggers(QAbstractItemView::NoEditTriggers);
	Table_NavalEquipmentStatusJudgment->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	Table_NavalEquipmentStatusJudgment->horizontalHeader()->setStyleSheet("QHeaderView::section{background:white;color: black;}");
	Table_NavalEquipmentStatusJudgment->verticalHeader()->hide();
	for (int a=0;a<HeaderList_NavalEquipmentStatusJudgment.size();a++) {
		Table_NavalEquipmentStatusJudgment->setHorizontalHeaderItem(a,new QTableWidgetItem(HeaderList_NavalEquipmentStatusJudgment.at(a)));
	}
	Table_NavalEquipmentStatusJudgment->verticalHeader()->setDefaultSectionSize(30);
}

void NavalEquipmentStatusJudgment::setupLayout()
{
	Layout_NavalEquipmentStatusJudgment = new QVBoxLayout;

	QHBoxLayout* tmpLayout = new QHBoxLayout ;
	QWidget * tmpWidget= new QWidget;

	tmpLayout->addWidget(ComboBox_NavalEquipmentStatusJudgment_Sides);
	tmpLayout->addWidget(ComboBox_NavalEquipmentStatusJudgment_Entity);
	tmpLayout->addWidget(Button_NavalEquipmentStatusJudgment);
	tmpWidget->setLayout(tmpLayout);

	Layout_NavalEquipmentStatusJudgment->addWidget(tmpWidget);
	Layout_NavalEquipmentStatusJudgment->addWidget(Table_NavalEquipmentStatusJudgment);

	Button_NavalEquipmentStatusJudgment->setEnabled(true);
	setLayout(Layout_NavalEquipmentStatusJudgment);
}

void NavalEquipmentStatusJudgment::resetUi()
{
	slotNESJSides(ComboBox_NavalEquipmentStatusJudgment_Sides->currentIndex());
}

void NavalEquipmentStatusJudgment::preHandleDataFrame()
{
	Table_NavalEquipmentStatusJudgment->clearContents();
	Table_NavalEquipmentStatusJudgment->setRowCount(0);
}

void NavalEquipmentStatusJudgment::slotBtnNESJ()
{
	if(isDataUpdata())
	{
		stopDataHandle();
		ComboBox_NavalEquipmentStatusJudgment_Sides->setEnabled(true);
		ComboBox_NavalEquipmentStatusJudgment_Entity->setEnabled(true);
		Button_NavalEquipmentStatusJudgment->setText(START_QUERY);
	}
	else
	{
		startDataHandle(1000);
		ComboBox_NavalEquipmentStatusJudgment_Sides->setEnabled(false);
		ComboBox_NavalEquipmentStatusJudgment_Entity->setEnabled(false);
		Button_NavalEquipmentStatusJudgment->setText(STOP_QUERY);
	}
}

void NavalEquipmentStatusJudgment::slotNESJSides(int index)
{
	ComboBox_NavalEquipmentStatusJudgment_Entity->clear();
	ComboBox_NavalEquipmentStatusJudgment_Entity->addItems(getEntitiesByType(ComboBox_NavalEquipmentStatusJudgment_Sides->currentText(), ShipEntityObject));
}

void NavalEquipmentStatusJudgment::handleDataFrame()
{
	EquipmentJudgments infos;
	ByyEntityObject* entityObject = getEntityByName(ComboBox_NavalEquipmentStatusJudgment_Entity->currentText());
	if(entityObject)
	{
		ByyDescribeEntry* entry = entityObject->describeEntry();
		const ByyDescribeEntry::ComponentList& components=entry->getComponent();
		foreach(ByyDescribeComponent component, components)
		{
			if("ESR" != component.getType().toUpper() && "BEHAVE" != component.getType().toUpper())
			{
				EquipmentJudgment info;
				info.equipmentName = component.getTag();
				info.equipmentStatus =getJudgmentList().at(entityObject->damageState());
				infos.append(info);
			}
		}
		updataJudgmentData(infos);
	}
}

void NavalEquipmentStatusJudgment::updataJudgmentData(const EquipmentJudgments& infos)
{
	QStringList tList =getStateJudgmentList();
	foreach(EquipmentJudgment info, infos)
	{
		int rowIndex = indexOfEquipmentName(Table_NavalEquipmentStatusJudgment, info.equipmentName, 0);
		if(rowIndex < 0)
		{
			int rowCount = Table_NavalEquipmentStatusJudgment->rowCount();

			Table_NavalEquipmentStatusJudgment->setRowCount(rowCount+1);
			Table_NavalEquipmentStatusJudgment->setItem(rowCount,0, new QTableWidgetItem(info.equipmentName));
			Table_NavalEquipmentStatusJudgment->setItem(rowCount,1, new QTableWidgetItem(info.equipmentStatus));   //lmq20240528,设置为定值

			QComboBox* ComboBox_NESJ = new QComboBox;
			ComboBox_NESJ->setEditable(true);
			ComboBox_NESJ->lineEdit()->setAlignment(Qt::AlignCenter);
			ComboBox_NESJ->lineEdit()->setReadOnly(true);
			ComboBox_NESJ->addItems(tList);
			ComboBox_NESJ->setCurrentIndex(0);

			Table_NavalEquipmentStatusJudgment->setCellWidget(rowCount,2,ComboBox_NESJ);
			QPushButton* pbt_NESJ = new QPushButton;
			pbt_NESJ->setText(QString::fromLocal8Bit("确认修改"));
			Table_NavalEquipmentStatusJudgment->setCellWidget(rowCount,3, pbt_NESJ);
			connect(pbt_NESJ, SIGNAL(clicked()), this, SLOT(slotCellNESJ()));
		}
		else
		{
			Table_NavalEquipmentStatusJudgment->item(rowIndex, 1)->setText(info.equipmentStatus);
		}
	}
}

void NavalEquipmentStatusJudgment::stopHandle()
{
	slotBtnNESJ();
}

void NavalEquipmentStatusJudgment::slotCellNESJ()
{
	QWidget* cellwidget = qobject_cast<QWidget*>(sender());
	int currentRow = indexOfCellWidget(Table_NavalEquipmentStatusJudgment, cellwidget, 3);
	if(currentRow >= 0)
	{
		QString entityname = ComboBox_NavalEquipmentStatusJudgment_Entity->currentText();
		QString equpmentname = Table_NavalEquipmentStatusJudgment->item(currentRow, 0)->text();
		int status = ByyDamageNone;
		QComboBox* combox = qobject_cast<QComboBox*>(Table_NavalEquipmentStatusJudgment->cellWidget(currentRow, 2));
		if(combox) status = combox->currentIndex();
		sendStatusChangeCommand(entityname, equpmentname,status);
	}
}
