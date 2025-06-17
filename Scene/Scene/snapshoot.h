#ifndef SNAPSHOOT_H
#define SNAPSHOOT_H

#include <osg/Camera>
#include "scene_global.h"

class MyVideoWriter;

class SCENE_EXPORT ByySnapshoot : public osg::Camera::DrawCallback
{
public:
	ByySnapshoot();
	~ByySnapshoot();

	virtual void operator()(osg::RenderInfo &renderInfo) const;

	void snapshoot(const std::string& filename);

	bool startRecord(const std::string& filename,int w,int h);
	bool stopRecord();
	void pauseOrCountiue();
	bool isSnapshootdown(){return snapshootdown;}
private:
	std::string                         d_snapFilename;
	mutable bool                        d_snapshoot;
	mutable osg::ref_ptr<osg::Image>    d_image;

	int				d_recordWidth;
	int				d_recordHeight;
	double fps;
	bool	d_record;
	mutable double	d_lastRecordTime;

	mutable QQueue<osg::Image*> usedImages,unusedImages;

	mutable QMutex usedMutex,unusedMutex;

	MyVideoWriter *_videoWriter;

	mutable bool	snapshootdown;
};

#endif // SNAPSHOOT_H
