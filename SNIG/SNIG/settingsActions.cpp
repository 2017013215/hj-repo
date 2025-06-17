#include "StdAfx.h"
#include "settingsActions.h"
#include "ByyApp.h"
#include "driver.h"
#include "driverManager.h"
#include "audioSettingsDialog.h"
#include "stereoSettings.h"
#include "stereoSettingsDialog.h"
#include "entitySettings.h"
#include "audioSettings.h"
#include "vrlConnectWidget.h"
#include "observerSettings.h"
#include "waitBackEngineWidget.h"
#include "hdoseSettingsWidget.h"
#include "../snigHdose/hdoseSettings.h"
#include "../snigCore/coreUtils.h"
#include "scenarioFileUtil.h"
#include "scenarioManager.h"
#include "scnProfileWidget.h"
#include "signalSingleton.h"
#include "fileActions.h"
ByyConnectSingleHdoseAction::ByyConnectSingleHdoseAction( ByyApp& app,QWidget* parent/*=0*/ ) 
	: ByyCheckableAction(app,"ByyConnectSingleHdoseAction",parent)
{
	setText( QString::fromLocal8Bit("单机模式(&S)"));
	setIcon( QIcon("../data/icons/h.svg"));
	setChecked(myApp.hdose().started()&&myApp.hdose().mode()==ByyHdose::Single);
	connect(&myApp.hdose(),SIGNAL(signal_runningChanged()),SLOT(checkEnabled()));
}

ByyConnectSingleHdoseAction::~ByyConnectSingleHdoseAction()
{

}

void ByyConnectSingleHdoseAction::on_triggered()
{
	if (isChecked())
	{
		myApp.ig().block(true);
		myApp.core().block(true);
		myApp.hdose().start(ByyHdose::Single);

		myApp.ig().block(false);
		myApp.core().block(false);
	}
	else
		myApp.hdose().stop();
}

void ByyConnectSingleHdoseAction::checkEnabled()
{
	setEnabled((!myApp.hdose().started())||(myApp.hdose().started()&&myApp.hdose().mode()==ByyHdose::Single));
}

//联机模式
ByyConnectMultiHdoseAction::ByyConnectMultiHdoseAction(ByyApp& app, ByyMainWindow* mw, QWidget* parent /*=0*/)
    : ByyCheckableAction(app, "ByyConnectMultiHdoseAction", parent),
	  clientA_ready_(false),
      clientB_ready_(false),
      waiting_to_start_(false),
	  logDialog_(nullptr),
	  server(nullptr),
	  m_mw(mw)
{
    setText(QString::fromLocal8Bit("联机模式(&M)"));
    setIcon(QIcon("../data/icons/h.svg"));
    setChecked(myApp.hdose().started() && myApp.hdose().mode() == ByyHdose::Create);

    connect(&myApp.hdose(), SIGNAL(signal_runningChanged()), this, SLOT(checkEnabled()));
	//等待加载想定成功
	connect(signalSingleton::instance(), SIGNAL(loadSuccess()), this, SLOT(onLoadSuccess()));
}

ByyConnectMultiHdoseAction::~ByyConnectMultiHdoseAction()
{

}

void ByyConnectMultiHdoseAction::on_triggered()
{
	if(!server)
	{
		// 初始化 LogDialog 只显示一次
		if (!logDialog_) {
			logDialog_ = new LogDialog(qobject_cast<QWidget*>(parent()));
			// 连接 LogDialog 的关闭信号,断开连接
			connect(logDialog_, SIGNAL(windowClosed()), this, SLOT(onDisconnect()));
			logDialog_->show();  // 弹出对话框
		}
		// 启动联邦服务器
		QDir currentDir;
		currentDir.cdUp();	//获取上级目录
		currentDir.cdUp();
		exePath = currentDir.absoluteFilePath("hdose/Tools/Hdose_exe/Release/QNetSrv.exe");
		process.start(exePath);
		if (!process.waitForStarted()) {
			QMessageBox::warning(&myApp.mainWindow(),QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("联邦服务器启动失败！"));
			return;
		}
		QTimer::singleShot(1000, this, SLOT(minimizeServerWindow())); // 延迟1秒后最小化窗口
		//启动引擎
		logDialog_->appendLog(QString::fromLocal8Bit("正在加载引擎..."));
		myApp.ig().block(true);
		myApp.core().block(true);
		myApp.hdose().start(ByyHdose::Create);
		myApp.ig().block(false);
		myApp.core().block(false);
		logDialog_->appendLog(QString::fromLocal8Bit("引擎加载成功！"));
		// 进入等待状态，等待 A 和 B 同时 ready
		waiting_to_start_ = true;
		clientA_ready_ = false;
		clientB_ready_ = false;
		// 监听端口号
		server = new tcpServer(this);
		server->startListening(8888);
		connect(server, SIGNAL(clientAReady()), this, SLOT(onClientAReady()));
		connect(server, SIGNAL(clientBReady()), this, SLOT(onClientBReady()));
		logDialog_->appendLog(QString::fromLocal8Bit("等待红方和蓝方准备完成..."));
	}else{
		QMessageBox::warning(&myApp.mainWindow(),QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("请先关闭连接！"));
	}
}

void ByyConnectMultiHdoseAction::checkEnabled()
{
    setEnabled((!myApp.hdose().started()) ||
               (myApp.hdose().started() && myApp.hdose().mode() == ByyHdose::Create));
}

void ByyConnectMultiHdoseAction::minimizeServerWindow()
{
	QString title = QString::fromLocal8Bit("联邦服务器");
	HWND hwnd = FindWindowW(NULL, (LPCWSTR)title.utf16());
	if (hwnd) {
		ShowWindow(hwnd, SW_MINIMIZE);
		SetForegroundWindow(hwnd); // 确保操作生效
	}
}

void ByyConnectMultiHdoseAction::onLoadSuccess()
{
	if(server && logDialog_)
	{
		logDialog_->appendLog(QString::fromLocal8Bit("加载想定成功！"));
	}
}

void ByyConnectMultiHdoseAction::onClientAReady()
{
    clientA_ready_ = true;
    logDialog_->appendLog(QString::fromLocal8Bit("红方准备就绪"));
    tryStartHdose();
}

void ByyConnectMultiHdoseAction::onClientBReady()
{
    clientB_ready_ = true;
    logDialog_->appendLog(QString::fromLocal8Bit("蓝方准备就绪"));
    tryStartHdose();
}

void ByyConnectMultiHdoseAction::tryStartHdose()
{
    if (waiting_to_start_ && clientA_ready_ && clientB_ready_)
	//if (waiting_to_start_ && clientA_ready_)
    {
        logDialog_->appendLog(QString::fromLocal8Bit("红方和蓝方已准备完毕，加载想定..."));
		//合并红蓝想定
		ScenarioMerger merger;
		QDir currentDir;
		if (merger.mergeScenarios(currentDir.absoluteFilePath("ScnData/redScn.scn"), currentDir.absoluteFilePath("ScnData/blueScn.scn"), currentDir.absoluteFilePath("ScnData/bothScn.scn"))) {
			qDebug() << "合并成功";
		} else {
			QMessageBox::warning(&myApp.mainWindow(),QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("合并想定失败！"));
			return;
		}
		//加载想定
		QAction* loadScnAction=new LoadScenarioAction(myApp,currentDir.absoluteFilePath("ScnData/bothScn.scn"),&myApp.mainWindow());
		loadScnAction->trigger();  // 自动触发
		loadScnAction->deleteLater();
    }
}

void ByyConnectMultiHdoseAction::onDisconnect()
{
	waiting_to_start_ = false;
	clientA_ready_ = false;
	clientB_ready_ = false;
	logDialog_ = nullptr;
	if (server) {
        server->deleteLater();  // 让 Qt 安全删除（非立即）
        server = nullptr;      // 置空指针
    }
	//关闭联邦服务器
	process.kill();
	//关闭引擎
	myApp.hdose().stop();
	//关闭想定
	//QAction* closeScnAction=new ByyCloseScenarioAction(myApp,&myApp.mainWindow());
	//closeScnAction->trigger();  // 自动触发
	//delete closeScnAction;
	//m_mw->closeScnAction->trigger();  // 自动触发
}

//红方连接
ByyRedConnectAction::ByyRedConnectAction(ByyApp& app, ByyMainWindow* mw, QWidget* parent /*=0*/)
    : ByyCheckableAction(app, "ByyRedConnectAction", parent),
	  logDialog_(nullptr),
	  clA(nullptr),
	  m_mw(mw)
{
    setText(QString::fromLocal8Bit("红方加入联机(&M)"));
    setIcon(QIcon("../data/icons/h.svg"));
	//0604
	setChecked(myApp.hdose().started()&&myApp.hdose().mode()==ByyHdose::Join);
	connect(&myApp.hdose(),SIGNAL(signal_runningChanged()),SLOT(checkEnabled()));
}

ByyRedConnectAction::~ByyRedConnectAction()
{

}

void ByyRedConnectAction::on_triggered()
{
	//改变角色为红方（1）
	myApp.core().remoteObjectManager().setShowRole((ByyRemoteObjectManager::ShowRole)1);
	if(!clA)
	{
		//未加载想定
		if(!ByyConfigIni::instance()->getisScn())
		{
			QMessageBox::warning(&myApp.mainWindow(),QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("请先加载想定！"));
			return;
		}else{
			scnPath = ByyConfigIni::instance()->scnPath();
		}
		// 初始化 LogDialog 只显示一次
		if (!logDialog_) {
			logDialog_ = new LogDialog(qobject_cast<QWidget*>(parent()));
			// 连接 LogDialog 的关闭信号,断开连接
			connect(logDialog_, SIGNAL(windowClosed()), this, SLOT(onDisconnect()));
			logDialog_->show();  // 弹出对话框
		}
		// 请求连接
		bool ok;
		ip=QInputDialog::getText(frameWnd(),CNTR("输入服务器IP地址"),"IP",QLineEdit::Normal,ByyHdoseSettings::instance().getRemoteSvrIP(),&ok);
		if(!ok)
			return;
		ByyHdoseSettings::instance().setRemoteSvrIP(ip);
		logDialog_->appendLog(QString::fromLocal8Bit("正在加载引擎..."));
		myApp.ig().block(true);
		myApp.core().block(true);
		myApp.hdose().start(ByyHdose::Join,ip);
		myApp.ig().block(false);
		myApp.core().block(false);
		ByyWaitBackEngineWidget *w=new ByyWaitBackEngineWidget(myApp.hdose(),&myApp.mainWindow());
		w->show();
		logDialog_->appendLog(QString::fromLocal8Bit("引擎加载成功！"));
		clA = new ClientA(this);
		clA->setScenarioPath(scnPath);
		connect(clA, SIGNAL(connected()), this, SLOT(onClientAConnected()));
		clA->connectToServer(ip,8888);
	}else{
		QMessageBox::warning(&myApp.mainWindow(),QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("请先关闭连接！"));
	}
}

void ByyRedConnectAction::checkEnabled()
{

}

void ByyRedConnectAction::onClientAConnected()
{
	logDialog_->appendLog(QString::fromLocal8Bit("红方已连接至服务器，等待服务器确认..."));
	connect(clA, SIGNAL(identified()), this, SLOT(onClientAIdentified()));
}

void ByyRedConnectAction::onClientAIdentified()
{
	logDialog_->appendLog(QString::fromLocal8Bit("服务器已确认红方，等待红方准备完毕..."));
	connect(clA, SIGNAL(readySent()), this, SLOT(onClientAReadied()));
}

void ByyRedConnectAction::onClientAReadied()
{
	logDialog_->appendLog(QString::fromLocal8Bit("红方已准备完毕！"));
}

void ByyRedConnectAction::onDisconnect()
{
	logDialog_ = nullptr;
	if (clA) {
        clA->deleteLater();  // 让 Qt 安全删除（非立即）
        clA = nullptr;      // 置空指针
    }
	myApp.hdose().stop();
}

//蓝方连接
ByyBlueConnectAction::ByyBlueConnectAction(ByyApp& app, ByyMainWindow* mw, QWidget* parent /*=0*/)
    : ByyCheckableAction(app, "ByyBlueConnectAction", parent),
	  logDialog_(nullptr),
	  clB(nullptr),
	  m_mw(mw)
{
    setText(QString::fromLocal8Bit("蓝方加入联机(&M)"));
    setIcon(QIcon("../data/icons/h.svg"));
	//0604
	setChecked(myApp.hdose().started()&&myApp.hdose().mode()==ByyHdose::Join);
	connect(&myApp.hdose(),SIGNAL(signal_runningChanged()),SLOT(checkEnabled()));
}

ByyBlueConnectAction::~ByyBlueConnectAction()
{

}

void ByyBlueConnectAction::on_triggered()
{
	//改变角色为蓝方（2）
	myApp.core().remoteObjectManager().setShowRole((ByyRemoteObjectManager::ShowRole)2);
	if(!clB)
	{
		//未加载想定
		if(!ByyConfigIni::instance()->getisScn())
		{
			QMessageBox::warning(&myApp.mainWindow(),QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("请先加载想定！"));
			return;
		}else{
			scnPath = ByyConfigIni::instance()->scnPath();
		}
		// 初始化 LogDialog 只显示一次
		if (!logDialog_) {
			logDialog_ = new LogDialog(qobject_cast<QWidget*>(parent()));
			// 连接 LogDialog 的关闭信号,断开连接
			connect(logDialog_, SIGNAL(windowClosed()), this, SLOT(onDisconnect()));
			logDialog_->show();  // 弹出对话框
		}
		// 请求连接
		bool ok;
		ip=QInputDialog::getText(frameWnd(),CNTR("输入服务器IP地址"),"IP",QLineEdit::Normal,ByyHdoseSettings::instance().getRemoteSvrIP(),&ok);
		if(!ok)
			return;
		ByyHdoseSettings::instance().setRemoteSvrIP(ip);
		logDialog_->appendLog(QString::fromLocal8Bit("正在加载引擎..."));
		myApp.ig().block(true);
		myApp.core().block(true);
		myApp.hdose().start(ByyHdose::Join,ip);
		myApp.ig().block(false);
		myApp.core().block(false);
		ByyWaitBackEngineWidget *w=new ByyWaitBackEngineWidget(myApp.hdose(),&myApp.mainWindow());
		w->show();
		logDialog_->appendLog(QString::fromLocal8Bit("引擎加载成功！"));
		clB = new ClientB(this);
		clB->setScenarioPath(scnPath);
		connect(clB, SIGNAL(connected()), this, SLOT(onClientBConnected()));
		clB->connectToServer(ip,8888);
	}else{
		QMessageBox::warning(&myApp.mainWindow(),QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("请先关闭连接！"));
	}
}

void ByyBlueConnectAction::checkEnabled()
{

}

void ByyBlueConnectAction::onClientBConnected()
{
	logDialog_->appendLog(QString::fromLocal8Bit("蓝方已连接至服务器，等待服务器确认..."));
	connect(clB, SIGNAL(identified()), this, SLOT(onClientBIdentified()));
}

void ByyBlueConnectAction::onClientBIdentified()
{
	logDialog_->appendLog(QString::fromLocal8Bit("服务器已确认蓝方，等待蓝方准备完毕..."));
	connect(clB, SIGNAL(readySent()), this, SLOT(onClientBReadied()));
}

void ByyBlueConnectAction::onClientBReadied()
{
	logDialog_->appendLog(QString::fromLocal8Bit("蓝方已准备完毕！"));
}

void ByyBlueConnectAction::onDisconnect()
{
	logDialog_ = nullptr;
	if (clB) {
        clB->deleteLater();  // 让 Qt 安全删除（非立即）
        clB = nullptr;      // 置空指针
    }
	myApp.hdose().stop();
}
//加载想定
LoadScenarioAction::LoadScenarioAction( ByyApp& app,QString scnPath,QWidget* parent/*=0*/ )
	:ByyAction(app,"LoadScenarioAction",parent),
	m_scnPath(scnPath)
{
	connect(&myApp.core().remoteObjectManager(),SIGNAL(showRoleChanged(int)),SLOT(checkEnabled()));
}

LoadScenarioAction::~LoadScenarioAction()
{

}

void LoadScenarioAction::on_triggered()
{
	if (ByyScenarioFileUtil::okToContinue(myApp))
	{
		QString fileName = m_scnPath;

		if (!fileName.isEmpty())
		{
			if (!myApp.hdose().scenarioManager().loadScenario(fileName))
			{
				QMessageBox::warning(&myApp.mainWindow(),QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("加载想定失败！"));
			}

			ByyConfigIni::instance()->setScnPath(fileName);
		}
	}
}

void LoadScenarioAction::checkEnabled()
{
	if(myApp.hdose().mode() == ByyHdose::Join)
	{
		setEnabled(myApp.core().remoteObjectManager().getShowRole() == ByyRemoteObjectManager::White);
	}
}

ByyConnectJoinHdoseAction::ByyConnectJoinHdoseAction( ByyApp& app,QWidget* parent/*=0*/ ) 
	: ByyCheckableAction(app,"ByyConnectJoinHdoseAction",parent)
{
	setText( QString::fromLocal8Bit("加入演练(&J)"));
	setIcon( QIcon("../data/icons/h.svg"));
	setChecked(myApp.hdose().started()&&myApp.hdose().mode()==ByyHdose::Join);
	connect(&myApp.hdose(),SIGNAL(signal_runningChanged()),SLOT(checkEnabled()));
}

ByyConnectJoinHdoseAction::~ByyConnectJoinHdoseAction()
{

}

void ByyConnectJoinHdoseAction::on_triggered()
{
	if (isChecked())
	{

		bool ok;
		QString ip=QInputDialog::getText(frameWnd(),CNTR("输入服务器IP地址"),"IP",QLineEdit::Normal,ByyHdoseSettings::instance().getRemoteSvrIP(),&ok);
		if(!ok)
			return;
		ByyHdoseSettings::instance().setRemoteSvrIP(ip);
		myApp.ig().block(true);
		myApp.core().block(true);
		myApp.hdose().start(ByyHdose::Join,ip);
		myApp.ig().block(false);
		myApp.core().block(false);

		ByyWaitBackEngineWidget *w=new ByyWaitBackEngineWidget(myApp.hdose(),&myApp.mainWindow());
		w->show();
	}
	else
		myApp.hdose().stop();
}

void ByyConnectJoinHdoseAction::checkEnabled()
{
	setEnabled((!myApp.hdose().started())||(myApp.hdose().started()&&myApp.hdose().mode()==ByyHdose::Join));
}

ByyConnectCreateHdoseAction::ByyConnectCreateHdoseAction( ByyApp& app,QWidget* parent/*=0*/ ) 
	: ByyCheckableAction(app,"ByyConnectCreateHdoseAction",parent)
{
	setText( QString::fromLocal8Bit("创建演练(&C)"));
	setIcon( QIcon("../data/icons/h.svg"));
	setChecked(myApp.hdose().started()&&myApp.hdose().mode()==ByyHdose::Create);
	connect(&myApp.hdose(),SIGNAL(signal_runningChanged()),SLOT(checkEnabled()));
}

ByyConnectCreateHdoseAction::~ByyConnectCreateHdoseAction()
{

}

void ByyConnectCreateHdoseAction::on_triggered()
{
	if (isChecked())
	{
		myApp.ig().block(true);
		myApp.core().block(true);
		myApp.hdose().start(ByyHdose::Create);
		myApp.ig().block(false);
		myApp.core().block(false);

		//ByyWaitBackEngineWidget *w=new ByyWaitBackEngineWidget(myApp.hdose(),&myApp.mainWindow());
		//w->show();
	}
	else
		myApp.hdose().stop();
}

void ByyConnectCreateHdoseAction::checkEnabled()
{
	setEnabled((!myApp.hdose().started())||(myApp.hdose().started()&&myApp.hdose().mode()==ByyHdose::Create));
}

ByyConnectVRLAction::ByyConnectVRLAction( ByyApp& app,QWidget* parent/*=0*/ ) 
	: ByyAction(app,"ByyConnectVRLAction",parent)
{
	setText( QString::fromLocal8Bit("MAK HLA(&M)"));
	setIcon( QIcon("../data/icons/vrlink.ico") );
}

ByyConnectVRLAction::~ByyConnectVRLAction()
{

}

void ByyConnectVRLAction::on_triggered()
{
	ByyVRLConnectWidget* w=new ByyVRLConnectWidget(myApp.core(),&myApp.mainWindow());
	w->setAttribute(Qt::WA_DeleteOnClose);
	w->setWindowFlags(Qt::Dialog);
	w->show();
}

ByyAudioSettingsAction::ByyAudioSettingsAction( ByyApp& app,QWidget* parent/*=0*/ ) 
	: ByyAction(app,"ByyAudioSettingsAction",parent)
{
	setText( QString::fromLocal8Bit("声音设置(&A)...") );
	setIcon( QIcon("../data/icons/audio.png") );
}

ByyAudioSettingsAction::~ByyAudioSettingsAction()
{

}

void ByyAudioSettingsAction::on_triggered()
{
	ByyAudioSettingsDialog *audioSettings=new ByyAudioSettingsDialog(&myApp.mainWindow());
	audioSettings->setAttribute(Qt::WA_DeleteOnClose);
	audioSettings->show();
}

ByyAudioAction::ByyAudioAction( ByyApp& app,QWidget* parent/*=0*/ )
	: ByyAction(app,"ByyAudioAction",parent)
{
	setText( QString::fromLocal8Bit("声音(&A)") );
	setIcon( QIcon("../data/icons/audio.png") );
}

ByyAudioAction::~ByyAudioAction()
{

}

void ByyAudioAction::on_triggered()
{
	ByyAudioSettings::instance().setEnabled(isChecked());
}

ByyStereoModeAction::ByyStereoModeAction( const osg::DisplaySettings::StereoMode& mode,ByyApp& app,const QString& objectName,QWidget* parent/*=0*/ ) 
	: ByyCheckableAction(app,objectName,parent)
	,myMode(mode)
{
	setChecked(myMode==osg::DisplaySettings::instance()->getStereoMode());
}

ByyStereoModeAction::~ByyStereoModeAction()
{

}

void ByyStereoModeAction::on_triggered()
{
	ByyStereoSettings::instance().setMode(myMode);
}

ByyQuadBufferStereoAction::ByyQuadBufferStereoAction( ByyApp& app,QWidget* parent/*=0*/ )
	: ByyStereoModeAction(osg::DisplaySettings::QUAD_BUFFER,app,"ByyQuadBufferStereoAction",parent)
{
	setText(QString::fromLocal8Bit("主动立体(&Q)"));
}

ByyQuadBufferStereoAction::~ByyQuadBufferStereoAction()
{

}

ByyHSplitStereoAction::ByyHSplitStereoAction( ByyApp& app,QWidget* parent/*=0*/ )
	: ByyStereoModeAction(osg::DisplaySettings::HORIZONTAL_SPLIT,app,"ByyHSplitStereoAction",parent)
{
	setText(QString::fromLocal8Bit("水平分割(&H)"));
}

ByyHSplitStereoAction::~ByyHSplitStereoAction()
{

}

ByyAnaglyphicStereoAction::ByyAnaglyphicStereoAction( ByyApp& app,QWidget* parent/*=0*/ )
	: ByyStereoModeAction(osg::DisplaySettings::ANAGLYPHIC,app,"ByyAnaglyphicStereoAction",parent)
{
	setText(QString::fromLocal8Bit("红蓝立体(&A)"));
}

ByyAnaglyphicStereoAction::~ByyAnaglyphicStereoAction()
{

}

ByyPassivityStereoAction::ByyPassivityStereoAction( ByyApp& app,QWidget* parent/*=0*/ )
	: ByyStereoModeAction(osg::DisplaySettings::CHECKERBOARD,app,"ByyPassivityStereoAction",parent)
{
	setEnabled(false);
	setText(QString::fromLocal8Bit("被动立体(&P)"));
}

ByyPassivityStereoAction::~ByyPassivityStereoAction()
{

}

ByyStereoSettingsAction::ByyStereoSettingsAction( ByyApp& app,QWidget* parent/*=0*/ )
	:ByyAction(app,"ByyStereoSettingsAction",parent)
{
	setText(QString::fromLocal8Bit("立体设置(&E)"));
}

ByyStereoSettingsAction::~ByyStereoSettingsAction()
{

}

void ByyStereoSettingsAction::on_triggered()
{
	ByyStereoSettingsDialog *stereoSettings=new ByyStereoSettingsDialog(&myApp.mainWindow());
	stereoSettings->setAttribute(Qt::WA_DeleteOnClose);
	stereoSettings->show();
}

ByyStereoShowAction::ByyStereoShowAction( ByyApp& app,QWidget* parent/*=0*/ )
		:ByyCheckableAction(app,"ByyStereoShowAction",parent)
{
	setText(QString::fromLocal8Bit("立体显示(&R)"));
	setIcon(QIcon("../data/icons/3d.png"));
}

ByyStereoShowAction::~ByyStereoShowAction()
{

}

void ByyStereoShowAction::on_triggered()
{
	ByyStereoSettings::instance().setEnabled(isChecked());
}

ByyEnvironmentSettingsAction::ByyEnvironmentSettingsAction( ByyApp& app,QWidget* parent/*=0*/ )
	:ByyAction(app,"ByyEnvironmentSettingsAction",parent)
{
	setText(QString::fromLocal8Bit("天象海况(&E)"));
	setIcon(QIcon("../data/icons/TerrainScene.svg"));
}

ByyEnvironmentSettingsAction::~ByyEnvironmentSettingsAction()
{

}

void ByyEnvironmentSettingsAction::on_triggered()
{
	myApp.mainWindow().showEnviromentWidget();
}

ByyTerrainEffectAction::ByyTerrainEffectAction( ByyApp& app,QWidget* parent/*=0*/ )
	:ByyAction(app,"ByyTerrainEffectAction",parent)
{
	setText(QString::fromLocal8Bit("地形特效(&T)"));
	setIcon(QIcon("../data/icons/terraineffects.png"));
}

ByyTerrainEffectAction::~ByyTerrainEffectAction()
{

}

void ByyTerrainEffectAction::on_triggered()
{
	myApp.mainWindow().showTerrainEffectWidget();
}

ByyScreenRecordAction::ByyScreenRecordAction( ByyApp& app,QWidget* parent/*=0*/ )
	:ByyAction(app,"ByyScreenRecordAction",parent)
{
	setText(QString::fromLocal8Bit("影像拾取(&V)"));
	setIcon(QIcon("../data/icons/video.png"));
}

ByyScreenRecordAction::~ByyScreenRecordAction()
{

}

void ByyScreenRecordAction::on_triggered()
{
	myApp.mainWindow().showScreenRecordWidget();
}

ByyGroundClampAction::ByyGroundClampAction( ByyApp& app,QWidget* parent/*=0*/ )
	:ByyCheckableAction(app,"ByyGroundClampAction",parent)
{
	setText(QString::fromLocal8Bit("贴在地面(&G)"));
	setChecked(ByyEntitySettings::instance().groundClampEnabled());
	setIcon(QIcon("../data/icons/DisplayGroundClamp.png"));
	connect(this,SIGNAL(triggered(bool)),&ByyEntitySettings::instance(),SLOT(setGroundClampEnabled(bool)));
}

ByyGroundClampAction::~ByyGroundClampAction()
{

}

ByyEntityScaleAction::ByyEntityScaleAction( ByyApp& app,QWidget* parent/*=0*/ )
	:ByyCheckableAction(app,"ByyEntityScaleAction",parent)
{
	setText(QString::fromLocal8Bit("实体缩放(&S)"));
	setChecked(ByyEntitySettings::instance().autoScaleEnabled());
	connect(this,SIGNAL(triggered(bool)),&ByyEntitySettings::instance(),SLOT(setAutoScaleEnabled(bool)));
}

ByyEntityScaleAction::~ByyEntityScaleAction()
{

}

/*
ByyEntityLabelAction::ByyEntityLabelAction( ByyApp& app,QWidget* parent/ *=0* / )
	:ByyCheckableAction(app,"ByyEntityLabelAction",parent)
{
	setText(QString::fromLocal8Bit("实体标签(&P)"));
	setChecked(ByyEntitySettings::instance().entityLabelEnabled());
	setIcon(QIcon("../data/icons/circle_red.png"));
	connect(this,SIGNAL(triggered(bool)),&ByyEntitySettings::instance(),SLOT(setEntityLabelEnabled(bool)));
}

ByyEntityLabelAction::~ByyEntityLabelAction()
{

}

ByyEntityNameAction::ByyEntityNameAction( ByyApp& app,QWidget* parent )
:ByyCheckableAction(app,"ByyEntityNameAction",parent)
{
	setText(QString::fromLocal8Bit("实体名称(&N)"));
	setChecked(ByyEntitySettings::instance().entityNameEnabled());
	setIcon(QIcon("../data/icons/EntityInformation.png"));
	connect(this,SIGNAL(triggered(bool)),&ByyEntitySettings::instance(),SLOT(setEntityNameEnabled(bool)));
}

ByyEntityNameAction::~ByyEntityNameAction()
{

}

ByyCtrlObjAction::ByyCtrlObjAction( ByyApp& app,QWidget* parent )
	:ByyCheckableAction(app,"ByyCtrlObjAction",parent)
{
	setText(QString::fromLocal8Bit("战术图形(&Z)"));
	setIcon(QIcon("../data/icons/DisplayTacticalGraphics.svg"));
	setChecked(true);
	connect(this,SIGNAL(triggered(bool)),myApp.display().sceneObjectManager(),SLOT(setControlObjectVisible(bool)));
}

ByyCtrlObjAction::~ByyCtrlObjAction()
{

}

ByyTrajectoryAction::ByyTrajectoryAction( ByyApp& app,QWidget* parent )
	:ByyCheckableAction(app,"ByyTrajectoryAction",parent)
{
	setText(QString::fromLocal8Bit("飞行轨迹(&T)"));
	setChecked(ByyEntitySettings::instance().trackHistoryEnabled());
	setIcon(QIcon("../data/icons/DisplayTrajectories.png"));
	connect(this,SIGNAL(triggered(bool)),&ByyEntitySettings::instance(),SLOT(setTrackHistoryEnabled(bool)));
}

ByyTrajectoryAction::~ByyTrajectoryAction()
{

}
*/
ByyFullScreenAction::ByyFullScreenAction( ByyApp& app,QWidget* parent/*=0*/ )
	:ByyCheckableAction(app,"ByyFullScreenAction",parent)
{
	setText(QString::fromLocal8Bit("全屏显示"));
	setIcon(QIcon("../data/icons/fullscreen.png"));
	setShortcut(QKeySequence("Ctrl+Enter"));
}

ByyFullScreenAction::~ByyFullScreenAction()
{

}

void ByyFullScreenAction::on_triggered()
{
	myApp.mainWindow().showFullScreen(isChecked());
}

ByyOceanVisibleAction::ByyOceanVisibleAction( ByyApp& app,QWidget* parent/*=0*/ )
:ByyCheckableAction(app,"ByyOceanVisibleAction",parent)
{
	setText(QString::fromLocal8Bit("动态海洋(&D)"));
	setIcon(QIcon("../data/icons/TerrainWater.svg"));
	setChecked(ByyObserverSettings::instance().oceanVisible());
}

ByyOceanVisibleAction::~ByyOceanVisibleAction()
{

}

void ByyOceanVisibleAction::on_triggered()
{
	ByyObserverSettings::instance().setOceanVisible(isChecked());
}

ByyHdoseSettingsAction::ByyHdoseSettingsAction( ByyApp& app,QWidget* parent/*=0*/ )
	:ByyAction(app,"ByyHdoseSettingsAction",parent)
{
	setText(QString::fromLocal8Bit("Hdose设置(&H)"));
}

ByyHdoseSettingsAction::~ByyHdoseSettingsAction()
{

}

void ByyHdoseSettingsAction::on_triggered()
{
	ByyHdoseSettingsWidget *w=new ByyHdoseSettingsWidget(&myApp.mainWindow());
	w->setAttribute(Qt::WA_DeleteOnClose);
	w->show();
}
/*

ByyEntityInfoAction::ByyEntityInfoAction( ByyApp& app,QWidget* parent/ *=0* / )
	:ByyCheckableAction(app,"ByyEntityInfoAction",parent)
{
	setText(QString::fromLocal8Bit("实体信息(&I)"));
	setIcon(QIcon("../data/icons/stock_new_labels.png"));

	setChecked(ByyEntitySettings::instance().entityInfoEnabled());
}

ByyEntityInfoAction::~ByyEntityInfoAction()
{

}


void ByyEntityInfoAction::on_triggered()
{
	ByyEntitySettings::instance().setEntityInfoEnabled(isChecked());
}
*/

Byy2DProjectionAction::Byy2DProjectionAction( ByyApp& app,QWidget* parent/*=0*/ )
	:ByyCheckableAction(app,"Byy2DProjectionAction",parent)
{
	setText(QString::fromLocal8Bit("二维地图(&P)"));
	setIcon(QIcon("../data/icons/coordinate_axis.svg"));
	setChecked(ByyObserverSettings::instance().is2D());
}

Byy2DProjectionAction::~Byy2DProjectionAction()
{

}

void Byy2DProjectionAction::on_triggered()
{
	ByyObserverSettings::instance().set2D(isChecked());
}

ByyScreenLogAction::ByyScreenLogAction( ByyApp& app,QWidget* parent/*=0*/ )
	:ByyCheckableAction(app,"ByyScreenLogAction",parent)
{
	setText(QString::fromLocal8Bit("屏幕日志(&S)"));
	//setIcon(QIcon("../data/icons/coordinate_axis.svg"));
	setChecked(true);
}

ByyScreenLogAction::~ByyScreenLogAction()
{

}
#include "ByyLogCanvas.h"
void ByyScreenLogAction::on_triggered()
{
	myApp.ig().logCanvas()->setVisible(isChecked());
}

ByyShowScanAction::ByyShowScanAction( ByyApp& app,QWidget* parent/*=0*/ )
	:ByyCheckableAction(app,"ByyShowScanAction",parent)
{
	setText(QString::fromLocal8Bit("特效展示(&T)"));
	//setIcon(QIcon("../data/icons/coordinate_axis.svg"));
	setChecked(false);
}

ByyShowScanAction::~ByyShowScanAction()
{

}
#include "Scan.h"
void ByyShowScanAction::on_triggered()
{
	if (myRoot.valid())
	{
		myApp.ig().renderer()->removeNodeFromRoot(myRoot,ByyOsgRenderer::ControlObject);
		myRoot=0;
	}
	else
	{
		myRoot=new osg::Group;
		osg::Vec3d pos2(110.0,34.0,1000000.0);

		osg::Vec3d vecXYZ2;
		FeUtil::DegreeLLH2XYZ(pos2,vecXYZ2);
		osg::ref_ptr<CScan > rpScan = new CScan(vecXYZ2);

		/// 加载圆锥扫描波
		osg::ref_ptr<TestNode> tn = new TestNode(1000000,osg::DegreesToRadians(6.0f));
		tn->setMoveSpeed(120000);
		tn->setWaveColor(osg::Vec3d(1.0,0.5,0.0));

		osg::MatrixTransform* tnMt = new osg::MatrixTransform();
		osg::Vec3d tnPos(105.0,34.0,10000);
		osg::Matrix tnMat;
		FeUtil::DegreeLLH2Matrix(tnPos,tnMat);
		tnMt->setMatrix(tnMat);

		tnMt->addChild(tn);


		osg::ref_ptr<PyramidNode> pn = new PyramidNode(1000000,osg::DegreesToRadians(30.0f),osg::DegreesToRadians(20.0f));

		osg::MatrixTransform* pnMt = new osg::MatrixTransform();
		osg::Vec3d pnPos(100.0,34.0,1000000);
		osg::Matrix pnMat;
		FeUtil::DegreeLLH2Matrix(pnPos,pnMat);
		pnMt->setMatrix(pnMat);

		pnMt->addChild(pn);


		/// 加载搜索屏
		osg::Vec3d ssPos(95.0,34.0,10000);
		float ssRadius = 800000;
		float ssa = 0;
		float sse = 60;
		SearchScreenPointList sspl;
		for (int i = 0;i < 100;i++)
		{
			SearchScreenPoint ssp(ssRadius,osg::DegreesToRadians(ssa),DegreesToRadians(sse));
			sspl.push_back(ssp);

			ssa += 1.8;
		}
		osg::ref_ptr<SearchScreen> ss = new SearchScreen(sspl,ssPos);

		osg::Vec3d ssPos1(90.0,34.0,10000);
		float ssRadius1 = 800000;
		float ssa1 = 0;
		float sse1 = 60;
		SearchScreenPointList sspl1;
		for (int i = 0;i < 50;i++)
		{
			float ie = sse1;
			if(i % 2 == 0)
			{
				ie += 5;
			}

			SearchScreenPoint ssp1(ssRadius1,osg::DegreesToRadians(ssa1),DegreesToRadians(ie));
			sspl1.push_back(ssp1);

			ssa1 += 3.6;
		}
		osg::ref_ptr<SearchScreen> ss1 = new SearchScreen(sspl1,ssPos1);

		/// 加载误差管道
		osg::Vec3d plPos(85.0,34.0,1000000);
		float plMaxRadius = 100;
		float lonAdd = -0.1;
		float latAdd = -0.04;
		float altAdd = -10000;
		float rAdd = 1000;
		PipeLine::CenterLineList cl;
		for (int i = 0;i < 100;i++)
		{
			osg::Vec4 cp(plPos.x() + i * lonAdd,plPos.y() + i * latAdd,plPos.z() + i * altAdd,plMaxRadius+ i * rAdd);
			cl.push_back(cp);
		}
		osg::ref_ptr<PipeLine> pl = new PipeLine(cl);


		osg::Vec3d plPos1(85.0,34.0,1000000);
		float plMaxRadius1 = 100;
		float lonAdd1 = -0.1;
		float latAdd1 = 0.04;
		float altAdd1 = -10000;
		float rAdd1 = 1000;
		PipeLine::CenterLineList cl1;
		for (int i = 0;i < 100;i++)
		{
			osg::Vec4 cp(plPos1.x() + i * lonAdd1,plPos1.y() + i * latAdd1,plPos1.z() + i * altAdd1,plMaxRadius1+ i * rAdd1);
			cl1.push_back(cp);
		}
		osg::ref_ptr<PipeLine> pl1 = new PipeLine(cl1);
		pl1->setOutLineVisible(false);
		pl1->setDiffColor(osg::Vec3(1.0,0.0,0.0));
		pl1->setPipeColor(osg::Vec4(1.0,1.0,0.0,0.5));
		pl1->setPolygonMode(osg::PolygonMode::LINE);

		myRoot->addChild(rpScan);
		myRoot->addChild(tnMt);
		myRoot->addChild(pnMt);
		myRoot->addChild(ss);
		myRoot->addChild(ss1);
		myRoot->addChild(pl);
		myRoot->addChild(pl1);

		myApp.ig().renderer()->addNodeToRoot(myRoot,ByyOsgRenderer::ControlObject);
	}
}

#include "statisticsWidget.h"
ByyShowStatAction::ByyShowStatAction( ByyApp& app,QWidget* parent/*=0*/ )
	:ByyCheckableAction(app,"ByyShowScanAction",parent)
{
	setText(QString::fromLocal8Bit("统计"));
	//setIcon(QIcon("../data/icons/coordinate_axis.svg"));
	setIcon(QIcon("../data/icons/tongji.png"));
	setChecked(false);

	Statisticswidget::instance()->setApp(&app);
}

ByyShowStatAction::~ByyShowStatAction()
{

}

void ByyShowStatAction::on_triggered()
{
	Statisticswidget::instance()->setWindowFlags(Statisticswidget::instance()->windowFlags() | Qt::WindowStaysOnTopHint);
	Statisticswidget::instance()->show();
}
#include "indexwidget.h"
ByyShowIndexAction::ByyShowIndexAction( ByyApp& app,QWidget* parent/*=0*/ )
	:ByyCheckableAction(app,"ByyShowScanAction",parent)
{
	setText(QString::fromLocal8Bit("指标"));
	setIcon(QIcon("../data/icons/index.png"));
	setChecked(false);

}

ByyShowIndexAction::~ByyShowIndexAction()
{

}

void ByyShowIndexAction::on_triggered()
{
	IndexWidget::instance()->calcAndShow();
	IndexWidget::instance()->setWindowFlags(Statisticswidget::instance()->windowFlags() | Qt::WindowStaysOnTopHint);
	IndexWidget::instance()->show();
}