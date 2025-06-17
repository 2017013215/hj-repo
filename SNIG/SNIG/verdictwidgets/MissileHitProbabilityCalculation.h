#pragma once
#include "VerdictBaseWidget.h" 
#include "Verdict.h"

struct MHPCData
{
	QString missileName;
	QString targetName;
	double  distance;
	double  probablility;
};
typedef QList<MHPCData> MHPCDatas;

//ÊµÌåÔ¤¾¯ÅÐ¶Ï
class MissileHitProbabilityCalculation : public VerdictBaseWidget
{
	Q_OBJECT

public:
	MissileHitProbabilityCalculation(VerdictBaseWidget* parent = nullptr);

protected:
	virtual void createButton();
	virtual void createChart();
	virtual void createComBox();
	virtual void createTableView();
	virtual void setupLayout();
	virtual void preHandleDataFrame();
	virtual void handleDataFrame();
	void updataMissileHitProbabilityData(const MHPCDatas& infos);
	virtual void stopHandle();
	double getProbablility(QString EntClsname, int kind, int domain, double lchdist_m, 
		double hitR_m, double hitH_m, double hitVel_mps, double hitTgtVel_mps); // xks, 20240528

protected Q_SLOTS:
	void slotBtnMissileHitProbabilityCalculation();
	//ÐÞ¸Ä
	void slotBtnChangedMHP();

private:
	QVBoxLayout	*Layout_MissileHitProbabilityCalculation;
	QComboBox	*ComboBox_MissileHitProbabilityCalculation;
	QTableWidget	*Table_MissileHitProbabilityCalculation;
	QPushButton		*Button_MissileHitProbabilityCalculation;
	QStandardItemModel *Model_MissileHitProbabilityCalculation;
};