#include "StdAfx.h"
#include "ByyEntityOperator.h"
#include "ByySelectionManager.h"
#include "ByyObjectIdManager.h"
#include "ByyApp.h"
#include "ByyCore.h"
#include "sceneEntity.h"
#include "ByyEditEntityWidget.h"
#include "ByyEntityInfoWidget.h"
#include "ByyEntityPlotWidget.h"
#include "ByyDescribeWidget.h"
#include "ByyEntitySensorWidget.h"
#include "entityTaskWidget.h"
#include "mainWindow.h"
#include "ByyDetectTargetWidget.h"
#include "ByyWeaponControlWidget.h"
#include "sensorVisualizer.h"
#include "ByyRangeOfVisibilityVisualizer.h"
#include <osgEarth/Terrain>
#include <osgEarth/MapNode>
#include "coreUtils.h"
#include "ByyUserOperateLog.h"
#include "BTFile/behaviortreeeditwdg.h"
#include "Verdict.h"
#include "../snigHdose/simuController.h"
#define SETWINDOW_AUTODEL(xx) xx->setWindowFlags(Qt::Window);xx->setAttribute(Qt::WA_DeleteOnClose);

ByyEntityOperator::ByyEntityOperator(QObject *parent)
	: QObject(parent)
	,myApp(0),mKeyW(false),mKeyA(false),mKeyS(false),mKeyD(false)
{
	myAttachAction=new QAction(QIcon("../data/icons/EarthTool/find.png"),QString::fromLocal8Bit("锁定"),this);
	myEditAction=new QAction(QIcon("../data/icons/UtilityDocumentEdit.png"),QString::fromLocal8Bit("编辑..."),this);
	myEditAction->setShortcut(QKeySequence("E"));
	myTaskAction=new QAction(QString::fromLocal8Bit("任务..."),this);
	myTaskAction->setShortcut(QKeySequence("K"));
	//特殊版本 chenpeng modify
	//myTaskAction->setVisible(false);

//	mySetSuperiorAction=new QAction(QString::fromLocal8Bit("设置上级..."),this);
	myRemoveAction=new QAction(QIcon("../data/icons/UtilityClose.png"),QString::fromLocal8Bit("删除"),this);
	myRemoveAction->setShortcut(QKeySequence::Delete);
	myMoveAction=new QAction(QIcon("../data/icons/move.png"),QString::fromLocal8Bit("移动..."),this);
	myMoveAction->setShortcut(QKeySequence("M"));

	// 行为树...
	myBehaviorTreeEditAction =  new QAction(QString::fromLocal8Bit("行为树..."),this);
	myBehaviorTreeEditAction->setShortcut(QKeySequence("B"));

	myInfoAction=new QAction(QString::fromLocal8Bit("信息..."),this);
	myPlotAction=new QAction(QString::fromLocal8Bit("数据曲线..."),this);
	myDescribeAction=new QAction(QString::fromLocal8Bit("描述参数..."),this);
	mySensorControlAction=new QAction(QString::fromLocal8Bit("传感器..."),this);
	myWeaponControlAction=new QAction(QString::fromLocal8Bit("武器系统..."),this);

	mySensorShowAction=new QAction(QString::fromLocal8Bit("显示雷达效果"),this);
	mySensorHideAction=new QAction(QString::fromLocal8Bit("关闭雷达效果"),this);

	myDetectTargetAction=new QAction(QString::fromLocal8Bit("目标列表..."),this);

	myVerdictMenu = new QMenu(QString::fromLocal8Bit("裁决"));	
	myEntityAlertAction=new QAction(QString::fromLocal8Bit("实体告警"),this);
	myAircraftTraftAction=new QAction(QString::fromLocal8Bit("飞机目标跟踪"),this);
	myMissileTraftAction=new QAction(QString::fromLocal8Bit("导弹目标跟踪"),this);
	myJudgementAction=new QAction(QString::fromLocal8Bit("状态裁决"),this);
	mySensorAlarmAction=new QAction(QString::fromLocal8Bit("传感器告警"),this);
	myVerdictMenu->addAction(myEntityAlertAction);
	myVerdictMenu->addAction(myAircraftTraftAction);
	myVerdictMenu->addAction(myMissileTraftAction);
	myVerdictMenu->addAction(mySensorAlarmAction);
	myVerdictMenu->addAction(myJudgementAction);

	connect(myAttachAction,SIGNAL(triggered()),SLOT(onAttach()));
	connect(myEditAction,SIGNAL(triggered()),SLOT(onEdit()));
	connect(myTaskAction,SIGNAL(triggered()),SLOT(onShowEditTask()));
//	connect(mySetSuperiorAction,SIGNAL(triggered()),SLOT(onSetSuperior()));
	connect(myRemoveAction,SIGNAL(triggered()),SLOT(onRemove()));
	connect(myInfoAction,SIGNAL(triggered()),SLOT(onShowInfo()));
	connect(myPlotAction,SIGNAL(triggered()),SLOT(onShowPlot()));
	connect(myDescribeAction,SIGNAL(triggered()),SLOT(onShowDescribe()));
	connect(mySensorControlAction,SIGNAL(triggered()),SLOT(onShowSensorControl()));
	connect(myWeaponControlAction,SIGNAL(triggered()),SLOT(onShowWeaponControl()));
	connect(mySensorShowAction,SIGNAL(triggered()),SLOT(onShowSensorImage()));
	connect(mySensorHideAction,SIGNAL(triggered()),SLOT(onHideSensorImage()));
	connect(myDetectTargetAction,SIGNAL(triggered()),SLOT(onShowDetectTarget()));

	connect(myMoveAction,SIGNAL(triggered()),SLOT(onMoveEty()));

	// 行为树连接
	connect(myBehaviorTreeEditAction,SIGNAL(triggered()),SLOT(onBehaviorTreeEdit()));

	// 裁决
	connect(myEntityAlertAction,	SIGNAL(triggered()),SLOT(onEntityAlertAction()));
	connect(myAircraftTraftAction,	SIGNAL(triggered()),SLOT(onAircraftTraftAction()));
	connect(myMissileTraftAction,	SIGNAL(triggered()),SLOT(onMissileTraftAction()));
	connect(mySensorAlarmAction,	SIGNAL(triggered()),SLOT(onSensorAlarm()));
	connect(myJudgementAction,		SIGNAL(triggered()),SLOT(onJudgementAction()));

	ByyIG::IGInstance()->inputController().addEventProcessor(this);

	myMovingEty=false;
}

ByyEntityOperator::~ByyEntityOperator()
{

}

void ByyEntityOperator::setApp( ByyApp* app )
{
	myApp=app;
}

ByyEntityOperator& ByyEntityOperator::instance()
{
	static ByyEntityOperator entOperator;
	return entOperator;
}

void ByyEntityOperator::showContextMenu()
{
	ByyEntityObject *entObj=getSelectionEntity();

	if (!entObj) return;

	QMenu *menu=new QMenu;

	menu->setAttribute(Qt::WA_DeleteOnClose);
	

	if (entObj->isLocal())
	{
		menu->addAction(myAttachAction);
		menu->addSeparator();
		menu->addAction(myEditAction);
		menu->addAction(myTaskAction);
		menu->addAction(myBehaviorTreeEditAction); // 行为树
		menu->addSeparator();
		menu->addAction(myMoveAction);
		menu->addAction(myRemoveAction);
//		menu->addAction(mySetSuperiorAction);
		menu->addSeparator();
		menu->addAction(myInfoAction);
		menu->addAction(myDescribeAction);
		menu->addSeparator();
		menu->addAction(mySensorControlAction);
		menu->addAction(myWeaponControlAction);
		menu->addSeparator();
		menu->addAction(mySensorShowAction);
		menu->addAction(mySensorHideAction);
		//menu->addSeparator();
		//menu->addMenu(myVerdictMenu);
	}
	else
	{
		menu->addAction(myAttachAction);
		menu->addSeparator();
		menu->addAction(myTaskAction);
		menu->addSeparator();
		menu->addAction(myInfoAction);
		menu->addAction(myDescribeAction);
		menu->addSeparator();
		menu->addAction(mySensorControlAction);
		menu->addAction(myWeaponControlAction);
		menu->addAction(myDetectTargetAction);
		menu->addSeparator();
		menu->addAction(myPlotAction);
		menu->addSeparator();
		menu->addAction(mySensorShowAction);
		menu->addAction(mySensorHideAction);
		menu->addSeparator();
		menu->addMenu(myVerdictMenu);
		if(isAircraft(entObj))
		{
			myAircraftTraftAction->setVisible(true);
			myMissileTraftAction->setVisible(true);
		}
		else if(isMissile(entObj))
		{
			myAircraftTraftAction->setVisible(false);
			myMissileTraftAction->setVisible(false);
		}
	}
	menu->popup(QCursor::pos());
}

void ByyEntityOperator::attach( ByyEntityObject* entObj )
{
	if(!entObj)
		return;

	ByySceneEntity* sceneObj=myApp->ig().sceneObjectManager()->findEntity(entObj);

	if (!sceneObj)
		return;

	myApp->ig().inputController().currentObserver()->attach(sceneObj->modelNode());
}

void ByyEntityOperator::edit( ByyEntityObject* entObj ,const QObject * receiver, const char * method)
{
	ByyEditEntityWidget* entObjEditWidget=new ByyEditEntityWidget(myApp->core(),myApp->hdose().attrEditTypeMng(),dynamic_cast<ByyLocalEntity*>(entObj),&myApp->mainWindow());

	SETWINDOW_AUTODEL(entObjEditWidget)

	//myAttachEntWidgetMap.insert(entObj,entObjEditWidget);
	if(getSelectionEntity())
		entObjEditWidget->setWindowTitle(QString::fromLocal8Bit("实体信息编辑-")+getSelectionEntity()->name());  //xks

	entObjEditWidget->show();

	if (receiver&&method)
	{
		connect(entObjEditWidget,SIGNAL(editFinished(ByyLocalEntity*)),receiver,method);
	}
}

void ByyEntityOperator::remove( ByyEntityObject* entObj )
{
	if (entObj->isLocal())
	{
		myApp->core().localObjectManager().removeEntity(entObj);
	}
	else if(entObj->driverName()=="HDose")
	{
		myApp->hdose().removeEntity(entObj->id(),entObj->name());
	}
}

void ByyEntityOperator::showInfo( ByyEntityObject* entObj )
{
	ByyEntityInfoWidget* entObjInfoWidget=new ByyEntityInfoWidget(myApp->core(),entObj,&myApp->mainWindow());

	SETWINDOW_AUTODEL(entObjInfoWidget)

	//myAttachEntWidgetMap.insert(entObj,entObjInfoWidget);

	entObjInfoWidget->show();
}

void ByyEntityOperator::showPlot( ByyEntityObject* entObj )
{
	ByyEntityPlotWidget *entPlotWidget=new ByyEntityPlotWidget(*myApp,entObj,&myApp->mainWindow());

	SETWINDOW_AUTODEL(entPlotWidget)

	entPlotWidget->setWindowTitle(QString::fromLocal8Bit("数据曲线-")+getSelectionEntity()->name());  //xks

	entPlotWidget->show();
}

void ByyEntityOperator::showDescribe( ByyEntityObject* entObj )
{
	ByyDescribeWidget *describeWidget=new ByyDescribeWidget(myApp->core(),entObj,/*myApp,*/&myApp->mainWindow());

	SETWINDOW_AUTODEL(describeWidget)

	//myAttachEntWidgetMap.insert(entObj,describeWidget);

	describeWidget->show();
}

void ByyEntityOperator::showSensorControl( ByyEntityObject* entObj )
{
	ByyEntitySensorWidget *sensorWidget=new ByyEntitySensorWidget(*myApp,entObj,/*myApp,*/&myApp->mainWindow());

	SETWINDOW_AUTODEL(sensorWidget)

	//myAttachEntWidgetMap.insert(entObj,sensorWidget);

	sensorWidget->show();
}

void ByyEntityOperator::showWeaponControl( ByyEntityObject* entObj )
{
	ByyWeaponControlWidget* entTaskWidget=new ByyWeaponControlWidget(myApp->core(),entObj,&myApp->mainWindow());

	SETWINDOW_AUTODEL(entTaskWidget)

		//myAttachEntWidgetMap.insert(entObj,entTaskWidget);

		entTaskWidget->show();
}

void ByyEntityOperator::showEditTask( ByyEntityObject* entObj )
{
	ByyEntityTaskWidget* entTaskWidget=new ByyEntityTaskWidget(*myApp,entObj,&myApp->mainWindow());

	SETWINDOW_AUTODEL(entTaskWidget)

	//myAttachEntWidgetMap.insert(entObj,entTaskWidget);

	entTaskWidget->setWindowTitle(QString::fromLocal8Bit("实体任务-")+getSelectionEntity()->name());  //xks
	entTaskWidget->show();
}

void ByyEntityOperator::controlEntity( ByyEntityObject* entObj )
{

}

void ByyEntityOperator::onAttach()
{
	attach(getSelectionEntity());
}

void ByyEntityOperator::onEdit()
{
	edit(getSelectionEntity());
}

void ByyEntityOperator::onShowEditTask()
{
	showEditTask(getSelectionEntity());
}

void ByyEntityOperator::onShowInfo()
{
	showInfo(getSelectionEntity());
}

void ByyEntityOperator::onShowDescribe()
{
	showDescribe(getSelectionEntity());
}

void ByyEntityOperator::onShowSensorControl()
{
	showSensorControl(getSelectionEntity());
}

void ByyEntityOperator::onShowPlot()
{
	showPlot(getSelectionEntity());
}

void ByyEntityOperator::onRemove()
{
	remove(getSelectionEntity());
}

void ByyEntityOperator::onControlEntity()
{
	controlEntity(getSelectionEntity());
}

/*
void ByyEntityOperator::onSetSuperior()
{

}
*/

void ByyEntityOperator::onShowDetectTarget()
{
	ByyDetectTargetWidget* entTaskWidget=new ByyDetectTargetWidget(*myApp,getSelectionEntity(),&myApp->mainWindow());

	SETWINDOW_AUTODEL(entTaskWidget)

		//myAttachEntWidgetMap.insert(entObj,entTaskWidget);
	entTaskWidget->setWindowTitle(QString::fromLocal8Bit("目标列表显示-")+getSelectionEntity()->name());  //xks

	entTaskWidget->show();
}

ByyEntityObject* ByyEntityOperator::getSelectionEntity()
{
	ByyEntityObject* rel=0;

	if (ByySelectionManager::instance().selectionType()==ByySelectionManager::Entity)
	{
		rel=dynamic_cast<ByyEntityObject*>(ByyObjectIdManager::instance().getObject(ByySelectionManager::instance().selectionId()));
	}

	return rel;
}

void ByyEntityOperator::onShowWeaponControl()
{
	showWeaponControl(getSelectionEntity());
}

void ByyEntityOperator::onShowSensorImage()
{
	setSensorVisible(getSelectionEntity(),true);
}

void ByyEntityOperator::onHideSensorImage()
{
	setSensorVisible(getSelectionEntity(),false);
}

void ByyEntityOperator::setSensorVisible( ByyEntityObject* entObj,bool visible )
{
	if(!entObj)
		return;

	ByySceneEntity* sceneObj=myApp->ig().sceneObjectManager()->findEntity(entObj);

	if (!sceneObj)
		return;

	QList<ByyStateVisualizer*>& stateVisualizers=sceneObj->getStateVisualizerList();


	for (int i=0;i!=stateVisualizers.size();++i)
	{
		ByySensorVisualizer* sensorVisualizer=dynamic_cast<ByySensorVisualizer*>(stateVisualizers[i]);

		if (sensorVisualizer)
		{
			sensorVisualizer->setVisible(visible);
		}
		else
		{
			ByyRangeOfVisibilityVisualizer* rovVisualizer=dynamic_cast<ByyRangeOfVisibilityVisualizer*>(stateVisualizers[i]);

			if (rovVisualizer)
			{
				rovVisualizer->setVisible(visible);
			}
		}
		

	}
}

void ByyEntityOperator::processDoubleClicked()
{
	ByyEntityObject* selectedEntity=getSelectionEntity();

	if (!selectedEntity)
		return;


	if (selectedEntity->isLocal())
	{
		edit(selectedEntity);
	}
	else
	{
		showInfo(selectedEntity);
	}
}

void ByyEntityOperator::onMoveEty()
{
	myMovingEty =getSelectionEntity();
	ByyIG::IGInstance()->sceneWidget()->setCursor(Qt::CrossCursor);
}

void ByyEntityOperator::onBehaviorTreeEdit()
{
	//行为树编辑界面
	BehaviorTreeEditWdg* pBehaviorWidget = new BehaviorTreeEditWdg(myApp->core(),myApp->hdose().attrEditTypeMng(),dynamic_cast<ByyLocalEntity*>(getSelectionEntity()),&(myApp->mainWindow()));
	SETWINDOW_AUTODEL(pBehaviorWidget)
		pBehaviorWidget->show();
}

void ByyEntityOperator::onEntityAlertAction()
{
	ByyEntityObject *entObj=getSelectionEntity();
	QVariantMap map;
	map.insert("EntityName", entObj->name());
	Verdict::getInstance()->showDataDialog(EntityAlertDialog, map);
}

void ByyEntityOperator::onAircraftTraftAction()
{
	ByyEntityObject *entObj=getSelectionEntity();
	QVariantMap map;
	map.insert("EntityName", entObj->name());
	Verdict::getInstance()->showDataDialog(AircraftTargetTrackingDialog, map);
}

void ByyEntityOperator::onMissileTraftAction()
{
	ByyEntityObject *entObj=getSelectionEntity();
	QVariantMap map;
	map.insert("EntityName", entObj->name());
	Verdict::getInstance()->showDataDialog(MissileTargetTrackingDialog, map);
}

void ByyEntityOperator::onJudgementAction()
{
	ByyEntityObject *entObj=getSelectionEntity();
	QVariantMap map;
	map.insert("EntityName", entObj->name());
	Verdict::getInstance()->showDataDialog(ExitJudgmentDialog, map);
}

void ByyEntityOperator::onSensorAlarm()
{
	ByyEntityObject *entObj=getSelectionEntity();
	QVariantMap map;
	map.insert("EntityName", entObj->name());
	Verdict::getInstance()->showDataDialog(SensorAlarmCalculationDialog, map);
}

void ByyEntityOperator::updateMovement() {
    bool up    = mKeyW;  // 前/北
    bool down  = mKeyS;  // 后/南
    bool left  = mKeyA;  // 左/西
    bool right = mKeyD;  // 右/东
	int direction = -1;
    // 8方向优先级判断
    if (up && !down) {
        if (left && !right)       direction = 315;  // 西北
        else if (right && !left)  direction = 45;  // 东北
        else                      direction = 0;   // 北
    } 
    else if (down && !up) {
        if (left && !right)       direction = 225;  // 西南
        else if (right && !left)  direction = 135;  // 东南
        else                      direction = 180;   // 南
    } 
    else if (left && !right)      direction = 270;   // 西
    else if (right && !left)      direction = 90;   // 东
	qDebug() << direction;
	if(direction == -1) return;
	//发送事件
	myControlEty = getSelectionEntity();
	string EntityName = myControlEty->name().toStdString();
	ByySimuController *simctl = new ByySimuController(myApp->core(),myApp->hdose());
	simctl->sendTargetControlEvt(EntityName,direction);
}

bool ByyEntityOperator::processOsgEvent(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
	if(getSelectionEntity()!=0)
	{
		if(ea.getEventType()==ea.KEYDOWN)			// 处理按键按下
		{
			if(ea.getKey()=='m' || ea.getKey()=='M')
			{
				onMoveEty();
				return true;
			}
			if(ea.getKey()=='e' || ea.getKey()=='E')
			{
				onEdit();
				return true;
			}
			if(ea.getKey()=='k' || ea.getKey()=='K')
			{
				onShowEditTask();
				return true;
			}
			if(ea.getKey()=='b' || ea.getKey()=='B')
			{
				onBehaviorTreeEdit();
				return true;
			}
			if(ea.getKey()==65535)
			{
				onRemove();
				return true;
			}
			 // 新增 WASD 方向控制
			switch(tolower(ea.getKey())) {  // 统一转为小写判断
				case 'w': mKeyW = true; updateMovement(); break;
				case 'a': mKeyA = true; updateMovement(); break;
				case 's': mKeyS = true; updateMovement(); break;
				case 'd': mKeyD = true; updateMovement(); break;
			}
			
		}
		else if(ea.getEventType() == ea.KEYUP) {			// 处理按键释放
			switch(tolower(ea.getKey())) {
				case 'w': mKeyW = false; updateMovement(); break;
				case 'a': mKeyA = false; updateMovement(); break;
				case 's': mKeyS = false; updateMovement(); break;
				case 'd': mKeyD = false; updateMovement(); break;
			}
		}
	}


	if(!myMovingEty)
		return false;
	if(ea.getEventType()==ea.RELEASE)
	{
		if (ea.getButton()==ea.LEFT_MOUSE_BUTTON)
		{
			ByyEntityObject* ety=myMovingEty;

			osg::Vec3d world;
			if ( myApp->ig().terrain()->mapNode()->getTerrain()->getWorldCoordsUnderMouse(aa.asView(), ea.getX(), ea.getY(), world) )
			{
				osgEarth::GeoPoint map;
				osg::Vec3d oldPos=ety->position();
				map.fromWorld( myApp->ig().terrain()->mapNode()->getMapSRS(),world);
				//ety->setPosition(osg::Vec3d(map.x(),map.y(),map.z()));
				//  陆地实体移动到地面高程，海、空、太空实体则高度不变
				if( ety->entityType().kind==1 && ety->entityType().domain == 1  )
					ety->setPosition(osg::Vec3d(map.x(),map.y(),map.z()));
				else
					ety->setPosition(osg::Vec3d(map.x(),map.y(),oldPos.z()));

				QString info=QString(CNTR("移动\t%1\t%2->%3")).arg(ety->name()).arg(pos2str(oldPos.x(),oldPos.y())).arg(pos2str(map.x(),map.y()));
				ByyUserOperateLog::instance().addLog(info);
			}
		}

		myMovingEty=0;
		ByyIG::IGInstance()->sceneWidget()->setCursor(Qt::ArrowCursor);
		return true;
	}
	return false;
}
