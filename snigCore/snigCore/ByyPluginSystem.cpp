#include "ByyPluginSystem.h"
#include "ByyPluginInterface.h"

ByyPluginSystem::ByyPluginSystem(void)
{
}


ByyPluginSystem::~ByyPluginSystem(void)
{
	close();
}

void ByyPluginSystem::load(QStringList plugins)
{
	foreach(QString plugin,plugins)
	{
		QFileInfo fi(plugin);
		QString oldpath=QDir::currentPath();
		QString file=fi.fileName();
		QString dir=fi.absolutePath();
		QDir::setCurrent(dir);
		QPluginLoader loader(file);
		bool ret=loader.load();
		
		QDir::setCurrent(oldpath);
		if(ret)
		{
			ByyPluginInterface* itf=qobject_cast<ByyPluginInterface*>(loader.instance());
			itf->ready();
			itfs.append(itf);
		}
	}
}

void ByyPluginSystem::close()
{
	foreach(ByyPluginInterface* itf,itfs)
	{
		itf->close();
	}
}