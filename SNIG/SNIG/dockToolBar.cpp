#include "stdafx.h"
#include "dockToolBar.h"
#include "settings.h"
#include "snapshoot.h"
#include "mainWindow.h"
#include "audioSettings.h"

ByyDockToolBar::ByyDockToolBar( ByyMainWindow* mainWindow,QWidget *parent /*= 0*/ ) 
	: QDockWidget(parent)
	,myMainWindow(mainWindow)
{
	ui.setupUi(this);

	ui.animationPathButton->setDefaultAction(mainWindow->ui.viewMenu->actions()[2]);
	ui.viewpointButton->setDefaultAction(mainWindow->ui.viewMenu->actions()[3]);
	ui.objectButton->setDefaultAction(mainWindow->ui.viewMenu->actions()[1]);

	ui.measureButton->setDefaultAction(mainWindow->ui.viewMenu->actions()[5]);
	ui.topographyButton->setDefaultAction(mainWindow->ui.viewMenu->actions()[4]);
	ui.effectsButton->setIcon(QIcon("../data/icons/terraineffects.png"));

	ui.environmentButton->setIcon(QIcon("../data/icons/TerrainScene.svg"));
	ui.videoRecordButton->setIcon(QIcon("../data/icons/video.png"));
	ui.electromagneticButton->setIcon(QIcon("../data/icons/electromagnetism.png"));
	ui.realForceButton->setIcon(QIcon("../data/icons/soldier.png"));

	setupIcons();
	init();

	setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);

	toggleViewAction()->setText(QString::fromLocal8Bit("²Ù¿Ø(&C)"));
}

ByyDockToolBar::~ByyDockToolBar()
{

}

void ByyDockToolBar::setupIcons()
{
	ui.audioButton->setIcon(QIcon("../data/icons/audio.png"));
	ui.pathButton->setIcon(QIcon("../data/icons/DisplayTacticalGraphics.svg"));
	ui.screenshotButton->setIcon(QIcon("../data/icons/snapshoot.png"));
	ui.infoButton->setIcon(QIcon("../data/icons/building_info.png"));
	ui.videoMonitorButton->setIcon(QIcon("../data/icons/camera.png"));
	ui.mouseCoordsButton->setIcon(QIcon("../data/icons/location.png"));
	ui.caveTransparencyButton->setIcon(QIcon("../data/icons/cave.png"));
	ui.wakeButton->setIcon(QIcon("../data/icons/DisplayTrails.png"));
	ui.fullScreenButton->setIcon(QIcon("../data/icons/fullscreen.png"));
	ui.stereoButton->setIcon(QIcon("../data/icons/3d.png"));
}

void ByyDockToolBar::init()
{
	ui.audioButton->setChecked(ByyAudioSettings::instance().enabled());
	connect(ui.audioButton,SIGNAL(clicked(bool)),this,SLOT(setAudio(bool)));

	ui.pathButton->setChecked(ByySettings::instance()->envirProcessSettings()->isShow());
	connect(ui.pathButton,SIGNAL(clicked(bool)),ByySettings::instance()->envirProcessSettings(),SLOT(show(bool)));

	//ui.screenshotButton->setEnabled(!osg::DisplaySettings::instance()->getStereo());
	connect(ui.screenshotButton,SIGNAL(clicked()),SLOT(snapshoot()));

	ui.infoButton->setChecked(ByySettings::instance()->buildingSettings()->isShowInfo());
	connect(ui.infoButton,SIGNAL(clicked(bool)),ByySettings::instance()->buildingSettings(),SLOT(showInfo(bool)));

	ui.videoMonitorButton->setChecked(ByySettings::instance()->videoSettings()->isShow());
	connect(ui.videoMonitorButton,SIGNAL(clicked(bool)),ByySettings::instance()->videoSettings(),SLOT(show(bool)));

	ui.mouseCoordsButton->setChecked(ByySettings::instance()->mouseCoordsSettings()->enabled());
	connect(ui.mouseCoordsButton,SIGNAL(clicked(bool)),ByySettings::instance()->mouseCoordsSettings(),SLOT(setEnabled(bool)));

	connect(ui.caveTransparencyButton,SIGNAL(clicked(bool)),this,SLOT(onCaveTransparency(bool)));

	connect(ui.fullScreenButton,SIGNAL(toggled(bool)),this,SLOT(setFullScreen(bool)));

	ui.stereoButton->setChecked(osg::DisplaySettings::instance()->getStereo());
	connect(ui.stereoButton,SIGNAL(clicked(bool)),this,SIGNAL(quadBufferClicked(bool)));
}

void ByyDockToolBar::snapshoot()
{
	myMainWindow->snapshoot();
}

void ByyDockToolBar::onCaveTransparency( bool enable )
{
	/*if (enable)
	{
		for (int i=0;i!=maskLayerVector.size();++i)
		{
			d_scene->getMap()->addTerrainMaskLayer(maskLayerVector[i]);
		}
	}
	else
	{
		maskLayerVector.clear();
		d_scene->getMap()->getTerrainMaskLayers(maskLayerVector);

		for (int i=0;i!=maskLayerVector.size();++i)
		{
			d_scene->getMap()->removeTerrainMaskLayer(maskLayerVector[i]);
		}
	}*/
}

void ByyDockToolBar::toggleFullScreen()
{
	ui.fullScreenButton->toggle();
}

void ByyDockToolBar::closeEvent( QCloseEvent *event )
{
	emit closed();

	QDockWidget::closeEvent(event);
}

void ByyDockToolBar::setFullScreen( bool fullscreen )
{
	emit fullScreenChanged(fullscreen);

	ui.fullScreenButton->setIcon(QIcon(ICONPATH+(fullscreen?"unfullscreen.png":"fullscreen.png")));
}

void ByyDockToolBar::setAudio( bool audio )
{
	ByyAudioSettings::instance().setEnabled(audio);

	ui.audioButton->setIcon(QIcon(ICONPATH+(audio?"closeaudio.png":"audio.png")));
}
