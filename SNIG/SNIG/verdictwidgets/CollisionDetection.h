#pragma once
#include "VerdictBaseWidget.h" 

class QStandardItemModel;

struct CollisionDetectionInfo {
	QString entityA;
	QString entityB;
	double  distance;
	double  orientation;
	double  preMinDistance;
	double  preMinCollisionTime;
};
typedef QList<CollisionDetectionInfo> CollisionDetectionInfos;

//ÊµÌåÔ¤¾¯ÅÐ¶Ï
class CollisionDetection : public VerdictBaseWidget
{
	Q_OBJECT

public:
	CollisionDetection(VerdictBaseWidget* parent = nullptr);

protected:
	virtual void createButton();
	virtual void createChart();
	virtual void createComBox();
	virtual void createTableView();
	virtual void setupLayout();
	virtual void resetUi();
	virtual void preHandleDataFrame();
	virtual void handleDataFrame();
	void updataCollisionDetectionData(const CollisionDetectionInfos& infos);
	virtual void stopHandle();

protected Q_SLOTS:
	void slotBtnCollisionDetection();
	void slotCollisionDetectionType(int index);

private:
	QVBoxLayout* Layout_CollisionDetection;

	QComboBox *ComboBox_CollisionDetection_Type;
	QComboBox *ComboBox_CollisionDetection_Entity;
	QSpinBox* SpinBox_CollisionDetection;
	QPushButton *Button_CollisionDetection;

	QTableView*	Table_CollisionDetection;
	QStandardItemModel *Model_CollisionDetection;
	double m_recordSimtime;
};