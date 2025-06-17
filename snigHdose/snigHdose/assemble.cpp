#include "assemble.h"
#include <QDomElement>

ByyAssemble::ByyAssemble()
	:myOpdEntry(0)
{

}

ByyAssemble::~ByyAssemble()
{

}

bool ByyAssemble::valid()
{
	return (!myItemClass.isEmpty())&&(!myType.isEmpty())&&(!myTag.isEmpty());
}

void ByyAssemble::fromConfig( QDomElement& element,QMap<QString,ByyOpdEntry>& myOpdEntrys )
{
	if (element.tagName()!="Assemble")
		return;

	myItemClass=element.attribute("ItemClass");
	myType=element.attribute("Type");
	myTag=element.attribute("Tag");
	myOpd=element.attribute("OPD");

	if (myItemClass=="CE2CEntity")
	{
		int jk=0;
		++jk;
	}

	if (!myOpd.isEmpty())
	{
		QMap<QString,ByyOpdEntry>::iterator it=myOpdEntrys.find(myOpd);

		if (it!=myOpdEntrys.end())
		{
			myOpdEntry=&it.value();
		}
	}

	QDomNodeList children=element.childNodes();

	for (int i=0;i!=children.size();++i)
	{
		if (children.item(i).nodeName()=="Assemble")
		{
			ByyAssemble child;
			child.fromConfig(children.item(i).toElement(),myOpdEntrys);
			myChildren.insert(child.myType,child);
		}
	}
}

ByyOpdEntry* ByyAssemble::getEsrOpd()
{
	QMultiMap<QString,ByyAssemble>::iterator it=myChildren.find("ESR");

	if (it!=myChildren.end())
	{
		return it.value().myOpdEntry;
	}
	else
	{
		return 0;
	}
}

QString ByyAssemble::getEsrItemClass()
{
	QMultiMap<QString,ByyAssemble>::iterator it=myChildren.find("ESR");

	if (it!=myChildren.end())
	{
		return it.value().myItemClass;
	}
	else
	{
		return QString();
	}
}

ByyEntityType ByyAssemble::getEntityType()
{
	ByyOpdEntry* opdEntry=getEsrOpd();

	if (opdEntry)
	{
		return opdEntry->getEntityType();
	}
	else
		return ByyEntityType();
}

ByyDescribeEntry ByyAssemble::toDescribeEntry()
{
	ByyDescribeEntry descEntry;
	descEntry.setItemClass(myItemClass);
	descEntry.setTag(myTag);
	descEntry.setType(myType);

	QMultiMap<QString,ByyAssemble>::iterator it=myChildren.begin();

	for (;it!=myChildren.end();++it)
	{
		ByyDescribeComponent component;
		component.setItemClass(it.value().myItemClass);
		component.setTag(it.value().myTag);
		component.setType(it.value().myType);

		if (!it.value().myOpd.isEmpty()&&it.value().myOpdEntry)
		{
			component.setParamList(*it.value().myOpdEntry);
		}

		descEntry.addComponent(component);

	}

	return descEntry;
}
