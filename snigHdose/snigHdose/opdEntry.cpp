#include "opdEntry.h"
#include <QDomElement>

ByyOpdEntry::ByyOpdEntry()
{

}

ByyOpdEntry::~ByyOpdEntry()
{

}

void ByyOpdEntry::fromConfig( QDomElement& element )
{
	if (element.tagName()!="OPDEntry")
		return;

	setName(element.attribute("Name"));

	QDomNodeList Parameters=element.elementsByTagName("Parameter");

	for (int j=0;j!=Parameters.size();++j)
	{
		QDomElement p=Parameters.item(j).toElement();

		addParameter(p.attribute("Name"),p.attribute("Value"));

		if (p.attribute("Name")=="OpdEntityType" || p.attribute("Name")=="EntityType")  // xks
		{
			QString values=p.attribute("Value").replace(".",":");

			myEntityType=ByyEntityType(values.toStdString().c_str());
		}
	}
}

const ByyEntityType& ByyOpdEntry::getEntityType()
{
	return myEntityType;
}
