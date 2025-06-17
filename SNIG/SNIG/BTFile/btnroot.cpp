#include "StdAfx.h"
#include "btnroot.h"
#include <QDebug>
#include <QFile>
#include <QDomComment>
#include "forcetask.h"
#include "btntrigger.h"
#include "btncompose.h"
#include "btntime.h"
#include "btncondition.h"
#include "btnloop.h"
#include "btnbreak.h"
#include "btnend.h"

BTNRoot::BTNRoot()
{

}

BTNRoot::~BTNRoot()
{

}

void BTNRoot::setObjectName(QString &name)
{
	m_ObjectName = name;
}

QString BTNRoot::getObjectName()
{
	return m_ObjectName;
}

void BTNRoot::setNote(QString &note)
{
	m_Note = note;
}

QString BTNRoot::getNote()
{
	return m_Note;
}

void BTNRoot::setList(QList<RefAttr_Root> &list)
{
	m_list_attrs = list;
}

QList<RefAttr_Root> BTNRoot::getList()
{
	return m_list_attrs;
}

void BTNRoot::listAppend(RefAttr_Root &attr)
{
	m_list_attrs.append(attr);
}

void BTNRoot::setValue(QString &note,QString &value)
{
	QList<RefAttr_Root> temp_list;
	for (int i=0;i<m_list_attrs.size();i++)
	{
		RefAttr_Root attr = m_list_attrs.at(i);
		if (attr.note == note)
		{
			attr.initVal = value;
		}
		temp_list.append(attr);
	}
	m_list_attrs = temp_list;
}

QString BTNRoot::getContent()
{
	QStringList attrStrList;
	for (int i=0;i<m_list_attrs.size();i++)
	{
		RefAttr_Root attr = m_list_attrs.at(i);
		attrStrList.append(attr.map.key(attr.initVal));
	}
	QString content;
	content = QString::fromLocal8Bit("BTNCompose(%1)").arg(attrStrList.join(","));
	bool hasChild=false;
	if (m_TriggerList.size()>0 || m_TaskList.size()>0 || m_ComposeList.size()>0 || m_TimeList.size()>0 || m_ConditionList.size()>0 || m_LoopList.size()>0 || m_BreakList.size()>0 || m_EndList.size()>0)
	{
		content += "{";
		hasChild = true;
	}
	//触发器
	for (int i=0;i<m_TriggerList.size();i++)
	{
		BTNTrigger *trigger = m_TriggerList.at(i);
		if(trigger == NULL) continue;
		QString trigger_content = trigger->getContent();
		content += "\n    ";
		content += trigger_content;
	}
	//复合节点
	for (int i=0;i<m_ComposeList.size();i++)
	{
		BTNCompose *compose = m_ComposeList.at(i);
		if(compose == NULL) continue;
		QString compose_content = compose->getContent();
		content += "\n    ";
		content += compose_content;
	}
	//时间节点
	for (int i=0;i<m_TimeList.size();i++)
	{
		BTNTime *time = m_TimeList.at(i);
		if(time == NULL) continue;
		QString time_content = time->getContent();
		content += "\n    ";
		content += time_content;
	}
	//条件节点
	for (int i=0;i<m_ConditionList.size();i++)
	{
		BTNCondition *condition = m_ConditionList.at(i);
		if(condition == NULL) continue;
		QString condition_content = condition->getContent();
		content += "\n    ";
		content += condition_content;
	}
	//循环节点
	for (int i=0;i<m_LoopList.size();i++)
	{
		BTNLoop *loop = m_LoopList.at(i);
		if(loop == NULL) continue;
		QString loop_content = loop->getContent();
		content += "\n    ";
		content += loop_content;
	}
	//任务
	for (int i=0;i<m_TaskList.size();i++)
	{
		ForceTask *task = m_TaskList.at(i);
		if(task == NULL) continue;
		QString task_content = task->getContent();
		content += "\n    ";
		content += task_content;
	}
	//跳出
	for (int i=0;i<m_BreakList.size();i++)
	{
		BTNBreak *br = m_BreakList.at(i);
		if(br == NULL) continue;
		QString br_content = br->getContent();
		content += "\n    ";
		content += br_content;
	}
	//结束
	for (int i=0;i<m_EndList.size();i++)
	{
		BTNEnd *end = m_EndList.at(i);
		if(end == NULL) continue;
		QString end_content = end->getContent();
		content += "\n    ";
		content += end_content;
	}

	if (hasChild)
	{
		content += "\n}";
	}
	return content;
}

BTNRootManager * BTNRootManager::instance()
{
	static BTNRootManager instance;
	return &instance;
}

void BTNRootManager::readConfig()
{
	QString strFile = "../data/BehaviorTree/nodes.xml";
	QFile file(strFile);
	if(!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "open failed";
		file.close();
		return;
	}
	QString xmlInfo = QString::fromLocal8Bit(file.readAll().data());
	QDomDocument doc;
	QString error;
	int row = 0;
	int column = 0;
	if(!doc.setContent(xmlInfo,false,&error,&row,&column))
	{
		qDebug() << QString("read failed at %1 row %2 column, error: %3").arg(row).arg(column).arg(error);
		return;
	}
	QDomElement root = doc.documentElement();
	if (root.hasChildNodes())
	{
		//BTNRoot
		QDomElement Ele_Item = root.firstChildElement("Item");
		while(!Ele_Item.isNull())
		{
			if(Ele_Item.attribute("Class") == "BTNRoot")
			{
				BTNRoot *root = new BTNRoot;
				//Attribute
				QDomElement Ele_Attribute = Ele_Item.firstChildElement("Attribute");
				while(!Ele_Attribute.isNull())
				{
					if (Ele_Attribute.attribute("Name") == "objectName")
						root->setObjectName(Ele_Attribute.attribute("Value"));
					else if(Ele_Attribute.attribute("Name") == "note")
						root->setNote(Ele_Attribute.attribute("Value"));
					Ele_Attribute = Ele_Attribute.nextSiblingElement("Attribute");
				}
				//Nested_Items
				QDomElement Ele_Nested_Items = Ele_Item.firstChildElement("Nested_Items");
				while(!Ele_Nested_Items.isNull())
				{
					//Item
					QDomElement Ele_Nested_Item = Ele_Nested_Items.firstChildElement("Item");
					while(!Ele_Nested_Item.isNull())
					{
						RefAttr_Root attr;
						//Attribute
						QDomElement Ele_Nested_Item_Attribute = Ele_Nested_Item.firstChildElement("Attribute");
						while(!Ele_Nested_Item_Attribute.isNull())
						{
							if(Ele_Nested_Item_Attribute.attribute("Name") == "idf")
								attr.idf = Ele_Nested_Item_Attribute.attribute("Value");
							else if(Ele_Nested_Item_Attribute.attribute("Name") == "note")
								attr.note = Ele_Nested_Item_Attribute.attribute("Value");
							else if(Ele_Nested_Item_Attribute.attribute("Name") == "type")
								attr.type = Ele_Nested_Item_Attribute.attribute("Value");
							else if(Ele_Nested_Item_Attribute.attribute("Name") == "initVal")
								attr.initVal = Ele_Nested_Item_Attribute.attribute("Value");
							else
							{
								QString name = Ele_Nested_Item_Attribute.attribute("Name");
								QString value = Ele_Nested_Item_Attribute.attribute("Value");
								attr.nameList.append(name);
								attr.valueList.append(value);
								attr.map.insert(name,value);
							}

							Ele_Nested_Item_Attribute = Ele_Nested_Item_Attribute.nextSiblingElement("Attribute");
						}

						root->listAppend(attr);
						Ele_Nested_Item = Ele_Nested_Item.nextSiblingElement("Item");
					}

					Ele_Nested_Items = Ele_Nested_Items.nextSiblingElement("Nested_Items");
				}

				QString tNode = root->getNote();
				m_RootMap.insert(tNode,root);
			}
			Ele_Item = Ele_Item.nextSiblingElement("Item");
		}
	}
}

BTNRoot * BTNRootManager::getBTNRoot(QString &rootname)
{
	return m_RootMap.value(rootname);
}

BTNRootManager::BTNRootManager()
{
	readConfig();
}

BTNRootManager::~BTNRootManager()
{

}
