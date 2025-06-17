#include "stdafx.h"
#include "ExitJudgment.h" 

ExitJudgment::ExitJudgment(VerdictBaseWidget* parent /*= nullptr*/)
	: VerdictBaseWidget(parent)
{
	setSingleFlage(false);
	createWidget();
	setupLayout();
	updateEntities();
	resetUi();
}

void ExitJudgment::showEntityName(const QString& entityName)
{
	ByyEntityObject* entityObject = getEntityByName(entityName);
	if(entityObject)
	{
		setSingleFlage(true);
		ComboBox_ExitJudgment_Sides->setEnabled(false);
		ComboBox_ExitJudgment_Entity->setEnabled(false);
		ComboBox_ExitJudgment_Sides->hide();
		ComboBox_ExitJudgment_Sides->setCurrentIndex(getPartyNameIndex(getPartyName(entityObject)));
		ComboBox_ExitJudgment_Entity->setCurrentIndex(ComboBox_ExitJudgment_Entity->findText(entityName));
		slotBtnExitJudgment();
	}
}

//退出裁决
void ExitJudgment::createButton()
{ 
	Button_ExitJudgment = new QPushButton;
	Button_ExitJudgment->setText(QString::fromLocal8Bit("查看"));
	Button_ExitJudgment->setEnabled(false);
	connect(Button_ExitJudgment, SIGNAL(clicked()), this, SLOT(slotBtnExitJudgment()));
}

void ExitJudgment::createChart()
{

}

//退出裁决
void ExitJudgment::createComBox()
{ 
	QStringList EntityList;
	EntityList << RED_SIDE << BLUE_SIDE;
	ComboBox_ExitJudgment_Sides = new QComboBox;
	ComboBox_ExitJudgment_Sides->setEditable(true);
	ComboBox_ExitJudgment_Sides->lineEdit()->setAlignment(Qt::AlignCenter);
	ComboBox_ExitJudgment_Sides->lineEdit()->setReadOnly(true);
	ComboBox_ExitJudgment_Sides->addItems(EntityList);
	connect(ComboBox_ExitJudgment_Sides,SIGNAL(currentIndexChanged(int)),this,SLOT(slotExitJudgmentSides(int)));

	ComboBox_ExitJudgment_Entity = new QComboBox;
	ComboBox_ExitJudgment_Entity->addItem(QString::fromLocal8Bit("All"));
	ComboBox_ExitJudgment_Entity->setStyleSheet("QComboBox {combobox-popup: 1;}");
	ComboBox_ExitJudgment_Entity->setEditable(true);
	ComboBox_ExitJudgment_Entity->lineEdit()->setAlignment(Qt::AlignCenter);
	ComboBox_ExitJudgment_Entity->lineEdit()->setReadOnly(true);
	//connect(ComboBox_ExitJudgment_Entity,SIGNAL(currentIndexChanged(int)),this,SLOT(slotExitJudgment_Entity(int)));
}

//退出裁决
void ExitJudgment::createTableView()
{ 
	QStringList HeaderList_ExitJudgment;
	HeaderList_ExitJudgment<<QString::fromLocal8Bit("实体名称")<<QString::fromLocal8Bit("生存状态")<<QString::fromLocal8Bit("退出控制")<<QString::fromLocal8Bit("");

	Table_ExitJudgment = new QTableWidget;
	Table_ExitJudgment->setColumnCount(HeaderList_ExitJudgment.size());
	Table_ExitJudgment->setFocusPolicy(Qt::NoFocus);
	Table_ExitJudgment->setEditTriggers(QAbstractItemView::NoEditTriggers);
	Table_ExitJudgment->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	Table_ExitJudgment->horizontalHeader()->setStyleSheet("QHeaderView::section{background:white;color: black;}");
	Table_ExitJudgment->verticalHeader()->hide();
	for (int a=0;a<HeaderList_ExitJudgment.size();a++) 
	{
		Table_ExitJudgment->setHorizontalHeaderItem(a,new QTableWidgetItem(HeaderList_ExitJudgment.at(a)));
	}
	Table_ExitJudgment->verticalHeader()->setDefaultSectionSize(30);
}

void ExitJudgment::setupLayout()
{
	Layout_ExitJudgment = new QVBoxLayout;
	QHBoxLayout* tmpLayout = new QHBoxLayout ;
	QWidget * tmpWidget= new QWidget;

	tmpLayout->addWidget(ComboBox_ExitJudgment_Sides);
	tmpLayout->addWidget(ComboBox_ExitJudgment_Entity);
	tmpLayout->addWidget(Button_ExitJudgment);
	tmpWidget->setLayout(tmpLayout);

	Layout_ExitJudgment->addWidget(tmpWidget);
	Layout_ExitJudgment->addWidget(Table_ExitJudgment);

	//初始化界面按钮
	Button_ExitJudgment->setEnabled(true);
	setLayout(Layout_ExitJudgment);
}

void ExitJudgment::resetUi()
{
	slotExitJudgmentSides(ComboBox_ExitJudgment_Sides->currentIndex());
}

void ExitJudgment::updataJudgmentData(const EquipmentJudgments& infos)
{
	QStringList tList =getExitJudgmentList();
	foreach(EquipmentJudgment info, infos)
	{
		int rowIndex = indexOfEquipmentName(Table_ExitJudgment, info.equipmentName, 0);
		if(rowIndex < 0)
		{
			int rowCount = Table_ExitJudgment->rowCount();

			Table_ExitJudgment->setRowCount(rowCount+1);
			//Table_ExitJudgment->setItem(rowCount,0, new QTableWidgetItem(info.equipmentName));
          //  getEntitiesByParty(ComboBox_ExitJudgment_Sides->currentText());
			Table_ExitJudgment->setItem(rowCount,0, new QTableWidgetItem(info.equipmentName));
			Table_ExitJudgment->setItem(rowCount,1, new QTableWidgetItem(info.equipmentStatus));

			QComboBox* ComboBox_EJ = new QComboBox;
			ComboBox_EJ->setEditable(true);
			ComboBox_EJ->lineEdit()->setAlignment(Qt::AlignCenter);
			ComboBox_EJ->lineEdit()->setReadOnly(true);
			ComboBox_EJ->addItems(tList);
			ComboBox_EJ->setCurrentIndex(0);

			Table_ExitJudgment->setCellWidget(rowCount,2,ComboBox_EJ);
			QPushButton* pbt_EJ = new QPushButton;
			pbt_EJ->setText(QString::fromLocal8Bit("确认修改"));
			Table_ExitJudgment->setCellWidget(rowCount,3, pbt_EJ);
			connect(pbt_EJ, SIGNAL(clicked()), this, SLOT(slotCellEJ()));
		}
		else
		{
			Table_ExitJudgment->item(rowIndex, 1)->setText(info.equipmentStatus);
		}
	}
}

void ExitJudgment::stopHandle()
{
	slotBtnExitJudgment();
}

void ExitJudgment::slotExitJudgmentSides(int index)
{
	ComboBox_ExitJudgment_Entity->clear();
	ComboBox_ExitJudgment_Entity->addItems(getEntitiesByParty(ComboBox_ExitJudgment_Sides->currentText()));
}

void ExitJudgment::slotBtnExitJudgment()
{
	if(isDataUpdata())
	{
		stopDataHandle();
		if(!getSingleFlage())
		{
			ComboBox_ExitJudgment_Sides->setEnabled(true);
			ComboBox_ExitJudgment_Entity->setEnabled(true);
		}
		Button_ExitJudgment->setText(START_QUERY);
	}
	else
	{
		startDataHandle(1000);
		ComboBox_ExitJudgment_Sides->setEnabled(false);
		ComboBox_ExitJudgment_Entity->setEnabled(false);
		Button_ExitJudgment->setText(STOP_QUERY);
	}
}

void ExitJudgment::preHandleDataFrame()
{
	Table_ExitJudgment->clearContents();
	Table_ExitJudgment->setRowCount(0);
}

void ExitJudgment::handleDataFrame()
{
	QString side = ComboBox_ExitJudgment_Sides->currentText();
	EquipmentJudgments infos;
	if(side == QString::fromLocal8Bit("红方") )
	{
		foreach(ByyEntityObject* entObject,m_redRemoteEntities)
		{
			EquipmentJudgment info;
			info.equipmentName = entObject->name();
			info.equipmentStatus = getJudgmentList().at(entObject->damageState());
			infos.append(info);
		}
	}

	if(side == QString::fromLocal8Bit("蓝方") )
	{
		foreach(ByyEntityObject* entObject,m_blueRemoteEntities)
		{
			EquipmentJudgment info;
			info.equipmentName = entObject->name();
			info.equipmentStatus = getJudgmentList().at(entObject->damageState());
			infos.append(info);
		}
	}

	updataJudgmentData(infos);


	/*
	EquipmentJudgments infos;
	ByyEntityObject* entityObject = getEntityByName(ComboBox_ExitJudgment_Entity->currentText());
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
				info.equipmentStatus = getJudgmentList().at(entityObject->damageState());
				infos.append(info);
			}
		}
		updataJudgmentData(infos);
	}
	*/
}

void ExitJudgment::slotCellEJ()
{
	QWidget* cellwidget = qobject_cast<QWidget*>(sender());
	int currentRow = indexOfCellWidget(Table_ExitJudgment, cellwidget, 3);
	if(currentRow >= 0)
	{
		QString entityname = ComboBox_ExitJudgment_Entity->currentText();
		QString equpmentname = Table_ExitJudgment->item(currentRow, 0)->text();
		int status = ByyDamageNone;
		QComboBox* combox = qobject_cast<QComboBox*>(Table_ExitJudgment->cellWidget(currentRow, 2));
		if(combox) status = combox->currentIndex();
		sendStatusChangeCommand(entityname, equpmentname,status);
	}
}

