#pragma once

#include <QObject>

class ByyApp;
class EntityControlPanel;  // 前向声明

class ByyEntityOperator : public QObject ,public ByyEventProcessor
{
	Q_OBJECT

public:
	~ByyEntityOperator();

	void setApp(ByyApp* app);

	static ByyEntityOperator& instance();

	void attach(ByyEntityObject* entObj);
	void edit(ByyEntityObject* entObj,const QObject * receiver=0, const char * method=0);
	void remove(ByyEntityObject* entObj);
	void showInfo(ByyEntityObject* entObj);
	void showPlot(ByyEntityObject* entObj);
	void showDescribe(ByyEntityObject* entObj);
	void showSensorControl(ByyEntityObject* entObj);
	void showWeaponControl(ByyEntityObject* entObj);
	void showEditTask(ByyEntityObject* entObj);

	bool processOsgEvent(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa); //xks

	void controlEntity(ByyEntityObject* entObj);
public slots:
	void showContextMenu();
	void processDoubleClicked();

protected slots:
	void onAttach();
	void onEdit();
	void onRemove();
	void onShowInfo();
	void onShowPlot();
	void onShowDescribe();
	void onShowSensorControl();
	void onShowWeaponControl();
	void onShowEditTask();
	//void onSetSuperior();
	void onShowDetectTarget();
	void onShowSensorImage();
	void onHideSensorImage();

	void	onMoveEty(); //xks

	//新模型，行为树
	void onBehaviorTreeEdit();

	//裁决
	void onEntityAlertAction();		//实体告警
	void onAircraftTraftAction();	//飞机目标跟踪
	void onMissileTraftAction();	//导弹目标跟踪
	void onJudgementAction();		//状态裁决
	void onSensorAlarm();			//传感器告警

	//控制
	void onControlEntity();

	void onShowControlPanel();

private:
	ByyEntityOperator(QObject *parent=0);

	ByyEntityObject* getSelectionEntity();

	void setSensorVisible(ByyEntityObject* entObj,bool visible);

	// 方向按键状态记录
    bool mKeyW;
    bool mKeyA;
    bool mKeyS;
    bool mKeyD;
	//更新方向
	void updateMovement();

private:
	ByyApp* myApp;

	QAction *myAttachAction;
	QAction *myEditAction;
	QAction *myTaskAction;
//	QAction *mySetSuperiorAction;
	QAction *myRemoveAction;
	QAction *myInfoAction;
	QAction *myPlotAction;
	QAction *myDescribeAction;
	QAction *mySensorControlAction;
	QAction *myWeaponControlAction;
	QAction *mySensorShowAction;
	QAction *mySensorHideAction;
	QAction *myDetectTargetAction;

	QAction*	myMoveAction; //xks
	ByyEntityObject* myMovingEty; // xks
	ByyEntityObject* myControlEty; // szh

	QMenu*	 myVerdictMenu;

	//新模型，行为树
	QAction *myBehaviorTreeEditAction;//行为树编辑

	//裁决
	QAction *myEntityAlertAction; //实体告警
	QAction *myAircraftTraftAction; //飞机目标跟踪
	QAction *myMissileTraftAction; //导弹目标跟踪
	QAction *mySensorAlarmAction; //状态裁决
	QAction *myJudgementAction; //状态裁决

	QAction* myControlPanelAction;

	friend class EntityControlPanel;
};