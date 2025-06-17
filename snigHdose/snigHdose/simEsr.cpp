#include "simEsr.h"
#include <QDomElement>

ByySimEsr::ByySimEsr()
{

}

ByySimEsr::~ByySimEsr()
{

}

void ByySimEsr::fromConfig( QDomElement& element )
{
	if (element.tagName()!="SimESR")
		return;

	ItemClass=element.attribute("ItemClass");
	Tag=element.attribute("Tag");
	Description=element.attribute("Description");
	OPDPara=element.attribute("OPDPara");
	SCNPara=element.attribute("SCNPara");
}
