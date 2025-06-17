#include "stdafx.h"
#include "mainWindow.h"
#include "layersControlWidget.h"
#include "environmentWidget.h"
#include "model.h"
#include "viewpointEditWidget.h"
#include "animationPathEditWidget.h"
#include "terrainEffectWidget.h"
#include "assembleWidget.h"
#include "scenarioV2.h"
#include "objectListWidget.h"
#include "screenRecordWidget.h"
#include "cameraAnimationWidget.h"
#include "viewpointWidget.h"
#include "sandboxWidget.h"
#include "measureWidget.h"
#include "sceneObjectManager.h"
#include "mouseCoordsWidget.h"
#include "ByyApp.h"

#include "mainMenus.h"
#include "helpActions.h"
#include "simulationActions.h"
#include "settingsActions.h"
#include "stereoSettings.h"
#include "fileActions.h"
#include "measureActions.h"
#include "scenarioFileUtil.h"
#include "showControlWidget.h"

#include "settings.h"

#include "autoHidePanel.h"
#include "autoHideDock.h"

#include "createActions.h"
#include "tacticalGraphicsWidget.h"
#include "scenarioManager.h"

#include "logOutPutWidget.h"
#include "simTimeWidget.h"

#include "ByyDisplay.h"
#include "ByyOsgWindow.h"
#include "ByyChannel.h"
#include "ByyEntityOperator.h"

#include <osgQt/GraphicsWindowQt>

#include "displayManageWidget.h"

ByyMainWindow::ByyMainWindow( ByyApp& app,QWidget *parent /*= 0*/, Qt::WFlags flags /*= 0*/ ) 
	: QMainWindow(parent, flags)
	,myApp(app)
{
	setObjectName("ByyMainWindow");

	ByyEntityOperator::instance().setApp(&app);

	QWidget *central=new QWidget;
	central->setLayout(new QVBoxLayout);
	central->layout()->setContentsMargins(0,0,0,0);
	setCentralWidget(central);

	myAutoHidePanel=new ByyAutoHidePanel(central,this);

	ByyOsgWindow* window=myApp.ig().display().addWindow();	

	QWidget* osgWidget=window->getAdapterWidget();

	osgWidget->installEventFilter(this);

	central->layout()->addWidget(osgWidget);

	createMenuToolBars();
	createDockWidgets();

	central->installEventFilter(this);

	/*ByyDisplayManageWidget *WW=new ByyDisplayManageWidget(myApp,this);
	WW->setWindowFlags(Qt::Window);
	WW->setAttribute(Qt::WA_DeleteOnClose);
	WW->show();*/

	connect(&myApp.ig().inputController(),SIGNAL(requestContextMenu()),&ByyEntityOperator::instance(),SLOT(showContextMenu()));

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(onScenarioChanged()));
	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_modifiedChanged(bool)),SLOT(setWindowModified(bool)));

	SbInit();
}

ByyMainWindow::~ByyMainWindow()
{

}

void ByyMainWindow::createMenuToolBars()
{
	recentMenu=new QMenu(QString::fromLocal8Bit("最近打开想定"));

	for (int i = 0; i < MaxRecentFiles; ++i) {
		recentFileActs[i] = new QAction(this);
		recentFileActs[i]->setVisible(false);
		connect(recentFileActs[i], SIGNAL(triggered()),
			this, SLOT(openRecentFile()));

		recentMenu->addAction(recentFileActs[i]);
	}

	updateRecentFileActions();

	QMenu* toolBarMenu=new ByySimpleMenu(QString::fromLocal8Bit("工具栏(&T)"),"ByyToolBarMenu",this);

	{
		QMenu *fileMenu=new ByyFileMenu(this);

		QAction* newScnAction=new ByyNewScenarioAction(myApp,this);
		QAction* loadScnAction=new ByyLoadScenarioAction(myApp,this);
		QAction* saveScnAction=new ByySaveScenarioAction(myApp,this);
		QAction* saveAsScnAction=new ByySaveAsScenarioAction(myApp,this);
		fileMenu->addAction(newScnAction);
		fileMenu->addAction(loadScnAction);

		fileMenu->addMenu(recentMenu);

		fileMenu->addAction(saveScnAction);
		fileMenu->addAction(saveAsScnAction);
		fileMenu->addAction(new ByyCloseScenarioAction(myApp,this));
		fileMenu->addAction(new ByySeparatorAction(myApp,this));
		fileMenu->addAction(new ByyOpenTerrainAction(myApp,this));
		fileMenu->addAction(new ByyCloseTerrainAction(myApp,this));
		fileMenu->addAction(new ByySaveTerrainAction(myApp,this));
		fileMenu->addAction(new ByySaveAsTerrainAction(myApp,this));
		fileMenu->addAction(new ByySeparatorAction(myApp,this));
		fileMenu->addAction(new ByyViewpointCreateAction(myApp,this));
		fileMenu->addAction(new ByyAnimationPathCreateAction(myApp,this));
		fileMenu->addAction(new ByyTerrainEditAction(myApp,this));
		fileMenu->addAction(new ByySeparatorAction(myApp,this));
		fileMenu->addAction(new ByySnapshootAction(myApp,this));
		fileMenu->addAction(new ByySeparatorAction(myApp,this));
		fileMenu->addAction(new ByyExitAction(myApp,this));

		menuBar()->addMenu(fileMenu);

		QToolBar* scenarioToolBar=addToolBar(QString::fromLocal8Bit("想定"));
		scenarioToolBar->addAction(newScnAction);
		scenarioToolBar->addAction(loadScnAction);
		scenarioToolBar->addAction(saveScnAction);
		scenarioToolBar->addAction(saveAsScnAction);
		toolBarMenu->addAction(scenarioToolBar->toggleViewAction());
	}
	{
		menuBar()->addMenu(new ByySimpleMenu(QString::fromLocal8Bit("视图(&V)"),"ByyViewMenu",this));
	}
	/*if (0)
	{
		QMenu *createMenu=new ByyCreateMenu(this);

		QAction* createPathAction=new ByyCreatePathAction(myApp,this);
		createMenu->addAction(createPathAction);

		QAction* createAreaAction=new ByyCreateAreaAction(myApp,this);
		createMenu->addAction(createAreaAction);

		QAction* createCircleAction=new ByyCreateCircleAction(myApp,this);
		createMenu->addAction(createCircleAction);

		menuBar()->addMenu(createMenu);

		QToolBar* createToolBar=addToolBar(QString::fromLocal8Bit("创建"));
		createToolBar->addAction(createPathAction);
		createToolBar->addAction(createAreaAction);
		createToolBar->addAction(createCircleAction);
	}*/
	{
		ByyShowControlWidget *showCtrlWidget=new ByyShowControlWidget(this);
		QDockWidget* showCtrlDockWidget=new QDockWidget(QString::fromLocal8Bit("显示控制"),this);
		showCtrlDockWidget->setWidget(showCtrlWidget);
		showCtrlDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
		addDockWidget(Qt::RightDockWidgetArea,showCtrlDockWidget);
		showCtrlDockWidget->setFloating(true);
		showCtrlDockWidget->setVisible(false);

		QMenu* settinsMenu=new ByySettingsMenu(this);
		QMenu* connectMenu=new ByySimpleMenu(QString::fromLocal8Bit("连接(&C)"),"ByyConnectMenu");
		settinsMenu->addMenu(connectMenu);

		QAction* connectSingleHdoseAction=new ByyConnectSingleHdoseAction(myApp,this);
		QAction* connectJoinHdoseAction=new ByyConnectJoinHdoseAction(myApp,this);
		QAction* connectHLAAction=new ByyConnectVRLAction(myApp,this);

		connectMenu->addAction(connectSingleHdoseAction);
		connectMenu->addAction(connectJoinHdoseAction);
		connectMenu->addAction(connectHLAAction);

		settinsMenu->addAction(new ByySeparatorAction(myApp,this));

		QMenu* stereoModeMenu=new ByyStereoModeMenu(myApp);
		settinsMenu->addMenu(stereoModeMenu);

		QActionGroup *stereoActionGroup=new QActionGroup(this);

		stereoActionGroup->addAction(new ByyQuadBufferStereoAction(myApp,this));
		stereoActionGroup->addAction(new ByyAnaglyphicStereoAction(myApp,this));
		stereoActionGroup->addAction(new ByyHSplitStereoAction(myApp,this));
		stereoActionGroup->addAction(new ByyPassivityStereoAction(myApp,this));

		stereoModeMenu->addActions(stereoActionGroup->actions());

		settinsMenu->addAction(new ByyStereoSettingsAction(myApp,this));
		settinsMenu->addAction(new ByyStereoShowAction(myApp,this));
		settinsMenu->addAction(new ByySeparatorAction(myApp,this));
		settinsMenu->addAction(new ByyAudioSettingsAction(myApp,this));
		settinsMenu->addAction(new ByyAudioAction(myApp,this));
		settinsMenu->addAction(new ByyOceanVisibleAction(myApp,this));
		settinsMenu->addAction(new ByySeparatorAction(myApp,this));

		settinsMenu->addAction(new ByyEnvironmentSettingsAction(myApp,this));
		settinsMenu->addAction(new ByyTerrainEffectAction(myApp,this));
		settinsMenu->addAction(new ByyScreenRecordAction(myApp,this));
		settinsMenu->addAction(new ByySeparatorAction(myApp,this));

		QAction* groundClampAction=new ByyGroundClampAction(myApp,this);
		settinsMenu->addAction(groundClampAction);
		settinsMenu->addAction(new ByyEntityScaleAction(myApp,this));
		settinsMenu->addAction(showCtrlDockWidget->toggleViewAction());
		//QAction* entityLabelAction=new ByyEntityLabelAction(myApp,this);
		//settinsMenu->addAction(entityLabelAction);
		//QAction *entityNameAction=new ByyEntityNameAction(myApp,this);
		//settinsMenu->addAction(entityNameAction);
		//QAction *ctrlObjAction=new ByyCtrlObjAction(myApp,this);
		//settinsMenu->addAction(ctrlObjAction);
		//QAction *trajectoryAction=new ByyTrajectoryAction(myApp,this);
		//settinsMenu->addAction(trajectoryAction);
		//QAction *entityInfoAction=new ByyEntityInfoAction(myApp,this);
		//settinsMenu->addAction(entityInfoAction);
		QAction *proj2dAction=new Byy2DProjectionAction(myApp,this);
		settinsMenu->addAction(proj2dAction);

		settinsMenu->addAction(new ByySeparatorAction(myApp,this));
		settinsMenu->addAction(new ByyHdoseSettingsAction(myApp,this));

		menuBar()->addMenu(settinsMenu);

		QToolBar* hdoseConnectToolBar=addToolBar(QString::fromLocal8Bit("连接"));
		QToolButton* hdoseToolButton=new QToolButton;
		hdoseToolButton->setPopupMode(QToolButton::InstantPopup);
		connect(hdoseToolButton,SIGNAL(triggered(QAction*)),hdoseToolButton,SLOT(setDefaultAction(QAction*)));
		hdoseToolButton->setIcon(QIcon("../data/icons/h.svg"));
		hdoseConnectToolBar->addWidget(hdoseToolButton);
		hdoseToolButton->addAction(connectSingleHdoseAction);
		hdoseToolButton->addAction(connectJoinHdoseAction);
		toolBarMenu->addAction(hdoseConnectToolBar->toggleViewAction());

		QToolBar* settingsToolBar=addToolBar(QString::fromLocal8Bit("设置"));
		
		showCtrlDockWidget->toggleViewAction()->setIcon(QIcon("../data/icons/options.svg"));
		settingsToolBar->addAction(showCtrlDockWidget->toggleViewAction());
		//settingsToolBar->addAction(entityLabelAction);
		//settingsToolBar->addAction(entityNameAction);
		//settingsToolBar->addAction(ctrlObjAction);
		//settingsToolBar->addAction(trajectoryAction);
		//settingsToolBar->addAction(entityInfoAction);
		settingsToolBar->addAction(proj2dAction);
		//settingsToolBar->addAction(groundClampAction);
		toolBarMenu->addAction(settingsToolBar->toggleViewAction());

	}
	{
		QMenu* simulationMenu=new ByySimulationMenu(this);

		QAction* rewindAction=new ByyRewindAction(myApp,this);
		QAction* startAction=new ByyStartAction(myApp,this);
		QAction* pauseAction=new ByyPauseAction(myApp,this);
		QAction* fastBestAction=new ByyFastBestAction(myApp,this);
		QAction* scnProfileSettingsAction=new ByyScnProfileSettingsAction(myApp,this);
		QAction* scnEnvironmentSettingsAction=new ByyScnEnvironmentSettingsAction(myApp,this);

		simulationMenu->addAction(rewindAction);
		simulationMenu->addAction(startAction);
		simulationMenu->addAction(pauseAction);
		simulationMenu->addAction(new ByySeparatorAction(myApp,this));
		simulationMenu->addAction(fastBestAction);
		simulationMenu->addAction(new ByySeparatorAction(myApp,this));
		simulationMenu->addAction(scnProfileSettingsAction);
		simulationMenu->addAction(scnEnvironmentSettingsAction);

		menuBar()->addMenu(simulationMenu);

		QToolBar* simuToolBar=addToolBar(QString::fromLocal8Bit("仿真"));

		simuToolBar->addAction(rewindAction);
		simuToolBar->addAction(startAction);
		simuToolBar->addAction(pauseAction);
		simuToolBar->addAction(new ByySeparatorAction(myApp,this));
		simuToolBar->addAction(fastBestAction);
		simuToolBar->addAction(new ByySeparatorAction(myApp,this));
		simuToolBar->addAction(scnProfileSettingsAction);
		simuToolBar->addAction(scnEnvironmentSettingsAction);

		toolBarMenu->addAction(simuToolBar->toggleViewAction());
		
	}
	{
		QToolBar *simTimeToolBar=addToolBar(QString::fromLocal8Bit("仿真时间"));
		simTimeToolBar->addWidget(new ByySimTimeWidget(myApp,this));

		toolBarMenu->addAction(simTimeToolBar->toggleViewAction());
	}
	{
		QMenu *measureMenu=new ByyMeasureMenu(this);

		QAction* lineAction=new ByyLineMeasureAction(myApp,this);
		QAction* areaAction=new ByyPolygonMeasureAction(myApp,this);

		measureMenu->addAction(lineAction);
		measureMenu->addAction(areaAction);

		QToolBar* measureToolBar=addToolBar(QString::fromLocal8Bit("测量"));
		measureToolBar->addAction(lineAction);
		measureToolBar->addAction(areaAction);

		menuBar()->addMenu(measureMenu);

		toolBarMenu->addAction(measureToolBar->toggleViewAction());

	}
	{
		QMenu* helpMenu=new ByyHelpMenu(this);
		helpMenu->addAction(new ByyAboutAction(myApp,this));

		helpMenu->addAction(new ByyShowDocAction(myApp,"ByyShowSNIGDocAction",
			QString::fromLocal8Bit("数字战场支持系统文档(&S)"),
			QString::fromLocal8Bit("../docs/数字战场支持系统文档.pdf"),this));

		helpMenu->addAction(new ByyShowDocAction(myApp,"ByyShowEarthToolDocAction",
			QString::fromLocal8Bit("地球工具文档(&E)"),
			QString::fromLocal8Bit("../docs/地球配置工具文档.pdf"),this));

		helpMenu->addAction(new ByyShowDocAction(myApp,"ByyShowConfigToolDocAction",
			QString::fromLocal8Bit("配置工具文档(&C)"),
			QString::fromLocal8Bit("../docs/配置工具文档.pdf"),this));

		helpMenu->addAction(new ByyShowDocAction(myApp,"ByyShowTotalConfigAction",
			QString::fromLocal8Bit("总体配置文档(&F)"),
			QString::fromLocal8Bit("../docs/框架说明文档.pdf"),this));

		helpMenu->addAction(new ByyShowDocAction(myApp,"ByyShow2DSimuAction",
			QString::fromLocal8Bit("二维态势仿真文档(&M)"),
			QString::fromLocal8Bit("../docs/仿真操作文档.pdf"),this));

		menuBar()->addMenu(helpMenu);
	}

	connect(&ByyStereoSettings::instance(),SIGNAL(enableChanged(bool)),SLOT(changeStereo(bool)));

	QList<ByyAction*> actions=findChildren<ByyAction*>();

	foreach(ByyAction* act,actions)
	{
		act->checkEnabled();
	}
}

void ByyMainWindow::createDockWidgets()
{
	QMenu* viewMenu=findChild<QMenu*>("ByyViewMenu");

	ByyAssembleWidget *assembleWidget=new ByyAssembleWidget(myApp);
	ByyAutoHideDock *assembleWidgetDockWidget=new ByyAutoHideDock(QString::fromLocal8Bit("实体面板"));
	assembleWidgetDockWidget->setObjectName("assembleWidgetDockWidget");
	assembleWidgetDockWidget->setWidget(assembleWidget);
	assembleWidgetDockWidget->toggleViewAction()->setIcon(QIcon("../data/icons/entity.png"));

	ByyAutoHideDock *tacticalGraphicsDockWidget=new ByyAutoHideDock(QString::fromLocal8Bit("战术图形"));
	ByyTacticalGraphicsWidget *tacticalGraphicsWidget=new ByyTacticalGraphicsWidget(myApp);
	tacticalGraphicsDockWidget->setObjectName("tacticalGraphicsDockWidget");
	tacticalGraphicsDockWidget->setWidget(tacticalGraphicsWidget);
	tacticalGraphicsDockWidget->toggleViewAction()->setIcon(QIcon("../data/icons/DisplayTacticalGraphics.svg"));

	ByyObjectListWidget* entObjWidget=new ByyObjectListWidget(myApp);
	QDockWidget *objListDockWidget=new QDockWidget(QString::fromLocal8Bit("对象列表"));
	objListDockWidget->setObjectName("objListDockWidget");
	objListDockWidget->setWidget(entObjWidget);
	objListDockWidget->layout()->setContentsMargins(0,0,0,0);
	objListDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea);
	addDockWidget(Qt::LeftDockWidgetArea,objListDockWidget);
	viewMenu->addAction(objListDockWidget->toggleViewAction());
	objListDockWidget->toggleViewAction()->setIcon(QIcon("../data/icons/entity.png"));

	ByyCameraAnimationWidget *cameraAnimationWidget=new ByyCameraAnimationWidget(myApp.ig(),this);
	QDockWidget *cameraAnimationDockWidget=new QDockWidget(QString::fromLocal8Bit("路径漫游"));
	cameraAnimationDockWidget->setObjectName("cameraAnimationDockWidget");
	cameraAnimationDockWidget->setWidget(cameraAnimationWidget);
	cameraAnimationDockWidget->layout()->setContentsMargins(0,0,0,0);
	cameraAnimationDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea);
	addDockWidget(Qt::LeftDockWidgetArea,cameraAnimationDockWidget);
	cameraAnimationDockWidget->setVisible(false);
	viewMenu->addAction(cameraAnimationDockWidget->toggleViewAction());
	cameraAnimationDockWidget->toggleViewAction()->setIcon(QIcon("../data/icons/cameraanimation.png"));


	ByyViewpointWidget *viewpointWidget=new ByyViewpointWidget(myApp.ig(),this);
	QDockWidget *observerViewsDockWidget=new QDockWidget(QString::fromLocal8Bit("视点切换"));
	observerViewsDockWidget->setObjectName("observerViewsDockWidget");
	observerViewsDockWidget->setWidget(viewpointWidget);
	observerViewsDockWidget->layout()->setContentsMargins(0,0,0,0);
	addDockWidget(Qt::LeftDockWidgetArea,observerViewsDockWidget);
	observerViewsDockWidget->setVisible(false);
	viewMenu->addAction(observerViewsDockWidget->toggleViewAction());
	observerViewsDockWidget->toggleViewAction()->setIcon(QIcon("../data/icons/viewpoint.png"));

	ByySandboxWidget *sandboxWidget=new ByySandboxWidget(myApp.ig(),this);
	QDockWidget *sandBoxDockWidget=new QDockWidget(QString::fromLocal8Bit("战场标绘"));
	sandBoxDockWidget->setObjectName("sandBoxDockWidget");
	sandBoxDockWidget->setWidget(sandboxWidget);
	sandBoxDockWidget->layout()->setContentsMargins(0,0,0,0);
	sandBoxDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea);
	addDockWidget(Qt::RightDockWidgetArea,sandBoxDockWidget);
	sandBoxDockWidget->setVisible(false);
	viewMenu->addAction(sandBoxDockWidget->toggleViewAction());
	sandBoxDockWidget->toggleViewAction()->setIcon(QIcon("../data/icons/sandbox.png"));

	/*ByyMeasureWidget *measureWidget=new ByyMeasureWidget(myApp.ig(),this);
	QDockWidget *measureDockWidget=new QDockWidget(QString::fromLocal8Bit("测量标注"));
	measureDockWidget->setObjectName("measureDockWidget");
	measureDockWidget->setWidget(measureWidget);
	measureDockWidget->layout()->setContentsMargins(0,0,0,0);
	addDockWidget(Qt::RightDockWidgetArea,measureDockWidget);
	measureDockWidget->setVisible(false);
	viewMenu->addAction(measureDockWidget->toggleViewAction());
	measureDockWidget->toggleViewAction()->setIcon(QIcon("../data/icons/measure.png"));*/

	ByyMouseCoordsWidget *mouseCoordsWidget=new ByyMouseCoordsWidget(myApp.ig(),this);
	QDockWidget *mouseCoordsWidgetDockWidget=new QDockWidget(QString::fromLocal8Bit("坐标拾取"));
	mouseCoordsWidgetDockWidget->setObjectName("mouseCoordsWidgetDockWidget");
	mouseCoordsWidgetDockWidget->setWidget(mouseCoordsWidget);
	mouseCoordsWidgetDockWidget->layout()->setContentsMargins(0,0,0,0);
	addDockWidget(Qt::LeftDockWidgetArea,mouseCoordsWidgetDockWidget);
	//mouseCoordsWidgetDockWidget->setVisible(false);
	viewMenu->addAction(mouseCoordsWidgetDockWidget->toggleViewAction());
	mouseCoordsWidgetDockWidget->toggleViewAction()->setIcon(QIcon("../data/icons/location.png"));

	ByyLogOutputWidget *logOutputWidget=new ByyLogOutputWidget;
	QDockWidget *logOutputDockWidget=new QDockWidget(QString::fromLocal8Bit("日志输出"),this,Qt::Window);
	logOutputDockWidget->setObjectName("logOutputDockWidget");
	logOutputDockWidget->setWidget(logOutputWidget);
	logOutputDockWidget->layout()->setContentsMargins(0,0,0,0);

	logOutputDockWidget->setVisible(false);

	viewMenu->addAction(logOutputDockWidget->toggleViewAction());

	viewMenu->addAction(new ByySeparatorAction(myApp,this));
	viewMenu->addAction(new ByyFullScreenAction(myApp,this));
/*
	ByyDockToolBar *docktoolbar=new ByyDockToolBar(this);
	docktoolbar->setObjectName("docktoolbarDockWidget");
	docktoolbar->layout()->setContentsMargins(0,0,0,0);
	addDockWidget(Qt::LeftDockWidgetArea,docktoolbar);
	docktoolbar->setVisible(false);
	ui.viewMenu->addAction(docktoolbar->toggleViewAction());*/

	/*QToolBar *rightDockToolBar=new QToolBar;
	rightDockToolBar->setAllowedAreas(Qt::RightToolBarArea);
	rightDockToolBar->setFloatable(false);
	rightDockToolBar->setMovable(false);
	rightDockToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	rightDockToolBar->setParent(centralWidget());
	rightDockToolBar->setAutoFillBackground(true);
	rightDockToolBar->show();
	//addToolBar(Qt::RightToolBarArea,rightDockToolBar);

	rightDockToolBar->addAction(assembleWidgetDockWidget->toggleViewAction());*/

	myAutoHidePanel->addDockWidget(assembleWidgetDockWidget,QString::fromLocal8Bit("实体面板"),QIcon("../data/icons/entity.png"));
	myAutoHidePanel->addDockWidget(tacticalGraphicsDockWidget,QString::fromLocal8Bit("战术图形"),QIcon("../data/icons/DisplayTacticalGraphics.svg"));
}

void ByyMainWindow::closeEvent(QCloseEvent *event)
{
	if (ByyScenarioFileUtil::okToContinue(myApp))
	{
		event->accept();
	}
	else
		event->ignore();
}

void ByyMainWindow::openRecentFile()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (!action)
		return;
	
	if (ByyScenarioFileUtil::okToContinue(myApp))
	{
		if (!myApp.hdose().scenarioManager().loadScenario(action->data().toString()))
		{
			QMessageBox::warning(&myApp.mainWindow(),QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("加载想定失败！"));
		}
	}
}

void ByyMainWindow::currentSandboxChanged( bool show,const QString& desc )
{
	/*if (!textBrowser)
	{
		textBrowser=new ByyTextBrowser(this);
		textBrowser->setAttribute(Qt::WA_ShowWithoutActivating);
	}
	if (!show)
	{
		textBrowser->close();
	}
	else if (show)
	{
		textBrowser->setColor(QColor(240,255,255,120));
		textBrowser->setText(desc);
		textBrowser->move(QCursor::pos());
		textBrowser->adjustSize();
		textBrowser->repaint();
		textBrowser->show();
	}
	else
	{
		textBrowser->close();
	}*/
}

QWidget* ByyMainWindow::createViewWidget( osgViewer::View* view,bool quadbuffer )
{
	int _width=100;
	int _height=100;

	osg::GraphicsContext::createNewContextID();

	ByyStereoSettings::instance().setEnabled(quadbuffer);

	osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();

	osg::ref_ptr<osgQt::GraphicsWindowQt::Traits> traits=new osgQt::GraphicsWindowQt::Traits(ds);

	traits->readDISPLAY();

	if (traits->displayNum<0) 
		traits->displayNum = 0;

	traits->windowDecoration=false;
	traits->x=0;
	traits->y=0;
	traits->width=_width;
	traits->height=_height;

	traits->doubleBuffer=true;

	traits->quadBufferStereo=ds->getStereo()&&ds->getStereoMode()==osg::DisplaySettings::QUAD_BUFFER;
	

	traits->alpha = ds->getMinimumNumAlphaBits();
	traits->stencil = ds->getMinimumNumStencilBits();
	traits->sampleBuffers = ds->getMultiSamples();
	traits->samples = ds->getNumMultiSamples();

	osg::ref_ptr<osgQt::GraphicsWindowQt> gw =new osgQt::GraphicsWindowQt(traits.get());

	view->getCamera()->setGraphicsContext(gw);

	view->getCamera()->setViewport(new osg::Viewport(0,0,traits->width,traits->height));

	view->getCamera()->setProjectionMatrixAsPerspective(30.0f,
		static_cast<double>(traits->width)/static_cast<double>(traits->height),
		1.0f, 10000.0f );

	return gw->getGLWidget();
}

void ByyMainWindow::showFullScreen( bool fullScreen )
{
	menuBar()->setHidden(fullScreen);

	QList<QToolBar*> toolBars=findChildren<QToolBar*>();

	foreach (QToolBar* tb,toolBars)
	{
		tb->setHidden(fullScreen);
	}

	if (fullScreen)
	{
		QMainWindow::showFullScreen();
	}
	else
	{
		showMaximized();
	}
}

void ByyMainWindow::changeStereo( bool stereo )
{
	/*osgEarth::Viewpoint vp=myApp.ig().observer().getViewpoint();
//	createGraphicsContext(stereo);

	myApp.ig().observer().setViewpoint(vp);*/
}

/*
void ByyMainWindow::createGraphicsContext(bool quadBuffer)
{
	myApp.ig().renderer()->viewer()->stopThreading();

	QWidget *graphicsWidget=createViewWidget(myApp.ig().channel()->view(),quadBuffer);

	graphicsWidget->installEventFilter(this);

	QList<QWidget*> widgets=centralWidget()->findChildren<QWidget*>();

	foreach(QWidget* w,widgets)
	{
		centralWidget()->layout()->removeWidget(w);
		w->deleteLater();
	}
	centralWidget()->layout()->addWidget(graphicsWidget);

	myApp.ig().renderer()->viewer()->startThreading();
}*/

void ByyMainWindow::showViewpointCreateWidget()
{
	ByyViewpointEditWidget *w=new ByyViewpointEditWidget(myApp.ig(),this,Qt::Window);
	w->setAttribute(Qt::WA_DeleteOnClose);
	w->show();
}

void ByyMainWindow::showAnimationPathCreateWidget()
{
	ByyAnimationPathEditWidget *w=new ByyAnimationPathEditWidget(myApp.ig(),this,Qt::Window);
	w->setAttribute(Qt::WA_DeleteOnClose);
	w->show();
}

bool ByyMainWindow::eventFilter( QObject * object, QEvent * event )
{
	if (object->parent()==centralWidget()&&event->type()==QEvent::KeyRelease)
	{
		QKeyEvent *kEvent=dynamic_cast<QKeyEvent*>(event);

		if (kEvent&&kEvent->key()==Qt::Key_Return&&kEvent->modifiers()==Qt::ControlModifier)
		{
			showFullScreen(!isFullScreen());
		}
	}

	return QMainWindow::eventFilter(object,event);
}

void ByyMainWindow::showTerrainEditWidget()
{
	ByyLayersControlWidget *layersControl=new ByyLayersControlWidget(myApp.ig(),this,Qt::Window);
	layersControl->setAttribute(Qt::WA_DeleteOnClose);
	layersControl->show();
}

void ByyMainWindow::showEnviromentWidget()
{
	if (!myEnvironmentWidget)
	{
		myEnvironmentWidget=new ByyEnvironmentWidget(myApp.ig().environment(),this);
	}

	myEnvironmentWidget->show();
}

void ByyMainWindow::showTerrainEffectWidget()
{
	if (!myTerrainEffectWidget)
	{
		myTerrainEffectWidget=new ByyTerrainEffectWidget(myApp.ig().terrain()->effect(),this);
	}
	myTerrainEffectWidget->show();
}

void ByyMainWindow::showScreenRecordWidget()
{
	if (!myScreenRecordWidget)
	{
		myScreenRecordWidget=new ByyScreenRecordWidget(myApp.ig(),this,this);
	}
	myScreenRecordWidget->show();
}

void ByyMainWindow::onScenarioChanged()
{
	ByyScenarioV2 *scn=myApp.hdose().scenarioManager().scenario();

	if (scn)
	{
		QString shownName=scn->filename();

		if (shownName.isEmpty())
		{
			shownName="Untitled";
		}
		else
		{
			addRecentFile(shownName);
		}

		setWindowModified(scn->modified());

		setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(ByyConfigIni::instance()->title()));
	}
	else
	{
		setWindowTitle(ByyConfigIni::instance()->title());
		setWindowModified(false);
	}
}


QString strippedName(const QString &fullFileName)
{
	return QFileInfo(fullFileName).fileName();
}

void ByyMainWindow::addRecentFile( const QString& filename )
{
	QSettings settings;
	QStringList files = settings.value("recentFileList").toStringList();
	files.removeAll(filename);
	files.prepend(filename);
	while (files.size() > MaxRecentFiles)
		files.removeLast();

	settings.setValue("recentFileList", files);

	updateRecentFileActions();
}

void ByyMainWindow::updateRecentFileActions()
{
	QSettings settings;
	QStringList files = settings.value("recentFileList").toStringList();

	int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

	for (int i = 0; i < numRecentFiles; ++i) {
		QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
		recentFileActs[i]->setText(text);
		recentFileActs[i]->setData(files[i]);
		recentFileActs[i]->setVisible(true);
	}
	for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
		recentFileActs[j]->setVisible(false);

	recentMenu->menuAction()->setVisible(numRecentFiles > 0);
}

#include "modelPlacer.h"
#include <Windows.h>
/* 3DxWare SDK Specific Includes */
#include "si.h"        /* Required for any 3DxWare driver support within an app.*/
#include "spwmacro.h"  /* Common macros used by 3DxWare SDK functions. */
extern "C"
{
#include "siapp.h"     /* Required for siapp.lib symbols */
}

SiHdl       m_DevHdl;       /* Handle to 3D mouse Device */

int ByyMainWindow::SbInit()
{
	int res;                                 //result of SiOpen, to be returned  
	SiOpenData oData;                        //OS Independent data to open ball  

	if (SiInitialize() == SPW_DLL_LOAD_ERROR)   //init the 3DxWare input library
	{
		QMessageBox::warning(this,"","Error: Could not load SiAppDll dll files");
	}
	SiOpenWinInit (&oData, winId());          //init Win. platform specific data  
	SiSetUiMode(&m_DevHdl, SI_UI_ALL_CONTROLS); //Config SoftButton Win Display 

	//open data, which will check for device type and return the device handle
	// to be used by this function  
	if ( (m_DevHdl = SiOpen ("TestSP", SI_ANY_DEVICE, SI_NO_MASK,  
		SI_EVENT, &oData)) == NULL )
	{
		SiTerminate();  //called to shut down the 3DxWare input library 
		res = 0;        //could not open device
		return res; 
	}
	else
	{
		res = 1;        //opened device succesfully
		return res;  
	}  
}

bool ByyMainWindow::winEvent( MSG *pMsg, long *result )
{
	int            num;      /* number of button returned */
	BOOL           handled;  /* is message handled yet */ 
	SiSpwEvent     Event;    /* 3DxWare Event */ 
	SiGetEventData EData;    /* 3DxWare Event Data */

	handled = SPW_FALSE;         /* init handled */

	/* init Window platform specific data for a call to SiGetEvent */
	SiGetEventWinInit(&EData, pMsg->message, pMsg->wParam, pMsg->lParam);

	/* check whether msg was a 3D mouse event and process it */
	if (SiGetEvent (m_DevHdl, 0, &EData, &Event) == SI_IS_EVENT)
	{
		/* figure out what kind of 3D mouse event it is */
		if (Event.type == SI_MOTION_EVENT)
		{
			//SbMotionEvent(&Event);  /* process 3D mouse motion event */    

			ByyModelPlacer::instance(myApp.ig()).connexion3D(
				Event.u.spwData.mData[SI_TX],
				Event.u.spwData.mData[SI_TZ],
				Event.u.spwData.mData[SI_TY],
				Event.u.spwData.mData[SI_RX],
				Event.u.spwData.mData[SI_RZ],
				Event.u.spwData.mData[SI_RY]);
		}
		if (Event.type == SI_ZERO_EVENT)
		{
			//SbZeroEvent();          /* process 3D mouse zero event */     
		}
		if (Event.type == SI_BUTTON_EVENT)
		{
			if ((num = SiButtonPressed (&Event)) != SI_NO_BUTTON)  
			{
				//SbButtonPressEvent(num);     /* process 3D mouse button event */
			}
			if ((num = SiButtonReleased (&Event)) != SI_NO_BUTTON)  
			{
				//SbButtonReleaseEvent(num);     /* process 3D mouse button event */
			}
		}
		handled = SPW_TRUE;            /* 3D mouse event handled */ 
	}

	/* if its not a 3D mouse event, let windows handle it */
	if (handled == SPW_FALSE)
	{
		return __super::winEvent(pMsg,result);
	}
	else
	{
		*result=handled;

		return handled;
	}

}
