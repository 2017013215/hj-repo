#pragma once
#include "VerdictBaseWidget.h" 


//ÊµÌåÔ¤¾¯ÅÐ¶Ï
class WarDamageStatistics : public VerdictBaseWidget
{
	Q_OBJECT

public:
	WarDamageStatistics(VerdictBaseWidget* parent = nullptr);

protected:
	virtual void createButton();
	virtual void createChart();
	virtual void createComBox();
	virtual void createTableView();
	virtual void setupLayout();
	virtual void resetUi();
	virtual void preHandleDataFrame();
	virtual void handleDataFrame();
	void updataWarDamageStatisticsData(const EntityTypeExpendInfos& ammunitioninfos, const EntityTypeExpendInfos& entityinfos);
	virtual void stopHandle();

protected Q_SLOTS: 
	void slotWarDamageStatisticsSides(int index);
	void slotWarDamageStatisticsDamageLevel(int index);

private:
	QVBoxLayout* Layout_WarDamageStatistics;

	QComboBox *ComboBox_WarDamageStatistics_Sides;
	QComboBox *ComboBox_WarDamageStatistics_DamageLevel;

	QStandardItemModel *Model_WarDamageStatistics_Munition;
	QTableView*Table_WarDamageStatistics_Munition;

	QStandardItemModel *Model_WarDamageStatistics_Damage;
	QTableView*Table_WarDamageStatistics_Damage;
};