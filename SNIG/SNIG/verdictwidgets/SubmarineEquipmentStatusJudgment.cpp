#include "stdafx.h"
#include "SubmarineEquipmentStatusJudgment.h" 

SubmarineEquipmentStatusJudgment::SubmarineEquipmentStatusJudgment(VerdictBaseWidget* parent /*= nullptr*/)
	: VerdictBaseWidget(parent)
{
	createWidget();
	setupLayout();
	updateEntities();
	resetUi();
}

//潜艇装备状态裁决
void SubmarineEquipmentStatusJudgment::createButton()
{
	Button_SubmarineEquipmentStatusJudgment = new QPushButton;
	Button_SubmarineEquipmentStatusJudgment->setText(START_QUERY);
	//Button_SubmarineEquipmentStatusJudgment->setEnabled(false);
	connect(Button_SubmarineEquipmentStatusJudgment, SIGNAL(clicked()), this, SLOT(slotBtnSESJ()));
}

void SubmarineEquipmentStatusJudgment::createChart()
{

}

void SubmarineEquipmentStatusJudgment::preHandleDataFrame()
{
	Table_SubmarineEquipmentStatusJudgment->clearContents();
	Table_SubmarineEquipmentStatusJudgment->setRowCount(0);
}

//潜艇装备状态裁决
void SubmarineEquipmentStatusJudgment::createComBox()
{
	QStringList EntityList;
	EntityList << RED_SIDE << BLUE_SIDE;
	ComboBox_SubmarineEquipmentStatusJudgment_Sides = new QComboBox;
	ComboBox_SubmarineEquipmentStatusJudgment_Sides->setEditable(true);
	ComboBox_SubmarineEquipmentStatusJudgment_Sides->lineEdit()->setAlignment(Qt::AlignCenter);
	ComboBox_SubmarineEquipmentStatusJudgment_Sides->lineEdit()->setReadOnly(true);
	ComboBox_SubmarineEquipmentStatusJudgment_Sides->addItems(EntityList);
	connect(ComboBox_SubmarineEquipmentStatusJudgment_Sides,SIGNAL(currentIndexChanged(int)),this,SLOT(slotSESJSides(int)));

	ComboBox_SubmarineEquipmentStatusJudgment_Entity = new QComboBox;
	ComboBox_SubmarineEquipmentStatusJudgment_Entity->addItem(QString::fromLocal8Bit("All"));
	ComboBox_SubmarineEquipmentStatusJudgment_Entity->setStyleSheet("QComboBox {combobox-popup: 1;}");
	ComboBox_SubmarineEquipmentStatusJudgment_Entity->setEditable(true);
	ComboBox_SubmarineEquipmentStatusJudgment_Entity->lineEdit()->setAlignment(Qt::AlignCenter);
	ComboBox_SubmarineEquipmentStatusJudgment_Entity->lineEdit()->setReadOnly(true);
	//connect(ComboBox_SubmarineEquipmentStatusJudgment_Entity,SIGNAL(currentIndexChanged(int)),this,SLOT(slot_SESJ_Entity(int)));
}

//潜艇装备状态裁决
void SubmarineEquipmentStatusJudgment::createTableView()
{
	QStringList HeaderList_SubmarineEquipmentStatusJudgment;
	HeaderList_SubmarineEquipmentStatusJudgment<<QString::fromLocal8Bit("装备名称")<<QString::fromLocal8Bit("可用状态")<<QString::fromLocal8Bit("损管控制")<<QString::fromLocal8Bit("");

	Table_SubmarineEquipmentStatusJudgment = new QTableWidget;
	Table_SubmarineEquipmentStatusJudgment->setColumnCount(HeaderList_SubmarineEquipmentStatusJudgment.size());
	Table_SubmarineEquipmentStatusJudgment->setFocusPolicy(Qt::NoFocus);
	Table_SubmarineEquipmentStatusJudgment->setEditTriggers(QAbstractItemView::NoEditTriggers);
	Table_SubmarineEquipmentStatusJudgment->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	Table_SubmarineEquipmentStatusJudgment->horizontalHeader()->setStyleSheet("QHeaderView::section{background:white;color: black;}");
	Table_SubmarineEquipmentStatusJudgment->verticalHeader()->hide();
	for (int a=0;a<HeaderList_SubmarineEquipmentStatusJudgment.size();a++) {
		Table_SubmarineEquipmentStatusJudgment->setHorizontalHeaderItem(a,new QTableWidgetItem(HeaderList_SubmarineEquipmentStatusJudgment.at(a)));
	}
	Table_SubmarineEquipmentStatusJudgment->verticalHeader()->setDefaultSectionSize(30);
}

void SubmarineEquipmentStatusJudgment::setupLayout()
{
	Layout_SubmarineEquipmentStatusJudgment = new QVBoxLayout;
	QHBoxLayout* tmpLayout = new QHBoxLayout ;
	QWidget * tmpWidget= new QWidget;

	tmpLayout->addWidget(ComboBox_SubmarineEquipmentStatusJudgment_Sides);
	tmpLayout->addWidget(ComboBox_SubmarineEquipmentStatusJudgment_Entity);
	tmpLayout->addWidget(Button_SubmarineEquipmentStatusJudgment);
	tmpWidget->setLayout(tmpLayout);

	Layout_SubmarineEquipmentStatusJudgment->addWidget(tmpWidget);
	Layout_SubmarineEquipmentStatusJudgment->addWidget(Table_SubmarineEquipmentStatusJudgment);
	Button_SubmarineEquipmentStatusJudgment->setEnabled(true);

	setLayout(Layout_SubmarineEquipmentStatusJudgment);
}

void SubmarineEquipmentStatusJudgment::resetUi()
{
	slotSESJSides(ComboBox_SubmarineEquipmentStatusJudgment_Sides->currentIndex());
}

void SubmarineEquipmentStatusJudgment::slotBtnSESJ()
{
	if(isDataUpdata())
	{
		stopDataHandle();
		ComboBox_SubmarineEquipmentStatusJudgment_Sides->setEnabled(true);
		ComboBox_SubmarineEquipmentStatusJudgment_Entity->setEnabled(true);
		Button_SubmarineEquipmentStatusJudgment->setText(START_QUERY);
	}
	else
	{
		startDataHandle(1000);
		ComboBox_SubmarineEquipmentStatusJudgment_Sides->setEnabled(false);
		ComboBox_SubmarineEquipmentStatusJudgment_Entity->setEnabled(false);
		Button_SubmarineEquipmentStatusJudgment->setText(STOP_QUERY);
	}
}

void SubmarineEquipmentStatusJudgment::slotSESJSides(int index)
{
	ComboBox_SubmarineEquipmentStatusJudgment_Entity->clear();
	ComboBox_SubmarineEquipmentStatusJudgment_Entity->addItems(getEntitiesByType(ComboBox_SubmarineEquipmentStatusJudgment_Sides->currentText(), SubmarineObject));
}


void SubmarineEquipmentStatusJudgment::handleDataFrame()
{
	EquipmentJudgments infos;
	ByyEntityObject* entityObject = getEntityByName(ComboBox_SubmarineEquipmentStatusJudgment_Entity->currentText());
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

void SubmarineEquipmentStatusJudgment::updataJudgmentData(const EquipmentJudgments& infos)
{
	QStringList tList =getStateJudgmentList();
	foreach(EquipmentJudgment info, infos)
	{
		int rowIndex = indexOfEquipmentName(Table_SubmarineEquipmentStatusJudgment, info.equipmentName, 0);
		if(rowIndex < 0)
		{
			int rowCount = Table_SubmarineEquipmentStatusJudgment->rowCount();

			Table_SubmarineEquipmentStatusJudgment->setRowCount(rowCount+1);
			Table_SubmarineEquipmentStatusJudgment->setItem(rowCount,0, new QTableWidgetItem(info.equipmentName));
			Table_SubmarineEquipmentStatusJudgment->setItem(rowCount,1, new QTableWidgetItem(info.equipmentStatus));

			QComboBox* ComboBox_NESJ = new QComboBox;
			ComboBox_NESJ->setEditable(true);
			ComboBox_NESJ->lineEdit()->setAlignment(Qt::AlignCenter);
			ComboBox_NESJ->lineEdit()->setReadOnly(true);
			ComboBox_NESJ->addItems(tList);
			ComboBox_NESJ->setCurrentIndex(0);

			Table_SubmarineEquipmentStatusJudgment->setCellWidget(rowCount,2,ComboBox_NESJ);
			QPushButton* pbt_NESJ = new QPushButton;
			pbt_NESJ->setText(QString::fromLocal8Bit("确认修改"));
			Table_SubmarineEquipmentStatusJudgment->setCellWidget(rowCount,3, pbt_NESJ);
			connect(pbt_NESJ, SIGNAL(clicked()), this, SLOT(slotCellSESJ()));
		}
		else
		{
			Table_SubmarineEquipmentStatusJudgment->item(rowIndex, 1)->setText(info.equipmentStatus);
		}
	}
}

void SubmarineEquipmentStatusJudgment::stopHandle()
{
	slotBtnSESJ();
}

void SubmarineEquipmentStatusJudgment::slotCellSESJ()
{
	QWidget* cellwidget = qobject_cast<QWidget*>(sender());
	int currentRow = indexOfCellWidget(Table_SubmarineEquipmentStatusJudgment, cellwidget, 3);
	if(currentRow >= 0)
	{
		QString entityname = ComboBox_SubmarineEquipmentStatusJudgment_Entity->currentText();
		QString equpmentname = Table_SubmarineEquipmentStatusJudgment->item(currentRow, 0)->text();
		int status = ByyDamageNone;
		QComboBox* combox = qobject_cast<QComboBox*>(Table_SubmarineEquipmentStatusJudgment->cellWidget(currentRow, 2));
		if(combox) status = combox->currentIndex();
		sendStatusChangeCommand(entityname, equpmentname,status);
	}
}
