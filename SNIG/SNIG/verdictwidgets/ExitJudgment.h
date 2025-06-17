#pragma once
#include "VerdictBaseWidget.h" 
#include "NavalEquipmentStatusJudgment.h"

//实体预警判断
class ExitJudgment : public VerdictBaseWidget
{
	Q_OBJECT

public:
	ExitJudgment(VerdictBaseWidget* parent = nullptr);

	void showEntityName(const QString& entityName);

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
	void slotBtnExitJudgment();
	void slotExitJudgmentSides(int index);
	//需要通知进行管理
	void slotCellEJ();

private:
	QComboBox	*ComboBox_ExitJudgment_Sides;
	QComboBox	*ComboBox_ExitJudgment_Entity;
	QPushButton *Button_ExitJudgment;
	QTableWidget	*Table_ExitJudgment;
	QVBoxLayout		*Layout_ExitJudgment;
};