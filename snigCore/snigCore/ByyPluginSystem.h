#pragma once

#include <QtCore>


#include "snigcore_global.h"

class ByyPluginInterface;
class SNIGCORE_EXPORT ByyPluginSystem :	public QObject
{
public:
	ByyPluginSystem(void);
	~ByyPluginSystem(void);

	void	load(QStringList plugins);
	void	close();
protected:
	QList<ByyPluginInterface*>	itfs;
};

