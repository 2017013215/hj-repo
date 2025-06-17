#include "rsm.h"
#include "hdoseSettings.h"
#include <QFile>
#include <QDomDocument>
#include "parameter.h"

ByyRsm::ByyRsm()
{
	load();
}

ByyRsm::~ByyRsm()
{
	
}

void ByyRsm::load()
{
	QDomDocument doc("mydocument");

	QString rsmFile=ByyHdoseSettings::instance().getRsmFile();

	if (rsmFile.isEmpty())
	{
		rsmFile="./Hdose/RepSimModels.rsm";
	}

	QFile file(rsmFile);

	if (!file.open(QIODevice::ReadOnly))
		return;
	if (!doc.setContent(&file)) {
		file.close();
		return;
	}

	file.close();

	myScnEntrys.clear();

	QDomElement docElem = doc.documentElement();

	if (docElem.tagName()!="SimRep")
		return;

	{
		QDomElement scnEntrys=docElem.firstChildElement("SCNEntrys");

		QDomNodeList scnEntrysChildren=scnEntrys.childNodes();

		for (int i=0;i!=scnEntrysChildren.size();++i)
		{
			QDomElement entry=scnEntrysChildren.item(i).toElement();
			if (entry.tagName()!="SCNEntry")
				continue;

			ByyScnEntry scnEntry;
			scnEntry.fromConfig(entry);

			myScnEntrys.insert(scnEntry.myName,scnEntry);
		}
	}

	{
		QDomElement SimESRs=docElem.firstChildElement("SimESRs");

		QDomNodeList SimESRsChildren=SimESRs.childNodes();

		for (int i=0;i!=SimESRsChildren.size();++i)
		{
			QDomElement entry=SimESRsChildren.item(i).toElement();
			if (entry.tagName()!="SimESR")
				continue;

			ByySimEsr simEsr;
			simEsr.fromConfig(entry);

			mySimEsrs.insert(simEsr.ItemClass,simEsr);
		}
	}
}

ParameterList ByyRsm::getParametersFromScnEntry( const QString& scnEntry )
{
	ParameterList result;

	if (myScnEntrys.contains(scnEntry))
	{
		result=getParametersFromScnEntry(myScnEntrys[scnEntry].myParent);
		result+=myScnEntrys[scnEntry].myParams;
	}
	
	return result;
}

ParameterList ByyRsm::getParametersFromEsr( const QString& esrItemClass )
{
	if (mySimEsrs.contains(esrItemClass))
	{
		return getParametersFromScnEntry(mySimEsrs[esrItemClass].SCNPara);
	}
	else
		return ParameterList();
}
