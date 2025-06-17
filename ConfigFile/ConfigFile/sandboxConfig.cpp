#include "sandboxConfig.h"

#include <QFile>
#include <QDomElement>
#include <QTextStream>

ByySandboxConfig::ByySandboxConfig()
{
	read();
}

ByySandboxConfig::~ByySandboxConfig()
{

}

void ByySandboxConfig::read()
{
	QFile file("../data/config/SandBox.xml");
	if (!file.open(QIODevice::ReadOnly|QIODevice::Text))
	{
		return;
	}

	QDomDocument doc("SandBox");

	if (!doc.setContent(&file)) 
	{
		file.close();
		return;
	}
	file.close();

	QDomElement docElem = doc.documentElement();

	QDomNode n = docElem.firstChild();
	while(!n.isNull()) 
	{
		QDomElement e =n.toElement();
		if(!e.isNull())
		{
			SandboxElement1 element;
			element.name=e.attribute("name");
			element.iconfilename=e.firstChildElement().text();
			element.modelDescription=e.childNodes().at(1).toElement().text();
			element.forceType=e.childNodes().at(2).toElement().text();
			element.modelType=e.lastChildElement().text();

			d_elements.append(element);
		}
		n = n.nextSibling();
	}
}

void ByySandboxConfig::write()
{
	QDomDocument doc("SandBox");
	QDomElement root = doc.createElement("SandboxElements");
	doc.appendChild(root);

	for (int i=0;i!=d_elements.size();++i)
	{
		QDomElement model = doc.createElement("SandboxElement");
		root.appendChild(model);
		model.setAttribute("name",d_elements[i].name);
		QDomElement url = doc.createElement("icon");
		model.appendChild(url);
		url.appendChild( doc.createTextNode(d_elements[i].iconfilename));
	
		QDomElement model_Description=doc.createElement("ModelDescription");
		model.appendChild(model_Description);
		model_Description.appendChild(doc.createTextNode(d_elements[i].modelDescription));
		QDomElement force_Type=doc.createElement("ForceType");
		model.appendChild(force_Type);
		force_Type.appendChild(doc.createTextNode(d_elements[i].forceType));
		QDomElement model_Type=doc.createElement("ModelType");
		model.appendChild(model_Type);
		model_Type.appendChild(doc.createTextNode(d_elements[i].modelType));
	}

	QString xml=doc.toString();

	QFile file("../data/config/SandBox.xml");
	if (!file.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate))
	{
		return;
	}
	QTextStream s(&file);
	s.setCodec("utf8");
	s<<xml;
	file.close();
}

ByySandboxConfig* ByySandboxConfig::instance()
{
	static ByySandboxConfig _instance;
	return &_instance;
}

QVector<SandboxElement1>& ByySandboxConfig::elements()
{
	return d_elements;
}

void ByySandboxConfig::add( SandboxElement1& e )
{
	d_elements.append(e);
}

void ByySandboxConfig::remove( int index )
{
	if (index>=0&&index<d_elements.count())
	{
		d_elements.remove(index);
	}
}

SandboxElement1& ByySandboxConfig::element( int index )
{
	return d_elements[index];
}

void ByySandboxConfig::setName( int index,const QString& name )
{
	d_elements[index].name=name;
}

void ByySandboxConfig::setIcon( int index,const QString& filename )
{
	d_elements[index].iconfilename=filename;
}

void ByySandboxConfig::setModelText( int index,const QString& description )
{
	d_elements[index].modelDescription=description;
}

void ByySandboxConfig::setForceType( int index,const QString& type )
{
	d_elements[index].forceType=type;
}

void ByySandboxConfig::setModelType( int index,const QString& armyType )
{
	d_elements[index].modelType=armyType;
}
