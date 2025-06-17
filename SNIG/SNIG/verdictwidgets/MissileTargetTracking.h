#pragma once
#include "VerdictBaseWidget.h" 
#include "AircraftTargetTracking.h"

//实体预警判断
class MissileTargetTracking : public VerdictBaseWidget
{
	Q_OBJECT

public:
	MissileTargetTracking(VerdictBaseWidget* parent = nullptr);
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
	void slotBtnMissileTargetTracking();
	void slotMissileTargetTrackingSide(int index);

private:
	QComboBox		*ComboBox_MissileTargetTracking_Side;
	QComboBox		*ComboBox_MissileTargetTracking_Self;
	QPushButton		*Button_MissileTargetTracking;
	QVBoxLayout		*Layout_MissileTargetTracking;
	QStandardItemModel	*Model_MissileTargetTracking;
	QTableView			*Table_MissileTargetTracking;
	//辅助数据处理
	QMap<QString, TrackinfInfo> m_trackedTargetRecord;
};