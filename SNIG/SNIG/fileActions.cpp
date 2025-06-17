#include "StdAfx.h"
#include "fileActions.h"
#include "ByyApp.h"
#include "mainWindow.h"
#include "scenarioManager.h"
#include "scenarioFileUtil.h"
#include "../snigCore/settings.h"

ByyNewScenarioAction::ByyNewScenarioAction( ByyApp& app,QWidget* parent/*=0*/ )
	:ByyAction(app,"ByyNewScenarioAction",parent)
{
	setText(QString::fromLocal8Bit("新建想定(&N)"));
	setIcon(QIcon("../data/icons/FileNew.svg"));
}

ByyNewScenarioAction::~ByyNewScenarioAction()
{

}

void ByyNewScenarioAction::on_triggered()
{
	if (ByyScenarioFileUtil::okToContinue(myApp))
	{
		myApp.hdose().scenarioManager().newScenario();
	}
}

void ByyNewScenarioAction::checkEnabled()
{
	
}

ByyLoadScenarioAction::ByyLoadScenarioAction( ByyApp& app,QWidget* parent/*=0*/ )
	:ByyAction(app,"ByyLoadScenarioAction",parent)
{
	setText(QString::fromLocal8Bit("加载想定(&L)..."));
	setIcon(QIcon("../data/icons/FileOpen.svg"));
	
	connect(&myApp.core().remoteObjectManager(),SIGNAL(showRoleChanged(int)),SLOT(checkEnabled()));
}

ByyLoadScenarioAction::~ByyLoadScenarioAction()
{

}

void ByyLoadScenarioAction::on_triggered()
{
	if (ByyScenarioFileUtil::okToContinue(myApp))
	{
		QString fileName = QFileDialog::getOpenFileName(&myApp.mainWindow(), QString::fromLocal8Bit("加载想定"),
			ByyConfigIni::instance()->scnPath(),QString::fromLocal8Bit("scenario (*.scn)"));

		if (!fileName.isEmpty())
		{
			if (!myApp.hdose().scenarioManager().loadScenario(fileName))
			{
				QMessageBox::warning(&myApp.mainWindow(),QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("加载想定失败！"));
			}

			ByyConfigIni::instance()->setScnPath(fileName);
			//存在想定
			ByyConfigIni::instance()->setisScn(true);
		}
	}
}

void ByyLoadScenarioAction::checkEnabled()
{
	if(myApp.hdose().mode() == ByyHdose::Join)
	{
		setEnabled(myApp.core().remoteObjectManager().getShowRole() == ByyRemoteObjectManager::White);
	}
}

ByySaveScenarioAction::ByySaveScenarioAction( ByyApp& app,QWidget* parent/*=0*/ )
	:ByyAction(app,"ByySaveScenarioAction",parent)
{
	setText(QString::fromLocal8Bit("保存想定(&A)"));
	setIcon(QIcon("../data/icons/FileSave.svg"));

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
}

ByySaveScenarioAction::~ByySaveScenarioAction()
{

}

void ByySaveScenarioAction::on_triggered()
{
	ByyScenarioFileUtil::save(myApp);
}

void ByySaveScenarioAction::checkEnabled()
{
	setEnabled(myApp.hdose().scenarioManager().scenario());
}

ByySaveAsScenarioAction::ByySaveAsScenarioAction( ByyApp& app,QWidget* parent/*=0*/ )
	:ByyAction(app,"ByySaveAsScenarioAction",parent)
{
	setText(QString::fromLocal8Bit("另存为想定(&S)..."));
	setIcon(QIcon("../data/icons/FileSaveAs.svg"));

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
}

ByySaveAsScenarioAction::~ByySaveAsScenarioAction()
{

}

void ByySaveAsScenarioAction::on_triggered()
{
	ByyScenarioFileUtil::saveAs(myApp);
}

void ByySaveAsScenarioAction::checkEnabled()
{
	setEnabled(myApp.hdose().scenarioManager().scenario());
}

ByyCloseScenarioAction::ByyCloseScenarioAction( ByyApp& app,QWidget* parent/*=0*/ )
	:ByyAction(app,"ByyCloseScenarioAction",parent)
{
	setText(QString::fromLocal8Bit("关闭想定(&C)"));

	connect(&myApp.hdose().scenarioManager(),SIGNAL(signal_scenarioChanged()),SLOT(checkEnabled()));
	connect(&myApp.core().remoteObjectManager(),SIGNAL(showRoleChanged(int)),SLOT(checkEnabled()));
}

ByyCloseScenarioAction::~ByyCloseScenarioAction()
{

}

void ByyCloseScenarioAction::on_triggered()
{
	if (ByyScenarioFileUtil::okToContinue(myApp))
	{
		myApp.hdose().scenarioManager().closeScenario();
		//不存在想定
		ByyConfigIni::instance()->setisScn(false);
	}
}

void ByyCloseScenarioAction::checkEnabled()
{
	if(myApp.hdose().mode() == ByyHdose::Join)
	{
		setEnabled(myApp.hdose().scenarioManager().scenario() && myApp.core().remoteObjectManager().getShowRole() == ByyRemoteObjectManager::White);
	}
	else
	{
		setEnabled(myApp.hdose().scenarioManager().scenario());
	}
}

ByyOpenTerrainAction::ByyOpenTerrainAction( ByyApp& app,QWidget* parent/*=0*/ )
	:ByyAction(app,"ByyOpenTerrainAction",parent)
{
	setText(QString::fromLocal8Bit("打开地形(&O)..."));
	setIcon(QIcon("../data/icons/TerrainOpen.svg"));
}

ByyOpenTerrainAction::~ByyOpenTerrainAction()
{

}

void ByyOpenTerrainAction::on_triggered()
{
	QString filename=QFileDialog::getOpenFileName(&myApp.mainWindow(), QString::fromLocal8Bit("打开地形..."),
		"../data/earthfile/",
		tr("Terrain File(*.earth)"));

	if (filename.isEmpty())
		return;

	if (myApp.ig().terrain()->load(filename.toStdString()))
	{
//		myApp.ig().observer().reset();
	}
}

ByyCloseTerrainAction::ByyCloseTerrainAction( ByyApp& app,QWidget* parent/*=0*/ )
	:ByyAction(app,"ByyCloseTerrainAction",parent)
{
	setText(QString::fromLocal8Bit("关闭地形(&C)"));
	setIcon(QIcon("../data/icons/TerrainClose.svg"));
	setVisible(false);
}

ByyCloseTerrainAction::~ByyCloseTerrainAction()
{

}

void ByyCloseTerrainAction::on_triggered()
{
	myApp.ig().terrain()->unload();
}

ByySaveTerrainAction::ByySaveTerrainAction( ByyApp& app,QWidget* parent/*=0*/ )
	:ByyAction(app,"ByySaveTerrainAction",parent)
{
	setText(QString::fromLocal8Bit("保存地形(&S)"));
	setIcon(QIcon("../data/icons/TerrainSave.svg"));
}

ByySaveTerrainAction::~ByySaveTerrainAction()
{

}

void ByySaveTerrainAction::on_triggered()
{

}

ByySaveAsTerrainAction::ByySaveAsTerrainAction( ByyApp& app,QWidget* parent/*=0*/ )
	:ByyAction(app,"ByySaveAsTerrainAction",parent)
{
	setText(QString::fromLocal8Bit("另存为地形(&A)..."));
}

ByySaveAsTerrainAction::~ByySaveAsTerrainAction()
{

}

void ByySaveAsTerrainAction::on_triggered()
{
	QString filename=QFileDialog::getSaveFileName(&myApp.mainWindow(), QString::fromLocal8Bit("另存为地形..."),
		"../data/earthfile/",
		tr("Terrain File(*.earth)"));

	if (!filename.isEmpty())
	{
		myApp.ig().terrain()->saveAs(filename.toStdString());
	}
}

ByyViewpointCreateAction::ByyViewpointCreateAction( ByyApp& app,QWidget* parent/*=0*/ )
	:ByyAction(app,"ByyViewpointCreateAction",parent)
{
	setText(QString::fromLocal8Bit("视点制作(&V)..."));
}

ByyViewpointCreateAction::~ByyViewpointCreateAction()
{

}

void ByyViewpointCreateAction::on_triggered()
{
	myApp.mainWindow().showViewpointCreateWidget();
}

ByyAnimationPathCreateAction::ByyAnimationPathCreateAction( ByyApp& app,QWidget* parent/*=0*/ )
	:ByyAction(app,"ByyAnimationPathCreateAction",parent)
{
	setText(QString::fromLocal8Bit("路径制作(&P)..."));
}

ByyAnimationPathCreateAction::~ByyAnimationPathCreateAction()
{

}

void ByyAnimationPathCreateAction::on_triggered()
{
	myApp.mainWindow().showAnimationPathCreateWidget();
}

ByyTerrainEditAction::ByyTerrainEditAction( ByyApp& app,QWidget* parent/*=0*/ )
	:ByyAction(app,"ByyTerrainEditAction",parent)
{
	setText(QString::fromLocal8Bit("地形制作(&T)..."));
}

ByyTerrainEditAction::~ByyTerrainEditAction()
{

}

void ByyTerrainEditAction::on_triggered()
{
	myApp.mainWindow().showTerrainEditWidget();
}

ByySnapshootAction::ByySnapshootAction( ByyApp& app,QWidget* parent/*=0*/ )
	:ByyAction(app,"ByySnapshootAction",parent)
{
	setText(QString::fromLocal8Bit("截图(&D)"));
	setIcon(QIcon("../data/icons/snapshoot.png"));
}

ByySnapshootAction::~ByySnapshootAction()
{

}

void ByySnapshootAction::on_triggered()
{
	ByyChannel* channel=myApp.ig().inputController().currentChannel();

	if (channel)
	{
		QString file=QString("../data/snapshoots/")+QDateTime::currentDateTime().toString("yyyyMMddhhmmss")+".png";
		channel->snapshoot()->snapshoot(file.toStdString());

		QMessageBox::warning(&myApp.mainWindow(),QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("图片已保存到%1。").arg(file));
	}
}

ByyExitAction::ByyExitAction( ByyApp& app,QWidget* parent/*=0*/ )
	:ByyAction(app,"ByyExitAction",parent)
{
	setText(QString::fromLocal8Bit("退出(&E)"));
	setIcon(QIcon("../data/icons/FileExit.svg"));
}

ByyExitAction::~ByyExitAction()
{

}

void ByyExitAction::on_triggered()
{
	myApp.mainWindow().close();
}
