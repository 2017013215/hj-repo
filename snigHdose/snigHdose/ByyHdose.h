#ifndef HDOSE_H
#define HDOSE_H

#include <QObject>
#include <QTimer>
#include "enums.h"
#include "snighdose_global.h"


#include <QDateTime>
#include <QVariant>


class ByyCore;
class ByyHdoseDriver;
class ByyOpd;
class ByyRsm;
class ByySimuController;
class ByyScenarioManager;
class ByyEntityObject;
class ByyControlObject;
class ByyKeyValueList;
class ByyTask;
class ByyForeStationProcessor;
class ByyHdoseSimEventLogDatabase;
class ByyAttributeEditMng;


namespace osg
{
	class Vec3d;
}

class SNIGHDOSE_EXPORT ByyHdose : public QObject
{
	Q_OBJECT

public:
	ByyHdose(ByyCore& core,QObject *parent=0);
	virtual ~ByyHdose();

	enum Mode{Single,Join,Create,End};

	bool start(Mode mode=Join,QString ip="");
	bool stop();

	inline bool started() const;

	ByyOpd& opd();
	ByyRsm& rsm();
	ByyAttributeEditMng& attrEditTypeMng();

	ByyCore& core();

	Mode mode();

	ByySimuController& simuController();
	ByyScenarioManager& scenarioManager();
	ByyForeStationProcessor* foreStationProcessor();
	ByyHdoseSimEventLogDatabase* simEventLogDatabase();

	ByyEntityObject* createEntity(const QString& assemble,const ByyKeyValueList& keyValues);
	ByyEntityObject* createEntity(const QString& assemble,ByyForceType ft,const osg::Vec3d& position);

	void appendControlObject(ByyControlObject *obj,ByyForceType ft);

	void removeEntity(long id,const QString& name);
	void removeControlObject(long id,const QString& name);

	void sendTask(ByyTask *task);

	double simTime();
	bool simDateTime(QDateTime& ret);

	double logicClock();

	void setSimMultiplier(double val);

	// type:1-反舰导弹  2-拦截弹 3--命中 4-被拦截 
	void setExploInfo(int typ, QString name = QString());
	
	void	snapShot();
	// wqb 新增弹药命中函数 
	// targetEntName targetId 目标信息
	//RecverEntName RecverEntID 实体信息
	//HittedPosX HittedPosY HittedPosZ 碰撞位置信息 
	void  onHit(int targetId,QString targetEntName ,int RecverEntID,QString RecverEntName,double HittedPosX,double HittedPosY,double HittedPosZ);
	//HittedPosX HittedPosY HittedPosZ 碰撞位置信息 
	void  onHit(const QVariantMap& hitEventInfo);
	// wqb 新增导弹发送信息
	// targetEntName targetId 目标信息
	//RecverEntName RecverEntID 实体信息
	//side  0 红方 1蓝方
	void onMissileLaunch(int side, int targetId,QString targetEntName ,int RecverEntID,QString RecverEntName);
	void onMissileLaunch(int side, const QString& missileName, const QString& missileType);

protected slots:
	void onBackScnLoaded();
	void onBackExited();
	void onBackStateChanged(int state);
	void onTick();

signals:
	void signal_started();
	void signal_scnLoaded();
	void signal_entityUpdate();
	void signal_stopVerdict();
	void signal_stopped();
	void signal_runningChanged();
	// wqb 新增弹药命中函数 
	// targetEntName targetId 目标信息
	//RecverEntName RecverEntID 实体信息
	//HittedPosX HittedPosY HittedPosZ 碰撞位置信息 
	//
	void signal_Hit(int targetId,QString targetEntName ,int RecverEntID,QString RecverEntName,double HittedPosX,double HittedPosY,double HittedPosZ);
	void signal_Hit(const QVariantMap& hitEventInfo);
	// wqb 新增导弹发送信息
	// targetEntName targetId 目标信息
	//RecverEntName RecverEntID 实体信息
	//side  0 红方 1蓝方
	void signal_MissileLaunch(int side,int targetId,QString targetEntName ,int RecverEntID,QString RecverEntName);
	void signal_MissileLaunch(int side, const QString& missileName, const QString& missileType);
	//void signal_Hit(CExplosionEvt* pevent);
	// type:1-反舰导弹  2-拦截弹 3--命中 4-被拦截
	void signal_missileTJ(int type);
	// type:1-红方  2-蓝方 
	void signal_entityDm(int side, const QString& objectName);
	void signal_entityDm(int side, const QString& objectName, const QString& typeName);
	// type:1-红方  2-蓝方 
	void signal_misslieInfo(int id, double height, double time, double v, QString name, int type);

protected:
	ByyCore& myCore;
	ByySimuController* mySimuController;
	ByyHdoseDriver* myDriver;
	ByyScenarioManager* myScenarioManager;
	ByyForeStationProcessor* myForeStationProcessor;
	ByyHdoseSimEventLogDatabase *mySimEventLogDatabase;

	ByyOpd* myOpd;
	ByyRsm* myRsm;
	ByyAttributeEditMng* myAEMng;

	bool myStarted;

	Mode myConnectMode;
	QTimer timer;
	int m_currNum;
};

#include <QStringList>

class SNIGHDOSE_EXPORT ByyHdoseLogDatabase : public QObject
{
	Q_OBJECT
public:
	static ByyHdoseLogDatabase& instance()
	{
		static ByyHdoseLogDatabase _instance;
		return _instance;
	}

	void appendLog(const QString& log)
	{
		myLogs.append(log);
		emit logAdded(log);
	}

	const QStringList& logList(){return myLogs;}

signals:
	void logAdded(QString);

protected:
	ByyHdoseLogDatabase(){}

protected:
	QStringList myLogs;
};

class SNIGHDOSE_EXPORT ByyHdoseCollisionDetectionLogDatabase : public QObject
{
	Q_OBJECT
public:
	static ByyHdoseCollisionDetectionLogDatabase& instance()
	{
		static ByyHdoseCollisionDetectionLogDatabase _instance;
		return _instance;
	}

	void appendCollisionDetectionLog(const QString& log)
	{
		myLogs.append(log);
		emit appendCollisionDetectionLogAdded(log);
	}

	const QStringList& logList(){return myLogs;}

signals:
	void appendCollisionDetectionLogAdded(QString);

protected:
	ByyHdoseCollisionDetectionLogDatabase(){}

protected:
	QStringList myLogs;
};

class SNIGHDOSE_EXPORT ByyHdoseSimEventLogDatabase : public QObject
{
	Q_OBJECT
public:
	struct Log
	{
		double simTime;
		QString log;
	};

	typedef QList<Log> LogList;

	LogList& logList();

public slots:
	void appendLog(double simTime,QString log);
	void clear();

signals:
	void logAdded(double simTime,QString log);
	void cleard();

protected:
	LogList myLogList;
};

#endif // HDOSE_H
