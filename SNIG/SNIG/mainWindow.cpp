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
#include "ByyHdose.h"
#include "simuController.h"
#include "Verdict.h"

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

#include "ByyOutputWidget.h"
#include "simTimeWidget.h"

#include "ByyDisplay.h"
#include "ByyOsgWindow.h"
#include "ByyChannel.h"
#include "ByyEntityOperator.h"
#include "ByyControlObjectOperator.h"
#include "ByySimMultipleWidget.h"

#include <osgQt/GraphicsWindowQt>

#include "displayManageWidget.h"
#include "ByyShowRoleWidget.h"

#include "XFrame/XTest_GISWnd.h"
#include "ByyQGis2DApdater.h"

#include "comApp.h"
#include "../snigCore/Event.h"


//#include "Verdict.h"

#include "WarDamageStatistics.h"

ByyMainWindow::ByyMainWindow( ByyApp& app,QWidget *parent /*= 0*/, Qt::WFlags flags /*= 0*/ ) 
	: QMainWindow(parent, flags)
	,myApp(app)
{
	setObjectName("ByyMainWindow");

	ByyEntityOperator::instance().setApp(&app);
	ByyControlObjectOperator::instance().setApp(&app);
	comApp::instance()->setApp(&app);
	Verdict::getInstance(); //构建

	//QTabWidget* centralTab=new QTabWidget;
	centralTab=new QTabWidget;
	centralTab->setDocumentMode(true);
	setCentralWidget(centralTab);

	central3d=new QWidget;
	central3d->setLayout(new QVBoxLayout);
	central3d->setMinimumSize(100,100);
	central3d->layout()->setContentsMargins(0,0,0,0);
	centralTab->addTab(central3d,QString::fromLocal8Bit("三维"));
	myAutoHidePanel=new ByyAutoHidePanel(central3d,this);
	ByyOsgWindow* window=myApp.ig().display().addWindow();	
	QWidget* osgWidget=window->getAdapterWidget();
	//osgWidget->setMinimumSize(centralTab->width(),centralTab->height());
	osgWidget->installEventFilter(this);
	central3d->layout()->addWidget(osgWidget);
	central3d->installEventFilter(this);

	createMenuToolBars(); 
	createDockWidgets();
	cratteVerdictDockWidgets();

	////////////////////////////
	// 多地球窗口test xks,20240226
	/*QWidget *central3db=new QWidget;
	central3db->setLayout(new QVBoxLayout);
	central3db->setMinimumSize(100,100);
	central3db->layout()->setContentsMargins(0,0,0,0);
	centralTab->addTab(central3db,QString::fromLocal8Bit("三维2"));
	ByyOsgWindow* windowb=myApp.ig().display().addWindow();
	QWidget* osgWidgetb=windowb->getAdapterWidget();
	osgWidgetb->installEventFilter(this);
	central3db->layout()->addWidget(osgWidgetb);*/
	//////////////////

	if (ByyConfigIni::instance()->enable2d())
	{
		XTest_GISWnd *gisWnd=new XTest_GISWnd(this);
		centralTab->addTab(gisWnd,QString::fromLocal8Bit("二维"));

		addToolBar(gisWnd->toolBar());

		ByyQGis2DApdater* qgisApdater=new ByyQGis2DApdater(app,gisWnd,this);
	}

	myApp.ig().initLogCanvas(myApp.hdose().simEventLogDatabase(),SIGNAL(logAdded(double,QString)));

	connect(&myApp.ig().inputController(),SIGNAL(requestContextMenu()),&ByyEntityOperator::instance(),SLOT(showContextMenu()));
	connect(&myApp.ig().inputController(),SIGNAL(requsetDoubleClick()),&ByyEntityOperator::instance(),SLOT(processDoubleClicked()));

	connect(&myApp.ig().inputController(),SIGNAL(requestContextMenu()),&ByyControlObjectOperator::instance(),SLOT(showContextMenu()));
	connect(&myApp.ig().inputController(),SIGNAL(requsetDoubleClick()),&ByyControlObjectOperator::instance(),SLOT(processDoubleClicked()));

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(onScenarioChanged()));
	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_modifiedChanged(bool)),SLOT(setWindowModified(bool)));

	connect(&myApp.hdose().simuController(), SIGNAL(signal_StatusChanged(int)), this, SLOT(onHdoseStatusChanged(int)));

	setQss();
}

ByyMainWindow::~ByyMainWindow()
{

}

void ByyMainWindow::createMenuToolBars()
{
	recentMenu=new QMenu(QString::fromLocal8Bit("最近打开想定"));

	for (int i = 0; i < MAX_REC_FILES_SIZE; ++i) {
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
		closeScnAction=new ByyCloseScenarioAction(myApp,this);
		fileMenu->addAction(newScnAction);
		fileMenu->addAction(loadScnAction);

		fileMenu->addMenu(recentMenu);

		fileMenu->addAction(saveScnAction);
		fileMenu->addAction(saveAsScnAction);
		fileMenu->addAction(closeScnAction);
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
		connectMenu->setIcon(QIcon("../data/icons/h.svg"));
		settinsMenu->addMenu(connectMenu);

		QAction* connectSingleHdoseAction=new ByyConnectSingleHdoseAction(myApp,this);
		QAction* connectMultiHdoseAction=new ByyConnectMultiHdoseAction(myApp,this);
		QAction* redConnectAction=new ByyRedConnectAction(myApp,this);
		QAction* blueConnectAction=new ByyBlueConnectAction(myApp,this);
		QAction* connectCreateHdoseAction=new ByyConnectCreateHdoseAction(myApp,this);
		QAction* connectJoinHdoseAction=new ByyConnectJoinHdoseAction(myApp,this);
		//QAction* connectHLAAction=new ByyConnectVRLAction(myApp,this);

		connectMenu->addAction(connectSingleHdoseAction);
		connectMenu->addAction(connectMultiHdoseAction);
		connectMenu->addAction(redConnectAction);
		connectMenu->addAction(blueConnectAction);
		connectMenu->addAction(connectCreateHdoseAction);
		connectMenu->addAction(connectJoinHdoseAction);
		//connectMenu->addAction(connectHLAAction);

		settinsMenu->addAction(new ByySeparatorAction(myApp,this));

		QMenu* stereoModeMenu=new ByyStereoModeMenu(myApp);    // 立体模式
		settinsMenu->addMenu(stereoModeMenu);

		QActionGroup *stereoActionGroup=new QActionGroup(this);

		stereoActionGroup->addAction(new ByyQuadBufferStereoAction(myApp,this));
		stereoActionGroup->addAction(new ByyAnaglyphicStereoAction(myApp,this));
		stereoActionGroup->addAction(new ByyHSplitStereoAction(myApp,this));
		stereoActionGroup->addAction(new ByyPassivityStereoAction(myApp,this));

		stereoModeMenu->addActions(stereoActionGroup->actions());

		settinsMenu->addAction(new ByyStereoSettingsAction(myApp,this)); // 立体设置
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
		settinsMenu->addAction(new ByyScreenLogAction(myApp,this));
		settinsMenu->addAction(new ByyShowScanAction(myApp,this));

		menuBar()->addMenu(settinsMenu);

		QToolBar* hdoseConnectToolBar=addToolBar(QString::fromLocal8Bit("连接"));
		QToolButton* hdoseToolButton=new QToolButton;
		hdoseToolButton->setPopupMode(QToolButton::InstantPopup);
		connect(hdoseToolButton,SIGNAL(triggered(QAction*)),hdoseToolButton,SLOT(setDefaultAction(QAction*)));
		hdoseToolButton->setIcon(QIcon("../data/icons/h.svg"));
		hdoseConnectToolBar->addWidget(hdoseToolButton);
		hdoseToolButton->addAction(connectSingleHdoseAction);
		hdoseToolButton->addAction(connectMultiHdoseAction);
		hdoseToolButton->addAction(redConnectAction);
		hdoseToolButton->addAction(blueConnectAction);
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
		QAction* snapShotAction=new ByySnapShotAction(myApp,this);

		simulationMenu->addAction(rewindAction);
		simulationMenu->addAction(startAction);
		simulationMenu->addAction(pauseAction);
		simulationMenu->addAction(new ByySeparatorAction(myApp,this));
		simulationMenu->addAction(fastBestAction);
		simulationMenu->addAction(new ByySeparatorAction(myApp,this));
		simulationMenu->addAction(scnProfileSettingsAction);
		simulationMenu->addAction(scnEnvironmentSettingsAction);
		simulationMenu->addAction(snapShotAction);

		menuBar()->addMenu(simulationMenu);

		QToolBar* simuToolBar=addToolBar(QString::fromLocal8Bit("仿真"));

		simuToolBar->addAction(rewindAction);
		simuToolBar->addAction(startAction);
		simuToolBar->addAction(pauseAction);
		simuToolBar->addAction(new ByySeparatorAction(myApp,this));
		simuToolBar->addAction(fastBestAction);
		simuToolBar->addWidget(new ByySimMultipleWidget(myApp.hdose(),this));
		simuToolBar->addAction(new ByySeparatorAction(myApp,this));

		{
			ByyShowRoleWidget *showRoleWidget=new ByyShowRoleWidget(myApp,this);
			simuToolBar->addWidget(showRoleWidget);
		}

		simuToolBar->addAction(new ByySeparatorAction(myApp,this));
		simuToolBar->addAction(scnProfileSettingsAction);
		simuToolBar->addAction(scnEnvironmentSettingsAction);
		simuToolBar->addAction(snapShotAction);

		toolBarMenu->addAction(simuToolBar->toggleViewAction());
		
	}
	{
		//编队阵型菜单
		QMenu* ftMenu=new ByyFormationMenu(this);
		QAction* ftAction=new ByyFormationEditAction(myApp,this);
		ftMenu->addAction(ftAction);
		menuBar()->addMenu(ftMenu);

		QToolBar* ftToolBar=addToolBar(QString::fromLocal8Bit("编队阵型"));

		ftToolBar->addAction(ftAction);
		toolBarMenu->addAction(ftToolBar->toggleViewAction());
	}

	{
		// 20230831，裁决菜单
		QMenu *verdictMenu = new ByySimpleMenu(QString::fromLocal8Bit("裁决"), "VerdictMenu", this);

#if 0
		QAction* acsAction = new ByyAirCombatSuperiorityAction(myApp,this);
		verdictMenu->addAction(acsAction);
		QAction* taAction = new ByyTargetAlertAction(myApp,this);
		QAction* cdAction = new ByyCollisionDetectionAction(myApp,this);
		QAction* dAction = new ByyDamageAction(myApp,this);
		verdictMenu->addAction(acsAction);
		verdictMenu->addAction(taAction);
		verdictMenu->addAction(cdAction);
		verdictMenu->addAction(dAction);
#else
		QMenu* detectiomEarlywaringJudgmentMenu = new ByyDetectiomEarlywaringJudgmentMenu(myApp);    // 探测预警判断
		detectiomEarlywaringJudgmentMenu->addAction(Verdict::getInstance()->registerDockWidget(EntityAlertDialog, this));
		detectiomEarlywaringJudgmentMenu->addAction(Verdict::getInstance()->registerDockWidget(ExercisePartyAlertDialog, this));
		verdictMenu->addMenu(detectiomEarlywaringJudgmentMenu);

		QMenu* targetTrackingJudgmentMenu = new ByyTargetTrackingJudgmentMenu(myApp);    //目标跟踪判断
		verdictMenu->addMenu(targetTrackingJudgmentMenu);
		targetTrackingJudgmentMenu->addAction(Verdict::getInstance()->registerDockWidget(AircraftTargetTrackingDialog,this));
		targetTrackingJudgmentMenu->addAction(Verdict::getInstance()->registerDockWidget(MissileTargetTrackingDialog,this));


		QMenu* advantageCalculationMenu = new ByyAdvantageCalculationMenu(myApp);    //优势计算
		verdictMenu->addMenu(advantageCalculationMenu);
		advantageCalculationMenu->addAction(Verdict::getInstance()->registerDockWidget(AirCombatAdvantageCalculationDialog,this));
		advantageCalculationMenu->addAction(Verdict::getInstance()->registerDockWidget(FireControlLockingCalculationDialog,this));
		advantageCalculationMenu->addAction(Verdict::getInstance()->registerDockWidget(SensorAlarmCalculationDialog,this));
		advantageCalculationMenu->addAction(Verdict::getInstance()->registerDockWidget(MissileHitProbabilityCalculationDialog,this));

		//碰撞检测计算
		verdictMenu->addAction(Verdict::getInstance()->registerDockWidget(CollisionDetectionDialog, this));

		QMenu* damageCalculationMenu = new ByyDamageCalculationMenu(myApp);    //毁伤计算
		verdictMenu->addMenu(damageCalculationMenu);
		damageCalculationMenu->addAction(Verdict::getInstance()->registerDockWidget(TorpedoVsSubmarineDamageCalculationDialog,this));
		damageCalculationMenu->addAction(Verdict::getInstance()->registerDockWidget(TorpedoVsShipDamageCalculationDialog,this));
		damageCalculationMenu->addAction(Verdict::getInstance()->registerDockWidget(DepthChargeVsSubmarineDamageCalculationDialog,this));
		damageCalculationMenu->addAction(Verdict::getInstance()->registerDockWidget(MissileVsShipDamageCalculationDialog,this));

		QMenu* entityStatusVerdictMenu = new ByyEntityStatusVerdictMenu(myApp);    //实体状态裁决
		verdictMenu->addMenu(entityStatusVerdictMenu);
		entityStatusVerdictMenu->addAction(Verdict::getInstance()->registerDockWidget(NavalEquipmentStatusJudgmentDialog,this));
		entityStatusVerdictMenu->addAction(Verdict::getInstance()->registerDockWidget(SubmarineEquipmentStatusJudgmentDialog,this));
		entityStatusVerdictMenu->addAction(Verdict::getInstance()->registerDockWidget(ExitJudgmentDialog,this));

#endif
		menuBar()->addMenu(verdictMenu);
		verdictMenu->setEnabled(false);
		m_verdictMenu = verdictMenu;
	}

	{
		QToolBar *simTimeToolBar=addToolBar(QString::fromLocal8Bit("仿真时间"));
		simTimeToolBar->addWidget(new ByySimTimeWidget(myApp,this));

		toolBarMenu->addAction(simTimeToolBar->toggleViewAction());
	}
	{
		QMenu *measureMenu=new ByyMeasureMenu(this);

		QAction* lineAction=new ByyLineMeasureAction(myApp,this);
		lineAction->setShortcut(QKeySequence("Ctrl+1"));
		QAction* areaAction=new ByyPolygonMeasureAction(myApp,this);
		areaAction->setShortcut(QKeySequence("Ctrl+2"));
		QAction* intervisibilityLineAction=new ByyIntervisibilityLineAction(myApp,this);
		intervisibilityLineAction->setShortcut(QKeySequence("Ctrl+3"));
		QAction* intervisibilityFanAction=new ByyIntervisibilityFanAction(myApp,this);
		intervisibilityFanAction->setShortcut(QKeySequence("Ctrl+4"));

		measureMenu->addAction(lineAction);
		measureMenu->addAction(areaAction);
		measureMenu->addAction(intervisibilityLineAction);
		measureMenu->addAction(intervisibilityFanAction);

		QToolBar* measureToolBar=addToolBar(QString::fromLocal8Bit("测量"));
		measureToolBar->addAction(lineAction);
		measureToolBar->addAction(areaAction);
		measureToolBar->addAction(intervisibilityLineAction);
		measureToolBar->addAction(intervisibilityFanAction);

		menuBar()->addMenu(measureMenu);

		toolBarMenu->addAction(measureToolBar->toggleViewAction());

	}
	
	//add by lyj 20211224
	{
		if( ByyConfigIni::instance()->defense() || ByyConfigIni::instance()->uav())
		{
			QToolBar* pgToolBar=addToolBar(QString::fromLocal8Bit("统计评估"));

			if(ByyConfigIni::instance()->defense())
			{
				QAction* pgAction=new ByyShowStatAction(myApp,this);
				pgToolBar->addAction(pgAction);
				toolBarMenu->addAction(pgToolBar->toggleViewAction());
			}
			if(ByyConfigIni::instance()->uav())
			{
				QAction* indexAction=new ByyShowIndexAction(myApp,this);
				pgToolBar->addAction(indexAction);
				toolBarMenu->addAction(pgToolBar->toggleViewAction());
			}
		}
	}
	//end
	{
		QMenu* helpMenu=new ByyHelpMenu(this);
		//helpMenu->addAction(new ByyAboutAction(myApp,this));

		helpMenu->addAction(new ByyShowDocAction(myApp,"ByyShowSNIGDocAction",
			QString::fromLocal8Bit("用户手册(&M)"),
			QString::fromLocal8Bit("../docs/"),this));

		/*helpMenu->addAction(new ByyShowDocAction(myApp,"ByyShowEarthToolDocAction",
			QString::fromLocal8Bit("地球工具文档(&E)"),
			QString::fromLocal8Bit("../docs/地球配置工具文档.pdf"),this));

		helpMenu->addAction(new ByyShowDocAction(myApp,"ByyShowConfigToolDocAction",
			QString::fromLocal8Bit("配置工具文档(&C)"),
			QString::fromLocal8Bit("../docs/配置工具文档.pdf"),this));*/

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

	ByyOutputWidget *outputWidget=new ByyOutputWidget(myApp.hdose(),this);
	QDockWidget* outputDockWidget=new QDockWidget(QString::fromLocal8Bit("日志输出"),this,Qt::Window);
	outputDockWidget->setObjectName("outputDockWidget");
	outputDockWidget->setWidget(outputWidget);
	outputDockWidget->layout()->setContentsMargins(0,0,0,0);

	outputDockWidget->setVisible(false);

	viewMenu->addAction(outputDockWidget->toggleViewAction());

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


void ByyMainWindow::cratteVerdictDockWidgets()
{
	QMenu* viewMenu=findChild<QMenu*>("VerdictMenu");
	WarDamageStatistics* statisticsWidget=new WarDamageStatistics();
	QDockWidget *statisticsWidgetDock=new QDockWidget(QString::fromLocal8Bit("战损统计"));
	statisticsWidgetDock->setObjectName("objListDockWidget");
	statisticsWidgetDock->setWidget(statisticsWidget);
	statisticsWidgetDock->layout()->setContentsMargins(0,0,0,0);
	statisticsWidgetDock->setAllowedAreas(Qt::LeftDockWidgetArea);
	addDockWidget(Qt::LeftDockWidgetArea,statisticsWidgetDock);
	viewMenu->addAction(statisticsWidgetDock->toggleViewAction());
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
			ByyConfigIni::instance()->removeRecScnFile(action->data().toString());
			updateRecentFileActions();
		}
	}
}

void ByyMainWindow::onHdoseStatusChanged(int status)
{
	//qDebug() << __FUNCDNAME__ << __LINE__ << status;
	if(!m_verdictMenu->isEnabled() && ByySimuController::S_RUNING == status)
	{
		m_verdictMenu->setEnabled(true);
	}
	else if(m_verdictMenu->isEnabled() && ByySimuController::S_RUNING > status)
	{
		m_verdictMenu->setEnabled(false);
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
	if (object->parent()==central3d&&event->type()==QEvent::KeyRelease)
	{
		QKeyEvent *kEvent=dynamic_cast<QKeyEvent*>(event);
		// 全屏显示与退出处理Ctrl+Enter
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
	ByyConfigIni::instance()->addRecScnFile(filename);

	updateRecentFileActions();
}

void ByyMainWindow::updateRecentFileActions()
{
	QStringList files = ByyConfigIni::instance()->recScnFiles(); 
	if(files.isEmpty())
		return;

	int numRecentFiles = qMin(files.size(), (int)MAX_REC_FILES_SIZE);

	for (int i = 0; i < numRecentFiles; ++i) {
		QString text = tr("&%1 %2").arg(i + 1).arg(files[i]);//strippedName(files[i]));
		recentFileActs[i]->setText(text);
		recentFileActs[i]->setData(files[i]);
		recentFileActs[i]->setVisible(true);
	}
	for (int j = numRecentFiles; j < MAX_REC_FILES_SIZE; ++j)
		recentFileActs[j]->setVisible(false);

	recentMenu->menuAction()->setVisible(numRecentFiles > 0);
}

void ByyMainWindow::setQss()
{
	QFile file("../data/qss/style_blue.css");
	int test = file.open(QFile::ReadOnly);
	QString qss = QLatin1String(file.readAll());
	qApp->setStyleSheet(qss);
	qApp->setPalette(QPalette(QColor("#F0F0F0")));
}

bool ByyMainWindow::event(QEvent* e)
{
	switch(e->type())
	{
	case EVT_FRAME_VIEW:{
		Event* evt=(Event*)e;
		QWidget* wnd=(QWidget*)evt->data(1).toLongLong();
		switch(evt->data(0).toInt())
		{
		case EVT_FRAME_VIEW_OPEN:
			centralTab->addTab(wnd,evt->data(2).toString());
			return true;
		default:
			break;
		}
						}
	default:break;
						
	}
	if(QMainWindow::event(e))
		return true;
	return false;
}