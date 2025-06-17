#include "StdAfx.h"
#include "ByyChannel.h"
#include "channelCompass.h"
#include "ByyIG.h"
#include "ByyChannel.h"
#include "ByyObserver.h"
#include "clipPlaneCullCallback.h"
#include "ByyEventProcessor.h"
#include "ByyOsgWindow.h"
#include "ByyRenderer.h"
#include "ByyDisplay.h"
#include "ByyInputController.h"
#include "observerSettings.h"

ByyChannel* channelObj=0;
ByyChannel* ByyChannel::instance()
{
	return channelObj;
}
ByyChannel::ByyChannel( ByyIG& ig,ByyOsgWindow& window,const QString& name ) 
	:myIG(ig)
	,myName(name)
	,myObserver(0)
	,myWindow(window)
{
	channelObj=this;
	myView=new osgViewer::View;

	myView->addEventHandler(new osgViewer::StatsHandler);
	myView->addEventHandler(new osgViewer::LODScaleHandler);
	myView->addEventHandler(new osgGA::StateSetManipulator(myView->getCamera()->getStateSet()));

	myView->getDatabasePager()->setUnrefImageDataAfterApplyPolicy(false,false);

	osg::Camera *camera=myView->getCamera();
	camera->setClearColor(osg::Vec4(0,0,0,1));

	camera->setSmallFeatureCullingPixelSize(-1.0f);
	camera->setNearFarRatio(0.0001);
	camera->setClampProjectionMatrixCallback(new ByyClampProjectionMatrixCallback(myView->getCamera(),ig,*this));

	myChannelRoot=new osg::Group;

	myCanvas=new osgEarth::Util::Controls::ControlCanvas;
	myCanvas->setComputeNearFarMode(osg::Camera::DO_NOT_COMPUTE_NEAR_FAR);

	ByyObserver* obeserver=myIG.inputController().currentObserver();
	
	myIG.inputController().setCurrentChannel(this);

	myView->addEventHandler(new ByyChannelCompass(*this,*obeserver));

	myChannelRoot->addChild(myCanvas);

	myView->setSceneData(myChannelRoot);

	myChannelRoot->addChild(myIG.renderer()->getSceneRoot());

	//myView->getCamera()->addCullCallback(new ByyClipPlaneCullCallback);

	myView->setCameraManipulator(obeserver->manip());

	camera->setGraphicsContext(window.getGraphicsContext());

	camera->setViewport(new osg::Viewport(0,0,window.width(),window.height()));

	camera->setProjectionMatrixAsPerspective(30.0f,
		static_cast<double>(window.width())/static_cast<double>(window.height()),
		1.0f, 10000.0f );

	myIG.display().getViewer()->addView(myView);

	if (ByyObserverSettings::instance().is2D())
	{
		QTimer::singleShot(1000,this,SLOT(set2D()));
	}

	connect(&ByyObserverSettings::instance(),SIGNAL(projectionChanged(bool)),this,SLOT(set2D(bool)));
}

ByyChannel::~ByyChannel()
{
	myIG.display().getViewer()->removeView(myView);
}

void ByyChannel::initiate()
{
	
}

osgViewer::View* ByyChannel::view()
{
	return myView;
}

void ByyChannel::setViewport( int x,int y,int width,int height )
{
	myView->getCamera()->setViewport(x,y,width,height);

	double fovy,aspectRatio,zNear,zFar;

	myView->getCamera()->getProjectionMatrixAsPerspective(fovy,aspectRatio,zNear,zFar);

	myView->getCamera()->setProjectionMatrixAsPerspective(fovy,static_cast<double>(width)/static_cast<double>(height)
		,zNear,zFar);
}

void ByyChannel::addToScene( osg::Node* node )
{
	myChannelRoot->addChild(node);
}

void ByyChannel::removeFromScene( osg::Node* node )
{
	myChannelRoot->removeChild(node);
}

void ByyChannel::addToCanvas( osgEarth::Util::Controls::Control* c )
{
	myCanvas->addControl(c);
}

void ByyChannel::addEventHandler( osgGA::GUIEventHandler* handler )
{
	myView->addEventHandler(handler);
}

void ByyChannel::removeEventHandler( osgGA::GUIEventHandler* handler )
{
	myView->removeEventHandler(handler);
}

ByySnapshoot* ByyChannel::snapshoot()
{
	if (!mySnapshoot.valid())
	{
		mySnapshoot=new ByySnapshoot;
		myView->getCamera()->setFinalDrawCallback(mySnapshoot);
	}
	return mySnapshoot;
}

osg::Vec3d ByyChannel::worldToScreen( const osg::Vec3d& world,bool invertY/*=false*/ )
{
	osg::Camera* cam=myView->getCamera();

	osg::Vec3d rel= world*(cam->getViewMatrix()*cam->getProjectionMatrix()*cam->getViewport()->computeWindowMatrix());

	if (invertY)
	{
		rel.y()=cam->getViewport()->height()-rel.y();
	}

	return rel;
}

void ByyChannel::set2D( bool is2d )
{
	if (is2d)
	{
		myView->getCamera()->setProjectionMatrixAsOrtho(-1, 1, -1, 1, 1, 10000);
	}
	else
	{
		osgViewer::GraphicsWindow* gw=dynamic_cast<osgViewer::GraphicsWindow*>(myView->getCamera()->getGraphicsContext());

		if (gw)
		{
			int x,y,w,h;

			gw->getWindowRectangle(x,y,w,h);

			myView->getCamera()->setProjectionMatrixAsPerspective(30,w*1.0/h,1,10000);
		}
	}
}

void ByyChannel::setObserver( ByyObserver* observer )
{
	myView->setCameraManipulator(observer->manip());
	myObserver=observer;
}

ByyObserver* ByyChannel::getObserver()
{
	return myObserver;
}

const QString& ByyChannel::getName()
{
	return myName;
}

ByyOsgWindow& ByyChannel::window()
{
	return myWindow;
}
