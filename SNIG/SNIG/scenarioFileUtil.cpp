#include "StdAfx.h"
#include "scenarioFileUtil.h"
#include "ByyApp.h"
#include "scenarioManager.h"
#include "mainWindow.h"
#include "scenarioV2.h"
#include "../snigCore/settings.h"

ByyScenarioFileUtil::ByyScenarioFileUtil()
{

}

ByyScenarioFileUtil::~ByyScenarioFileUtil()
{

}

bool ByyScenarioFileUtil::doSave(ByyApp& myApp)
{
	return myApp.hdose().scenarioManager().saveScenario();
}

bool ByyScenarioFileUtil::saveAs(ByyApp& myApp)
{
	QString fileName = QFileDialog::getSaveFileName(&myApp.mainWindow(), QString::fromLocal8Bit("保存想定"),
		ByyConfigIni::instance()->scnPath(),QString::fromLocal8Bit("scenario (*.scn)"));

	if (fileName.isEmpty())
		return false;
	else{
		myApp.hdose().scenarioManager().scenario()->setFilename(fileName);
		ByyConfigIni::instance()->setScnPath(fileName);
	}

	return doSave(myApp);
}

bool ByyScenarioFileUtil::save(ByyApp& myApp)
{
	if (myApp.hdose().scenarioManager().scenario()->filename().isEmpty())
		return saveAs(myApp);
	else 
		return doSave(myApp);
}

bool ByyScenarioFileUtil::okToContinue(ByyApp& myApp)
{
	if (myApp.hdose().scenarioManager().scenarioModified())
	{
		int r=QMessageBox::warning(&myApp.mainWindow(),QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("想定已经有改变，是否保存？"),QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);

		switch(r)
		{
		case QMessageBox::Cancel:
			return false;
		case QMessageBox::Yes:
			{
				return save(myApp);
			}
		case QMessageBox::No:
			return true;
		default:
			return true;
		}
	}
	else
		return true;
}