//******************************************************************************
// @file formationWidget.h
// @Description:编队队形编辑界面及功能
// @Author：LYJ
// @Date：2017.12.20
// @Copyright:军尚科技
//******************************************************************************

#ifndef FORMATIONWIDGET_H
#define FORMATIONWIDGET_H

#include <QWidget>
#include "ui_formationWidget.h"
#include "commonStruct.h"
#include "scenarioV2.h"

#define PI 3.14159265358979323846

typedef struct FormationElement
{
	bool isCenter;
	int axis_x;
	int axis_y;
}FormationElement;

class ByyApp;
class unitsGraphicsScene;
class uinitsGraphicsItem;
class ByyEntityObject;
class formationWidget : public QWidget
{
	Q_OBJECT

public:
	formationWidget(ByyApp* app,QWidget *parent = 0);
	~formationWidget();
	void debugFormation(ByyScenarioV2::Formation& f);
private:
	formationWidget(QWidget *parent = 0);
	Ui::formationWidget ui;

public:
	//单例接口
	static formationWidget* getInstance();

	void setByyApp(ByyApp* app);

	//添加实体
	void appendEntityToList( ByyEntityObject* entObj );

	//设置实体列表
	void setEntityList( QList<ByyEntityObject*> list );

	//获取想定中的编队信息
	void getScnFormation();

	//显示编队阵型
	void showFormation(ByyScenarioV2::Formation& f);

	//获取实体类型
	UNITSTYPE getEntityUNITTYPE(ByyEntityObject* entObj);

protected:
	void paintEvent(QPaintEvent *);

	void showEvent(QShowEvent *);

	void closeEvent(QCloseEvent *);

public slots:

	//比例信息
	void onShowPlottingscale(int value);

	//显示距离中心距离和与之角度
	void onShowDistanceAndAngle(uinitsGraphicsItem* item);

private slots:
	//关闭
	void on_pushButton_mp_close_clicked();

	//应用
	void on_pushButton_ok_clicked();

	//自定义兵力个数确定
	void on_pushButton_zdy_ok_clicked();

	//响应保存编队按钮
	void on_pushButton_leaveFormation_clicked();

	//点击队形列表
	void on_ListWidget_clicked(QListWidgetItem * item );

	void on_combobox_clicked(const QString& text);

	//点击树状按钮
	void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

	void on_treeWidget_customContextMenuRequested(const QPoint &pos);

	void on_CreateFormationAct();
	void on_DisbandFormationAct();
	void on_SpecifyLeaderAct();
	void on_AddMemberAct();
	void on_DeleteMemberAct();

public:
	//初始化界面
	void initUi();

	//为场景绘制网格背景
	void paintGridBackground();

	//清除场景中实体
	void clearSceneItem();

	//保存编队
	bool saveFormation(const QString &fileName);

	//加载编队
	bool loadFormation(const QString &fileName);

	//创建编队列表
	void createFormationListWidget();

public:
	bool closeFlag; // 窗口关闭标志

private:
	ByyApp* myApp; 
	int m_gridWidth;									// 网格宽度
	QPixmap* m_pix_viewBackground;						// 网格背景图
	unitsGraphicsScene* m_scene;						// 场景
	uinitsGraphicsItem* m_centerItem;					// 中心图形项
	uinitsGraphicsItem* m_currentItem;					// 当前选中图形项
	QList<FormationElement> m_formationElementList;		// 编队中实体列表
	UNITSTYPE m_formationType;							// 编队类型

	QList<ByyEntityObject*> m_entityList;				// 编队实体列表
	//////////////////////////////////////////////////////////////////////////
	ByyScenarioV2::FormationList myFormationList;
	QTreeWidgetItem* m_customContextMenu;
	QMenu* m_FormationMenu;
	QAction* m_CreateFormationAct;//创建编队
	QAction* m_DisbandFormationAct;//解散编队
	QAction* m_SpecifyLeaderAct;//指定编队长
	QAction* m_AddMemberAct;//新增成员
	QAction* m_DeleteMemberAct;//删除成员
};

#endif // MOBRMENTPLANNERWIDGET_H
