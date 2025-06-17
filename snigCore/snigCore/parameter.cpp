#include "parameter.h"
#include <QStringList>
#include <QDomElement>
#include <QUuid>
#include <QDebug>
#include <QFile>

ByyParameter::ByyParameter()
{
	IsCanDesign = "0";
	myUUID = QUuid::createUuid().toString();
}

ByyParameter::~ByyParameter()
{

}

void ByyParameter::updataUUID()
{
	myUUID = QUuid::createUuid().toString();
}

void ByyParameter::fromConfig( QDomElement& element )
{
	Name=element.attribute("Name");
	Tag=element.attribute("Tag");
	Type=element.attribute("Type");
	Description=element.attribute("Description");
	Group=element.attribute("Group");
	myDefValue=element.attribute("DefValue");
	IsCanDesign=element.attribute("ExpFactor");
}

QString ByyValueParameter::getValue()
{
	return myValue;
}

void ByyValueParameter::setValue( const QString& val )
{
	myValue=val;
}

void ByyValueParameter::setDesign( const QString& val )
{
	IsDesign = val;
}

ByyParameter* ByyValueParameter::clone()
{
	ByyValueParameter * param = new ByyValueParameter(*this);
	param->updataUUID();
	return param;
}

void ByyValueParameter::fromConfig( QDomElement& element )
{
	__super::fromConfig(element);
	myValue=element.attribute("DefValue");
}

ByyListParameter::ByyListParameter( const ByyListParameter& orig )
	:ByyParameter(orig)
{
	myChildren=orig.myChildren;
	mySeparator=orig.mySeparator;
}

ByyListParameter::ByyListParameter()
{

}

ByyListParameter::~ByyListParameter()
{

}

void ByyListParameter::fromConfig( QDomElement& element )
{
	__super::fromConfig(element);

	mySeparator=element.attribute("Separator");
}

void ByyListParameter::removeOne( ByyParameter* param )
{
	myChildren.removeOne(param);

	delete param;
}

ByyStructParameter::ByyStructParameter()
	:ByyListParameter()
{

}

ByyStructParameter::ByyStructParameter( const ByyStructParameter& orig )
	:ByyListParameter(orig)
{
	
}

ByyStructParameter::~ByyStructParameter()
{

}

ByyParameter* ByyStructParameter::clone()
{
	return new ByyStructParameter(*this);
}

QString ByyStructParameter::getValue()
{
	QStringList strList;

	for (int i=0;i<myChildren.size();++i)
	{
		strList.append(myChildren[i]->getValue());
	}

	return strList.join(mySeparator);
}

void ByyStructParameter::setValue( const QString& val )
{
	QStringList strList=val.split(mySeparator);

	for (int i=0;i<strList.size()&&i<myChildren.size();++i)
	{
		myChildren[i]->setValue(strList[i]);
	}
}

void ByyStructParameter::setDesign( const QString& val )
{
	QStringList strList=val.split(mySeparator);

	for (int i=0;i<strList.size()&&i<myChildren.size();++i)
	{
		myChildren[i]->setDesign(strList[i]);
	}
}

/*

const ByyStructParameter& ByyStructParameter::operator=( const ByyStructParameter& orig )
{
	mySeparator=orig.mySeparator;

	return *this;
}*/

void ByyStructParameter::fromConfig( QDomElement& element )
{
	__super::fromConfig(element);

	QDomNodeList children=element.childNodes();

	for (int j=0;j!=children.size();++j)
	{
		if (children.at(j).isElement()&&children.at(j).nodeName()=="Component")
		{
			myChildren.append(ByyParameterFactory::create(children.at(j).toElement()));
		}
	}
}

ByyStructArrayParameter::ByyStructArrayParameter()
	:ByyListParameter()
{

}

ByyStructArrayParameter::ByyStructArrayParameter( const ByyStructArrayParameter& orig )
	:ByyListParameter(orig)
{
	mySeparator=orig.mySeparator;
	myArraySeparator=orig.myArraySeparator;
	myTemplate=orig.myTemplate;
}

ByyStructArrayParameter::~ByyStructArrayParameter()
{
	
}

/*
const ByyStructArrayParameter& ByyStructArrayParameter::operator=( const ByyStructArrayParameter& orig )
{
	mySeparator=orig.mySeparator;
	myArraySeparator=orig.myArraySeparator;
	myTemplate=orig.myTemplate;

	return *this;
}*/

QString ByyStructArrayParameter::getValue()
{
	if (myChildren.size()==0)
		return QString();

	QStringList strList;

	for (int i=0;i<myChildren.size();++i)
	{
		strList.append(myChildren[i]->getValue());
	}

	return strList.join(myArraySeparator)+myArraySeparator;
}

void ByyStructArrayParameter::setValue( const QString& val )
{
	QStringList strList=val.split(myArraySeparator);

	strList.removeOne("");

	while(myChildren.size()<strList.size())
	{
		push_back();
	}

	for (int i=0;i<strList.size()&&i<myChildren.size();++i)
	{
		myChildren[i]->setValue(strList[i]);
	}
}

void ByyStructArrayParameter::setDesign( const QString& val )
{
	QStringList strList=val.split(myArraySeparator);

	strList.removeOne("");

	while(myChildren.size()<strList.size())
	{
		push_back();
	}

	for (int i=0;i<strList.size()&&i<myChildren.size();++i)
	{
		myChildren[i]->setDesign(strList[i]);
	}
}

ByyParameter* ByyStructArrayParameter::clone()
{
	return new ByyStructArrayParameter(*this);
}

void ByyStructArrayParameter::fromConfig( QDomElement& element )
{
	__super::fromConfig(element);

	myArraySeparator=element.attribute("ArraySeparator");

	myTemplate.fromConfig(element);
	myTemplate.Type="struct";
}

void ByyStructArrayParameter::push_back()
{
	myChildren.append(myTemplate.clone());
}

ByyParameter* ByyParameterFactory::create( QDomElement& element )
{
	if (element.tagName()!="Parameter"&&element.tagName()!="Component")
		return 0;

	QString type=element.attribute("Type");

	ByyParameter *param;

	if (type=="struct")
	{
		param=new ByyStructParameter;
	}
	else if (type=="structArray")
	{
		param=new ByyStructArrayParameter;
	}
	else if (type=="enum")
	{
		param=new ByyEnumParameter;
	}
	else if (type=="path")
	{
		param=new ByyPathParameter;
	}
	else
	{
		param=new ByyValueParameter;
	}

	param->fromConfig(element);

	return param;
}

ParameterList::ParameterList()
{
	
}

ParameterList::ParameterList( const ParameterList& orig )
{
	qDeleteAll(*this);

	clear();

	for (int i=0;i!=orig.size();++i)
	{
		append(orig.at(i)->clone());
	}
}

ParameterList::~ParameterList()
{
	while(size()>0)
	{
		delete takeFirst();
	}
}

const ParameterList& ParameterList::operator=( const ParameterList& orig )
{
	qDeleteAll(*this);
	clear();
	myParamMap.clear();

	for (int i=0;i!=orig.size();++i)
	{
		ByyParameter* p=orig.at(i)->clone();
		append(p);
		myParamMap.insert(p->Name,p);
	}

	return *this;
}

const ParameterList& ParameterList::operator+=( const ParameterList& orig )
{
	for (int i=0;i!=orig.size();++i)
	{
		ByyParameter* p=orig.at(i)->clone();
		append(p);
		myParamMap.insert(p->Name,p);
	}

	return *this;
}

void ParameterList::setValue( const QString& name,const QString& value )
{
	ByyParameter* p=findParameter(name);

	if (p)
	{
		p->setValue(value);
	}
}

void ParameterList::setDesign( const QString& name,const QString& value )
{
	ByyParameter* p=findParameter(name);

	if (p)
	{
		p->setDesign(value);
	}
}

ByyParameter* ParameterList::findParameter( const QString& param )
{
	QHash<QString,ByyParameter*>::iterator it=myParamMap.find(param);

	if (it!=myParamMap.end())
	{
		return it.value();
	}
	else
		return 0;
}

/*
const ByyListParameter& ByyListParameter::operator=( const ByyListParameter& orig )
{
	myChildren=orig.myChildren.clone();
}*/

ByyEnumParameter::ByyEnumParameter()
{

}

ByyEnumParameter::ByyEnumParameter(const ByyEnumParameter& orig)
{
	Name=orig.Name;
	Tag=orig.Tag;
	Type=orig.Type;
	Description=orig.Description;
	Group=orig.Group;
	myDefValue=orig.myDefValue;
	myValue=orig.myValue;
	IsCanDesign=orig.IsCanDesign;
	FilePath=orig.FilePath;
	Keyword=orig.Keyword;
	m_EnumMap=orig.m_EnumMap;
}

ByyEnumParameter::~ByyEnumParameter()
{

}

void ByyEnumParameter::fromConfig(QDomElement& element)
{
	__super::fromConfig(element);
	FilePath=element.attribute("FilePath");
	Keyword=element.attribute("Keyword");

	QDomDocument doc("mydocument");
	QFile file(FilePath);
	if (!file.open(QIODevice::ReadOnly))
		return;
	if (!doc.setContent(&file))
	{
		file.close();
		return;
	}
	file.close();
	QDomElement root = doc.documentElement();
	if (root.tagName()!="SimRepEnum") return;

	QDomElement enumTypeElement = root.firstChildElement(Keyword);
	if (!enumTypeElement.isNull())
	{
		QDomElement enumElement = enumTypeElement.firstChildElement("enum");
		while(!enumElement.isNull())
		{
			int value_ = enumElement.attribute("Value").toInt();
			QString desc_ = enumElement.attribute("Desc");
			m_EnumMap.insert(value_, desc_);

			enumElement = enumElement.nextSiblingElement("enum");
		}
	}
}

QMap<int, QString> ByyEnumParameter::getEnumMap()
{
	return m_EnumMap;
}

ByyParameter* ByyEnumParameter::clone()
{
	ByyEnumParameter * param = new ByyEnumParameter(*this);
	param->updataUUID();
	return param;
}

QString ByyEnumParameter::getValue()
{
	return myValue;
}

void ByyEnumParameter::setValue( const QString& val )
{
	__super::setValue(val);
	myValue=val;
}

ByyPathParameter::ByyPathParameter()
{

}

ByyPathParameter::ByyPathParameter(const ByyPathParameter& orig)
{
	Name=orig.Name;
	Tag=orig.Tag;
	Type=orig.Type;
	Description=orig.Description;
	Group=orig.Group;
	myDefValue=orig.myDefValue;
	myValue=orig.myValue;
	IsCanDesign=orig.IsCanDesign;
	FileSuffix=orig.FileSuffix;
}

ByyPathParameter::~ByyPathParameter()
{

}

QString ByyPathParameter::getValue()
{
	return myValue;
}

void ByyPathParameter::setValue(const QString& val)
{
	__super::setValue(val);
	myValue=val;
}

ByyParameter* ByyPathParameter::clone()
{
	ByyPathParameter * param = new ByyPathParameter(*this);
	param->updataUUID();
	return param;
}

void ByyPathParameter::fromConfig(QDomElement& element)
{
	__super::fromConfig(element);
	FileSuffix=element.attribute("FileSuffix");
}

QString ByyPathParameter::getFileSuffix()
{
	return FileSuffix;
}

