#pragma once
#include "VerdictBaseWidget.h" 

struct EquipmentJudgment {
	QString equipmentName;		//装备名称
	QString equipmentStatus;	//可用状态
};
typedef QList<EquipmentJudgment> EquipmentJudgments;

inline bool operator==(const EquipmentJudgment& info1, const EquipmentJudgment& info2)
{
	return info1.equipmentName == info2.equipmentName;
}
inline bool infoLessThan(const EquipmentJudgment& info1, const EquipmentJudgment& info2)
{
	return info1.equipmentName <= info2.equipmentName;
}

//实体预警判断
class NavalEquipmentStatusJudgment : public VerdictBaseWidget
{
	Q_OBJECT

public:
	NavalEquipmentStatusJudgment(VerdictBaseWidget* parent = nullptr);

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
	void slotBtnNESJ();
	void slotNESJSides(int index);
	//需要通知进行管理
	void slotCellNESJ();

private:
	QTableWidget	*Table_NavalEquipmentStatusJudgment;
	QPushButton		*Button_NavalEquipmentStatusJudgment;
	QComboBox		*ComboBox_NavalEquipmentStatusJudgment_Sides;
	QComboBox		*ComboBox_NavalEquipmentStatusJudgment_Entity;
	QVBoxLayout		*Layout_NavalEquipmentStatusJudgment;
};