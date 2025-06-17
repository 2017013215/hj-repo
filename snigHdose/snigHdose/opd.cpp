#include "opd.h"
#include "hdoseSettings.h"

#include <QFile>
#include <QDomDocument>

ByyOpd* opdInstance=0;
ByyOpd*	ByyOpd::instance()
{
	return opdInstance;
}

ByyOpd::ByyOpd()
{
	opdInstance=this;
	load();
}

ByyOpd::~ByyOpd()
{

}

void ByyOpd::load()
{
	QDomDocument doc("mydocument");

	QString opdFile=ByyHdoseSettings::instance().getOpdFile();
	
	if (opdFile.isEmpty())
	{
		opdFile="./Hdose/Missile.opd";
	}

	QFile file(opdFile);

	if (!file.open(QIODevice::ReadOnly))
		return;
	if (!doc.setContent(&file)) {
		file.close();
		return;
	}

	file.close();

	myOpdEntrys.clear();
	myAssembles.clear();

	QDomElement docElem = doc.documentElement();

	if (docElem.tagName()!="Simulation")
		return;

	QDomElement OPDEntrys=docElem.firstChildElement("OPDEntrys");

	QDomNodeList OPDEntrysChildren=OPDEntrys.childNodes();

	for (int i=0;i!=OPDEntrysChildren.size();++i)
	{
		QDomElement entry=OPDEntrysChildren.item(i).toElement();
		if (entry.tagName()!="OPDEntry")
			continue;

		ByyOpdEntry opdEntry;
		opdEntry.fromConfig(entry);

		myOpdEntrys.insert(opdEntry.getName(),opdEntry);
	}

	QDomElement Assembles=docElem.firstChildElement("Assembles");

	QDomNodeList AssemblesChildren=Assembles.childNodes();

	for (int i=0;i!=AssemblesChildren.size();++i)
	{
		QDomElement AssembleEle=AssemblesChildren.item(i).toElement();

		ByyAssemble assemble;
		assemble.fromConfig(AssembleEle,myOpdEntrys);
		assemble.index=myAssembles.size();

		myAssembles.insert(assemble.myItemClass,assemble);
	}
}

const ByyOpd::OPDEntrys& ByyOpd::opdEntrys()
{
	return myOpdEntrys;
}

const ByyOpd::Assembles& ByyOpd::assembles()
{
	return myAssembles;
}

ByyAssemble ByyOpd::findAssemble( const QString& itemClass )
{
	ByyOpd::Assembles::iterator it=myAssembles.find(itemClass);

	if (it==myAssembles.end())
	{
		return ByyAssemble();
	}
	else
		return it.value();
}

ByyAssemble ByyOpd::findAssemble( const ByyEntityType& entType )
{
	ByyOpd::Assembles::iterator it=myAssembles.begin();

	for (;it!=myAssembles.end();++it)
	{
		if (it.value().getEntityType()==entType)
		{
			return it.value();
		}
	}

	return ByyAssemble();
}

ByyOpdEntry* ByyOpd::findOpdEntry( const ByyEntityType& entType )
{
	ByyOpd::OPDEntrys::iterator it=myOpdEntrys.begin();

	for (;it!=myOpdEntrys.end();++it)
	{
		if (it.value().getEntityType()==entType)
		{
			return &(it.value());
		}
	}

	return 0;
}

ByyOpdEntry ByyOpd::findOpdEntry( const QString& name )
{
	ByyOpd::OPDEntrys::iterator it=myOpdEntrys.find(name);

	if (it==myOpdEntrys.end())
	{
		return ByyOpdEntry();
	}
	else
		return it.value();
}