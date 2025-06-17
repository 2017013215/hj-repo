#include "StdAfx.h"
#include "btnbreak.h"
#include <QDebug>
#include <QFile>
#include <QDomComment>

BTNBreak::BTNBreak()
{

}

BTNBreak::~BTNBreak()
{

}

void BTNBreak::setObjectName(QString &name)
{
	m_ObjectName = name;
}

QString BTNBreak::getObjectName()
{
	return m_ObjectName;
}

void BTNBreak::setNote(QString &note)
{
	m_Note = note;
}

QString BTNBreak::getNote()
{
	return m_Note;
}

void BTNBreak::setList(QList<RefAttr_Break> &list)
{
	m_list_attrs = list;
}

QList<RefAttr_Break> BTNBreak::getList()
{
	return m_list_attrs;
}

void BTNBreak::listAppend(RefAttr_Break &attr)
{
	m_list_attrs.append(attr);
}

void BTNBreak::setValue(QString &note,QString &value)
{
	QList<RefAttr_Break> temp_list;
	for (int i=0;i<m_list_attrs.size();i++)
	{
		RefAttr_Break attr = m_list_attrs.at(i);
		if (attr.note == note)
		{
			attr.initVal = value;
		}
		temp_list.append(attr);
	}
	m_list_attrs = temp_list;
}

QString BTNBreak::getContent()
{
	//BTNBreak(0)
	QStringList attrStrList;
	for (int i=0;i<m_list_attrs.size();i++)
	{
		RefAttr_Break attr = m_list_attrs.at(i);
		QString value = attr.type=="combox" ? attr.map.key(attr.initVal) : attr.initVal;
		attrStrList.append(value);
	}
	QString content;
	content = QString::fromLocal8Bit("BTNBreak(%1)").arg(attrStrList.join(","));
	return content;
}

BTNBreakManager * BTNBreakManager::instance()
{
	static BTNBreakManager instance;
	return &instance;
}

void BTNBreakManager::readConfig()
{
	QString strFile = "../data/BehaviorTree/breaks.xml";
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
			if(Ele_Item.attribute("Class") == "BTNBreak")
			{
				BTNBreak *break_ = new BTNBreak;
				//Attribute
				QDomElement Ele_Attribute = Ele_Item.firstChildElement("Attribute");
				while(!Ele_Attribute.isNull())
				{
					if (Ele_Attribute.attribute("Name") == "objectName")
						break_->setObjectName(Ele_Attribute.attribute("Value"));
					else if(Ele_Attribute.attribute("Name") == "note")
						break_->setNote(Ele_Attribute.attribute("Value"));
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
						RefAttr_Break attr;
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

						break_->listAppend(attr);
						Ele_Nested_Item = Ele_Nested_Item.nextSiblingElement("Item");
					}

					Ele_Nested_Items = Ele_Nested_Items.nextSiblingElement("Nested_Items");
				}

				QString tNode = break_->getNote();
				m_BreakMap.insert(tNode,break_);
				m_BreakList.append(tNode);
			}
			Ele_Item = Ele_Item.nextSiblingElement("Item");
		}
	}
}

BTNBreak * BTNBreakManager::getBBTNBreak(QString &breakname)
{
	return m_BreakMap.value(breakname);
}

QStringList BTNBreakManager::getBreakNodeList()
{
	return m_BreakList;
}

BTNBreakManager::BTNBreakManager()
{
	readConfig();
}

BTNBreakManager::~BTNBreakManager()
{

}