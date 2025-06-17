#include "StdAfx.h"
#include "behaviortreeeditwdg.h"
#include "localEntity.h"
#include "ByyApp.h"
#include "ByyCore.h"
#include "ByyAttributeEditMng.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QMenu>
#include <QAction>
#include <QTreeWidgetItem>
#include <QComboBox>
#include <QActionGroup>
#include <QDebug.h>
#include <QTextCodec>
#include "behaviortreetablewidgetdelegate.h"
#include "behaviortreetaskdlg.h"
#include "behaviortreetriggerdlg.h"
#include "behaviortreetriggernudlg.h"
#include "forcetask.h"
#include "btnroot.h"
#include "btntrigger.h"
#include "btncompose.h"
#include "btntime.h"
#include "btncondition.h"
#include "btnloop.h"
#include "btnbreak.h"
#include "btnend.h"

Q_DECLARE_METATYPE(NodeType)
Q_DECLARE_METATYPE(ForceTask*)
Q_DECLARE_METATYPE(BTNRoot*)
Q_DECLARE_METATYPE(BTNTrigger*)
Q_DECLARE_METATYPE(BTNCompose*)
Q_DECLARE_METATYPE(BTNTime*)
Q_DECLARE_METATYPE(BTNCondition*)
Q_DECLARE_METATYPE(BTNLoop*)
Q_DECLARE_METATYPE(BTNBreak*)
Q_DECLARE_METATYPE(BTNEnd*)

static int countSign=0;
static int m_count=0;
BehaviorTreeEditWdg::BehaviorTreeEditWdg(ByyCore& core,ByyAttributeEditMng &atmng,ByyLocalEntity* entObj,QWidget *parent)
	: QWidget(parent)
	,myEntObj(entObj)
	,myCore(core)
	,myATMng(atmng)
{
	 
	ui.setupUi(this);
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBK"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK"));
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));

	this->setWindowTitle(QString::fromLocal8Bit("行为树编辑"));
	this->setWindowFlags(Qt::Window/* | Qt::WindowTitleHint | Qt::CustomizeWindowHint*/);
	this->setWindowFlags(this->windowFlags()&~Qt::WindowMinMaxButtonsHint);
	m_CurItem = NULL;
	myEditParameters = entObj->getParameters();
	ParameterList::iterator it=myEditParameters.begin();
	for (;it!=myEditParameters.end();++it)
	{
		QString groupName = (*it)->Group;
		if (groupName.contains(QString::fromLocal8Bit("行为树")))
		{
			if ((*it)->Name == QString::fromLocal8Bit("PlanRef"))
				m_Path = (*it)->getValue();
			else if((*it)->Name == QString::fromLocal8Bit("Plan"))
				m_Content = (*it)->getValue();
		}
	}
	ByyTaskManager::TaskList& taskList=entObj->taskManager().taskList();
	//QString k=analysisTsk(taskList);
	m_pos=0;
    m_token=TOKEN_UNKNOWN;
	m_lastrow=m_lastcol=m_row=m_col=1;
	m_bTokenParseStop=0;
	m_Content = m_Content.replace("$gt;",">");
	m_Content = m_Content.replace("&lt;","<");
	m_Content = m_Content.replace("   ","\n");
	QByteArray cdata=m_Content.toLocal8Bit();
	m_content=string(cdata);
	m_length=m_content.length();
	
    m_count=0;
	countSign=0;
	qDebug()<<"******"<<m_Content;
	initWidgetLayout();
	parse();
}

BehaviorTreeEditWdg::~BehaviorTreeEditWdg()
{

}

void BehaviorTreeEditWdg::initWidgetLayout()
{
	//节点列表初始化
	ui.treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.treeWidget->expandAll();
	m_TreeRootMenu = new QMenu();
	m_AddRootNode = new QAction(QString::fromLocal8Bit("添加任务包"),this); //添加根节点
	m_ClearChildNode = new QAction(QString::fromLocal8Bit("清空子元素"),this);
	m_DeleteNode = new QAction(QString::fromLocal8Bit("删除"),this);
	m_TreeNodeMenu = new QMenu(QString::fromLocal8Bit("新建"));
	m_TaskNode = new QAction(QString::fromLocal8Bit("任务节点"),this);
	m_ComposeNode = new QAction(QString::fromLocal8Bit("任务包"),this); //复合节点
	m_LoopNode = new QAction(QString::fromLocal8Bit("循环节点"),this);
	m_TimeNode = new QAction(QString::fromLocal8Bit("时间节点"),this);
	m_ConditionNode = new QAction(QString::fromLocal8Bit("条件节点"),this);
	m_BreakNode = new QAction(QString::fromLocal8Bit("跳出"),this);
	m_EndNode = new QAction(QString::fromLocal8Bit("结束"),this);
	m_TriggerNode = new QAction(QString::fromLocal8Bit("触发器"),this);
	m_TriggerNumberMenu = new QMenu(QString::fromLocal8Bit("设置次数"));
	m_TriggerNumberUnlimited = new QAction(QString::fromLocal8Bit("无限制"),this);
	m_TriggerNumberUnlimited->setCheckable(true);
	m_TriggerNumberSet = new QAction(QString::fromLocal8Bit("设置"),this);
	m_TriggerNumberSet->setCheckable(true);
	m_TriggerNuGroup = new QActionGroup(this);
	m_TriggerNuGroup->addAction(m_TriggerNumberUnlimited);
	m_TriggerNuGroup->addAction(m_TriggerNumberSet);
	m_TriggerInterruptPolicyForSuccessMenu = new QMenu(QString::fromLocal8Bit("触发任务成功后策略"));
	m_TriggerSuccessUnInterrupt = new QAction(QString::fromLocal8Bit("UnInterrupt"),this);
	m_TriggerSuccessUnInterrupt->setCheckable(true);
	m_TriggerSuccessFinish = new QAction(QString::fromLocal8Bit("Finish"),this);
	m_TriggerSuccessFinish->setCheckable(true);
	m_TriggerSuccessCurrent = new QAction(QString::fromLocal8Bit("Current"),this);
	m_TriggerSuccessCurrent->setCheckable(true);
	m_TriggerSuccessNext = new QAction(QString::fromLocal8Bit("Next"),this);
	m_TriggerSuccessNext->setCheckable(true);
	m_TriggerSuccessReset = new QAction(QString::fromLocal8Bit("Reset"),this);
	m_TriggerSuccessReset->setCheckable(true);
	m_TriggerInterruptPolicyForSuccessGroup = new QActionGroup(this);
	m_TriggerInterruptPolicyForSuccessGroup->addAction(m_TriggerSuccessUnInterrupt);
	m_TriggerInterruptPolicyForSuccessGroup->addAction(m_TriggerSuccessFinish);
	m_TriggerInterruptPolicyForSuccessGroup->addAction(m_TriggerSuccessCurrent);
	m_TriggerInterruptPolicyForSuccessGroup->addAction(m_TriggerSuccessNext);
	m_TriggerInterruptPolicyForSuccessGroup->addAction(m_TriggerSuccessReset);
	m_TriggerInterruptPolicyForFaileMenu = new QMenu(QString::fromLocal8Bit("触发任务失败后策略"));
	m_TriggerFaileUnInterrupt = new QAction(QString::fromLocal8Bit("UnInterrupt"),this);
	m_TriggerFaileUnInterrupt->setCheckable(true);
	m_TriggerFaileFinish = new QAction(QString::fromLocal8Bit("Finish"),this);
	m_TriggerFaileFinish->setCheckable(true);
	m_TriggerFaileCurrent = new QAction(QString::fromLocal8Bit("Current"),this);
	m_TriggerFaileCurrent->setCheckable(true);
	m_TriggerFaileNext = new QAction(QString::fromLocal8Bit("Next"),this);
	m_TriggerFaileNext->setCheckable(true);
	m_TriggerFaileReset = new QAction(QString::fromLocal8Bit("Reset"),this);
	m_TriggerFaileReset->setCheckable(true);
	m_TriggerInterruptPolicyForFaileGroup = new QActionGroup(this);
	m_TriggerInterruptPolicyForFaileGroup->addAction(m_TriggerFaileUnInterrupt);
	m_TriggerInterruptPolicyForFaileGroup->addAction(m_TriggerFaileFinish);
	m_TriggerInterruptPolicyForFaileGroup->addAction(m_TriggerFaileCurrent);
	m_TriggerInterruptPolicyForFaileGroup->addAction(m_TriggerFaileNext);
	m_TriggerInterruptPolicyForFaileGroup->addAction(m_TriggerFaileReset);
	connect(m_AddRootNode,SIGNAL(triggered()),this,SLOT(on_m_AddRootNode_triggered()));
	connect(m_ClearChildNode,SIGNAL(triggered()),this,SLOT(on_m_ClearChildNode_triggered()));
	connect(m_TaskNode,SIGNAL(triggered()),this,SLOT(on_m_TaskNode_triggered()));
	connect(m_ComposeNode,SIGNAL(triggered()),this,SLOT(on_m_ComposeNode_triggered()));
	connect(m_LoopNode,SIGNAL(triggered()),this,SLOT(on_m_LoopNode_triggered()));
	connect(m_TimeNode,SIGNAL(triggered()),this,SLOT(on_m_TimeNode_triggered()));
	connect(m_ConditionNode,SIGNAL(triggered()),this,SLOT(on_m_ConditionNode_triggered()));
	connect(m_BreakNode,SIGNAL(triggered()),this,SLOT(on_m_BreakNode_triggered()));
	connect(m_EndNode,SIGNAL(triggered()),this,SLOT(on_m_EndNode_triggered()));
	connect(m_TriggerNode,SIGNAL(triggered()),this,SLOT(on_m_TriggerNode_triggered()));
	connect(m_DeleteNode,SIGNAL(triggered()),this,SLOT(on_m_DeleteNode_triggered()));
	connect(m_TriggerNumberUnlimited,SIGNAL(triggered()),this,SLOT(on_m_TriggerNumberUnlimited_triggered()));
	connect(m_TriggerNumberSet,SIGNAL(triggered()),this,SLOT(on_m_TriggerNumberSet_triggered()));
	connect(m_TriggerSuccessUnInterrupt,SIGNAL(triggered()),this,SLOT(on_m_TriggerSuccessUnInterrupt_triggered()));
	connect(m_TriggerSuccessFinish,SIGNAL(triggered()),this,SLOT(on_m_TriggerSuccessFinish_triggered()));
	connect(m_TriggerSuccessCurrent,SIGNAL(triggered()),this,SLOT(on_m_TriggerSuccessCurrent_triggered()));
	connect(m_TriggerSuccessNext,SIGNAL(triggered()),this,SLOT(on_m_TriggerSuccessNext_triggered()));
	connect(m_TriggerSuccessReset,SIGNAL(triggered()),this,SLOT(on_m_TriggerSuccessReset_triggered()));
	connect(m_TriggerFaileUnInterrupt,SIGNAL(triggered()),this,SLOT(on_m_TriggerFaileUnInterrupt_triggered()));
	connect(m_TriggerFaileFinish,SIGNAL(triggered()),this,SLOT(on_m_TriggerFaileFinish_triggered()));
	connect(m_TriggerFaileCurrent,SIGNAL(triggered()),this,SLOT(on_m_TriggerFaileCurrent_triggered()));
	connect(m_TriggerFaileNext,SIGNAL(triggered()),this,SLOT(on_m_TriggerFaileNext_triggered()));
	connect(m_TriggerFaileReset,SIGNAL(triggered()),this,SLOT(on_m_TriggerFaileReset_triggered()));
	m_AddRootNode->setIcon( QIcon("../data/icons/BehaviorTree/BTNCompose.png") );
	m_ClearChildNode->setIcon( QIcon("../data/icons/BehaviorTree//clear.png") );
	m_TaskNode->setIcon( QIcon("../data/icons/BehaviorTree//BTNTask.png") );
	m_ComposeNode->setIcon( QIcon("../data/icons/BehaviorTree//BTNCompose.png") );
	m_LoopNode->setIcon( QIcon("../data/icons/BehaviorTree//BTNLoop.png") );
	m_TimeNode->setIcon( QIcon("../data/icons/BehaviorTree//BTNTime.png") );
	m_ConditionNode->setIcon( QIcon("../data/icons/BehaviorTree//BTNCondition.png") );
	m_BreakNode->setIcon( QIcon("../data/icons/BehaviorTree//BTNBreak.png") );
	m_EndNode->setIcon( QIcon("../data/icons/BehaviorTree//BTNEnd.png") );
	m_TriggerNode->setIcon( QIcon("../data/icons/BehaviorTree//BTNTrigger.png") );
	m_DeleteNode->setIcon( QIcon("../data/icons/BehaviorTree//delete.png") );
	m_TreeRootMenu->addAction(m_AddRootNode);
	m_TreeRootMenu->addAction(m_ClearChildNode);
	m_TreeRootMenu->addSeparator();
	m_TreeRootMenu->addAction(m_DeleteNode);
	m_TreeRootMenu->addSeparator();
	m_TreeRootMenu->addMenu(m_TreeNodeMenu);
	m_TreeNodeMenu->addAction(m_TaskNode);
	m_TreeNodeMenu->addAction(m_ComposeNode);
	//m_TreeNodeMenu->addAction(m_LoopNode);
	//m_TreeNodeMenu->addAction(m_TimeNode);
	//m_TreeNodeMenu->addAction(m_ConditionNode);
	//m_TreeNodeMenu->addAction(m_BreakNode);
	//m_TreeNodeMenu->addAction(m_EndNode);
	m_TreeNodeMenu->addAction(m_TriggerNode);
	m_TreeRootMenu->addSeparator();
	m_TreeRootMenu->addMenu(m_TriggerNumberMenu);
	m_TriggerNumberMenu->addAction(m_TriggerNumberUnlimited);
	m_TriggerNumberMenu->addAction(m_TriggerNumberSet);
	m_TreeRootMenu->addMenu(m_TriggerInterruptPolicyForSuccessMenu);
	m_TriggerInterruptPolicyForSuccessMenu->addAction(m_TriggerSuccessUnInterrupt);
	m_TriggerInterruptPolicyForSuccessMenu->addAction(m_TriggerSuccessFinish);
	m_TriggerInterruptPolicyForSuccessMenu->addAction(m_TriggerSuccessCurrent);
	m_TriggerInterruptPolicyForSuccessMenu->addAction(m_TriggerSuccessNext);
	m_TriggerInterruptPolicyForSuccessMenu->addAction(m_TriggerSuccessReset);
	m_TreeRootMenu->addMenu(m_TriggerInterruptPolicyForFaileMenu);
	m_TriggerInterruptPolicyForFaileMenu->addAction(m_TriggerFaileUnInterrupt);
	m_TriggerInterruptPolicyForFaileMenu->addAction(m_TriggerFaileFinish);
	m_TriggerInterruptPolicyForFaileMenu->addAction(m_TriggerFaileCurrent);
	m_TriggerInterruptPolicyForFaileMenu->addAction(m_TriggerFaileNext);
	m_TriggerInterruptPolicyForFaileMenu->addAction(m_TriggerFaileReset);
	ui.pushButton_load->setIcon(QIcon("../data/icons/FileOpen.png"));
	ui.pushButton_save->setIcon(QIcon("../data/icons/FileSave.png"));
	//参数列表初始化
	ui.tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	m_TableWdgDelegate = new BehaviorTreeTableWidgetDelegate(ui.tableWidget);
	ui.tableWidget->setItemDelegate(m_TableWdgDelegate);
	//文本内容初始化
	//XML的转义符
	//&lt; <
	//&gt; >
	//&amp; &
	//&apos; '
	//&quot; "
	//	nbsp; 空格
	m_Content = m_Content.replace("$gt;",">");
	//m_Content = m_Content.replace("&lt;","<");
	ui.textEdit->setText(m_Content);
}

ParameterList BehaviorTreeEditWdg::GetParameterList()
{
	return myEditParameters;
}

void BehaviorTreeEditWdg::updateParam(const QString& name,const QString& value)
{
	ByyParameter *p=myEditParameters.findParameter(name);
	if (p)
	{
		p->setValue(value);
	}
}

void BehaviorTreeEditWdg::updateTableWidget(ForceTask *task)
{
	int row = task->getList().size();
	ui.tableWidget->setRowCount(row);
	int rowTmp = 0;
	QStringList list;
	QMap<QString,QStringList> map;
	for (int i=0;i<row;i++)
	{
		RefAttr_Task attr = task->getList().at(i);
		if( "true" != attr.editEnable && !attr.editEnable.isEmpty()) continue;
		QTableWidgetItem *itemtmp = setTableItemText(rowTmp,0,attr.note);
		if(itemtmp)
		{
			itemtmp->setData(Qt::UserRole, attr.idf);
		}
		setTableItemText(rowTmp,1,attr.initVal);

		//代理
		AttrEditType eType;
		if(myATMng.getTaskRefAttricbeEditType(attr.idf, eType))
		{
			if(!eType.sourceInfo.isEmpty() && !eType.values.isEmpty())
			{
				list.append(attr.note);
				map.insert(attr.note,eType.values.keys());

				setTableItemText(rowTmp,1,eType.values.key(attr.initVal));
			}
			else if(!eType.sourceInfo.isEmpty())
			{
				if(("INNER" == eType.sourceInfo.at(0)) && (eType.sourceInfo.size() >= 2))
				{
					if("CTRL" == eType.sourceInfo.at(1))
					{
						//控制对象
						QList<ByyControlObject*> ctrlObjs = myCore.localObjectManager().controlObjects().values();
						QStringList objList;
						for(int j = 0; j < ctrlObjs.size();j++)
						{
							ByyControlObject* tmpObj = ctrlObjs.at(j);
							if(myEntObj->forceType() == tmpObj->forceType())
							{
								objList.append(tmpObj->name());
							}
						}
						list.append(attr.note);
						map.insert(attr.note,objList);

						setTableItemText(rowTmp,1,attr.initVal);
					}
					else if(("ENTITY" == eType.sourceInfo.at(1)) && (eType.sourceInfo.size() >= 3))
					{
						QString friendState = eType.sourceInfo.at(2);
						//攻击对象
						QList<ByyEntityObject*> entityObjs = myCore.localObjectManager().entityObjects().values();
						QStringList objList;
						for(int j = 0; j < entityObjs.size();j++)
						{
							ByyEntityObject* tmpObj = entityObjs.at(j);
							if(("HOSTILE" == friendState) && (myEntObj->forceType() != tmpObj->forceType()))
							{
								objList.append(tmpObj->name());
							}
							else if(("FRIEND" == friendState) && (myEntObj->forceType() == tmpObj->forceType()))
							{
								objList.append(tmpObj->name());
							}
						}
						list.append(attr.note);
						map.insert(attr.note,objList);

						setTableItemText(rowTmp,1,attr.initVal);
					}
					else if(("DEV" == eType.sourceInfo.at(1)) && (eType.sourceInfo.size() >= 3))
					{
						QStringList devList;

						ByyDescribeEntry *descEntry=myEntObj->describeEntry();
						QString devType = eType.sourceInfo.at(2).toLower();
						if (descEntry)
						{
							const ByyDescribeEntry::ComponentList& components=descEntry->getComponent(devType);

							for (int j=0;j!=components.size();++j)
							{
								devList.append(components[j].getTag());
							}
						}
						list.append(attr.note);
						map.insert(attr.note,devList);

						setTableItemText(rowTmp,1,attr.initVal);
					}
				}
			}
		}

		rowTmp++;
	}
	m_TableWdgDelegate->setNameList(list);
	m_TableWdgDelegate->setMap(map);

	for (int i = 0; i < (row-rowTmp); i++)
	{
		ui.tableWidget->removeRow(ui.tableWidget->rowCount()-1);
	}
}

void BehaviorTreeEditWdg::updateTableWidget(BTNRoot *root)
{
	int row = root->getList().size();
	ui.tableWidget->setRowCount(row);
	QStringList list;
	QMap<QString,QStringList> map;
	for (int i=0;i<row;i++)
	{
		RefAttr_Root attr = root->getList().at(i);
		setTableItemText(i,0,attr.note);
		setTableItemText(i,1,attr.initVal);
		//代理
		if(attr.type == "combox")
		{
			list.append(attr.note);
			map.insert(attr.note,attr.valueList);
		}
	}
	m_TableWdgDelegate->setNameList(list);
	m_TableWdgDelegate->setMap(map);
}

void BehaviorTreeEditWdg::updateTableWidget(BTNTrigger *trigger)
{
	int row = trigger->getList().size();
	ui.tableWidget->setRowCount(row);
	QStringList list;
	QMap<QString,QStringList> map;
	for (int i=0;i<row;i++)
	{
		RefAttr_Trigger attr = trigger->getList().at(i);
		setTableItemText(i,0,attr.note);
		setTableItemText(i,1,attr.initVal);
		//代理
		if(attr.type == "combox")
		{
			list.append(attr.note);
			map.insert(attr.note,attr.valueList);
		}
	}
	m_TableWdgDelegate->setNameList(list);
	m_TableWdgDelegate->setMap(map);
}

void BehaviorTreeEditWdg::updateTableWidget(BTNCompose *compose)
{
	int row = compose->getList().size();
	ui.tableWidget->setRowCount(row);
	QStringList list;
	QMap<QString,QStringList> map;
	for (int i=0;i<row;i++)
	{
		RefAttr_Compose attr = compose->getList().at(i);
		setTableItemText(i,0,attr.note);
		setTableItemText(i,1,attr.initVal);
		//代理
		if(attr.type == "combox")
		{
			list.append(attr.note);
			map.insert(attr.note,attr.valueList);
		}
	}
	m_TableWdgDelegate->setNameList(list);
	m_TableWdgDelegate->setMap(map);
}

void BehaviorTreeEditWdg::updateTableWidget(BTNTime *time)
{
	int row = time->getList().size();
	ui.tableWidget->setRowCount(row);
	QStringList list;
	QMap<QString,QStringList> map;
	for (int i=0;i<row;i++)
	{
		RefAttr_Time attr = time->getList().at(i);
		setTableItemText(i,0,attr.note);
		setTableItemText(i,1,attr.initVal);
		//代理
		if(attr.type == "combox")
		{
			list.append(attr.note);
			map.insert(attr.note,attr.valueList);
		}
	}
	m_TableWdgDelegate->setNameList(list);
	m_TableWdgDelegate->setMap(map);
}

void BehaviorTreeEditWdg::updateTableWidget(BTNCondition *condition)
{
	int row = condition->getList().size();
	ui.tableWidget->setRowCount(row);
	QStringList list;
	QMap<QString,QStringList> map;
	for (int i=0;i<row;i++)
	{
		RefAttr_Condition attr = condition->getList().at(i);
		setTableItemText(i,0,attr.note);
		setTableItemText(i,1,attr.initVal);
		//代理
		if(attr.type == "combox")
		{
			list.append(attr.note);
			map.insert(attr.note,attr.valueList);
		}
	}
	m_TableWdgDelegate->setNameList(list);
	m_TableWdgDelegate->setMap(map);
}

void BehaviorTreeEditWdg::updateTableWidget(BTNLoop *loop)
{
	int row = loop->getList().size();
	ui.tableWidget->setRowCount(row);
	QStringList list;
	QMap<QString,QStringList> map;
	for (int i=0;i<row;i++)
	{
		RefAttr_Loop attr = loop->getList().at(i);
		setTableItemText(i,0,attr.note);
		setTableItemText(i,1,attr.initVal);
		//代理
		if(attr.type == "combox")
		{
			list.append(attr.note);
			map.insert(attr.note,attr.valueList);
		}
	}
	m_TableWdgDelegate->setNameList(list);
	m_TableWdgDelegate->setMap(map);
}

void BehaviorTreeEditWdg::updateTableWidget(BTNBreak *break_)
{
	int row = break_->getList().size();
	ui.tableWidget->setRowCount(row);
	QStringList list;
	QMap<QString,QStringList> map;
	for (int i=0;i<row;i++)
	{
		RefAttr_Break attr = break_->getList().at(i);
		setTableItemText(i,0,attr.note);
		setTableItemText(i,1,attr.initVal);
		//代理
		if(attr.type == "combox")
		{
			list.append(attr.note);
			map.insert(attr.note,attr.valueList);
		}
	}
	m_TableWdgDelegate->setNameList(list);
	m_TableWdgDelegate->setMap(map);
}

void BehaviorTreeEditWdg::updateTableWidget(BTNEnd *end)
{
	int row = end->getList().size();
	ui.tableWidget->setRowCount(row);
	QStringList list;
	QMap<QString,QStringList> map;
	for (int i=0;i<row;i++)
	{
		RefAttr_End attr = end->getList().at(i);
		setTableItemText(i,0,attr.note);
		setTableItemText(i,1,attr.initVal);
		//代理
		if(attr.type == "combox")
		{
			list.append(attr.note);
			map.insert(attr.note,attr.valueList);
		}
	}
	m_TableWdgDelegate->setNameList(list);
	m_TableWdgDelegate->setMap(map);
}

QTableWidgetItem* BehaviorTreeEditWdg::setTableItemText(int row,int column, QString text)
{
	QTableWidgetItem *  item = ui.tableWidget->item(row,column);
	if(!item)
		item = new QTableWidgetItem(text);
	//if(column ==1)
	//	item->setTextAlignment(Qt::AlignCenter);
	if(column ==0)
		item->setFlags(item->flags() &= (~Qt::ItemIsEditable));
	item->setText(text);
	ui.tableWidget->setItem(row,column,item);
	//item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable|Qt::ItemIsSelectable);
	item->setToolTip(text);
	return item;
}

void BehaviorTreeEditWdg::deleteTreeItem(QTreeWidgetItem *item)
{
	if(item->childCount() > 0)
	{
		for(int i = 0; i < item->childCount(); i ++)
		{
			deleteTreeItem(item->child(i));
		}
	}
	else
	{
		deleteBTNode(item);
		delete item;
	}
}

void BehaviorTreeEditWdg::addBTNode()
{

}

// 添加根节点
QTreeWidgetItem* BehaviorTreeEditWdg::addThreeRoot(QString name)
{
	QTreeWidgetItem *item=new QTreeWidgetItem(QStringList()<<name);
	ui.treeWidget->addTopLevelItem(item);
	return item;
}
// 添加子节点
QTreeWidgetItem* BehaviorTreeEditWdg::addThreeNode(QTreeWidgetItem *parent,QString name)
{
	QTreeWidgetItem *item=new QTreeWidgetItem(QStringList()<<name);
	parent->addChild(item);
	return item;
}

//解析任务名
QString BehaviorTreeEditWdg::analysisTaskName(QString str)
{
	QMap <QString,ForceTask*> m_list_attr=ForceTaskManager::instance()->getTaskMap();
	QMap <QString,ForceTask*>::const_iterator i;
	QString m_TaskName;
	for(i=m_list_attr.constBegin();i!=m_list_attr.constEnd();++i)
	{
		QString k  =i.value()->getObjectName();
		if( k ==str)
		{
			m_TaskName=i.key();
		}
	}

	return m_TaskName;
}

//解析触发器名
QString BehaviorTreeEditWdg::analysisTriggerName(QString str)
{
	QMap <QString,BTNTrigger*> m_list_attr=BTNTriggerManager::instance()->getTriggerMap();
	QMap <QString,BTNTrigger*>::const_iterator i;
	QString m_TriggeName;
	for(i=m_list_attr.constBegin();i!=m_list_attr.constEnd();++i)
	{
		QString k  =i.value()->getObjectName();
		if( k ==str)
		{
			m_TriggeName=i.key();
		}
	}

	return m_TriggeName;
//	QStringList btlist=attr.split(",");

}

//解析任务列表
QString BehaviorTreeEditWdg::analysisTsk(ByyTaskManager::TaskList& tsk)
{

	if (tsk.size()>0)
	{
		m_Content.clear();

		for (int i=0;i<tsk.size();i++)
		{
			ByyKeyValueList m_task=tsk[i]->getConfig();
			qDebug()<<"@@@@"<<m_task;
			QString m_TaskType;
			m_task.get("TaskType",m_TaskType);
			if (!m_TaskType.compare("SensorOper"))
			{
					QString m_OpeName;
					QString m_SensorOperType;
					QString m_Power;
					QString m_startTime;

					m_task.get("DeviceName",m_OpeName);
					m_task.get("SensorOperType",m_SensorOperType);
					m_task.get("PowerOn",m_Power);
					m_task.get("StartTime",m_startTime);
					
					QString m_OperTsk= QString(" BTNCompose(And,SeqAll){  BTNTrigger(BTCTime(>,%4,SIMTIME),Finish,Finish,-1){  BTNTask(CEquipmentOperTsk,TaskName,%1,%2,,%3,,,,,,,)}}").arg(m_OpeName).arg(m_SensorOperType).arg(m_Power).arg(m_startTime);
					m_TskContent.append(m_OperTsk);
			}else if(!m_TaskType.compare("MoveAlongRoute"))
			{
				QString m_CtrlObjName;
				QString m_startTime;

				m_task.get("CtrlObjName",m_CtrlObjName);
				m_task.get("StartTime",m_startTime);

				QString m_OperTsk= QString(" BTNCompose(And,SeqAll){  BTNTrigger(BTCTime(>,%1,SIMTIME),Finish,Finish,-1){ BTNTask(CMoveAlongTsk,TaskName,1,0,%2,,,0,0,,)}}").arg(m_startTime).arg(m_CtrlObjName);
				m_TskContent.append(m_OperTsk);
			}else if(!m_TaskType.compare("PatrolAlongRoute"))
			{
				QString m_CtrlObjName;
				QString m_startTime;

				m_task.get("CtrlObjName",m_CtrlObjName);
				m_task.get("StartTime",m_startTime);

				QString m_OperTsk= QString(" BTNCompose(And,SeqAll){  BTNTrigger(BTCTime(>,%1,SIMTIME),Finish,Finish,-1){ BTNTask(CPatrolTsk,TaskName,2,1,,,,,,%2,,,,)}}").arg(m_startTime).arg(m_CtrlObjName);
				m_TskContent.append(m_OperTsk);
			}else if(!m_TaskType.compare("PatrolInZone"))
			{
				QString m_CtrlObjName;
				QString m_startTime;

				m_task.get("CtrlObjName",m_CtrlObjName);
				m_task.get("StartTime",m_startTime);

				QString m_OperTsk= QString(" BTNCompose(And,SeqAll){  BTNTrigger(BTCTime(>,%1,SIMTIME),Finish,Finish,-1){ BTNTask(CPatrolTsk,TaskName,3,1,,,,,,%2,,,,)}}").arg(m_startTime).arg(m_CtrlObjName);
				m_TskContent.append(m_OperTsk);
			}else if(!m_TaskType.compare("TakeOff"))
			{
				QString m_startTime;

				m_task.get("StartTime",m_startTime);

				QString m_OperTsk= QString(" BTNCompose(And,SeqAll){  BTNTrigger(BTCTime(>,%1,SIMTIME),Finish,Finish,-1){ BTNTask(CTakeOffTsk,TaskName,1)}}").arg(m_startTime);
				m_TskContent.append(m_OperTsk);
			}else if(!m_TaskType.compare("ReturnBase"))
			{
				QString m_startTime;
				QString m_Location;
				QString m_RunWayHead;

				m_task.get("StartTime",m_startTime);
				m_task.get("Location",m_Location);
				m_task.get("RunWayHead",m_RunWayHead);
				QString m_Location2=m_Location.replace(QRegExp("\\,"),"/");

				QString m_OperTsk= QString(" BTNCompose(And,SeqAll){  BTNTrigger(BTCTime(>,%1,SIMTIME),Finish,Finish,-1){ BTNTask(CReturnBaseTsk,TaskName,1,1,,,%2,0,%3,1)}}").arg(m_startTime).arg(m_Location2).arg(m_RunWayHead);
				m_TskContent.append(m_OperTsk);
			}else if(!m_TaskType.compare("WeaponLaunch"))
			{
				QString m_startTime;
				QString m_WeaponPlatform;
				QString m_WeaponSystem;
				QString m_WeaponTag;
				QString m_LaunchNumber;
				QString m_TargetName;
				QString m_TargetLocation;
				

				m_task.get("StartTime",m_startTime);
				m_task.get("WeaponPlatform",m_WeaponPlatform);
				m_task.get("WeaponSystem",m_WeaponSystem);
				m_task.get("WeaponTag",m_WeaponTag);
				m_task.get("LaunchNumber",m_LaunchNumber);
				m_task.get("TargetName",m_TargetName);
				m_task.get("TargetLocation",m_TargetLocation);
				m_TargetLocation=m_TargetLocation.replace(QRegExp("\\,"),"/");

				QString m_OperTsk= QString(" BTNCompose(And,SeqAll){  BTNTrigger(BTCTime(>,%1,SIMTIME),Finish,Finish,-1){BTNTask(CASMRoutePlanTsk,taskName,1,%2,0,%3,%4,0,%5,0,3,%6,,1,5,0,1,90,0,,,,%7)}}").arg(m_startTime).arg(m_WeaponPlatform).arg(m_WeaponSystem).arg(m_WeaponTag).arg(m_TargetName).arg(m_TargetLocation).arg(m_LaunchNumber);
				m_TskContent.append(m_OperTsk);
			}
		}
		m_TemplateHead="BTNCompose(And,Pararell){";
		m_TemplateEnd="}";
		m_Content=QString("%1%2%3").arg(m_TemplateHead).arg(m_TskContent).arg(m_TemplateEnd);
	}

	return m_Content;
}

void BehaviorTreeEditWdg::deleteBTNode(QTreeWidgetItem *item)
{
	//目前只支持删除触发器和任务
	QVariant variant = item->data(0,Qt::UserRole); // 获取数据
	//触发器
	BTNTrigger *trigger = variant.value<BTNTrigger*>();
	if(trigger)
	{
		QTreeWidgetItem *parentItem = item->parent();
		if (parentItem)
		{
			QVariant pvariant = parentItem->data(0,Qt::UserRole); // 获取数据
			//根节点
			BTNRoot *root = pvariant.value<BTNRoot*>();
			if (root)
			{
				root->m_TriggerList.removeOne(trigger);
			}
			//触发器
			BTNTrigger *trigger = pvariant.value<BTNTrigger*>();
			if(trigger)
			{
				trigger->m_TriggerList.removeOne(trigger);
			}
		}
		delete trigger;
		trigger = NULL;
	}
	//任务
	ForceTask *task = variant.value<ForceTask*>();
	if(task)
	{
		QTreeWidgetItem *parentItem = item->parent();
		if (parentItem)
		{
			QVariant pvariant = parentItem->data(0,Qt::UserRole); // 获取数据
			//根节点
			BTNRoot *root = pvariant.value<BTNRoot*>();
			if (root)
			{
				root->m_TaskList.removeOne(task);
			}
			//触发器
			BTNTrigger *trigger = pvariant.value<BTNTrigger*>();
			if(trigger)
			{
				trigger->m_TaskList.removeOne(task);
			}
		}
		delete task;
		task = NULL;
	}
}

void BehaviorTreeEditWdg::on_pushButton_load_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("载入"),"./","*.txt");
	if (!fileName.isEmpty())
	{
		// TODO:
	}
}

void BehaviorTreeEditWdg::on_pushButton_save_clicked()
{
	QString fileName = QFileDialog::getSaveFileName(this,QString::fromLocal8Bit("保存"),"./","*.txt");
	if (!fileName.isEmpty())
	{
		// TODO:
	}
}

void BehaviorTreeEditWdg::on_pushButton_update_clicked()
{
	QTreeWidgetItem *item = ui.treeWidget->topLevelItem(0);
	if (item)
	{
		QVariant variant = item->data(0,Qt::UserRole); // 获取数据
		//根节点
		BTNRoot *root = variant.value<BTNRoot*>();
		if (root)
		{
			QString content = root->getContent();
			//XML的转义符
			//&lt; <
			//&gt; >
			//&amp; &
			//&apos; '
			//&quot; "
			//	nbsp; 空格
			content = content.replace(">","$gt;");
			content = content.replace("<","&lt;");
			content = content.replace("\n","");
			updateParam("PlanRef",m_Path);
			updateParam("Plan",content);
			myEntObj->setParameters(myEditParameters);
			emit signal_updateBehaviorTreeInfo(m_Path,content);
		}
	}
	close();
}

void BehaviorTreeEditWdg::on_pushButton_cancel_clicked()
{
	close();
}

void BehaviorTreeEditWdg::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{
	m_AddRootNode->setVisible(false);
	m_ClearChildNode->setVisible(false);
	m_DeleteNode->setVisible(false);
	m_TreeNodeMenu->menuAction()->setVisible(false);
	m_TriggerNumberMenu->menuAction()->setVisible(false);
	m_TriggerInterruptPolicyForSuccessMenu->menuAction()->setVisible(false);
	m_TriggerInterruptPolicyForFaileMenu->menuAction()->setVisible(false);
	int count = ui.treeWidget->topLevelItemCount();
	if (count == 0)
	{
		m_AddRootNode->setVisible(true);
	}
	else
	{
		QTreeWidgetItem  *curItem =  ui.treeWidget->itemAt(pos);
		if(curItem == NULL) return;
		if(curItem->childCount()>0)
			m_ClearChildNode->setVisible(true);
		else
			m_ClearChildNode->setVisible(false);
		m_CurItem = curItem;
		QVariant variant = curItem->data(0,Qt::UserRole); // 获取数据
		//根节点
		BTNRoot *root = variant.value<BTNRoot*>();
		if (root)
		{
			m_ClearChildNode->setVisible(true);
			m_TreeNodeMenu->menuAction()->setVisible(true);
		}
		//复合节点
		BTNCompose *compose = variant.value<BTNCompose*>();
		if (compose)
		{
			m_DeleteNode->setVisible(true);
			m_TreeNodeMenu->menuAction()->setVisible(true);
		}
		//触发器
		BTNTrigger *trigger = variant.value<BTNTrigger*>();
		if(trigger)
		{
			m_DeleteNode->setVisible(true);
			m_TreeNodeMenu->menuAction()->setVisible(true);
			m_TriggerNumberMenu->menuAction()->setVisible(true);
			m_TriggerInterruptPolicyForSuccessMenu->menuAction()->setVisible(true);
			m_TriggerInterruptPolicyForFaileMenu->menuAction()->setVisible(true);
			int triggerNu = trigger->m_Count.toInt();
			if(triggerNu == -1)
			{
				m_TriggerNumberUnlimited->setChecked(true);
				m_TriggerNumberSet->setText(QString::fromLocal8Bit("设置"));
			}
			else
			{
				m_TriggerNumberSet->setChecked(true);
				m_TriggerNumberSet->setText(QString::fromLocal8Bit("次数:%1").arg(trigger->m_Count));
			}
			if (trigger->m_InterruptPolicyForSuccess == "UnInterrupt")
				m_TriggerSuccessUnInterrupt->setChecked(true);
			else if(trigger->m_InterruptPolicyForSuccess == "Finish")
				m_TriggerSuccessFinish->setChecked(true);
			else if(trigger->m_InterruptPolicyForSuccess == "Current")
				m_TriggerSuccessCurrent->setChecked(true);
			else if(trigger->m_InterruptPolicyForSuccess == "Next")
				m_TriggerSuccessNext->setChecked(true);
			else if(trigger->m_InterruptPolicyForSuccess == "Reset")
				m_TriggerSuccessReset->setChecked(true);
			if (trigger->m_InterruptPolicyForFaile == "UnInterrupt")
				m_TriggerFaileUnInterrupt->setChecked(true);
			else if(trigger->m_InterruptPolicyForFaile == "Finish")
				m_TriggerFaileFinish->setChecked(true);
			else if(trigger->m_InterruptPolicyForFaile == "Current")
				m_TriggerFaileCurrent->setChecked(true);
			else if(trigger->m_InterruptPolicyForFaile == "Next")
				m_TriggerFaileNext->setChecked(true);
			else if(trigger->m_InterruptPolicyForFaile == "Reset")
				m_TriggerFaileReset->setChecked(true);
		}
		//时间节点
		BTNTime *time = variant.value<BTNTime*>();
		if(time)
		{
			m_DeleteNode->setVisible(true);
			m_TreeNodeMenu->menuAction()->setVisible(true);
		}
		//任务
		ForceTask *task = variant.value<ForceTask*>();
		if(task)
		{
			m_DeleteNode->setVisible(true);
		}
	}
	m_TreeRootMenu->exec(QCursor::pos());

}

void BehaviorTreeEditWdg::on_treeWidget_itemClicked(QTreeWidgetItem * item, int column)
{
	ui.textEdit->clear();
	ui.tableWidget->clearContents();
	if (item)
	{
		QVariant variant = item->data(0,Qt::UserRole); // 获取数据
		//根节点
		BTNRoot *root = variant.value<BTNRoot*>();
		if (root)
		{
			updateTableWidget(root);
			QString content = root->getContent();
			ui.textEdit->setText(content);
		}
		//触发器
		BTNTrigger *trigger = variant.value<BTNTrigger*>();
		if(trigger)
		{
			updateTableWidget(trigger);
			QString content = trigger->getContent();
			ui.textEdit->setText(content);
		}
		//复合节点
		BTNCompose *compose = variant.value<BTNCompose*>();
		if(compose)
		{
			updateTableWidget(compose);
			QString content = compose->getContent();
			ui.textEdit->setText(content);
		}
		//时间节点
		BTNTime *time = variant.value<BTNTime*>();
		if(time)
		{
			updateTableWidget(time);
			QString content = time->getContent();
			ui.textEdit->setText(content);
		}
		//任务
		ForceTask *task = variant.value<ForceTask*>();
		if(task)
		{
			updateTableWidget(task);
			QString content = task->getContent();
			ui.textEdit->setText(content);
		}
	}
}

void BehaviorTreeEditWdg::on_treeWidget_itemDoubleClicked(QTreeWidgetItem * item, int column)
{
	
}

void BehaviorTreeEditWdg::on_tableWidget_itemChanged(QTableWidgetItem * item)
{
	QTreeWidgetItem * treeItem = ui.treeWidget->currentItem();
	if (treeItem)
	{
		QVariant variant = treeItem->data(0,Qt::UserRole); // 获取数据
		//根节点
		BTNRoot *root = variant.value<BTNRoot*>();
		if(root)
		{
			QString note = ui.tableWidget->item(item->row(),0)->text();
			QString value = item->text();
			root->setValue(note,value);
			QString content = root->getContent();
			ui.textEdit->setText(content);
		}
		//触发器
		BTNTrigger *trigger = variant.value<BTNTrigger*>();
		if(trigger)
		{
			QString note = ui.tableWidget->item(item->row(),0)->text();
			QString value = item->text();
			trigger->setValue(note,value);
			QString content = trigger->getContent();
			ui.textEdit->setText(content);
		}
		//复合节点
		BTNCompose *compose = variant.value<BTNCompose*>();
		if(compose)
		{
			QString note = ui.tableWidget->item(item->row(),0)->text();
			QString value = item->text();
			compose->setValue(note,value);
			QString content = compose->getContent();
			ui.textEdit->setText(content);
		}
		//时间节点
		BTNTime *time = variant.value<BTNTime*>();
		if(time)
		{
			QString note = ui.tableWidget->item(item->row(),0)->text();
			QString value = item->text();
			time->setValue(note,value);
			QString content = time->getContent();
			ui.textEdit->setText(content);
		}
		//任务
		ForceTask *task = variant.value<ForceTask*>();
		if(task)
		{
			QString note = ui.tableWidget->item(item->row(),0)->text();
			QString value = item->text();

			//RefAttr_Task attr = task->getList().at(i);
			AttrEditType eType;
			QString idf = ui.tableWidget->item(item->row(),0)->data(Qt::UserRole).toString();
			if(myATMng.getTaskRefAttricbeEditType(idf, eType))
			{
				if(!eType.sourceInfo.isEmpty())
				{
					if (!eType.sourceInfo.first().isEmpty())
					{
						value = value;
					}
					else
					{
						value = eType.values.value(value).toString();
					}
				}
			}

			task->setValue(note,value);
			QString content = task->getContent();
			ui.textEdit->setText(content);
		}
	}
}

void BehaviorTreeEditWdg::on_m_AddRootNode_triggered()
{
	QTreeWidgetItem *Item = new QTreeWidgetItem(QStringList() << QString::fromLocal8Bit("任务包")); //根节点
	Item->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNCompose.png"));
	BTNRoot *root = new BTNRoot;
	root->setObjectName(BTNRootManager::instance()->getBTNRoot(Item->text(0))->getObjectName());
	root->setNote(BTNRootManager::instance()->getBTNRoot(Item->text(0))->getNote());
	root->setList(BTNRootManager::instance()->getBTNRoot(Item->text(0))->getList());
	Item->setData(0, Qt::UserRole, QVariant::fromValue(root));
	ui.treeWidget->addTopLevelItem(Item);
}

void BehaviorTreeEditWdg::on_m_ClearChildNode_triggered()
{
	if (m_CurItem)
	{
		m_CurItem->takeChildren();
		//父节点
		QVariant variant = m_CurItem->data(0,Qt::UserRole); // 获取数据
		//根节点
		BTNRoot *root = variant.value<BTNRoot*>();
		if (root)
		{
			root->m_TaskList.clear();
			root->m_TriggerList.clear();
			root->m_ComposeList.clear();
			root->m_TimeList.clear();
		}
		//触发器
		BTNTrigger *trigger = variant.value<BTNTrigger*>();
		if (trigger)
		{
			trigger->m_TaskList.clear();
			trigger->m_TriggerList.clear();
			trigger->m_ComposeList.clear();
			trigger->m_TimeList.clear();
		}
		//复合节点
		BTNCompose *compose = variant.value<BTNCompose*>();
		if (compose)
		{
			compose->m_TaskList.clear();
			compose->m_TriggerList.clear();
			compose->m_ComposeList.clear();
			compose->m_TimeList.clear();
		}
		//时间节点
		BTNTime *time = variant.value<BTNTime*>();
		if (time)
		{
			time->m_TaskList.clear();
			time->m_TriggerList.clear();
			time->m_ComposeList.clear();
			time->m_TimeList.clear();
		}
	}
}

void BehaviorTreeEditWdg::on_m_TaskNode_triggered()
{
	if (m_CurItem)
	{
		BehaviorTreeTaskDlg bttdlg;
		if (bttdlg.exec() == QDialog::Accepted)
		{
			QString taskName = bttdlg.getSelectedTask();
			if(taskName.isEmpty()) return;
			QTreeWidgetItem *Item = new QTreeWidgetItem(QStringList() << taskName);
			Item->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNTask.png"));
			ForceTask *task = new ForceTask;
			task->setObjectName(ForceTaskManager::instance()->getForceTask(taskName)->getObjectName());
			task->setNote(ForceTaskManager::instance()->getForceTask(taskName)->getNote());
			task->setList(ForceTaskManager::instance()->getForceTask(taskName)->getList());
			Item->setData(0, Qt::UserRole, QVariant::fromValue(task));
			m_CurItem->addChild(Item);
			//父节点
			QVariant variant = m_CurItem->data(0,Qt::UserRole); // 获取数据
			//根节点
			BTNRoot *root = variant.value<BTNRoot*>();
			if (root)
			{
				root->m_TaskList.append(task);
			}
			//触发器
			BTNTrigger *trigger = variant.value<BTNTrigger*>();
			if (trigger)
			{
				trigger->m_TaskList.append(task);
			}
			//复合节点
			BTNCompose *compose = variant.value<BTNCompose*>();
			if (compose)
			{
				compose->m_TaskList.append(task);
			}
			//时间节点
			BTNTime *time = variant.value<BTNTime*>();
			if (time)
			{
				time->m_TaskList.append(task);
			}
			ui.treeWidget->expandAll();
		}
	}
}

void BehaviorTreeEditWdg::on_m_ComposeNode_triggered()
{  
	if (m_CurItem)
	{
		QTreeWidgetItem *Item = new QTreeWidgetItem(QStringList() << QString::fromLocal8Bit("任务包")); //复合节点
		Item->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNCompose.png"));
		BTNCompose *compose = new BTNCompose;
		compose->setObjectName(BTNComposeManager::instance()->getBTNCompose(Item->text(0))->getObjectName());
		compose->setNote(BTNComposeManager::instance()->getBTNCompose(Item->text(0))->getNote());
		compose->setList(BTNComposeManager::instance()->getBTNCompose(Item->text(0))->getList());
		Item->setData(0, Qt::UserRole, QVariant::fromValue(compose));
		m_CurItem->addChild(Item);
		//父节点
		QVariant variant = m_CurItem->data(0,Qt::UserRole); // 获取数据
		//根节点
		BTNRoot *root = variant.value<BTNRoot*>();
		if (root)
		{
			root->m_ComposeList.append(compose);
		}
		//触发器
		BTNTrigger *trigger = variant.value<BTNTrigger*>();
		if (trigger)
		{
			trigger->m_ComposeList.append(compose);
		}
		//复合节点
		BTNCompose *comp = variant.value<BTNCompose*>();
		if (comp)
		{
			comp->m_ComposeList.append(compose);
		}
		//时间节点
		BTNTime *time = variant.value<BTNTime*>();
		if (time)
		{
			time->m_ComposeList.append(compose);
		}
		ui.treeWidget->expandAll();
	}
}

void BehaviorTreeEditWdg::on_m_LoopNode_triggered()
{
	if (m_CurItem)
	{
		QTreeWidgetItem *Item = new QTreeWidgetItem(QStringList() << QString::fromLocal8Bit("循环节点"));
		Item->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNLoop.png"));
		Item->setData(0, Qt::UserRole, LOOP);
		m_CurItem->addChild(Item);
		ui.treeWidget->expandAll();
	}
}

void BehaviorTreeEditWdg::on_m_TimeNode_triggered()
{
	if (m_CurItem)
	{
		QTreeWidgetItem *Item = new QTreeWidgetItem(QStringList() << QString::fromLocal8Bit("时间节点"));
		Item->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNTime.png"));
		BTNTime *time = new BTNTime;
		time->setObjectName(BTNTimeManager::instance()->getBTNTime(Item->text(0))->getObjectName());
		time->setNote(BTNTimeManager::instance()->getBTNTime(Item->text(0))->getNote());
		time->setList(BTNTimeManager::instance()->getBTNTime(Item->text(0))->getList());
		Item->setData(0, Qt::UserRole, QVariant::fromValue(time));
		m_CurItem->addChild(Item);
		//父节点
		QVariant variant = m_CurItem->data(0,Qt::UserRole); // 获取数据
		//根节点
		BTNRoot *root = variant.value<BTNRoot*>();
		if (root)
		{
			root->m_TimeList.append(time);
		}
		//触发器
		BTNTrigger *trigger = variant.value<BTNTrigger*>();
		if (trigger)
		{
			trigger->m_TimeList.append(time);
		}
		//复合节点
		BTNCompose *comp = variant.value<BTNCompose*>();
		if (comp)
		{
			comp->m_TimeList.append(time);
		}
		//时间节点
		BTNTime *ptime = variant.value<BTNTime*>();
		if (ptime)
		{
			ptime->m_TimeList.append(time);
		}
		ui.treeWidget->expandAll();
	}
}

void BehaviorTreeEditWdg::on_m_ConditionNode_triggered()
{
	if (m_CurItem)
	{
		QTreeWidgetItem *Item = new QTreeWidgetItem(QStringList() << QString::fromLocal8Bit("条件节点"));
		Item->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNCondition.png"));
		Item->setData(0, Qt::UserRole, CONDITION);
		m_CurItem->addChild(Item);
		ui.treeWidget->expandAll();
	}
}

void BehaviorTreeEditWdg::on_m_BreakNode_triggered()
{
	if (m_CurItem)
	{
		QTreeWidgetItem *Item = new QTreeWidgetItem(QStringList() << QString::fromLocal8Bit("跳出"));
		Item->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNBreak.png"));
		Item->setData(0, Qt::UserRole, BREAK);
		m_CurItem->addChild(Item);
		ui.treeWidget->expandAll();
	}
}

void BehaviorTreeEditWdg::on_m_EndNode_triggered()
{
	if (m_CurItem)
	{
		QTreeWidgetItem *Item = new QTreeWidgetItem(QStringList() << QString::fromLocal8Bit("结束"));
		Item->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNEnd.png"));
		Item->setData(0, Qt::UserRole, END);
		m_CurItem->addChild(Item);
		ui.treeWidget->expandAll();
	}
}

void BehaviorTreeEditWdg::on_m_TriggerNode_triggered()
{
	if (m_CurItem)
	{
		BehaviorTreeTriggerDlg bttdlg;
		if (bttdlg.exec() == QDialog::Accepted)
		{
			QString triggerName = bttdlg.getSelectedTrigger();
			if(triggerName.isEmpty()) return;
			QTreeWidgetItem *Item = new QTreeWidgetItem(QStringList() << triggerName);
			Item->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNTrigger.png"));
			BTNTrigger *trigger = new BTNTrigger;
			trigger->setObjectName(BTNTriggerManager::instance()->getBBTNTrigger(triggerName)->getObjectName());
			trigger->setNote(BTNTriggerManager::instance()->getBBTNTrigger(triggerName)->getNote());
			trigger->setList(BTNTriggerManager::instance()->getBBTNTrigger(triggerName)->getList());
			Item->setData(0, Qt::UserRole, QVariant::fromValue(trigger));
			m_CurItem->addChild(Item);
			//父节点
			QVariant variant = m_CurItem->data(0,Qt::UserRole); // 获取数据
			//根节点
			BTNRoot *root = variant.value<BTNRoot*>();
			if (root)
			{
				root->m_TriggerList.append(trigger);
			}
			//触发器
			BTNTrigger *ptrigger = variant.value<BTNTrigger*>();
			if (ptrigger)
			{
				ptrigger->m_TriggerList.append(trigger);
			}
			//复合节点
			BTNCompose *comp = variant.value<BTNCompose*>();
			if (comp)
			{
				comp->m_TriggerList.append(trigger);
			}
			//时间节点
			BTNTime *time = variant.value<BTNTime*>();
			if (time)
			{
				time->m_TriggerList.append(trigger);
			}
			ui.treeWidget->expandAll();
		}
	}
}

void BehaviorTreeEditWdg::on_m_DeleteNode_triggered()
{
	if (m_CurItem)
	{
		QTreeWidgetItem* item = ui.treeWidget->currentItem();
		if(item->childCount() > 0)
		{
			for(int i = 0; i < item->childCount(); i ++)
			{
				deleteTreeItem(item->child(i));
			}
		}
		deleteBTNode(item);
		delete item;
	}
	m_CurItem = NULL;
}

void BehaviorTreeEditWdg::on_m_TriggerNumberUnlimited_triggered()
{
	//父节点
	QVariant variant = m_CurItem->data(0,Qt::UserRole); // 获取数据
	//触发器
	BTNTrigger *trigger = variant.value<BTNTrigger*>();
	if (trigger)
	{
		trigger->m_Count = "-1";
	}
}

void BehaviorTreeEditWdg::on_m_TriggerNumberSet_triggered()
{
	//父节点
	QVariant variant = m_CurItem->data(0,Qt::UserRole); // 获取数据
	//触发器
	BTNTrigger *trigger = variant.value<BTNTrigger*>();
	if (trigger)
	{
		BehaviorTreeTriggerNuDlg dlg;
		if (dlg.exec() == QDialog::Accepted)
		{
			trigger->m_Count = QString::number(dlg.getTriggerNumber());
		}
	}
}

void BehaviorTreeEditWdg::on_m_TriggerSuccessUnInterrupt_triggered()
{
	//父节点
	QVariant variant = m_CurItem->data(0,Qt::UserRole); // 获取数据
	//触发器
	BTNTrigger *trigger = variant.value<BTNTrigger*>();
	if (trigger)
	{
		trigger->m_InterruptPolicyForSuccess = "UnInterrupt";
	}
}

void BehaviorTreeEditWdg::on_m_TriggerSuccessFinish_triggered()
{
	//父节点
	QVariant variant = m_CurItem->data(0,Qt::UserRole); // 获取数据
	//触发器
	BTNTrigger *trigger = variant.value<BTNTrigger*>();
	if (trigger)
	{
		trigger->m_InterruptPolicyForSuccess = "Finish";
	}
}

void BehaviorTreeEditWdg::on_m_TriggerSuccessCurrent_triggered()
{
	//父节点
	QVariant variant = m_CurItem->data(0,Qt::UserRole); // 获取数据
	//触发器
	BTNTrigger *trigger = variant.value<BTNTrigger*>();
	if (trigger)
	{
		trigger->m_InterruptPolicyForSuccess = "Current";
	}
}

void BehaviorTreeEditWdg::on_m_TriggerSuccessNext_triggered()
{
	//父节点
	QVariant variant = m_CurItem->data(0,Qt::UserRole); // 获取数据
	//触发器
	BTNTrigger *trigger = variant.value<BTNTrigger*>();
	if (trigger)
	{
		trigger->m_InterruptPolicyForSuccess = "Next";
	}
}

void BehaviorTreeEditWdg::on_m_TriggerSuccessReset_triggered()
{
	//父节点
	QVariant variant = m_CurItem->data(0,Qt::UserRole); // 获取数据
	//触发器
	BTNTrigger *trigger = variant.value<BTNTrigger*>();
	if (trigger)
	{
		trigger->m_InterruptPolicyForSuccess = "Reset";
	}
}

void BehaviorTreeEditWdg::on_m_TriggerFaileUnInterrupt_triggered()
{
	//父节点
	QVariant variant = m_CurItem->data(0,Qt::UserRole); // 获取数据
	//触发器
	BTNTrigger *trigger = variant.value<BTNTrigger*>();
	if (trigger)
	{
		trigger->m_InterruptPolicyForFaile = "UnInterrupt";
	}
}

void BehaviorTreeEditWdg::on_m_TriggerFaileFinish_triggered()
{
	//父节点
	QVariant variant = m_CurItem->data(0,Qt::UserRole); // 获取数据
	//触发器
	BTNTrigger *trigger = variant.value<BTNTrigger*>();
	if (trigger)
	{
		trigger->m_InterruptPolicyForFaile = "Finish";
	}
}

void BehaviorTreeEditWdg::on_m_TriggerFaileCurrent_triggered()
{
	//父节点
	QVariant variant = m_CurItem->data(0,Qt::UserRole); // 获取数据
	//触发器
	BTNTrigger *trigger = variant.value<BTNTrigger*>();
	if (trigger)
	{
		trigger->m_InterruptPolicyForFaile = "Current";
	}
}

void BehaviorTreeEditWdg::on_m_TriggerFaileNext_triggered()
{
	//父节点
	QVariant variant = m_CurItem->data(0,Qt::UserRole); // 获取数据
	//触发器
	BTNTrigger *trigger = variant.value<BTNTrigger*>();
	if (trigger)
	{
		trigger->m_InterruptPolicyForFaile = "Next";
	}
}

void BehaviorTreeEditWdg::on_m_TriggerFaileReset_triggered()
{
	//父节点
	QVariant variant = m_CurItem->data(0,Qt::UserRole); // 获取数据
	//触发器
	BTNTrigger *trigger = variant.value<BTNTrigger*>();
	if (trigger)
	{
		trigger->m_InterruptPolicyForFaile = "Reset";
	}
}

void  BehaviorTreeEditWdg::parse()
{
	m_count=0;
	while(true)
	{
		getToken();
		if(matchSingleToken(TOKEN_COMMENT))
			continue;
		else if(matchSingleToken(TOKEN_END))
			return;
		else if(matchSingleToken(TOKEN_IDENTIFY))
		{
			if(m_tokenstring=="BTNCompose")
			{  
				if(m_count==0)
				{
					qDebug()<<"jianliROOT"<<m_count;
					QMap<int,QString> m_Nope;
					if(!DoParseParas("BTNCompose",m_Nope))
						return;
					m_curItemRoot = addThreeRoot(QString::fromLocal8Bit("任务包"));	//根节点
					m_curItemRoot->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNCompose.png"));
					BTNRoot *root = new BTNRoot;
					root->setObjectName(BTNRootManager::instance()->getBTNRoot(m_curItemRoot->text(0))->getObjectName());
					root->setNote(BTNRootManager::instance()->getBTNRoot(m_curItemRoot->text(0))->getNote());
					root->setList(BTNRootManager::instance()->getBTNRoot(m_curItemRoot->text(0))->getList());
					QList<RefAttr_Root> m_list_attrs=BTNRootManager::instance()->getBTNRoot(m_curItemRoot->text(0))->getList();
					for(int i=0;i<m_list_attrs.count();i++)
					{
						QMap<int,QString> ::Iterator itor;		
						itor=m_Nope.find(i);
						QString m_value;
						for(int k=0;k<m_list_attrs[i].nameList.count();k++)
						{
							if(itor.value()==m_list_attrs[i].nameList[k])
							{
								m_value=m_list_attrs[i].valueList[k];
							}
						}
						if(itor!=m_Nope.end()){
							root->setValue(m_list_attrs[i].note,m_value);
						}

					}
					m_curItemRoot->setData(0, Qt::UserRole, QVariant::fromValue(root));
					m_CurItemNode = m_curItemRoot;
				}else if(m_count==1)
				{
					countSign++;
					qDebug()<<"ONRoot111"<<m_count;
					QMap<int,QString> m_Nope;
					if(!DoParseParas("BTNCompose",m_Nope))
						return;
					QTreeWidgetItem *item = addThreeNode(m_curItemRoot,QString::fromLocal8Bit("任务包"));  //复合节点
					item->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNCompose.png"));
					BTNCompose *compose = new BTNCompose;
					compose->setObjectName(BTNComposeManager::instance()->getBTNCompose(item->text(0))->getObjectName());
					compose->setNote(BTNComposeManager::instance()->getBTNCompose(item->text(0))->getNote());
					compose->setList(BTNComposeManager::instance()->getBTNCompose(item->text(0))->getList());
					QList<RefAttr_Compose> m_list_attrs=BTNComposeManager::instance()->getBTNCompose(item->text(0))->getList();
					for(int i=0;i<m_list_attrs.count();i++)
					{
						QMap<int,QString> ::Iterator itor;		
						itor=m_Nope.find(i);
						QString m_value;
						for(int k=0;k<m_list_attrs[i].nameList.count();k++)
						{
							if(itor.value()==m_list_attrs[i].nameList[k])
							{
								m_value=m_list_attrs[i].valueList[k];
							}
						}
						if(itor!=m_Nope.end()){
							compose->setValue(m_list_attrs[i].note,m_value);
						}

					}
					item->setData(0, Qt::UserRole, QVariant::fromValue(compose));
					m_CurItemNode=item;
					//父节点
					QVariant variant = m_curItemRoot->data(0,Qt::UserRole); // 获取数据
					//根节点
					BTNRoot *root = variant.value<BTNRoot*>();
					if (root)
					{
						root->m_ComposeList.append(compose);
					}
					//触发器
					BTNTrigger *trigger = variant.value<BTNTrigger*>();
					if (trigger)
					{
						trigger->m_ComposeList.append(compose);
					}
					//复合节点
					BTNCompose *comp = variant.value<BTNCompose*>();
					if (comp)
					{
						comp->m_ComposeList.append(compose);
					}
					//时间节点
					BTNTime *time = variant.value<BTNTime*>();
					if (time)
					{
						time->m_ComposeList.append(compose);
					}
					ui.treeWidget->expandAll();
				}else if(m_count==2)
				{
					countSign++;
					qDebug()<<"ONRoot222"<<m_count;
					QMap<int,QString> m_Nope;
					if(!DoParseParas("BTNCompose",m_Nope))
						return;
					QTreeWidgetItem *item = addThreeNode(m_CurItemNode,QString::fromLocal8Bit("任务包")); //复合节点
					item->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNCompose.png"));
					BTNCompose *compose = new BTNCompose;
					compose->setObjectName(BTNComposeManager::instance()->getBTNCompose(item->text(0))->getObjectName());
					compose->setNote(BTNComposeManager::instance()->getBTNCompose(item->text(0))->getNote());
					compose->setList(BTNComposeManager::instance()->getBTNCompose(item->text(0))->getList());
					QList<RefAttr_Compose> m_list_attrs=BTNComposeManager::instance()->getBTNCompose(item->text(0))->getList();
					for(int i=0;i<m_list_attrs.count();i++)
					{
						QMap<int,QString> ::Iterator itor;		
						itor=m_Nope.find(i);
						QString m_value;
						for(int k=0;k<m_list_attrs[i].nameList.count();k++)
						{
							if(itor.value()==m_list_attrs[i].nameList[k])
							{
								m_value=m_list_attrs[i].valueList[k];
							}
						}
						if(itor!=m_Nope.end()){
							compose->setValue(m_list_attrs[i].note,m_value);
						}

					}
					item->setData(0, Qt::UserRole, QVariant::fromValue(compose));
					m_CurItemNode1=item;
					//父节点
					QVariant variant = m_CurItemNode->data(0,Qt::UserRole); // 获取数据
					//根节点
					BTNRoot *root = variant.value<BTNRoot*>();
					if (root)
					{
						root->m_ComposeList.append(compose);
					}
					//触发器
					BTNTrigger *trigger = variant.value<BTNTrigger*>();
					if (trigger)
					{
						trigger->m_ComposeList.append(compose);
					}
					//复合节点
					BTNCompose *comp = variant.value<BTNCompose*>();
					if (comp)
					{
						comp->m_ComposeList.append(compose);
					}
					//时间节点
					BTNTime *time = variant.value<BTNTime*>();
					if (time)
					{
						time->m_ComposeList.append(compose);
					}
					ui.treeWidget->expandAll();
				}else if(m_count==3)
				{
					countSign++;
					qDebug()<<"ONRoot222"<<m_count;
					QMap<int,QString> m_Nope;
					if(!DoParseParas("BTNCompose",m_Nope))
						return;
					QTreeWidgetItem *item = addThreeNode(m_CurItemNode1,QString::fromLocal8Bit("任务包")); //复合节点
					item->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNCompose.png"));
					BTNCompose *compose = new BTNCompose;
					compose->setObjectName(BTNComposeManager::instance()->getBTNCompose(item->text(0))->getObjectName());
					compose->setNote(BTNComposeManager::instance()->getBTNCompose(item->text(0))->getNote());
					compose->setList(BTNComposeManager::instance()->getBTNCompose(item->text(0))->getList());
					QList<RefAttr_Compose> m_list_attrs=BTNComposeManager::instance()->getBTNCompose(item->text(0))->getList();
					for(int i=0;i<m_list_attrs.count();i++)
					{
						QMap<int,QString> ::Iterator itor;		
						itor=m_Nope.find(i);
						QString m_value;
						for(int k=0;k<m_list_attrs[i].nameList.count();k++)
						{
							if(itor.value()==m_list_attrs[i].nameList[k])
							{
								m_value=m_list_attrs[i].valueList[k];
							}
						}
						if(itor!=m_Nope.end()){
							compose->setValue(m_list_attrs[i].note,m_value);
						}					
					}
					item->setData(0, Qt::UserRole, QVariant::fromValue(compose));
					m_CurItemNode2=item;
					//父节点
					QVariant variant = m_CurItemNode1->data(0,Qt::UserRole); // 获取数据
					//根节点
					BTNRoot *root = variant.value<BTNRoot*>();
					if (root)
					{
						root->m_ComposeList.append(compose);
					}
					//触发器
					BTNTrigger *trigger = variant.value<BTNTrigger*>();
					if (trigger)
					{
						trigger->m_ComposeList.append(compose);
					}
					//复合节点
					BTNCompose *comp = variant.value<BTNCompose*>();
					if (comp)
					{
						comp->m_ComposeList.append(compose);
					}
					//时间节点
					BTNTime *time = variant.value<BTNTime*>();
					if (time)
					{
						time->m_ComposeList.append(compose);
					}
					ui.treeWidget->expandAll();
				}else if(m_count==4)
				{
					countSign++;
					QMap<int,QString> m_Nope;
					if(!DoParseParas("BTNCompose",m_Nope))
						return;
					QTreeWidgetItem *item = addThreeNode(m_CurItemNode2,QString::fromLocal8Bit("任务包")); //复合节点
					item->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNCompose.png"));
					BTNCompose *compose = new BTNCompose;
					compose->setObjectName(BTNComposeManager::instance()->getBTNCompose(item->text(0))->getObjectName());
					compose->setNote(BTNComposeManager::instance()->getBTNCompose(item->text(0))->getNote());
					compose->setList(BTNComposeManager::instance()->getBTNCompose(item->text(0))->getList());
					QList<RefAttr_Compose> m_list_attrs=BTNComposeManager::instance()->getBTNCompose(item->text(0))->getList();
					for(int i=0;i<m_list_attrs.count();i++)
					{
						QMap<int,QString> ::Iterator itor;		
						itor=m_Nope.find(i);
						QString m_value;
						for(int k=0;k<m_list_attrs[i].nameList.count();k++)
						{
							if(itor.value()==m_list_attrs[i].nameList[k])
							{
								m_value=m_list_attrs[i].valueList[k];
							}
						}
						if(itor!=m_Nope.end()){
							compose->setValue(m_list_attrs[i].note,m_value);
						}					
					}
					item->setData(0, Qt::UserRole, QVariant::fromValue(compose));
					m_CurItemNode3=item;
					//父节点
					QVariant variant = m_CurItemNode2->data(0,Qt::UserRole); // 获取数据
					//根节点
					BTNRoot *root = variant.value<BTNRoot*>();
					if (root)
					{
						root->m_ComposeList.append(compose);
					}
					//触发器
					BTNTrigger *trigger = variant.value<BTNTrigger*>();
					if (trigger)
					{
						trigger->m_ComposeList.append(compose);
					}
					//复合节点
					BTNCompose *comp = variant.value<BTNCompose*>();
					if (comp)
					{
						comp->m_ComposeList.append(compose);
					}
					//时间节点
					BTNTime *time = variant.value<BTNTime*>();
					if (time)
					{
						time->m_ComposeList.append(compose);
					}
					ui.treeWidget->expandAll();
				}else if(m_count==5)
				{
					countSign++;
					QMap<int,QString> m_Nope;
					if(!DoParseParas("BTNCompose",m_Nope))
						return;
					QTreeWidgetItem *item = addThreeNode(m_CurItemNode3,QString::fromLocal8Bit("任务包")); //复合节点
					item->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNCompose.png"));
					BTNCompose *compose = new BTNCompose;
					compose->setObjectName(BTNComposeManager::instance()->getBTNCompose(item->text(0))->getObjectName());
					compose->setNote(BTNComposeManager::instance()->getBTNCompose(item->text(0))->getNote());
					compose->setList(BTNComposeManager::instance()->getBTNCompose(item->text(0))->getList());
					QList<RefAttr_Compose> m_list_attrs=BTNComposeManager::instance()->getBTNCompose(item->text(0))->getList();
					for(int i=0;i<m_list_attrs.count();i++)
					{
						QMap<int,QString> ::Iterator itor;		
						itor=m_Nope.find(i);
						QString m_value;
						for(int k=0;k<m_list_attrs[i].nameList.count();k++)
						{
							if(itor.value()==m_list_attrs[i].nameList[k])
							{
								m_value=m_list_attrs[i].valueList[k];
							}
						}
						if(itor!=m_Nope.end()){
							compose->setValue(m_list_attrs[i].note,m_value);
						}					
					}
					item->setData(0, Qt::UserRole, QVariant::fromValue(compose));
					//父节点
					QVariant variant = m_CurItemNode3->data(0,Qt::UserRole); // 获取数据
					//根节点
					BTNRoot *root = variant.value<BTNRoot*>();
					if (root)
					{
						root->m_ComposeList.append(compose);
					}
					//触发器
					BTNTrigger *trigger = variant.value<BTNTrigger*>();
					if (trigger)
					{
						trigger->m_ComposeList.append(compose);
					}
					//复合节点
					BTNCompose *comp = variant.value<BTNCompose*>();
					if (comp)
					{
						comp->m_ComposeList.append(compose);
					}
					//时间节点
					BTNTime *time = variant.value<BTNTime*>();
					if (time)
					{
						time->m_ComposeList.append(compose);
					}
					ui.treeWidget->expandAll();
				}

			}else if(m_tokenstring=="BTNTask")
			{
				countSign++;
				if(m_count==1)
				{
					QMap<int,QString> m_Nope;
					if(!DoParseParas("BTNTask",m_Nope))
						return;
					QMap<int,QString> ::const_iterator mi;
					mi=m_Nope.find(0);
					QTreeWidgetItem *item = addThreeNode(m_curItemRoot,analysisTaskName(mi.value()));
					item->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNTask.png"));
					ForceTask *task = new ForceTask;
					task->setObjectName(ForceTaskManager::instance()->getForceTask(item->text(0))->getObjectName());
					task->setNote(ForceTaskManager::instance()->getForceTask(item->text(0))->getNote());
					task->setList(ForceTaskManager::instance()->getForceTask(item->text(0))->getList());
					item->setData(0, Qt::UserRole, QVariant::fromValue(task));
					QList<RefAttr_Task> m_list_attrs=ForceTaskManager::instance()->getForceTask(item->text(0))->getList();
					for(int i=0;i<m_list_attrs.count();i++)
					{
						QMap<int,QString> ::Iterator itor;		
						itor=m_Nope.find(i+1);	
						if(itor!=m_Nope.end()){
							task->setValue(m_list_attrs[i].note,itor.value());
						}

					}
					m_CurItemNode=item;
					//父节点
					QVariant variant = m_curItemRoot->data(0,Qt::UserRole); // 获取数据
					//根节点
					BTNRoot *root = variant.value<BTNRoot*>();
					if (root)
					{
						root->m_TaskList.append(task);
					}
					//触发器
					BTNTrigger *trigger = variant.value<BTNTrigger*>();
					if (trigger)
					{
						trigger->m_TaskList.append(task);
					}
					//复合节点
					BTNCompose *compose = variant.value<BTNCompose*>();
					if (compose)
					{
						compose->m_TaskList.append(task);
					}
					//时间节点
					BTNTime *time = variant.value<BTNTime*>();
					if (time)
					{
						time->m_TaskList.append(task);
					}
					ui.treeWidget->expandAll();

				}else if(m_count==2)
				{
					QMap<int,QString> m_Nope;
					if(!DoParseParas("BTNTask",m_Nope))
						return;
					QMap<int,QString> ::const_iterator mi;
					mi=m_Nope.find(0);
					QTreeWidgetItem *item = addThreeNode(m_CurItemNode,analysisTaskName(mi.value()));
					item->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNTask.png"));
					ForceTask *task = new ForceTask;
					task->setObjectName(ForceTaskManager::instance()->getForceTask(item->text(0))->getObjectName());
					task->setNote(ForceTaskManager::instance()->getForceTask(item->text(0))->getNote());
					task->setList(ForceTaskManager::instance()->getForceTask(item->text(0))->getList());
					QList<RefAttr_Task> m_list_attrs=ForceTaskManager::instance()->getForceTask(item->text(0))->getList();

					for(int i=0;i<m_list_attrs.count();i++)
					{
						QMap<int,QString> ::Iterator itor;		
						itor=m_Nope.find(i+1);	
						if(itor!=m_Nope.end()){
							task->setValue(m_list_attrs[i].note,itor.value());
						}

					}
					item->setData(0, Qt::UserRole, QVariant::fromValue(task));
					m_CurItemNode1=item;
					//父节点
					QVariant variant = m_CurItemNode->data(0,Qt::UserRole); // 获取数据
					//根节点
					BTNRoot *root = variant.value<BTNRoot*>();
					if (root)
					{
						root->m_TaskList.append(task);
					}
					//触发器
					BTNTrigger *trigger = variant.value<BTNTrigger*>();
					if (trigger)
					{
						trigger->m_TaskList.append(task);
					}
					//复合节点
					BTNCompose *compose = variant.value<BTNCompose*>();
					if (compose)
					{
						compose->m_TaskList.append(task);
					}
					//时间节点
					BTNTime *time = variant.value<BTNTime*>();
					if (time)
					{
						time->m_TaskList.append(task);
					}
					ui.treeWidget->expandAll();
				}else if(m_count==3)
				{
					QMap<int,QString> m_Nope;
					if(!DoParseParas("BTNTask",m_Nope))
						return;
					QMap<int,QString> ::const_iterator mi;
					mi=m_Nope.find(0);
					QTreeWidgetItem *item = addThreeNode(m_CurItemNode1,analysisTaskName(mi.value()));
					item->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNTask.png"));
					ForceTask *task = new ForceTask;
					task->setObjectName(ForceTaskManager::instance()->getForceTask(item->text(0))->getObjectName());
					task->setNote(ForceTaskManager::instance()->getForceTask(item->text(0))->getNote());
					task->setList(ForceTaskManager::instance()->getForceTask(item->text(0))->getList());
					QList<RefAttr_Task> m_list_attrs=ForceTaskManager::instance()->getForceTask(item->text(0))->getList();
					for(int i=0;i<m_list_attrs.count();i++)
					{
						QMap<int,QString> ::Iterator itor;		
						itor=m_Nope.find(i+1);	
						if(itor!=m_Nope.end()){
							task->setValue(m_list_attrs[i].note,itor.value());
						}

					}
					item->setData(0, Qt::UserRole, QVariant::fromValue(task));
					m_CurItemNode2=item;
					//父节点
					QVariant variant = m_CurItemNode1->data(0,Qt::UserRole); // 获取数据
					//根节点
					BTNRoot *root = variant.value<BTNRoot*>();
					if (root)
					{
						root->m_TaskList.append(task);
					}
					//触发器
					BTNTrigger *trigger = variant.value<BTNTrigger*>();
					if (trigger)
					{
						trigger->m_TaskList.append(task);
					}
					//复合节点
					BTNCompose *compose = variant.value<BTNCompose*>();
					if (compose)
					{
						compose->m_TaskList.append(task);
					}
					//时间节点
					BTNTime *time = variant.value<BTNTime*>();
					if (time)
					{
						time->m_TaskList.append(task);
					}
					ui.treeWidget->expandAll();
				}else if(m_count==4)
				{
					QMap<int,QString> m_Nope;
					if(!DoParseParas("BTNTask",m_Nope))
						return;
					QMap<int,QString> ::const_iterator mi;
					mi=m_Nope.find(0);
					QTreeWidgetItem *item = addThreeNode(m_CurItemNode2,analysisTaskName(mi.value()));
					item->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNTask.png"));
					ForceTask *task = new ForceTask;
					task->setObjectName(ForceTaskManager::instance()->getForceTask(item->text(0))->getObjectName());
					task->setNote(ForceTaskManager::instance()->getForceTask(item->text(0))->getNote());
					task->setList(ForceTaskManager::instance()->getForceTask(item->text(0))->getList());
					QList<RefAttr_Task> m_list_attrs=ForceTaskManager::instance()->getForceTask(item->text(0))->getList();
					for(int i=0;i<m_list_attrs.count();i++)
					{
						QMap<int,QString> ::Iterator itor;		
						itor=m_Nope.find(i+1);	
						if(itor!=m_Nope.end()){
							task->setValue(m_list_attrs[i].note,itor.value());
						}

					}
					item->setData(0, Qt::UserRole, QVariant::fromValue(task));
					m_CurItemNode3=item;
					//父节点
					QVariant variant = m_CurItemNode2->data(0,Qt::UserRole); // 获取数据
					//根节点
					BTNRoot *root = variant.value<BTNRoot*>();
					if (root)
					{
						root->m_TaskList.append(task);
					}
					//触发器
					BTNTrigger *trigger = variant.value<BTNTrigger*>();
					if (trigger)
					{
						trigger->m_TaskList.append(task);
					}
					//复合节点
					BTNCompose *compose = variant.value<BTNCompose*>();
					if (compose)
					{
						compose->m_TaskList.append(task);
					}
					//时间节点
					BTNTime *time = variant.value<BTNTime*>();
					if (time)
					{
						time->m_TaskList.append(task);
					}
					ui.treeWidget->expandAll();
				}else if(m_count==5)
				{
					QMap<int,QString> m_Nope;
					if(!DoParseParas("BTNTask",m_Nope))
						return;
					QMap<int,QString> ::const_iterator mi;
					mi=m_Nope.find(0);
					QTreeWidgetItem *item = addThreeNode(m_CurItemNode3,analysisTaskName(mi.value()));
					item->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNTask.png"));
					ForceTask *task = new ForceTask;
					task->setObjectName(ForceTaskManager::instance()->getForceTask(item->text(0))->getObjectName());
					task->setNote(ForceTaskManager::instance()->getForceTask(item->text(0))->getNote());
					task->setList(ForceTaskManager::instance()->getForceTask(item->text(0))->getList());
					QList<RefAttr_Task> m_list_attrs=ForceTaskManager::instance()->getForceTask(item->text(0))->getList();
					for(int i=0;i<m_list_attrs.count();i++)
					{
						QMap<int,QString> ::Iterator itor;		
						itor=m_Nope.find(i+1);	
						if(itor!=m_Nope.end()){
							task->setValue(m_list_attrs[i].note,itor.value());
						}

					}
					item->setData(0, Qt::UserRole, QVariant::fromValue(task));
					//父节点
					QVariant variant = m_CurItemNode3->data(0,Qt::UserRole); // 获取数据
					//根节点
					BTNRoot *root = variant.value<BTNRoot*>();
					if (root)
					{
						root->m_TaskList.append(task);
					}
					//触发器
					BTNTrigger *trigger = variant.value<BTNTrigger*>();
					if (trigger)
					{
						trigger->m_TaskList.append(task);
					}
					//复合节点
					BTNCompose *compose = variant.value<BTNCompose*>();
					if (compose)
					{
						compose->m_TaskList.append(task);
					}
					//时间节点
					BTNTime *time = variant.value<BTNTime*>();
					if (time)
					{
						time->m_TaskList.append(task);
					}
					ui.treeWidget->expandAll();
				}

			}else if(m_tokenstring=="BTNTrigger")
			{
				countSign++;
				if(m_count==1)
				{
					QMap<int,QString> m_Nope;
					if(!DoParseParas("BTNTrigger",m_Nope))
						return;
					QMap<int,QString> ::const_iterator mi;
					mi=m_Nope.find(0);
					QTreeWidgetItem *item = addThreeNode(m_curItemRoot,analysisTriggerName(mi.value()));
					item->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNTrigger.png"));
					BTNTrigger *trigger = new BTNTrigger;
					trigger->setObjectName(BTNTriggerManager::instance()->getBBTNTrigger(item->text(0))->getObjectName());
					trigger->setNote(BTNTriggerManager::instance()->getBBTNTrigger(item->text(0))->getNote());
					trigger->setList(BTNTriggerManager::instance()->getBBTNTrigger(item->text(0))->getList());

					QList<RefAttr_Trigger> m_list_attrs=BTNTriggerManager::instance()->getBBTNTrigger(item->text(0))->getList();
					for(int i=0;i<m_list_attrs.count();i++)
					{
						QMap<int,QString> ::Iterator itor;		
						itor=m_Nope.find(i+1);	
						if(itor!=m_Nope.end()){
							trigger->setValue(m_list_attrs[i].note,itor.value());
						}						
					}
					item->setData(0, Qt::UserRole, QVariant::fromValue(trigger));
					m_CurItemNode=item;
					//父节点
					QVariant variant = m_curItemRoot->data(0,Qt::UserRole); // 获取数据
					//根节点
					BTNRoot *root = variant.value<BTNRoot*>();
					if (root)
					{
						root->m_TriggerList.append(trigger);
					}
					//触发器
					BTNTrigger *ptrigger = variant.value<BTNTrigger*>();
					if (ptrigger)
					{
						ptrigger->m_TriggerList.append(trigger);
					}
					//复合节点
					BTNCompose *comp = variant.value<BTNCompose*>();
					if (comp)
					{
						comp->m_TriggerList.append(trigger);
					}
					//时间节点
					BTNTime *time = variant.value<BTNTime*>();
					if (time)
					{
						time->m_TriggerList.append(trigger);
					}
					ui.treeWidget->expandAll();
				}else if(m_count==2)
				{
					QMap<int,QString> m_Nope;
					if(!DoParseParas("BTNTrigger",m_Nope))
						return;
					QMap<int,QString> ::const_iterator mi;
					mi=m_Nope.find(0);
					QTreeWidgetItem *item = addThreeNode(m_CurItemNode,analysisTriggerName(mi.value()));
					item->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNTrigger.png"));
					BTNTrigger *trigger = new BTNTrigger;
					trigger->setObjectName(BTNTriggerManager::instance()->getBBTNTrigger(item->text(0))->getObjectName());
					trigger->setNote(BTNTriggerManager::instance()->getBBTNTrigger(item->text(0))->getNote());
					trigger->setList(BTNTriggerManager::instance()->getBBTNTrigger(item->text(0))->getList());
					QList<RefAttr_Trigger> m_list_attrs=BTNTriggerManager::instance()->getBBTNTrigger(item->text(0))->getList();
					for(int i=0;i<m_list_attrs.count();i++)
					{
						QMap<int,QString> ::Iterator itor;		
						itor=m_Nope.find(i+1);	
						if(itor!=m_Nope.end()){
							trigger->setValue(m_list_attrs[i].note,itor.value());
						}						
					}
					item->setData(0, Qt::UserRole, QVariant::fromValue(trigger));
					m_CurItemNode1=item;
					//父节点
					QVariant variant = m_CurItemNode->data(0,Qt::UserRole); // 获取数据
					//根节点
					BTNRoot *root = variant.value<BTNRoot*>();
					if (root)
					{
						root->m_TriggerList.append(trigger);
					}
					//触发器
					BTNTrigger *ptrigger = variant.value<BTNTrigger*>();
					if (ptrigger)
					{
						ptrigger->m_TriggerList.append(trigger);
					}
					//复合节点
					BTNCompose *comp = variant.value<BTNCompose*>();
					if (comp)
					{
						comp->m_TriggerList.append(trigger);
					}
					//时间节点
					BTNTime *time = variant.value<BTNTime*>();
					if (time)
					{
						time->m_TriggerList.append(trigger);
					}
					ui.treeWidget->expandAll();
				}else if(m_count==3)
				{
					QMap<int,QString> m_Nope;
					if(!DoParseParas("BTNTrigger",m_Nope))
						return;
					QMap<int,QString> ::const_iterator mi;
					mi=m_Nope.find(0);
					QTreeWidgetItem *item = addThreeNode(m_CurItemNode1,analysisTriggerName(mi.value()));
					item->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNTrigger.png"));
					BTNTrigger *trigger = new BTNTrigger;
					trigger->setObjectName(BTNTriggerManager::instance()->getBBTNTrigger(item->text(0))->getObjectName());
					trigger->setNote(BTNTriggerManager::instance()->getBBTNTrigger(item->text(0))->getNote());
					trigger->setList(BTNTriggerManager::instance()->getBBTNTrigger(item->text(0))->getList());
					QList<RefAttr_Trigger> m_list_attrs=BTNTriggerManager::instance()->getBBTNTrigger(item->text(0))->getList();
					for(int i=0;i<m_list_attrs.count();i++)
					{
						QMap<int,QString> ::Iterator itor;		
						itor=m_Nope.find(i+1);	
						if(itor!=m_Nope.end()){
							trigger->setValue(m_list_attrs[i].note,itor.value());
						}						
					}
					item->setData(0, Qt::UserRole, QVariant::fromValue(trigger));
					m_CurItemNode2=item;
					//父节点
					QVariant variant = m_CurItemNode1->data(0,Qt::UserRole); // 获取数据
					//根节点
					BTNRoot *root = variant.value<BTNRoot*>();
					if (root)
					{
						root->m_TriggerList.append(trigger);
					}
					//触发器
					BTNTrigger *ptrigger = variant.value<BTNTrigger*>();
					if (ptrigger)
					{
						ptrigger->m_TriggerList.append(trigger);
					}
					//复合节点
					BTNCompose *comp = variant.value<BTNCompose*>();
					if (comp)
					{
						comp->m_TriggerList.append(trigger);
					}
					//时间节点
					BTNTime *time = variant.value<BTNTime*>();
					if (time)
					{
						time->m_TriggerList.append(trigger);
					}
					ui.treeWidget->expandAll();
				}else if(m_count==4)
				{
					QMap<int,QString> m_Nope;
					if(!DoParseParas("BTNTrigger",m_Nope))
						return;
					QMap<int,QString> ::const_iterator mi;
					mi=m_Nope.find(0);
					QTreeWidgetItem *item = addThreeNode(m_CurItemNode2,analysisTriggerName(mi.value()));
					item->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNTrigger.png"));
					BTNTrigger *trigger = new BTNTrigger;
					trigger->setObjectName(BTNTriggerManager::instance()->getBBTNTrigger(item->text(0))->getObjectName());
					trigger->setNote(BTNTriggerManager::instance()->getBBTNTrigger(item->text(0))->getNote());
					trigger->setList(BTNTriggerManager::instance()->getBBTNTrigger(item->text(0))->getList());
					QList<RefAttr_Trigger> m_list_attrs=BTNTriggerManager::instance()->getBBTNTrigger(item->text(0))->getList();
					for(int i=0;i<m_list_attrs.count();i++)
					{
						QMap<int,QString> ::Iterator itor;		
						itor=m_Nope.find(i+1);	
						if(itor!=m_Nope.end()){
							trigger->setValue(m_list_attrs[i].note,itor.value());
						}						
					}
					item->setData(0, Qt::UserRole, QVariant::fromValue(trigger));
					m_CurItemNode3=item;
					//父节点
					QVariant variant = m_CurItemNode2->data(0,Qt::UserRole); // 获取数据
					//根节点
					BTNRoot *root = variant.value<BTNRoot*>();
					if (root)
					{
						root->m_TriggerList.append(trigger);
					}
					//触发器
					BTNTrigger *ptrigger = variant.value<BTNTrigger*>();
					if (ptrigger)
					{
						ptrigger->m_TriggerList.append(trigger);
					}
					//复合节点
					BTNCompose *comp = variant.value<BTNCompose*>();
					if (comp)
					{
						comp->m_TriggerList.append(trigger);
					}
					//时间节点
					BTNTime *time = variant.value<BTNTime*>();
					if (time)
					{
						time->m_TriggerList.append(trigger);
					}
					ui.treeWidget->expandAll();
				}else if(m_count==5)
				{
					QMap<int,QString> m_Nope;
					if(!DoParseParas("BTNTrigger",m_Nope))
						return;
					QMap<int,QString> ::const_iterator mi;
					mi=m_Nope.find(0);
					QTreeWidgetItem *item = addThreeNode(m_CurItemNode3,analysisTriggerName(mi.value()));
					item->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNTrigger.png"));
					BTNTrigger *trigger = new BTNTrigger;
					trigger->setObjectName(BTNTriggerManager::instance()->getBBTNTrigger(item->text(0))->getObjectName());
					trigger->setNote(BTNTriggerManager::instance()->getBBTNTrigger(item->text(0))->getNote());
					trigger->setList(BTNTriggerManager::instance()->getBBTNTrigger(item->text(0))->getList());
					QList<RefAttr_Trigger> m_list_attrs=BTNTriggerManager::instance()->getBBTNTrigger(item->text(0))->getList();
					for(int i=0;i<m_list_attrs.count();i++)
					{
						QMap<int,QString> ::Iterator itor;		
						itor=m_Nope.find(i+1);	
						if(itor!=m_Nope.end()){
							trigger->setValue(m_list_attrs[i].note,itor.value());
						}						
					}
					item->setData(0, Qt::UserRole, QVariant::fromValue(trigger));
					//父节点
					QVariant variant = m_CurItemNode3->data(0,Qt::UserRole); // 获取数据
					//根节点
					BTNRoot *root = variant.value<BTNRoot*>();
					if (root)
					{
						root->m_TriggerList.append(trigger);
					}
					//触发器
					BTNTrigger *ptrigger = variant.value<BTNTrigger*>();
					if (ptrigger)
					{
						ptrigger->m_TriggerList.append(trigger);
					}
					//复合节点
					BTNCompose *comp = variant.value<BTNCompose*>();
					if (comp)
					{
						comp->m_TriggerList.append(trigger);
					}
					//时间节点
					BTNTime *time = variant.value<BTNTime*>();
					if (time)
					{
						time->m_TriggerList.append(trigger);
					}
					ui.treeWidget->expandAll();
				}
			}else if(m_tokenstring=="BTNTime")
			{
				countSign++;
				if(m_count==1)
				{
					qDebug()<<"ONROOT"<<m_count;
					QMap<int,QString> m_Nope;
					if(!DoParseParas("BTNTime",m_Nope))
						return;
					QTreeWidgetItem *item = addThreeNode(m_curItemRoot,QString::fromLocal8Bit("时间节点"));
					item->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNTime.png"));
					BTNTime *time = new BTNTime;
					time->setObjectName(BTNTimeManager::instance()->getBTNTime(item->text(0))->getObjectName());
					time->setNote(BTNTimeManager::instance()->getBTNTime(item->text(0))->getNote());
					time->setList(BTNTimeManager::instance()->getBTNTime(item->text(0))->getList());
					QList<RefAttr_Time> m_list_attrs=BTNTimeManager::instance()->getBTNTime(item->text(0))->getList();
					for(int i=0;i<m_list_attrs.count();i++)
					{
						QMap<int,QString> ::Iterator itor;		
						itor=m_Nope.find(i);	
						if(itor!=m_Nope.end()){
							time->setValue(m_list_attrs[i].note,itor.value());
						}						
					}
					item->setData(0, Qt::UserRole, QVariant::fromValue(time));
					m_CurItemNode=item;
					//父节点
					QVariant variant = m_curItemRoot->data(0,Qt::UserRole); // 获取数据
					//根节点
					BTNRoot *root = variant.value<BTNRoot*>();
					if (root)
					{
						root->m_TimeList.append(time);
					}
					//触发器
					BTNTrigger *trigger = variant.value<BTNTrigger*>();
					if (trigger)
					{
						trigger->m_TimeList.append(time);
					}
					//复合节点
					BTNCompose *comp = variant.value<BTNCompose*>();
					if (comp)
					{
						comp->m_TimeList.append(time);
					}
					//时间节点
					BTNTime *ptime = variant.value<BTNTime*>();
					if (ptime)
					{
						ptime->m_TimeList.append(time);
					}
					ui.treeWidget->expandAll();
				}else if(m_count==2)
				{
					qDebug()<<"ONChlid"<<m_count;
					QMap<int,QString> m_Nope;
					if(!DoParseParas("BTNTime",m_Nope))
						return;
					QTreeWidgetItem *item = addThreeNode(m_CurItemNode,QString::fromLocal8Bit("时间节点"));
					item->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNTime.png"));
					BTNTime *time = new BTNTime;
					time->setObjectName(BTNTimeManager::instance()->getBTNTime(item->text(0))->getObjectName());
					time->setNote(BTNTimeManager::instance()->getBTNTime(item->text(0))->getNote());
					time->setList(BTNTimeManager::instance()->getBTNTime(item->text(0))->getList());
					QList<RefAttr_Time> m_list_attrs=BTNTimeManager::instance()->getBTNTime(item->text(0))->getList();
					for(int i=0;i<m_list_attrs.count();i++)
					{
						QMap<int,QString> ::Iterator itor;		
						itor=m_Nope.find(i);	
						if(itor!=m_Nope.end()){
							time->setValue(m_list_attrs[i].note,itor.value());
						}						
					}
					item->setData(0, Qt::UserRole, QVariant::fromValue(time));
					m_CurItemNode1=item;
					//父节点
					QVariant variant = m_CurItemNode->data(0,Qt::UserRole); // 获取数据
					//根节点
					BTNRoot *root = variant.value<BTNRoot*>();
					if (root)
					{
						root->m_TimeList.append(time);
					}
					//触发器
					BTNTrigger *trigger = variant.value<BTNTrigger*>();
					if (trigger)
					{
						trigger->m_TimeList.append(time);
					}
					//复合节点
					BTNCompose *comp = variant.value<BTNCompose*>();
					if (comp)
					{
						comp->m_TimeList.append(time);
					}
					//时间节点
					BTNTime *ptime = variant.value<BTNTime*>();
					if (ptime)
					{
						ptime->m_TimeList.append(time);
					}
					ui.treeWidget->expandAll();
				}else if(m_count==3)
				{
					qDebug()<<"ONChlid"<<m_count;
					QMap<int,QString> m_Nope;
					if(!DoParseParas("BTNTime",m_Nope))
						return;
					QTreeWidgetItem *item = addThreeNode(m_CurItemNode1,QString::fromLocal8Bit("时间节点"));
					item->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNTime.png"));
					BTNTime *time = new BTNTime;
					time->setObjectName(BTNTimeManager::instance()->getBTNTime(item->text(0))->getObjectName());
					time->setNote(BTNTimeManager::instance()->getBTNTime(item->text(0))->getNote());
					time->setList(BTNTimeManager::instance()->getBTNTime(item->text(0))->getList());
					QList<RefAttr_Time> m_list_attrs=BTNTimeManager::instance()->getBTNTime(item->text(0))->getList();
					for(int i=0;i<m_list_attrs.count();i++)
					{
						QMap<int,QString> ::Iterator itor;		
						itor=m_Nope.find(i);	
						if(itor!=m_Nope.end()){
							time->setValue(m_list_attrs[i].note,itor.value());
						}						
					}
					item->setData(0, Qt::UserRole, QVariant::fromValue(time));
					m_CurItemNode2=item;
					//父节点
					QVariant variant = m_CurItemNode1->data(0,Qt::UserRole); // 获取数据
					//根节点
					BTNRoot *root = variant.value<BTNRoot*>();
					if (root)
					{
						root->m_TimeList.append(time);
					}
					//触发器
					BTNTrigger *trigger = variant.value<BTNTrigger*>();
					if (trigger)
					{
						trigger->m_TimeList.append(time);
					}
					//复合节点
					BTNCompose *comp = variant.value<BTNCompose*>();
					if (comp)
					{
						comp->m_TimeList.append(time);
					}
					//时间节点
					BTNTime *ptime = variant.value<BTNTime*>();
					if (ptime)
					{
						ptime->m_TimeList.append(time);
					}
					ui.treeWidget->expandAll();
				}else if(m_count==4)
				{
					qDebug()<<"ONChlid"<<m_count;
					QMap<int,QString> m_Nope;
					if(!DoParseParas("BTNTime",m_Nope))
						return;
					QTreeWidgetItem *item = addThreeNode(m_CurItemNode2,QString::fromLocal8Bit("时间节点"));
					item->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNTime.png"));
					BTNTime *time = new BTNTime;
					time->setObjectName(BTNTimeManager::instance()->getBTNTime(item->text(0))->getObjectName());
					time->setNote(BTNTimeManager::instance()->getBTNTime(item->text(0))->getNote());
					time->setList(BTNTimeManager::instance()->getBTNTime(item->text(0))->getList());
					QList<RefAttr_Time> m_list_attrs=BTNTimeManager::instance()->getBTNTime(item->text(0))->getList();
					for(int i=0;i<m_list_attrs.count();i++)
					{
						QMap<int,QString> ::Iterator itor;		
						itor=m_Nope.find(i);	
						if(itor!=m_Nope.end()){
							time->setValue(m_list_attrs[i].note,itor.value());
						}						
					}
					item->setData(0, Qt::UserRole, QVariant::fromValue(time));
					m_CurItemNode3=item;
					//父节点
					QVariant variant = m_CurItemNode1->data(0,Qt::UserRole); // 获取数据
					//根节点
					BTNRoot *root = variant.value<BTNRoot*>();
					if (root)
					{
						root->m_TimeList.append(time);
					}
					//触发器
					BTNTrigger *trigger = variant.value<BTNTrigger*>();
					if (trigger)
					{
						trigger->m_TimeList.append(time);
					}
					//复合节点
					BTNCompose *comp = variant.value<BTNCompose*>();
					if (comp)
					{
						comp->m_TimeList.append(time);
					}
					//时间节点
					BTNTime *ptime = variant.value<BTNTime*>();
					if (ptime)
					{
						ptime->m_TimeList.append(time);
					}
					ui.treeWidget->expandAll();
				}else if(m_count==5)
				{
					qDebug()<<"ONChlid"<<m_count;
					QMap<int,QString> m_Nope;
					if(!DoParseParas("BTNTime",m_Nope))
						return;
					QTreeWidgetItem *item = addThreeNode(m_CurItemNode3,QString::fromLocal8Bit("时间节点"));
					item->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNTime.png"));
					BTNTime *time = new BTNTime;
					time->setObjectName(BTNTimeManager::instance()->getBTNTime(item->text(0))->getObjectName());
					time->setNote(BTNTimeManager::instance()->getBTNTime(item->text(0))->getNote());
					time->setList(BTNTimeManager::instance()->getBTNTime(item->text(0))->getList());
					QList<RefAttr_Time> m_list_attrs=BTNTimeManager::instance()->getBTNTime(item->text(0))->getList();
					for(int i=0;i<m_list_attrs.count();i++)
					{
						QMap<int,QString> ::Iterator itor;		
						itor=m_Nope.find(i);	
						if(itor!=m_Nope.end()){
							time->setValue(m_list_attrs[i].note,itor.value());
						}						
					}
					item->setData(0, Qt::UserRole, QVariant::fromValue(time));
					//父节点
					QVariant variant = m_CurItemNode3->data(0,Qt::UserRole); // 获取数据
					//根节点
					BTNRoot *root = variant.value<BTNRoot*>();
					if (root)
					{
						root->m_TimeList.append(time);
					}
					//触发器
					BTNTrigger *trigger = variant.value<BTNTrigger*>();
					if (trigger)
					{
						trigger->m_TimeList.append(time);
					}
					//复合节点
					BTNCompose *comp = variant.value<BTNCompose*>();
					if (comp)
					{
						comp->m_TimeList.append(time);
					}
					//时间节点
					BTNTime *ptime = variant.value<BTNTime*>();
					if (ptime)
					{
						ptime->m_TimeList.append(time);
					}
					ui.treeWidget->expandAll();
				}
			}else if(m_tokenstring=="BTNLoop")
			{
				countSign++;
				if(m_count==1)
				{
					QTreeWidgetItem *Item = addThreeNode(m_curItemRoot,QString::fromLocal8Bit("循环节点"));
					Item->setIcon(0,QIcon("../data/icons/BehaviorTree//BTNLoop.png"));
					Item->setData(0, Qt::UserRole, LOOP);
					ui.treeWidget->expandAll();
				}
			}
		}
		if(m_count==0&&countSign) break;
	}
}

bool	BehaviorTreeEditWdg::DoParseParas(QString node,QMap<int,QString> &map, bool lparamStart)
{
	if(lparamStart){
		if(!matchToken(TOKEN_LPARAM)){
			//parse() : missing '('
			return 0;
		}
	}

	int idx=0;

	while(1){
		getTokenForPara();
		switch(m_token){
		case TOKEN_COMMA:
			idx++;
			break;
		case TOKEN_RPARAM:
			return 1;
		case TOKEN_END:
			//DoParseParas() : missing ')'
			return 0;
		case TOKEN_PARA:
			////任务参数
			if(node=="BTNTask")
			{
				map.insert(idx,QString::fromLocal8Bit(m_tokenstring.c_str()));
					qDebug()<<"DoParseParas() : parameter %s un expected!"<<map<<"ffffff"<<idx;				
			}
			//时间节点
			else if(node=="BTNTime")
			{
				qDebug()<<"BTNTime() : parameter %s un expected!"<<QString::fromStdString(m_tokenstring)<<"KKK"<<idx;
				map.insert(idx,QString::fromStdString(m_tokenstring));
			}
			//复合节点
			else if(node=="BTNCompose")
			{
				qDebug()<<"BTNCompose() : parameter %s un expected!"<<QString::fromStdString(m_tokenstring)<<"KKK"<<idx;
				map.insert(idx,QString::fromStdString(m_tokenstring));
			}
			//条件参数
			else if(node=="BTNTrigger")
			{
				qDebug()<<"BTNTrigger() : parameter %s un expected!"<<QString::fromStdString(m_tokenstring)<<"KKK"<<idx;
				string::size_type idx1 = m_tokenstring.find("(");
				if(idx1 !=string::npos)
				{
					QStringList list;
					list=QString::fromStdString(m_tokenstring).split("(");
					map.insert(0,list[0]);
					map.insert(1,list[1]);
					idx++;
				}else
				{
					map.insert(idx,QString::fromStdString(m_tokenstring));
				}
			}
			else
				return 0;			
			break;
		default:
			break;
		}
	}	

	return 1;
}
char BehaviorTreeEditWdg::getNextChar()
{
	if(m_pos>=m_length){
		m_pos++;
		m_col++;
		return EOF;
	}
	else{
		m_lastrow=m_row;
		m_lastcol=m_col;
		char ret=m_content[m_pos++];
		if(ret=='\n'){
			m_row++;
			m_col=0;
		}
		else if(ret=='\t')
			m_col+=5;
		else
			m_col++;
		return ret;
	}
}

void BehaviorTreeEditWdg::unGetNextChar()
{
	m_pos--;
	m_col=m_lastcol;
	m_row=m_lastrow;
}

void BehaviorTreeEditWdg::getToken()
{
	if(m_bTokenParseStop){
		m_bTokenParseStop=0;
		return;
	}
	//int tokenStringIndex=0;
	TokenType currentToken=TOKEN_UNKNOWN;
	StateType state=STATE_UNKNOWN;
	m_tokenstring.clear();
	bool save=0;

	while(state!=STATE_DONE)
	{
		char c=getNextChar();
		save=1;
		switch(state)
		{
		case STATE_UNKNOWN:
			if(isalpha(c)){
				state=STATE_INIDENTIFY;
			}
			else if(isdigit(c)||c=='-'||c=='.')
				state=STATE_INVALUE;
			else{		
				switch(c){				
				case ' ':
				case '\n':
				case '\t':
				case '>':
				case '<':
					save=0;
					break;
				case '/':
					state=STATE_EXPECTCOMMENT;
					save=0;
					break;
				case '(':
					state=STATE_DONE;
					currentToken=TOKEN_LPARAM;
					break;
				case ')':
					state=STATE_DONE;
					currentToken=TOKEN_RPARAM;
					break;
				case '{':
					state=STATE_DONE;
					currentToken=TOKEN_LBPARAM;
					m_count++;
					break;
				case '}':
					state=STATE_DONE;
					currentToken=TOKEN_RBPARAM;
					m_count--;
					break;
				case ',':
					state=STATE_DONE;
					currentToken=TOKEN_COMMA;
					break;
				case ';':
					state=STATE_DONE;
					currentToken=TOKEN_SEMI;
					break;
				case '!':
					state=STATE_DONE;
					currentToken=TOKEN_NOTOPE;
					break;
				case '&':
					state=STATE_DONE;
					currentToken=TOKEN_ANDOPE;
					break;
				case '|':
					state=STATE_DONE;
					currentToken=TOKEN_OROPE;
					break;
				case EOF:
					state=STATE_DONE;
					currentToken=TOKEN_END;
					save=0;
					break;
				default:
					unGetNextChar();
					save=0;
					state=STATE_DONE;
					currentToken=TOKEN_ERR;
					break;
				}
			}
			break;
		case STATE_EXPECTCOMMENT:
			if(c=='/'){
				state=STATE_INCOMMENT;
				save=0;
			}
			else{
				state=STATE_INCOMMENT;
				save=0;
				unGetNextChar();
				save=0;
				state=STATE_DONE;
				currentToken=TOKEN_ERR;
			}
			break;
		case STATE_INCOMMENT:
			if(c=='\n'){
				state=STATE_DONE;
				currentToken=TOKEN_COMMENT;
			}
			break;
		case STATE_INIDENTIFY:
			if(!(isalpha(c)||isdigit(c)||c=='_')){
				unGetNextChar();
				save=0;
				state=STATE_DONE;
				currentToken=TOKEN_IDENTIFY;
			}
			break;
		case STATE_INVALUE:
			if(!(isdigit(c)||c=='.')){
				unGetNextChar();
				save=0;
				state=STATE_DONE;
				currentToken=TOKEN_VALUE;
			}
			break;
		default:
			//save=0;
			break;
		}			
		if(save){
			m_tokenstring+=c;
			qDebug()<<"+++"<<QString::fromStdString(m_tokenstring);
		}
	}

	m_token=currentToken;
}

void BehaviorTreeEditWdg::getTokenForPara()
{
	TokenType currentToken=TOKEN_UNKNOWN;
	StateType state=STATE_UNKNOWN;
	m_tokenstring.clear();
	bool save;
	while(state!=STATE_DONE)
	{
		save=1;
		char c=getNextChar();
		switch(state)
		{
		case STATE_UNKNOWN:
			switch(c){	
			case ')':
				state=STATE_DONE;
				currentToken=TOKEN_RPARAM;
				break;
			case ',':
				state=STATE_DONE;
				currentToken=TOKEN_COMMA;
				break;
			case EOF:
				state=STATE_DONE;
				currentToken=TOKEN_END;
				save=0;
				break;
			case '.':
				state=STATE_DONE;
				currentToken=TOKEN_COMMA;
				break;
			default:
				state=STATE_INPARA;
				break;
			}
			break;
		case STATE_INPARA:
			if(c==','||c==')'||c==EOF){
				unGetNextChar();
				state=STATE_DONE;
				currentToken=TOKEN_PARA;
				save=0;
			}
			break;
		default:
			//save=0;
			break;
		}			
		if(save)
			m_tokenstring+=c;
		qDebug()<<"###"<<QString::fromStdString(m_tokenstring);
		qDebug()<<"#@#"<<QString::fromLocal8Bit(m_tokenstring.c_str());

	}
	m_token=currentToken;
}

bool BehaviorTreeEditWdg::matchToken(TokenType token,bool expected/*=false*/)
{
	getToken();
	return matchSingleToken(token,expected);
}

bool BehaviorTreeEditWdg::matchSingleToken(TokenType token,bool expected/*=false*/)
{
	bool ret=(m_token==token);
	if(!ret&&expected){
		m_parseState=STATE_ERR;
	}
	return ret;
}
