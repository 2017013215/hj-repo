#include "StdAfx.h"
#include "osgFileCache.h"
#include <osgDB/WriteFile>

ByyOsgFileCache::ByyOsgFileCache()
{

}

ByyOsgFileCache::~ByyOsgFileCache()
{

}

osg::Node* ByyOsgFileCache::readNodeFile( const QString& filename,const osg::CopyOp & copyop /*= osg::CopyOp::SHALLOW_COPY*/ )
{
	//printf("%s\n",filename.toLocal8Bit().data());
	NodeCache::iterator it=myNodeCache.find(filename);

	if (it!=myNodeCache.end())
	{
		return osg::clone(it.value().get(),copyop);
	}

	QFileInfo fileInfo(filename);

	QString suffix=fileInfo.suffix().toLower();

	osg::Node* node=0;

	if (suffix!="ive"&&suffix!="osgb")
	{
		QByteArray md5=QCryptographicHash::hash(filename.toLocal8Bit(),QCryptographicHash::Md5);

		QString cacheFilename=QString("../data/cache/model/%1_%2.osgb").arg(fileInfo.fileName()).arg(QString(md5.toHex()));

		node=osgDB::readNodeFile(cacheFilename.toStdString());

		if(node==NULL)
		{
			node=osgDB::readNodeFile(filename.toStdString());

			if (node!=NULL)
			{
				QDir dir("../data/cache/model/");

				if(!dir.exists())
				{
					dir.mkpath(".");
				}

				osgDB::writeNodeFile(*node,cacheFilename.toStdString());
			}
		}
	}
	
	if (node==NULL)
	{
		node=osgDB::readNodeFile(filename.toStdString());
	}

	osgEarth::Registry::shaderGenerator().run(node);

	myNodeCache.insert(filename,node);

	return node;
}

osg::Image* ByyOsgFileCache::readImageFile( const QString& filename )
{
	ImageCache::iterator it=myImageCache.find(filename);

	if (it!=myImageCache.end())
	{
		return osg::clone(it.value().get());
	}

	osg::Image* image=osgDB::readImageFile(filename.toStdString());

	myImageCache.insert(filename,image);

	return image;
}
