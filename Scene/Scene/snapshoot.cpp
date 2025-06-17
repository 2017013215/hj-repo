#include "stdafx.h"
#include "snapshoot.h"

#include <opencv2/highgui/highgui.hpp>

 class MyVideoWriter: public QThread
 {
 public:
	 MyVideoWriter(const std::string& filename,int w,int h, QQueue<osg::Image*> &_usedImages, QQueue<osg::Image*> &_unusedImages,QMutex& use,QMutex& unuse)
		 :usedImages(_usedImages),unusedImages(_unusedImages),_stoped(true),d_recordWidth(w),d_recordHeight(h),usedMutex(use),unusedMutex(unuse)
	 {
		 _videoWriter=cvCreateVideoWriter(filename.c_str(), CV_FOURCC('D', 'I', 'V', '3')  , 25.0, cvSize(w, h));
	 }
	 ~MyVideoWriter()
	 {
		 cvReleaseVideoWriter(&_videoWriter);
	 }
	 void start()
	 {
		 _stoped=false;
		 QThread::start(LowestPriority);
	 }

	 void stop()
	 {
		 _stoped=true;
	 }

	 void run()
	 {
		 while (!_stoped)
		 {
			if (!usedImages.isEmpty())
			{
				write();
				msleep(20);
			}
		 }

		 while(!usedImages.isEmpty())
		 {
			 write();
		 }

		 _stoped=true;
	 }

	 void write()
	 {
		 if (usedImages.isEmpty())
		 {
			 return;
		 }

		 usedMutex.lock();
		 osg::Image *img=usedImages.dequeue();
		 usedMutex.unlock();
		 if (img)
		 {
			 IplImage* _cvImg = cvCreateImageHeader(cvSize(d_recordWidth,d_recordHeight),8,3);
			 cvSetData(_cvImg,img->data(),3*d_recordWidth);
			 cvFlip(_cvImg,_cvImg,0);
			 cvWriteFrame(_videoWriter,_cvImg);
			 unusedMutex.lock();
			 
			 if (unusedImages.size()<20)
			 {
				 unusedImages.enqueue(img);
			 }
			 else
			 {
				 osg::ref_ptr<osg::Image> _img=img;
			 }

			 unusedMutex.unlock();
			// qDebug()<<"usedImages"<<usedImages.size()<<"unusedImages"<<unusedImages.size();
		 }
	 }

 private:
	 CvVideoWriter* _videoWriter;
	 QQueue<osg::Image*> &usedImages,&unusedImages;

	 volatile bool _stoped;
	 int d_recordWidth,d_recordHeight;

	 QMutex &usedMutex,&unusedMutex;
 };

ByySnapshoot::ByySnapshoot()
{
	d_image=new osg::Image;

	fps=25.0;

	d_record=false;
	d_snapshoot=false;
	snapshootdown=false;
}

ByySnapshoot::~ByySnapshoot()
{
	if (d_record)
	{
		stopRecord();
	}
}

void ByySnapshoot::operator()( osg::RenderInfo &renderInfo) const
{
	if (d_snapshoot)
	{
		const osg::Viewport* viewport = renderInfo.getCurrentCamera()->getViewport();

		if (viewport && d_image.valid())
		{
			d_image->readPixels(int(viewport->x()),int(viewport->y()),int(viewport->width()),int(viewport->height()),
				GL_RGBA,
				GL_UNSIGNED_BYTE);
			osgDB::writeImageFile(*d_image, d_snapFilename);
		}

		d_snapshoot = false;
		snapshootdown=true;
	}

	if (d_record)
	{
		double t=renderInfo.getView()->getFrameStamp()->getSimulationTime();

		if (t-d_lastRecordTime<1.0/fps)
		{
			return;
		}

		d_lastRecordTime=t;

		const osg::Viewport* viewport = renderInfo.getCurrentCamera()->getViewport();

		if (viewport)
		{
			osg::Image *img=NULL;
			unusedMutex.lock();
			if (!unusedImages.isEmpty())
			{
				img=unusedImages.dequeue();
			}
			else
				img=new osg::Image();
			unusedMutex.unlock();

 			img->readPixels(int(viewport->x()),int(viewport->y()),int(d_recordWidth),int(d_recordHeight),
 				GL_BGR,
 				GL_UNSIGNED_BYTE);

			usedMutex.lock();
			usedImages.enqueue(img);
			usedMutex.unlock();
		}
	}
}

void ByySnapshoot::snapshoot( const std::string& filename )
{
	snapshootdown=false;
	d_snapFilename=filename;
	d_snapshoot=true;
}

bool ByySnapshoot::startRecord( const std::string& filename,int w,int h )
{
	if (d_record)
		return false;

	d_lastRecordTime=0;

	d_recordWidth=w%2==0?w:w+1;
	d_recordHeight=h%2==0?h:h+1;

	usedImages.clear();
	unusedImages.clear();

	for (int i=0;i!=10;++i)
	{
		osg::Image *img=new osg::Image();

		unusedImages.enqueue(img);
	}

	d_record=true;

	_videoWriter=new MyVideoWriter(filename,d_recordWidth,d_recordHeight,usedImages,unusedImages,usedMutex,unusedMutex);

	_videoWriter->start();

	return true;
}

bool ByySnapshoot::stopRecord()
{
	if (!d_record)
		return true;

	d_record=false;

	_videoWriter->stop();

	while(_videoWriter->isRunning())
	{

	}

	delete _videoWriter;

	while(!usedImages.isEmpty())
	{
		osg::ref_ptr<osg::Image> img=usedImages.dequeue();
	}
	while(!unusedImages.isEmpty())
	{
		osg::ref_ptr<osg::Image> img=unusedImages.dequeue();
	}

	return true;
}

void ByySnapshoot::pauseOrCountiue()
{
	d_record=!d_record;
}
