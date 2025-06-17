#pragma once
#include "VerdictBaseWidget.h" 
#include "NavalEquipmentStatusJudgment.h"

//实体预警判断
class SubmarineEquipmentStatusJudgment : public VerdictBaseWidget
{
	Q_OBJECT

public:
	SubmarineEquipmentStatusJudgment(VerdictBaseWidget* parent = nullptr);

protected:
	virtual void createButton();
	virtual void createChart();
	virtual void createComBox();
	virtual void createTableView();
	virtual void setupLayout();
	virtual void resetUi();
	virtual void preHandleDataFrame();
	virtual void handleDataFrame();
	void updataJudgmentData(const EquipmentJudgments& infos);
	virtual void stopHandle();
	
protected Q_SLOTS:
	void slotBtnSESJ();
	void slotSESJSides(int index);
	//需要通知进行管理
	void slotCellSESJ();

private:
	QTableWidget	*Table_SubmarineEquipmentStatusJudgment;
	QPushButton		*Button_SubmarineEquipmentStatusJudgment;
	QComboBox		*ComboBox_SubmarineEquipmentStatusJudgment_Sides;
	QComboBox		*ComboBox_SubmarineEquipmentStatusJudgment_Entity;
	QVBoxLayout		*Layout_SubmarineEquipmentStatusJudgment;
};