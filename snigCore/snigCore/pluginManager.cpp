#include "pluginManager.h"
#include "ByyCore.h"
#include "driverManager.h"

#include <QDir>
#include <QDirIterator>
#include <QLibrary>

ByyPluginManager::ByyPluginManager(ByyCore& core,QObject *parent)
	: QObject(parent)
	,myCore(core)
{

}

ByyPluginManager::~ByyPluginManager()
{

}

void ByyPluginManager::initializePlugins()
{
#ifdef _DEBUG
	QDirIterator dir("../plugins/Debug/",QDir::Files);
#else
	QDirIterator dir("../plugins/Release/",QDir::Files);
#endif // _DEBUG

	typedef ByyDriver* (*DriverCreatorFunction)(ByyRemoteObjectManager& objectManager);

	QLibrary lib;

	while (dir.hasNext()) 
	{
		dir.next();

		if (dir.fileInfo().suffix()!="dll")
		{
			continue;
		}

		lib.setFileName(dir.filePath());

		if (lib.load())
		{
			DriverCreatorFunction func=(DriverCreatorFunction)lib.resolve("createDriver");

			if (func)
			{
				ByyDriver* driver=(*func)(myCore.remoteObjectManager());

				if (driver)
				{
					myCore.driverManager().addDriver(driver);
				}
			}
		}
		else
		{
			QString str;
			str=lib.errorString();
			printf(str.toLocal8Bit().data());
			printf("\n");
		}
	}
}
