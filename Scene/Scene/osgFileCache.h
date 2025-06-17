#ifndef OSGFILECACHE_H
#define OSGFILECACHE_H

class ByyOsgFileCache
{
public:
	ByyOsgFileCache();
	~ByyOsgFileCache();

	osg::Node* readNodeFile(const QString& filename,const osg::CopyOp &  copyop = osg::CopyOp::SHALLOW_COPY);

	osg::Image* readImageFile(const QString& filename);

protected:
	typedef QMap<QString,osg::ref_ptr<osg::Node>> NodeCache;
	NodeCache myNodeCache;

	typedef QMap<QString,osg::ref_ptr<osg::Image>> ImageCache;
	ImageCache myImageCache;
};

#endif // OSGFILECACHE_H
