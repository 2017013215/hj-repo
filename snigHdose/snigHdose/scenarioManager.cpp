#include "scenarioManager.h"
#include "scenarioV2.h"
#include "ByyCore.h"
#include "ByyHdose.h"
#include "localObjectManager.h"
#include "ByyUserOperateLog.h"

ByyScenarioManager::ByyScenarioManager( ByyHdose& hdose,QObject *parent/*=0*/ ) : QObject(parent)
	,myScenario(0)
	,myCore(hdose.core())
	,myHdose(hdose)
{
	connect(this,SIGNAL(signal_scenarioCreated(ByyScenarioV2*)),this,SIGNAL(signal_scenarioChanged()));
	connect(this,SIGNAL(signal_scenarioLoaded(ByyScenarioV2*)),this,SIGNAL(signal_scenarioChanged()));
	connect(this,SIGNAL(signal_scenarioClosed()),this,SIGNAL(signal_scenarioChanged()));
	connect(this,SIGNAL(signal_scenarioSaved()),this,SIGNAL(signal_scenarioChanged()));

	connect(&myCore.localObjectManager(),SIGNAL(entityAppend(ByyEntityObject*)),this,SLOT(setScenarioModified()));
	connect(&myCore.localObjectManager(),SIGNAL(entityRemoved(ByyEntityObject*)),this,SLOT(setScenarioModified()));
	connect(&myCore.localObjectManager(),SIGNAL(contrlObjectAppend(ByyControlObject*)),this,SLOT(setScenarioModified()));
	connect(&myCore.localObjectManager(),SIGNAL(contrlObjectRemoved(ByyControlObject*)),this,SLOT(setScenarioModified()));
}

ByyScenarioManager::~ByyScenarioManager()
{
	closeScenario();
}

ByyScenarioV2* ByyScenarioManager::scenario()
{
	return myScenario;
}

bool ByyScenarioManager::newScenario()
{
	if (myScenario)
	{
		closeScenario();
	}

	myCore.localObjectManager().resetIdHash();
	myCore.localObjectManager().resetNameHash();

	myScenario=new ByyScenarioV2(myHdose);

	emit signal_scenarioCreated(myScenario);


	ByyUserOperateLog::instance().addLog(QString::fromLocal8Bit("新建想定"));

	return true;
}

bool ByyScenarioManager::loadScenario(const QString& filename)
{
	if (!ByyScenarioV2::valid(filename))
		return false;

	if (myScenario)
	{
		closeScenario();
	}

	myCore.localObjectManager().resetIdHash();
	myCore.localObjectManager().resetNameHash();

	myScenario=new ByyScenarioV2(myHdose);

	blockSignals(true);

	if (myScenario->load(filename))
	{
		blockSignals(false);

		emit signal_scenarioLoaded(myScenario);

		return true;
	}
	else
	{
		blockSignals(false);

		delete myScenario;
		myScenario=0;

		return false;
	}

	ByyUserOperateLog::instance().addLog(QString::fromLocal8Bit("加载想定"));
}

bool ByyScenarioManager::saveScenario()
{
	if (myScenario->save())
	{
		emit signal_scenarioSaved();
		return true;
	}
	else
		return false;
}

bool ByyScenarioManager::saveAsScenario( const QString& filename )
{
	myScenario->setFilename(filename);

	return saveScenario();
}

bool ByyScenarioManager::closeScenario()
{
	if (myScenario)
	{
		delete myScenario;
		myScenario=0;

		myCore.localObjectManager().clear();

		myCore.localObjectManager().resetIdHash();
		myCore.localObjectManager().resetNameHash();

		emit signal_scenarioClosed();
	}

	return true;
}

bool ByyScenarioManager::scenarioModified()
{
	if (myScenario)
		return myScenario->modified();
	else
		return false;
}

void ByyScenarioManager::setScenarioModified( bool modified/*=true*/ )
{
	if(myScenario)
	{
		myScenario->setModified(modified);

		emit signal_modifiedChanged(modified);
	}
}

bool ByyScenarioManager::reload()
{
	if (myScenario)
	{
		QString filename=myScenario->filename();

		closeScenario();

		loadScenario(filename);

		return true;
	}

	return false;
}
