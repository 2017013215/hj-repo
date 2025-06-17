#include "stdafx.h"
#include "WarDamageStatistics.h" 
#include "Verdict.h"

WarDamageStatistics::WarDamageStatistics(VerdictBaseWidget* parent /*= nullptr*/)
	: VerdictBaseWidget(parent)
{
	createWidget();
	setupLayout();
	updateEntities();
	resetUi();
	startDataHandle(1000);
}

void WarDamageStatistics::createButton()
{

}

void WarDamageStatistics::createChart()
{

}

// 战损统计
void WarDamageStatistics::createComBox()
{
	QStringList EntityList;
	EntityList << RED_SIDE << BLUE_SIDE;
	ComboBox_WarDamageStatistics_Sides = new QComboBox;
	ComboBox_WarDamageStatistics_Sides->setEditable(true);
	ComboBox_WarDamageStatistics_Sides->lineEdit()->setAlignment(Qt::AlignCenter);
	ComboBox_WarDamageStatistics_Sides->lineEdit()->setReadOnly(true);
	ComboBox_WarDamageStatistics_Sides->addItems(EntityList);
	connect(ComboBox_WarDamageStatistics_Sides,SIGNAL(currentIndexChanged(int)),this,SLOT(slotWarDamageStatisticsSides(int)));

	ComboBox_WarDamageStatistics_DamageLevel = new QComboBox;
	ComboBox_WarDamageStatistics_DamageLevel->setStyleSheet("QComboBox {combobox-popup: 1;}");
	ComboBox_WarDamageStatistics_DamageLevel->setEditable(true);
	ComboBox_WarDamageStatistics_DamageLevel->addItems(getDamageJudgmentList());
	ComboBox_WarDamageStatistics_DamageLevel->lineEdit()->setAlignment(Qt::AlignCenter);
	ComboBox_WarDamageStatistics_DamageLevel->lineEdit()->setReadOnly(true);
	connect(ComboBox_WarDamageStatistics_DamageLevel,SIGNAL(currentIndexChanged(int)),this,SLOT(slotWarDamageStatisticsDamageLevel(int)));
}

// 毁伤裁决(导弹对水面舰艇)
void WarDamageStatistics::createTableView()
{
	QStringList HeaderList_WarDamageStatistics_Munition;
	HeaderList_WarDamageStatistics_Munition<<QString::fromLocal8Bit("弹药名称")<<QString::fromLocal8Bit("消耗数量");

	Model_WarDamageStatistics_Munition = new QStandardItemModel;
	Table_WarDamageStatistics_Munition = new QTableView;
	Table_WarDamageStatistics_Munition->setModel(Model_WarDamageStatistics_Munition);
	Model_WarDamageStatistics_Munition->setHorizontalHeaderLabels(HeaderList_WarDamageStatistics_Munition);
	Table_WarDamageStatistics_Munition->setFocusPolicy(Qt::NoFocus);
	Table_WarDamageStatistics_Munition->setEditTriggers(QAbstractItemView::NoEditTriggers);
	Table_WarDamageStatistics_Munition->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	Table_WarDamageStatistics_Munition->horizontalHeader()->setStyleSheet("QHeaderView::section{background:white;color: black;}");
	Table_WarDamageStatistics_Munition->verticalHeader()->hide();
	Table_WarDamageStatistics_Munition->verticalHeader()->setDefaultSectionSize(30);

	HeaderList_WarDamageStatistics_Munition.clear();
	HeaderList_WarDamageStatistics_Munition<<QString::fromLocal8Bit("装备名称")<<QString::fromLocal8Bit("损失数量");

	Model_WarDamageStatistics_Damage = new QStandardItemModel;
	Table_WarDamageStatistics_Damage = new QTableView;
	Table_WarDamageStatistics_Damage->setModel(Model_WarDamageStatistics_Damage);
	Model_WarDamageStatistics_Damage->setHorizontalHeaderLabels(HeaderList_WarDamageStatistics_Munition);
	Table_WarDamageStatistics_Damage->setFocusPolicy(Qt::NoFocus);
	Table_WarDamageStatistics_Damage->setEditTriggers(QAbstractItemView::NoEditTriggers);
	Table_WarDamageStatistics_Damage->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	Table_WarDamageStatistics_Damage->horizontalHeader()->setStyleSheet("QHeaderView::section{background:white;color: black;}");
	Table_WarDamageStatistics_Damage->verticalHeader()->hide();
	Table_WarDamageStatistics_Damage->verticalHeader()->setDefaultSectionSize(30);
}

void WarDamageStatistics::setupLayout()
{
	Layout_WarDamageStatistics = new QVBoxLayout;
	QHBoxLayout* tmpLayout = new QHBoxLayout ;
	QWidget * tmpWidget = new QWidget;
	QLabel * str = new QLabel(QString::fromLocal8Bit("弹药消耗")) ;
	Layout_WarDamageStatistics->addWidget( ComboBox_WarDamageStatistics_Sides);
	Layout_WarDamageStatistics->addWidget( str);
	Layout_WarDamageStatistics->addWidget( Table_WarDamageStatistics_Munition);
	str = new QLabel(QString::fromLocal8Bit("兵力损失")) ; 
	tmpLayout->addWidget(str );
	tmpLayout->addWidget(ComboBox_WarDamageStatistics_DamageLevel);
	tmpWidget->setLayout(tmpLayout);
	Layout_WarDamageStatistics->addWidget(tmpWidget);
	Layout_WarDamageStatistics->addWidget( Table_WarDamageStatistics_Damage);

	setLayout(Layout_WarDamageStatistics);
}

void WarDamageStatistics::resetUi()
{
}

void WarDamageStatistics::preHandleDataFrame()
{
	Model_WarDamageStatistics_Munition->removeRows(0, Model_WarDamageStatistics_Munition->rowCount()); //清空所有的数据
	Model_WarDamageStatistics_Damage->removeRows(0, Model_WarDamageStatistics_Damage->rowCount()); //清空所有的数据
}

void WarDamageStatistics::handleDataFrame()
{
	EntityTypeExpendInfos ammunitioninfos;
	EntityTypeExpendInfos entityinfos;
#if 0 //测试
	EntityTypeExpendInfo am;
	am.entityTypeName = "daodan";
	am.entityNumber = 12;
	ammunitioninfos.append(am);
	EntityTypeExpendInfo en;
	en.entityTypeName = "feiji";
	en.entityNumber = 10;
	entityinfos.append(en);
#else
	QString side = ComboBox_WarDamageStatistics_Sides->currentText();
	int status = ComboBox_WarDamageStatistics_DamageLevel->currentIndex()+1;
	ammunitioninfos = Verdict::getInstance()->getAmmunitionExpendInfos(side);
	entityinfos = Verdict::getInstance()->getEntityExpendInfos(side, status);
#endif
	updataWarDamageStatisticsData(ammunitioninfos, entityinfos);
}

void WarDamageStatistics::updataWarDamageStatisticsData(const EntityTypeExpendInfos& ammunitioninfos, const EntityTypeExpendInfos& entityinfos)
{
	Model_WarDamageStatistics_Munition->removeRows(0, Model_WarDamageStatistics_Munition->rowCount()); //清空所有的数据
	for(int row = 0; row < ammunitioninfos.size(); row++)
	{
		Model_WarDamageStatistics_Munition->setItem(row,0,new QStandardItem(ammunitioninfos[row].entityTypeName));
		Model_WarDamageStatistics_Munition->setItem(row,1,new QStandardItem(QString::number(ammunitioninfos[row].entityNumber)));
	}
	Model_WarDamageStatistics_Damage->removeRows(0, Model_WarDamageStatistics_Damage->rowCount()); //清空所有的数据
	for(int row = 0; row < entityinfos.size(); row++)
	{
		Model_WarDamageStatistics_Damage->setItem(row,0,new QStandardItem(entityinfos[row].entityTypeName));
		Model_WarDamageStatistics_Damage->setItem(row,1,new QStandardItem(QString::number(entityinfos[row].entityNumber)));
	}
}

void WarDamageStatistics::stopHandle()
{

}

void WarDamageStatistics::slotWarDamageStatisticsSides(int index)
{
	Q_UNUSED(index)
	handleDataFrame();
}

void WarDamageStatistics::slotWarDamageStatisticsDamageLevel(int index)
{
	Q_UNUSED(index)
	handleDataFrame();
}
