#pragma once
#include "EntityAlert.h"
#include "VerdictBaseWidget.h" 

class ByyEntityObject;

//实体预警判断
class ExercisePartyAlert : public VerdictBaseWidget
{
	Q_OBJECT

public:
	ExercisePartyAlert(VerdictBaseWidget* parent = nullptr);

protected:
	virtual void createButton();
	virtual void createChart();
	virtual void createComBox();
	virtual void createTableView();
	virtual void setupLayout();
	virtual void resetUi();
	virtual void stopHandle();

protected Q_SLOTS:
	void slotBtnExercisePartyAlert();

protected:
	void handleDataFrame();
	void updateEntityAlertInfos(const EntityAlertInfos& infos);
	bool isInPartySensorsFoV(ByyEntityObject* entity, const QString& partyName, bool &isAlerted, double &resolution);

private:
	QVBoxLayout *Layout_ExercisePartyAlert;

	QStandardItemModel *Model_ExercisePartyAlert;
	QTableView*Table_ExercisePartyAlert;//目标预警裁决

	QComboBox *ComboBox_ExercisePartyAlert_Sides;
	QPushButton*	Button_ExercisePartyAlert;

	QLabel *Label_StrAirCraftTotalCount ;
	QLabel *Label_AirCraftTotalCount ;
	QLabel *Label_StrShipTotalCount ;
	QLabel *Label_ShipTotalCount ;
	QLabel *Label_StrSubmarineTotalCount ;
	QLabel *Label_SubmarineTotalCount ;
	QLabel *Label_StrMissileTotalCount ;
	QLabel *Label_MissileTotalCount ;

	QLabel *Label_Coverage1 ;
	QLabel *Label_Percent1;
	QLabel *Label_Coverage2 ;
	QLabel *Label_Percent2;
	QLabel *Label_Coverage3 ;
	QLabel *Label_Percent3;
	QLabel *Label_Coverage4 ;
	QLabel *Label_Percent4;
	//QTimer* tr_ExercisePartyAlert;

	// 数据成员
	int totalAirCraftCount;
	int totalShipCount;
	int totalSubmarineCount;
	int totalMissileCount;
	int totalTorpedoCount;

	int alertedAirCraftCount;
	int alertedShipCount;
	int alertedSubmarineCount;
	int alertedMissileCount;
	int alertedTorpedoCount;
};