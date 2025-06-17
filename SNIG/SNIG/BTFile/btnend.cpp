#include "StdAfx.h"
#include "btnend.h"
#include <QDebug>
#include <QFile>
#include <QDomComment>

BTNEnd::BTNEnd()
{

}

BTNEnd::~BTNEnd()
{

}

void BTNEnd::setObjectName(QString &name)
{
	m_ObjectName = name;
}

QString BTNEnd::getObjectName()
{
	return m_ObjectName;
}

void BTNEnd::setNote(QString &note)
{
	m_Note = note;
}

QString BTNEnd::getNote()
{
	return m_Note;
}

void BTNEnd::setList(QList<RefAttr_End> &list)
{
	m_list_attrs = list;
}

QList<RefAttr_End> BTNEnd::getList()
{
	return m_list_attrs;
}

void BTNEnd::listAppend(RefAttr_End &attr)
{
	m_list_attrs.append(attr);
}

void BTNEnd::setValue(QString &note,QString &value)
{
	QList<RefAttr_End> temp_list;
	for (int i=0;i<m_list_attrs.size();i++)
	{
		RefAttr_End attr = m_list_attrs.at(i);
		if (attr.note == note)
		{
			attr.initVal = value;
		}
		temp_list.append(attr);
	}
	m_list_attrs = temp_list;
}

QString BTNEnd::getContent()
{
	//BTNEnd()
	QStringList attrStrList;
	for (int i=0;i<m_list_attrs.size();i++)
	{
		RefAttr_End attr = m_list_attrs.at(i);
		QString value = attr.type=="combox" ? attr.map.key(attr.initVal) : attr.initVal;
		attrStrList.append(value);
	}
	QString content;
	content = QString::fromLocal8Bit("BTNEnd()");
	return content;
}

BTNEndManager * BTNEndManager::instance()
{
	static BTNEndManager instance;
	return &instance;
}

void BTNEndManager::readConfig()
{
	QString strFile = "../data/BehaviorTree/ends.xml";
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
			if(Ele_Item.attribute("Class") == "BTNEnd")
			{
				BTNEnd *end_ = new BTNEnd;
				//Attribute
				QDomElement Ele_Attribute = Ele_Item.firstChildElement("Attribute");
				while(!Ele_Attribute.isNull())
				{
					if (Ele_Attribute.attribute("Name") == "objectName")
						end_->setObjectName(Ele_Attribute.attribute("Value"));
					else if(Ele_Attribute.attribute("Name") == "note")
						end_->setNote(Ele_Attribute.attribute("Value"));
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
						RefAttr_End attr;
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

						end_->listAppend(attr);
						Ele_Nested_Item = Ele_Nested_Item.nextSiblingElement("Item");
					}

					Ele_Nested_Items = Ele_Nested_Items.nextSiblingElement("Nested_Items");
				}

				QString tNode = end_->getNote();
				m_EndMap.insert(tNode,end_);
				m_EndList.append(tNode);
			}
			Ele_Item = Ele_Item.nextSiblingElement("Item");
		}
	}
}

BTNEnd * BTNEndManager::getBBTNEnd(QString &endname)
{
	return m_EndMap.value(endname);
}

QStringList BTNEndManager::getEndNodeList()
{
	return m_EndList;
}

BTNEndManager::BTNEndManager()
{
	readConfig();
}

BTNEndManager::~BTNEndManager()
{

}
