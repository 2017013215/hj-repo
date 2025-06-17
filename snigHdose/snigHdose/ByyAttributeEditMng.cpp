#include "ByyAttributeEditMng.h"
#include "hdoseSettings.h"
#include <QFile>
#include <QDomDocument>

ByyAttributeEditMng::ByyAttributeEditMng()
{
	QString editTypeFile=ByyHdoseSettings::instance().getEditTypeFile();
	load(editTypeFile);
}

void ByyAttributeEditMng::clear()
{
	m_rsmEnumTypes.clear();
	m_taskEnumTypes.clear();
}

bool operator==(const AttrEditType &p1,const AttrEditType &p2)
{
	return (p1.description == p2.description) && 
		   (p1.values == p2.values);
}

ByyAttributeEditMng* ByyAttributeEditMng::instance()
{
	static ByyAttributeEditMng s_mng;
	return &s_mng;
}

void ByyAttributeEditMng::load(const QString& filepath)
{	
	QDomDocument doc("mydocument");

	QString editTypeFile=ByyHdoseSettings::instance().getEditTypeFile();

	if (editTypeFile.isEmpty())
	{
		editTypeFile="./Hdose/enumType.xml";
	}

	QFile file(editTypeFile);

	if (!file.open(QIODevice::ReadOnly))
		return;
	if (!doc.setContent(&file)) {
		file.close();
		return;
	}

	file.close();

	clear();

	QDomElement docElem = doc.documentElement();
	QString testname=docElem.tagName();
	if (docElem.tagName()!="enums")
		return;
	
	QDomNodeList editTypeChildren=docElem.childNodes();
	
	//遍历变量说明：i，j，k
	for (int i=0; i!=editTypeChildren.size(); ++i)
	{
		QDomElement forDomain=editTypeChildren.item(i).toElement();
		QString domainName = forDomain.tagName();

		if((domainName!="rsm") && (domainName!="task")) continue;

		QDomNodeList domainTypeChildren=forDomain.childNodes();
		for (int j=0; j!= domainTypeChildren.size(); ++j)
		{
			QDomElement editTypeInfo=domainTypeChildren.item(j).toElement();

			AttrEditType editType;
			QString refAttr = editTypeInfo.attribute("name");
			editType.description = editTypeInfo.attribute("description");
			editType.sourceInfo = parseSourceInfo(editTypeInfo.attribute("sourceInfo"));
			QDomNodeList domainTypeValues=editTypeInfo.childNodes();
			for (int k=0; k!=domainTypeValues.size(); ++k)
			{
				QDomElement editTypeInfo=domainTypeValues.item(k).toElement();
				editType.values.insert(editTypeInfo.attribute("name"), editTypeInfo.text());
			}		

			if(domainName=="rsm")
			{
				m_rsmEnumTypes.insert(refAttr, editType);
			}
			else if(domainName=="task")
			{
				m_taskEnumTypes.insert(refAttr, editType);
			}
		}
	}
}

bool ByyAttributeEditMng::getRsmRefAttricbeEditType(const QString& attrName, AttrEditType& eType)
{
	if(m_rsmEnumTypes.contains(attrName))
	{
		eType = m_rsmEnumTypes.value(attrName);
		return true;
	}
	return false;
}


bool ByyAttributeEditMng::getTaskRefAttricbeEditType(const QString& attrName, AttrEditType& eType)
{
	if(m_taskEnumTypes.contains(attrName))
	{
		eType = m_taskEnumTypes.value(attrName);
		return true;
	}
	return false;
}


QStringList ByyAttributeEditMng::parseSourceInfo(const QString& sourceInfo)
{
	QStringList ret;
	if(sourceInfo.contains("_"))
	{
		ret = sourceInfo.split("_");
	}
	else
	{
		ret.append(sourceInfo);
	}
	return ret;
}