#include "StdAfx.h"
#include "forcetask.h"
#include <QDebug>
#include <QFile>
#include <QDomComment>

ForceTask::ForceTask()
{

}

ForceTask::~ForceTask()
{

}

void ForceTask::setObjectName(QString &name)
{
	m_ObjectName = name;
}

QString ForceTask::getObjectName()
{
	return m_ObjectName;
}

void ForceTask::setNote(QString &note)
{
	m_Note = note;
}

QString ForceTask::getNote()
{
	return m_Note;
}

void ForceTask::setList(QList<RefAttr_Task> &list)
{
	m_list_attrs = list;
}

QList<RefAttr_Task> ForceTask::getList()
{
	return m_list_attrs;
}

void ForceTask::listAppend(RefAttr_Task &attr)
{
	m_list_attrs.append(attr);
}

void ForceTask::setValue(QString &note,QString &value)
{
	QList<RefAttr_Task> temp_list;
	for (int i=0;i<m_list_attrs.size();i++)
	{
		RefAttr_Task attr = m_list_attrs.at(i);
		if (attr.note == note)
		{
			attr.initVal = value;
		}
		temp_list.append(attr);
	}
	m_list_attrs = temp_list;
}

QString ForceTask::getContent()
{
	QString content;
	content = QString::fromLocal8Bit("BTNTask(");
	QStringList attrStrList;
	attrStrList.append(m_ObjectName);
	for (int i=0;i<m_list_attrs.size();i++)
	{
		attrStrList.append(m_list_attrs.at(i).initVal);
	}
	content += attrStrList.join(",");
	content += QString::fromLocal8Bit(")");
	return content;
}

ForceTaskManager *ForceTaskManager::instance()
{
	static ForceTaskManager instance;
	return &instance;
}

void ForceTaskManager::readConfig()
{
	QString strFile = "../data/BehaviorTree/tasks.xml";
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
		//ForceTask
		QDomElement Ele_Item = root.firstChildElement("Item");
		while(!Ele_Item.isNull())
		{
			if(Ele_Item.attribute("Class") == "ForceTask")
			{
				ForceTask *task = new ForceTask;
				//Attribute
				QDomElement Ele_Attribute = Ele_Item.firstChildElement("Attribute");
				while(!Ele_Attribute.isNull())
				{
					if (Ele_Attribute.attribute("Name") == "objectName")
						task->setObjectName(Ele_Attribute.attribute("Value"));
					else if(Ele_Attribute.attribute("Name") == "note")
						task->setNote(Ele_Attribute.attribute("Value"));
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
						RefAttr_Task attr;
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
							else if(Ele_Nested_Item_Attribute.attribute("Name") == "range")
								attr.range = Ele_Nested_Item_Attribute.attribute("Value");
							else if(Ele_Nested_Item_Attribute.attribute("Name") == "unit")
								attr.unit = Ele_Nested_Item_Attribute.attribute("Value");
							else if(Ele_Nested_Item_Attribute.attribute("Name") == "options")
								attr.options = Ele_Nested_Item_Attribute.attribute("Value");
							else if(Ele_Nested_Item_Attribute.attribute("Name") == "initVal")
								attr.initVal = Ele_Nested_Item_Attribute.attribute("Value");
							else if(Ele_Nested_Item_Attribute.attribute("Name") == "EditEnable")
								attr.editEnable = Ele_Nested_Item_Attribute.attribute("Value");

							Ele_Nested_Item_Attribute = Ele_Nested_Item_Attribute.nextSiblingElement("Attribute");
						}

						task->listAppend(attr);
						Ele_Nested_Item = Ele_Nested_Item.nextSiblingElement("Item");
					}

					Ele_Nested_Items = Ele_Nested_Items.nextSiblingElement("Nested_Items");
				}
				QString tNode = task->getNote();
				m_TaskMap.insert(tNode,task);
				m_TaskList.append(task);
				m_TaskNodeList.append(tNode);
			}

			Ele_Item = Ele_Item.nextSiblingElement("Item");
		}
	}
}

ForceTask *ForceTaskManager::getForceTask(QString &taskname)
{
	return m_TaskMap.value(taskname);
}

QMap<QString,ForceTask*> ForceTaskManager::getTaskMap()
{
	return m_TaskMap;
}

QList<ForceTask*> ForceTaskManager::getTaskList()
{
	return m_TaskList;
}

QStringList ForceTaskManager::getTaskNodeList()
{
	return m_TaskNodeList;
}

ForceTaskManager::ForceTaskManager()
{
	readConfig();
}

ForceTaskManager::~ForceTaskManager()
{

}
