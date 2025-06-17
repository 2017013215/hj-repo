#pragma once
#include "VerdictBaseWidget.h" 

struct FireControlLockingData {
	QString lockingTarget;
	QString lockingStatus;
	double  distance;
	double  spendTime;
	FireControlLockingData():
		distance(-1.0),spendTime(-1.0)
	{

	}
};

//ÊµÌåÔ¤¾¯ÅÐ¶Ï
class FireControlLockingCalculation : public VerdictBaseWidget
{
	Q_OBJECT

public:
	FireControlLockingCalculation(VerdictBaseWidget* parent = nullptr);

protected:
	virtual void createButton();
	virtual void createChart();
	virtual void createComBox();
	virtual void createTableView();
	virtual void createLog();
	virtual void setupLayout();
	virtual void resetUi();
	virtual void preHandleDataFrame();
	virtual void handleDataFrame();
	void updataFireControlLockingCalculationData(const FireControlLockingData& info);
	virtual void stopHandle();

protected Q_SLOTS:
	void slotBtnFireControlLockingCalculation();
	void slotFireControlLockingCalculationRed(int index);
	void slotFireControlLockingCalculationBlue(int index);

private:
	QVBoxLayout		*Layout_FireControlLockingCalculation;
	QPushButton		*Button_FireControlLockingCalculation;
	QComboBox		*ComboBox_FireControlLockingCalculation_Red;
	QComboBox		*ComboBox_FireControlLockingCalculation_Blue;
	QTableWidget	*Table_FireControlLockingCalculation;
	int				 m_spendTimeRecord;
};