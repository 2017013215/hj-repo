#pragma once
#include "snigcore_global.h"
#include <QtPlugin>
class SNIGCORE_EXPORT ByyPluginInterface
{
public:
	ByyPluginInterface(void);
	~ByyPluginInterface(void);

	virtual	void	ready(){}
	virtual	void	close(){}
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(ByyPluginInterface, "ByyPluginInterface/1.0")
QT_END_NAMESPACE