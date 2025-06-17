#ifndef BYYRELATIVEPATH_H
#define BYYRELATIVEPATH_H

#include <QDir>
#include <QString>
#include "snigcore_global.h"

#define myRelativePath ByyRelativePath::instance()

class SNIGCORE_EXPORT ByyRelativePath
{
public:
	~ByyRelativePath();

	static ByyRelativePath& instance();

	void setCurrentPath(const QString& path);

	QString resolvePath(const QString& path);

private:
	ByyRelativePath();

protected:
	QDir myCurrentDir;
};

#endif // BYYRELATIVEPATH_H
