#pragma once
#include "VerdictBaseWidget.h" 
#include <QStringList>

enum TrackStatusEnum {
	UntrackStatus = 0,		//未跟踪
	TrackedUnknowStatus,	//不明
	TrackLockStatus,		//稳定跟踪
	TrackLoseStatus			//丢失
};

struct TrackinfInfo {
	QString targetName;
	double  distance;
	double  continueTime;
	bool	isTracked;
	int		trackedCount;
	int		trackStatus; //未跟踪，不明，稳定跟踪，丢失

	QString getStatusString()
	{
		QString ret;
		switch(trackStatus)
		{
		case UntrackStatus:
			ret = QString::fromLocal8Bit("未跟踪");
			break;
		case TrackedUnknowStatus:
			ret = QString::fromLocal8Bit("不明");
			break;
		case TrackLockStatus:
			ret = QString::fromLocal8Bit("稳定跟踪");
			break;
		case TrackLoseStatus:
			ret = QString::fromLocal8Bit("丢失");
			break;
		}
		return ret;
	}
	TrackinfInfo()
	{
		distance = 0.0;
		isTracked = false;
		trackedCount = 1;
		trackStatus = 0;
		trackStatus = UntrackStatus;
	}
	void addCount()
	{
		//发现一次加一次发现
		trackedCount++;
		//未跟踪，或者丢失变成不明
		if(UntrackStatus == trackStatus || TrackLoseStatus == trackStatus)
		{
			trackStatus = TrackedUnknowStatus;
		}
		judgeTracked();
	}
	void reduceCount()
	{
		//没有发现直接给零
		trackedCount = 0;
		distance = -1;
		//不明或者稳定，编程了丢失
		if(TrackedUnknowStatus == trackStatus || TrackLockStatus == trackStatus)
		{
			trackStatus = TrackLoseStatus;
		}
		judgeTracked();
	}
	void judgeTracked()
	{
		//判断跟踪的情况
		//if(continueTime >= 2.0) isTracked = true;
		//else isTracked=false;
		if(continueTime > 2.0)
		{
			trackStatus = TrackLockStatus;
		}
		else if(continueTime > 0.0)
		{
			trackStatus = TrackedUnknowStatus;
		}
	}
};
typedef QList<TrackinfInfo> TrackinfInfos;

inline bool operator==(const TrackinfInfo& info1, const TrackinfInfo& info2)
{
	return info1.targetName == info2.targetName;
}
inline bool infoLessThan(const TrackinfInfo& info1, const TrackinfInfo& info2)
{
	return info1.targetName <= info2.targetName;
}

//实体预警判断
class AircraftTargetTracking : public VerdictBaseWidget
{
	Q_OBJECT

public:
	AircraftTargetTracking(VerdictBaseWidget* parent = nullptr);
	void showEntityName(const QString& entityName);

protected:
	virtual void createButton();
	virtual void createChart();
	virtual void createComBox();
	virtual void createTableView();
	virtual void setupLayout();
	virtual void resetUi();
	virtual void handleDataFrame();
	virtual void preHandleDataFrame(void);
	void updataModelData(const TrackinfInfos& infos);
	virtual void stopHandle();

protected Q_SLOTS:
	void slotBtnAircraftTargetTracking();
	void slotAircraftTargetTrackingSide(int index);

private:
	QComboBox *ComboBox_AircraftTargetTracking_Side;
	QComboBox *ComboBox_AircraftTargetTracking_Self;
	QPushButton*	Button_AircraftTargetTracking;
	QVBoxLayout*	Layout_AircraftTargetTracking;
	QStandardItemModel *Model_AircraftTargetTracking;
	QTableView		   *Table_AircraftTargetTracking;
	//辅助数据处理
	QMap<QString, TrackinfInfo> m_trackedTargetRecord;
};