#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include "snigcore_global.h"

class ByyCore;

class SNIGCORE_EXPORT ByyPluginManager : public QObject
{
public:
	ByyPluginManager(ByyCore& core,QObject *parent=0);
	virtual ~ByyPluginManager();

	void initializePlugins();

protected:
	ByyCore& myCore;
};

#endif // PLUGINMANAGER_H
