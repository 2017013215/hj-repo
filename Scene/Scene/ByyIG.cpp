#include "StdAfx.h"
#include "ByyIG.h"

#include "ByyDisplay.h"
#include "ByyInputController.h"
#include "sceneRadioCommsManager.h"
#include "sceneObjectManager.h"
#include "osgFileCache.h"
#include "terrain.h"
#include "ByyRenderer.h"
#include "prop.h"
#include "ByyEnvironment.h"
#include "soundManager.h"
#include "observerSettings.h"
#include "tacticalGraphicsProcesser.h"
#include "ByyMeasureProcesser.h"
#include "sceneObjectRegistration.h"
#include "entityLevelDisplayManager.h"
#include "ByyObserver.h"
#include "ByyChannel.h"
#include "ByyLogCanvas.h"
#include "coreUtils.h"
#include "ByyOsgWindow.h"

#include <QGLWidget>
#include <osgQt/GraphicsWindowQt>

ByyIG* igInstance=0;
ByyIG* ByyIG::IGInstance()
{
	return igInstance;
}

ByyIG::ByyIG( ByyCore& committee)
	:myCgf(committee)
	,myOsgFileCache(0)
	,mySceneObjManager(0)
	,myRenderer(0)
	,myProp(0)
	,myEnvironment(0)
	,myTacticalGraphicsProcesser(0)
	,myLogCanvas(0)
{
	igInstance=this;
	osg::DisplaySettings::instance()->setMinimumNumStencilBits(8);
	osg::DisplaySettings::instance()->setNumMultiSamples(8);

	myDisplayEngine=new ByyDisplay(*this);
	myInputController=new ByyInputController(*this);
	myRenderer=new ByyOsgRenderer;

	myOsgFileCache=new ByyOsgFileCache;
	myTerrain=new ByyTerrain;
	myEnvironment=new ByyEnvironment(*this);

	connect(&timer,SIGNAL(timeout()),this,SLOT(tick()));
}

ByyIG::~ByyIG()
{
	ByyDELETE(myEntityLevelDisplayManager);
	ByyDELETE(myRadioCommsManager);
	ByyDELETE(myMeasureProcesser);
	ByyDELETE(myTacticalGraphicsProcesser);
	ByyDELETE(myProp);
	ByyDELETE(myEnvironment);
	ByyDELETE(mySceneObjManager);
	ByyDELETE(myOsgFileCache);
	ByyDELETE(myTerrain);
	ByyDELETE(myRenderer);
	ByyDELETE(mySoundManager);
	ByyDELETE(myDisplayEngine);
	ByyDELETE(myInputController);
}

void ByyIG::initiate(ByyHdose* byyhdose)
{
	myEnvironment->initiate();
	myDisplayEngine->initiate();
	myRenderer->initiate();

	myRenderer->addNodeToRoot(myTerrain->root(),ByyOsgRenderer::Terrain);

	myProp=new ByyProp(this);

	mySoundManager=new ByySoundManager(*this);

	mySceneObjManager=new ByySceneObjectManager(*this);

	myTacticalGraphicsProcesser=new ByyTacticalGraphicsProcesser(*this);

	myMeasureProcesser=new ByyMeasureProcesser(*this);

	myRadioCommsManager=new ByySceneRadioCommsManager(*this);

	myEntityLevelDisplayManager=new ByyEntityLevelDisplayManager(*this);

	registerSceneObject(*this);

	timer.start(16);

	myBlock=false;

	myHdose = byyhdose;

	// xks
	osgEarth::Config cfg;
	cfg.fromJSON(ByyObserverSettings::instance().viewPointInfo());
	osgEarth::Viewpoint vp(cfg);
	//oberverInstance()->setHomeViewpoint(vp); // 20240115×¢ÊÍµô£¬xks
}

QWidget* ByyIG::sceneWidget()
{
	return myDisplayEngine->windows().at(0)->getAdapterWidget();
}

ByyDisplay& ByyIG::display()
{
	return *myDisplayEngine;
}

ByyInputController& ByyIG::inputController()
{
	return *myInputController;
}

ByySceneObjectManager* ByyIG::sceneObjectManager()
{
	return mySceneObjManager;
}

ByyOsgFileCache& ByyIG::fileCache()
{
	return *myOsgFileCache;
}

ByyOsgRenderer* ByyIG::renderer()
{
	return myRenderer;
}

ByyTerrain* ByyIG::terrain()
{
	return myTerrain;
}

ByyCore& ByyIG::core()
{
	return myCgf;
}

ByyEnvironment& ByyIG::environment()
{
	return *myEnvironment;
}

ByySoundManager& ByyIG::soundManager()
{
	return *mySoundManager;
}

void ByyIG::tick()
{
	if (myBlock)
		return;

	emit preTick();

	myDisplayEngine->tick();
	mySoundManager->tick();

	emit postTick();
}

ByyTacticalGraphicsProcesser& ByyIG::tacticalGraphicsProcesser()
{
	return *myTacticalGraphicsProcesser;
}

ByyMeasureProcesser& ByyIG::measureProcesser()
{
	return *myMeasureProcesser;
}

void ByyIG::set2D( bool is2d )
{
	/*myInputController->currentChannel()->set2D(is2d);
	myInputController->currentObserver()->set2D(is2d);
	myEnvironment->set2D(is2d);	*/
}

ByySceneRadioCommsManager& ByyIG::radioCommsManager()
{
	return *myRadioCommsManager;
}

ByyEntityLevelDisplayManager& ByyIG::entityLevelDisplayManager()
{
	return *myEntityLevelDisplayManager;
}

void ByyIG::block(bool bl)
{
	myBlock=bl;
}

void ByyIG::initLogCanvas(QObject* src,const char* signal)
{
	if (!myLogCanvas)
	{
		myLogCanvas=new ByyLogCanvas(myInputController->currentChannel(),this);

		myLogCanvas->setSource(src,signal);
	}
	
}

ByyLogCanvas* ByyIG::logCanvas()
{
	return myLogCanvas;
}

ByyHdose& ByyIG::hdose()
{
	return *myHdose;
}

void ByyIG::createScaleBar()
{
	osgEarth::MapNode* mapNode=myTerrain->mapNode();
	osgViewer::View* view=myDisplayEngine->getViews().at(0);
	_scaleBar = new ScaleBar(mapNode, view);
	osgEarth::Util::Controls::HBox* scaleBox
		= new osgEarth::Util::Controls::HBox(osgEarth::Util::Controls::Control::ALIGN_CENTER,
		osgEarth::Util::Controls::Control::ALIGN_BOTTOM,
		osgEarth::Util::Controls::Gutter(2, 2, 2, 2), 2.0f);
	scaleBox->addControl(_scaleBar->_scaleLabel.get());
	scaleBox->addControl(_scaleBar->_scaleBar.get());
	scaleBox->setVertFill(true);
	scaleBox->setForeColor(osg::Vec4f(0, 0, 0, 0.8));
	scaleBox->setBackColor(osg::Vec4f(1, 1, 1, 0.5));

	//_controlCanvas = new osgEarth::Util::Controls::ControlCanvas();
	//node->asGroup()->addChild(g_controlCanvas);
	//_controlCanvas->addControl(scaleBox);
	ByyChannel::instance()->addToCanvas(scaleBox);
	view->addEventHandler(new ScaleBarHandler(_scaleBar));
	//_scaleBar->computeScale();
}