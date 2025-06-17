#ifndef SANDTABLECONFIG_H
#define SANDTABLECONFIG_H

#include <QVector>
#include <QString>

#include "configfile_global.h"

struct SandboxElement1
{
	QString name;
	QString iconfilename;
	QString modelDescription;
	QString forceType;
	QString modelType;
};

class CONFIGFILE_EXPORT ByySandboxConfig
{
public:
	ByySandboxConfig();
	~ByySandboxConfig();

	static ByySandboxConfig* instance();

	void read();
	void write();

	QVector<SandboxElement1>& elements();
	SandboxElement1& element(int index);

	void add(SandboxElement1& e);
	void remove(int index);

	void setName(int index,const QString& name);
	void setIcon(int index,const QString& filename);
	void setModelText(int index,const QString& description);
	void setForceType(int index,const QString& type);
	void setModelType(int index,const QString& armyType);

private:
	QVector<SandboxElement1> d_elements;
};

#endif // SANDTABLECONFIG_H
