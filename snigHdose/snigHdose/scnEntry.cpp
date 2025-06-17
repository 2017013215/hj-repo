#include "scnEntry.h"

#include <QDomElement>

ByyScnEntry::ByyScnEntry()
{

}

ByyScnEntry::~ByyScnEntry()
{
	
}

void ByyScnEntry::fromConfig( QDomElement& element )
{
	if (element.tagName()!="SCNEntry")
		return;

	myName=element.attribute("Name");
	myParent=element.attribute("parent");

	QDomNodeList Parameters=element.elementsByTagName("Parameter");

	for (int j=0;j!=Parameters.size();++j)
	{
		myParams.append(ByyParameterFactory::create(Parameters.item(j).toElement()));
	}
}