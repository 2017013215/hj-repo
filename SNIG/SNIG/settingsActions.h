#ifndef SETTINGSMENUITEMS_H
#define SETTINGSMENUITEMS_H

#include "ByyAction.h"
#include "LogDialog.h"
#include "tcpServer.h"
#include "mainWindow.h"
#include <QObject>
#include "ClientA.h"
#include "ClientB.h"
#include "ScenarioMerger.h"
class ByyConnectSingleHdoseAction : public ByyCheckableAction
{
	Q_OBJECT
public:
	ByyConnectSingleHdoseAction(ByyApp& app,QWidget* parent=0);
	~ByyConnectSingleHdoseAction();

protected slots:
	void on_triggered();
	void checkEnabled();
};

//联机模式
class ByyConnectMultiHdoseAction : public ByyCheckableAction
{
	Q_OBJECT
public:
    ByyConnectMultiHdoseAction(ByyApp& app, ByyMainWindow* mw, QWidget* parent = 0);
    ~ByyConnectMultiHdoseAction();

private:
    bool clientA_ready_; 
    bool clientB_ready_;
    bool waiting_to_start_;
    void tryStartHdose();
	LogDialog *logDialog_;
	tcpServer* server;
	ByyMainWindow* m_mw;
	QProcess process;
	QString exePath;

private slots:
    void on_triggered();
    void checkEnabled();
	void onClientAReady();
    void onClientBReady();
	void onDisconnect();
    void onLoadSuccess();
	void minimizeServerWindow();
};

//红方指挥
class ByyRedConnectAction : public ByyCheckableAction
{
	Q_OBJECT
public:
    ByyRedConnectAction(ByyApp& app, ByyMainWindow* mw, QWidget* parent = 0);
    ~ByyRedConnectAction();

private:
	ByyMainWindow* m_mw;
	LogDialog *logDialog_;
	ClientA* clA;
	QString ip;
	QString scnPath;
private slots:
	void on_triggered();
    void checkEnabled();
	void onClientAConnected();
	void onClientAIdentified();
	void onClientAReadied();
	void onDisconnect();
};

//蓝方指挥
class ByyBlueConnectAction : public ByyCheckableAction
{
	Q_OBJECT
public:
    ByyBlueConnectAction(ByyApp& app, ByyMainWindow* mw, QWidget* parent = 0);
    ~ByyBlueConnectAction();

private:
	ByyMainWindow* m_mw;
	LogDialog *logDialog_;
	ClientB* clB;
	QString ip;
	QString scnPath;
private slots:
	void on_triggered();
    void checkEnabled();
	void onClientBConnected();
	void onClientBIdentified();
	void onClientBReadied();
	void onDisconnect();
};

//加载想定
class LoadScenarioAction : public ByyAction
{
	Q_OBJECT
public:
	LoadScenarioAction(ByyApp& app,QString scnPath,QWidget* parent=0);
	~LoadScenarioAction();
private:
	QString m_scnPath;
protected slots:
	void on_triggered();
	void checkEnabled();

signals:
    void loadSuccess();  // 加载成功信号
};

class ByyConnectJoinHdoseAction : public ByyCheckableAction
{
	Q_OBJECT
public:
	ByyConnectJoinHdoseAction(ByyApp& app,QWidget* parent=0);
	~ByyConnectJoinHdoseAction();

protected slots:
	void on_triggered();
	void checkEnabled();
};

class ByyConnectCreateHdoseAction : public ByyCheckableAction
{
	Q_OBJECT
public:
	ByyConnectCreateHdoseAction(ByyApp& app,QWidget* parent=0);
	~ByyConnectCreateHdoseAction();

	protected slots:
		void on_triggered();
		void checkEnabled();
};

class ByyConnectVRLAction : public ByyAction
{
	Q_OBJECT
public:
	ByyConnectVRLAction(ByyApp& app,QWidget* parent=0);
	~ByyConnectVRLAction();

protected slots:
	void on_triggered();

};

class ByyAudioSettingsAction : public ByyAction
{
	Q_OBJECT
public:
	ByyAudioSettingsAction(ByyApp& app,QWidget* parent=0);
	~ByyAudioSettingsAction();

protected slots:
	void on_triggered();

};

class ByyAudioAction : public ByyAction
{
	Q_OBJECT
public:
	ByyAudioAction(ByyApp& app,QWidget* parent=0);
	~ByyAudioAction();

protected slots:
	void on_triggered();

};

class ByyStereoModeAction : public ByyCheckableAction
{
	Q_OBJECT
public:
	ByyStereoModeAction(const osg::DisplaySettings::StereoMode& mode,ByyApp& app,const QString& objectName,QWidget* parent=0);
	virtual ~ByyStereoModeAction();

protected slots:
	void on_triggered();

protected:
	osg::DisplaySettings::StereoMode myMode;

};

class ByyQuadBufferStereoAction : public ByyStereoModeAction
{
	Q_OBJECT
public:
	ByyQuadBufferStereoAction(ByyApp& app,QWidget* parent=0);
	~ByyQuadBufferStereoAction();
};

class ByyHSplitStereoAction : public ByyStereoModeAction
{
	Q_OBJECT
public:
	ByyHSplitStereoAction(ByyApp& app,QWidget* parent=0);
	~ByyHSplitStereoAction();

};
class ByyAnaglyphicStereoAction : public ByyStereoModeAction
{
	Q_OBJECT
public:
	ByyAnaglyphicStereoAction(ByyApp& app,QWidget* parent=0);
	~ByyAnaglyphicStereoAction();

};
class ByyPassivityStereoAction : public ByyStereoModeAction
{
	Q_OBJECT
public:
	ByyPassivityStereoAction(ByyApp& app,QWidget* parent=0);
	~ByyPassivityStereoAction();

};

class ByyStereoSettingsAction : public ByyAction
{
	Q_OBJECT
public:
	ByyStereoSettingsAction(ByyApp& app,QWidget* parent=0);
	~ByyStereoSettingsAction();

protected slots:
	void on_triggered();

};

class ByyStereoShowAction : public ByyCheckableAction
{
	Q_OBJECT
public:
	ByyStereoShowAction(ByyApp& app,QWidget* parent=0);
	~ByyStereoShowAction();

	protected slots:
		void on_triggered();

};

class ByyEnvironmentSettingsAction : public ByyAction
{
	Q_OBJECT
public:
	ByyEnvironmentSettingsAction(ByyApp& app,QWidget* parent=0);
	~ByyEnvironmentSettingsAction();

protected slots:
	void on_triggered();

};

class ByyTerrainEffectAction : public ByyAction
{
	Q_OBJECT
public:
	ByyTerrainEffectAction(ByyApp& app,QWidget* parent=0);
	~ByyTerrainEffectAction();

protected slots:
	void on_triggered();

};

class ByyGridAction : public ByyAction
{
	Q_OBJECT
public:
	ByyGridAction(ByyApp& app,QWidget* parent=0);
	~ByyGridAction();

protected slots:
	void on_triggered();

};

class ByyScreenRecordAction : public ByyAction
{
	Q_OBJECT
public:
	ByyScreenRecordAction(ByyApp& app,QWidget* parent=0);
	~ByyScreenRecordAction();

protected slots:
	void on_triggered();

};

class ByyGroundClampAction : public ByyCheckableAction
{
	Q_OBJECT
public:
	ByyGroundClampAction(ByyApp& app,QWidget* parent=0);
	~ByyGroundClampAction();
};

class ByyEntityScaleAction : public ByyCheckableAction
{
public:
	ByyEntityScaleAction(ByyApp& app,QWidget* parent=0);
	~ByyEntityScaleAction();
};

/*
class ByyEntityLabelAction : public ByyCheckableAction
{
public:
	ByyEntityLabelAction(ByyApp& app,QWidget* parent=0);
	~ByyEntityLabelAction();
};


class ByyEntityNameAction : public ByyCheckableAction
{
public:
	ByyEntityNameAction(ByyApp& app,QWidget* parent=0);
	~ByyEntityNameAction();
};


class ByyCtrlObjAction : public ByyCheckableAction
{
public:
	ByyCtrlObjAction(ByyApp& app,QWidget* parent=0);
	~ByyCtrlObjAction();
};

class ByyTrajectoryAction : public ByyCheckableAction
{
public:
	ByyTrajectoryAction(ByyApp& app,QWidget* parent=0);
	~ByyTrajectoryAction();
};
*/
class ByyFullScreenAction : public ByyCheckableAction
{
	Q_OBJECT
public:
	ByyFullScreenAction(ByyApp& app,QWidget* parent=0);
	~ByyFullScreenAction();

protected slots:
	void on_triggered();

};

class ByyOceanVisibleAction : public ByyCheckableAction
{
	Q_OBJECT
public:
	ByyOceanVisibleAction(ByyApp& app,QWidget* parent=0);
	~ByyOceanVisibleAction();

protected slots:
	void on_triggered();

};

class ByyHdoseSettingsAction : public ByyAction
{
	Q_OBJECT
public:
	ByyHdoseSettingsAction(ByyApp& app,QWidget* parent=0);
	~ByyHdoseSettingsAction();

protected slots:
	void on_triggered();

};


/*
class ByyEntityInfoAction : public ByyCheckableAction
{
public:
	ByyEntityInfoAction(ByyApp& app,QWidget* parent=0);
	~ByyEntityInfoAction();

protected slots:
	void on_triggered();

};*/

class Byy2DProjectionAction : public ByyCheckableAction
{
	Q_OBJECT
public:
	Byy2DProjectionAction(ByyApp& app,QWidget* parent=0);
	~Byy2DProjectionAction();

	protected slots:
		void on_triggered();

};

class ByyScreenLogAction : public ByyCheckableAction
{
	Q_OBJECT
public:
	ByyScreenLogAction(ByyApp& app,QWidget* parent=0);
	~ByyScreenLogAction();

protected slots:
	void on_triggered();

};

class ByyShowScanAction : public ByyCheckableAction
{
	Q_OBJECT
public:
	ByyShowScanAction(ByyApp& app,QWidget* parent=0);
	~ByyShowScanAction();

protected slots:
	void on_triggered();

protected:
	osg::ref_ptr<osg::Group> myRoot;

};

class ByyShowStatAction : public ByyCheckableAction
{
	Q_OBJECT
public:
	ByyShowStatAction(ByyApp& app,QWidget* parent=0);
	~ByyShowStatAction();

protected slots:
	void on_triggered();

protected:

};


class ByyShowIndexAction : public ByyCheckableAction
{
	Q_OBJECT
public:
	ByyShowIndexAction(ByyApp& app,QWidget* parent=0);
	~ByyShowIndexAction();

protected slots:
	void on_triggered();

protected:

};
#endif // SETTINGSMENUITEMS_H
