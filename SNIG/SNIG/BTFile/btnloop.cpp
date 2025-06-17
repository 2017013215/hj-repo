#include "StdAfx.h"
#include "btnloop.h"
#include <QDebug>
#include <QFile>
#include <QDomComment>
#include "forcetask.h"
#include "btncompose.h"
#include "btntime.h"
#include "btntrigger.h"

BTNLoop::BTNLoop()
{

}

BTNLoop::~BTNLoop()
{

}
void BTNLoop::setObjectName(QString &name)
{
	m_ObjectName = name;
}

QString BTNLoop::getObjectName()
{
	return m_ObjectName;
}

void BTNLoop::setNote(QString &note)
{
	m_Note = note;
}

QString BTNLoop::getNote()
{
	return m_Note;
}

void BTNLoop::setList(QList<RefAttr_Loop> &list)
{
	m_list_attrs = list;
}

QList<RefAttr_Loop> BTNLoop::getList()
{
	return m_list_attrs;
}

void BTNLoop::listAppend(RefAttr_Loop &attr)
{
	m_list_attrs.append(attr);
}

void BTNLoop::setValue(QString &note,QString &value)
{
	QList<RefAttr_Loop> temp_list;
	for (int i=0;i<m_list_attrs.size();i++)
	{
		RefAttr_Loop attr = m_list_attrs.at(i);
		if (attr.note == note)
		{
			attr.initVal = value;
		}
		temp_list.append(attr);
	}
	m_list_attrs = temp_list;
}

QString BTNLoop::getContent()
{
	//BTNLoop(UntilFaile,0)
	QStringList attrStrList;
	for (int i=0;i<m_list_attrs.size();i++)
	{
		RefAttr_Loop attr = m_list_attrs.at(i);
		QString value = attr.type=="combox" ? attr.map.key(attr.initVal) : attr.initVal;
		attrStrList.append(value);
	}
	QString content;
	content = QString::fromLocal8Bit("BTNLoop(%1)").arg(attrStrList.join(","));
	bool hasChild=false;
	if (m_TriggerList.size()>0 || m_TaskList.size()>0 || m_ComposeList.size()>0 || m_TimeList.size()>0)
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
	//任务
	for (int i=0;i<m_TaskList.size();i++)
	{
		ForceTask *task = m_TaskList.at(i);
		if(task == NULL) continue;
		QString task_content = task->getContent();
		content += "\n    ";
		content += task_content;
	}

	if (hasChild)
	{
		content += "\n}";
	}
	return content;
}

BTNLoopManager * BTNLoopManager::instance()
{
	static BTNLoopManager instance;
	return &instance;
}

void BTNLoopManager::readConfig()
{
	QString strFile = "../data/BehaviorTree/loops.xml";
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
			if(Ele_Item.attribute("Class") == "BTNLoop")
			{
				BTNLoop *loop = new BTNLoop;
				//Attribute
				QDomElement Ele_Attribute = Ele_Item.firstChildElement("Attribute");
				while(!Ele_Attribute.isNull())
				{
					if (Ele_Attribute.attribute("Name") == "objectName")
						loop->setObjectName(Ele_Attribute.attribute("Value"));
					else if(Ele_Attribute.attribute("Name") == "note")
						loop->setNote(Ele_Attribute.attribute("Value"));
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
						RefAttr_Loop attr;
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

						loop->listAppend(attr);
						Ele_Nested_Item = Ele_Nested_Item.nextSiblingElement("Item");
					}

					Ele_Nested_Items = Ele_Nested_Items.nextSiblingElement("Nested_Items");
				}

				QString tNode = loop->getNote();
				m_LoopMap.insert(tNode,loop);
				m_LoopList.append(tNode);
			}
			Ele_Item = Ele_Item.nextSiblingElement("Item");
		}
	}
}

BTNLoop * BTNLoopManager::getBBTNLoop(QString &loopname)
{
	return m_LoopMap.value(loopname);
}

QStringList BTNLoopManager::getLoopNodeList()
{
	return m_LoopList;
}

BTNLoopManager::BTNLoopManager()
{
	readConfig();
}

BTNLoopManager::~BTNLoopManager()
{

}