#include "StdAfx.h"
#include "ByyDisplay.h"
#include "ByyDisplaySignaler.h"
#include "ByyOsgWindow.h"
#include "ScaleBar.h"

void ByyDisplay::tryUpdateNet()
{
	scaleInstance()->updateNetVisible();
}
ByyDisplay::ByyDisplay( ByyIG& ig,QObject *parent/*=0*/ )
	: QObject(parent)
	,myIG(ig)
{
	connect(&timer,SIGNAL(timeout()),this,SLOT(tryUpdateNet()));
	timer.start(50);
}

ByyDisplay::~ByyDisplay()
{
	myCompositeViewer->setDone(true);
	myCompositeViewer=0;
}

void ByyDisplay::initiate()
{
	myCompositeViewer=new osgViewer::CompositeViewer;
	myCompositeViewer->setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);
	myCompositeViewer->setKeyEventSetsDone(0);

	//addWindow();
}

void ByyDisplay::tick()
{
	myCompositeViewer->frame();
	//d_controlCanvas->myBulidingInfoImage->setFrameLastRendered(d_viewer->getFrameStamp());

	/*double fovy,aspectRatio,zNear,zFar;

	myCompositeViewer->getView(0)->getCamera()->getProjectionMatrixAsPerspective(fovy,aspectRatio,zNear,zFar);

	osg::Vec3f eye,center,up;

	myCompositeViewer->getView(0)->getCamera()->getViewMatrixAsLookAt(eye,center,up);

	osg::EllipsoidModel em;
	double latitude,  longitude,  height;
	em.convertXYZToLatLongHeight(eye.x(),eye.y(),eye.z(),latitude,longitude,height);
		
	printf("%f,%f,%f\n",height,zNear,zFar);*/
}

ByyOsgWindow* ByyDisplay::addWindow()
{
	ByyOsgWindow* sharedWindow=0;

	if (myWindowList.size()>0)
	{
		sharedWindow=myWindowList.first();
	}

	static int index=1;

	QString windowName=QString::fromLocal8Bit("´°¿Ú %1").arg(index++);

	ByyOsgWindow* osgWindow=new ByyOsgWindow(myIG,windowName,sharedWindow);
	myWindowList.append(osgWindow);

	ByyDisplaySignaler::instance()->emitWindowCreated(osgWindow);

	return osgWindow;
}

void ByyDisplay::destroyWindow( ByyOsgWindow* window )
{
	for (int i=0;i!=myWindowList.size();++i)
	{
		if (myWindowList[i]==window)
		{
			ByyDisplaySignaler::instance()->emitWindowToBeDestroyed(window);
			delete window;
			myWindowList.removeAt(i);
			break;
		}
	}
}

osgViewer::CompositeViewer* ByyDisplay::getViewer()
{
	return myCompositeViewer;
}

ByyDisplay::WindowList& ByyDisplay::windows()
{
	return myWindowList;
}

osgViewer::ViewerBase::Views ByyDisplay::getViews()
{
	osgViewer::ViewerBase::Views rel;
	myCompositeViewer->getViews(rel);
	return rel;
}
