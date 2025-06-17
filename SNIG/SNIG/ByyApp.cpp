#include "StdAfx.h"
#include "ByyApp.h"
#include "ByyHdose.h"
#include "pluginManager.h"
#include "mainWindow.h"
#include "../snigCore/ByyPluginSystem.h"
#include "../snigCore/settings.h"
#include "../snigCore/coreUtils.h"

/*
ByyHdose* getHdose(ByyCore& core)
{
#ifdef _DEBUG
	QLibrary lib("snigHdosed.dll",QDir::Files);
#else
	QLibrary lib("snigHdose.dll",QDir::Files);
#endif // _DEBUG

	typedef ByyHdose* (*HdoseCreatorFunction)(ByyCore& core);

	if (lib.load())
	{
		HdoseCreatorFunction func=(HdoseCreatorFunction)lib.resolve("createHdose");

		if (func)
		{
			ByyHdose* hdose=(*func)(core);

			return hdose;
		}
	}
	else
		return 0;
}*/

ByyApp::ByyApp()
{

}

ByyApp::~ByyApp()
{
	delete _pluginSystem;
	delete myMainWindow;
	delete myIG;
	delete myHdose;
	delete myCore;
}

void ByyApp::initialize()
{
	myCore=new ByyCore;
	myCore->initialize();

	myHdose=new ByyHdose(*myCore);
	//myHdose->start();

	ByyPluginManager pluginManager(*myCore);

	pluginManager.initializePlugins();

	myIG=new ByyIG(*myCore);
	myIG->initiate(myHdose);

	myMainWindow=new ByyMainWindow(*this);


	//QToolBar* bar=frameWnd()->addToolBar("test");
	//bar->addAction("test",myIG,SLOT(onTest()));


	_pluginSystem=new ByyPluginSystem;
	_pluginSystem->load(ByyConfigIni::instance()->plugins());
}

ByyCore& ByyApp::core()
{
	return *myCore;
}

ByyHdose& ByyApp::hdose()
{
	return *myHdose;
}

ByyIG& ByyApp::ig()
{
	return *myIG;
}

ByyMainWindow& ByyApp::mainWindow()
{
	return *myMainWindow;
}

bool ByyApp::hdoseValid()
{
	return myHdose!=0;
}