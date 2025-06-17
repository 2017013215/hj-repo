#ifndef SIMUCONTROLLER_H
#define SIMUCONTROLLER_H

#include <QObject>
#include <QDateTime>
#include "snighdose_global.h"

class ByyHdose;
class ByyScenarioManager;
class CForeStation;
class ByyTask;
class ByyCore;

class SNIGHDOSE_EXPORT ByySimuController : public QObject
{
	Q_OBJECT
public:
	ByySimuController(ByyCore& core,ByyHdose& hdose,QObject *parent=0);
	virtual ~ByySimuController();

	enum SimuStatus{S_NULL=0,
		S_LOADING=1,
		S_LOADED=2,
		S_INITING=3,
		S_INITED=4,
		S_RUNING=5,
		S_PAUSE=59,
		S_EXITING=-1,
		S_EXITED=-2
	};

	SimuStatus status()const;

	CForeStation* foreStation();

	static void backEngineStateCallback(int state,int bestFast,void* user);

	const QDateTime& startDateTime();

	bool scnLoaded();

	bool isRunning();
	bool isPaused();
	bool isActive();
	bool isFastBest();

	void sendTargetControlEvt(std::string targetName, int direction);

public slots:
	void init(int mode);
	void release();

	bool start();
	bool pause();
	bool restart();

	bool fastBest();
	bool normal();

	bool loadScenario(char *fn);
	bool closeScenario();

	bool requestBackEngState();
	void processBackEngineStateCallback(int state,int bestFast);

	void createObject(int objType,const QString& itemClass,const QString& attributeValueSet);
	void removeObject(int objType,long id);

	void sendMoveToTask(int recverId,const QString& ctrlObjName);
	void sendMoveAlongTask(int recverId,const QString& ctrlObjName);
	void sendPatrolTask(int recverId,int type,const QString& ctrlObjName,long time);
	void sendFollowTask(int recverId,const QString& ObjName,int distance,int relativeAngle,int highOffset);
	void sendWaitTask(int recverId,int waitTime);
	void sendCruiseTask(int recverId);
	void sendLaunchMissileTask(int recverId,int targetId);
	void sendTakeOffTask(int recverId);
	void sendSenorOper(int recverId,const QString& device,bool powerOn);

protected slots:
	void checkBackState();

protected:
	inline void changeStatus(SimuStatus statue);
	void processBackExit();

signals:
	void signal_StatusChanged(int statue);
	void signal_scnLoaded();
	void signal_backExited();
	void signal_fastBestChanged(bool);
	void signal_reloadScn();

protected:
	SimuStatus myStatus;
	ByyHdose& myHdose;
	ByyCore& myCore;
	ByyScenarioManager& myScenarioManager;
	CForeStation* myForeStation;

	QDateTime myStartDateTime;

	bool myScnLoaded;

	bool myWaitBackendReply;

	int myBackendNoReplyNum;

	bool myFastBest;
};

#endif // SIMUCONTROLLER_H
