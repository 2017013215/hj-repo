#include "ByyRelativePath.h"

ByyRelativePath::ByyRelativePath()
{

}

ByyRelativePath::~ByyRelativePath()
{

}

ByyRelativePath& ByyRelativePath::instance()
{
	static ByyRelativePath obj;
	return obj;
}

void ByyRelativePath::setCurrentPath( const QString& path )
{
	myCurrentDir.setPath(path);
}

QString ByyRelativePath::resolvePath( const QString& path )
{
	return QDir::cleanPath(myCurrentDir.absoluteFilePath(path));
}
