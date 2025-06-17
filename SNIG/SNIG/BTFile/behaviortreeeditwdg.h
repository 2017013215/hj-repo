#ifndef BEHAVIORTREEEDITWDG_H
#define BEHAVIORTREEEDITWDG_H

#include <QWidget>
#include "ui_behaviortreeeditwdg.h"
#include "parameter.h"
#include <QTreeWidgetItem>
#include <string>
#include <vector>
#include <map>
#include <math.h>
#include "taskManager.h"
#include "task.h"


using namespace std;
class ByyApp;
class ByyCore;
class ByyLocalEntity;
class QMenu;
class QAction;
class BehaviorTreeTableWidgetDelegate;
class QTreeWidgetItem;
class QActionGroup;
class ForceTask;
class BTNRoot;
class BTNTrigger;
class BTNCompose;
class BTNTime;
class BTNCondition;
class BTNLoop;
class BTNBreak;
class BTNEnd;

enum NodeType
{
	NONE=0,
	ROOT,
	TASK,
	COMPOSE,
	LOOP,
	TIME,
	CONDITION,
	BREAK,
	END,
	TRIGGER
};

class BehaviorTreeEditWdg : public QWidget
{
	Q_OBJECT

public:
		enum TokenType{
		TOKEN_TYPE_START=0,
		TOKEN_UNKNOWN=TOKEN_TYPE_START,

		TOKEN_IDENTIFY,	
		TOKEN_VALUE,
		TOKEN_PARA,

		TOKEN_LPARAM,	//(
		TOKEN_RPARAM,	//)
		TOKEN_LBPARAM,	//{
		TOKEN_RBPARAM,	//}
		TOKEN_COMMA,	//,
		TOKEN_SEMI,		//;
		TOKEN_COMMENT,	////
		
		TOKEN_NOTOPE,	//!
		TOKEN_ANDOPE,	//&
		TOKEN_OROPE,	//|

		/*TOKEN_WORD_START,
		TOKEN_WORD_BLOCK=TOKEN_WORD_START,
		TOKEN_WORD_WHEN,
		TOKEN_WORD_WHENONCE,
		TOKEN_WORD_WHILE,
		TOKEN_WORD_IF,
		TOKEN_WORD_ELSEIF,
		TOKEN_WORD_ELSE,
		TOKEN_WORD_END=TOKEN_WORD_ELSE,*/

		TOKEN_END,	
		TOKEN_ERR,		
		TOKEN_TYPE_END=TOKEN_ERR,
	};
	enum StateType{
		STATE_TYPE_START=0,
		STATE_UNKNOWN=STATE_TYPE_START,
		STATE_INIDENTIFY,
		STATE_INPARA,
		STATE_INVALUE,
		STATE_EXPECTCOMMENT,
		STATE_INCOMMENT,
		STATE_DONE,
		BASE_STATE_TYPE_END=STATE_DONE,
	};
	enum ParseState
	{
		STATE_NORMAL=0,
		STATE_COMPLETE,
		STATE_ERR,
	};

	BehaviorTreeEditWdg(ByyCore& core,ByyAttributeEditMng &atmng,ByyLocalEntity* entObj,QWidget *parent = 0);
	~BehaviorTreeEditWdg();
	void initWidgetLayout();
	ParameterList GetParameterList();

	void updateParam(const QString& name,const QString& value);
	void updateTableWidget(ForceTask *task);
	void updateTableWidget(BTNRoot *root);
	void updateTableWidget(BTNTrigger *trigger);
	void updateTableWidget(BTNCompose *compose);
	void updateTableWidget(BTNTime *time);
	void updateTableWidget(BTNCondition *condition);
	void updateTableWidget(BTNLoop *loop);
	void updateTableWidget(BTNBreak *break_);
	void updateTableWidget(BTNEnd *end);
	QTableWidgetItem* setTableItemText(int row,int column, QString text);
	//删除TreeNode
	void deleteTreeItem(QTreeWidgetItem *item);
	//添加BTNode
	void addBTNode();
	//删除BTNode
	void deleteBTNode(QTreeWidgetItem *item);
	// 添加根节点
	QTreeWidgetItem* addThreeRoot(QString name);
	// 添加子节点
	QTreeWidgetItem* addThreeNode(QTreeWidgetItem *parent,QString name);
	//解析任务名
	QString analysisTaskName(QString str);
    //解析触发器名
	QString analysisTriggerName(QString str);
	//解析任务列表
	QString analysisTsk(ByyTaskManager::TaskList& tsk);

	void    parse();
	bool	DoParseParas(QString node ,QMap<int,QString> &map, bool lparamStart=true);

	char	getNextChar();
	void	unGetNextChar();
	void	getToken();
	void	getTokenForPara();
	void	unGetToken(){m_bTokenParseStop=1;}
	//void	getToken
	bool	matchToken(TokenType token,bool expected=false);
	bool	matchSingleToken(TokenType token,bool expected=false);

	std::string		m_tokenstring;
	TokenType	m_token;
	int		m_pos;	
	int		m_length;
	ParseState	m_parseState;
	int		m_row;
	int		m_col;
	int		m_lastrow;
	int		m_lastcol;
	bool	m_bTokenParseStop;

	std::string	m_content;


signals:
	void signal_updateBehaviorTreeInfo(QString &path,QString &content);

public slots:
	void on_pushButton_load_clicked();
	void on_pushButton_save_clicked();
	void on_pushButton_update_clicked();
	void on_pushButton_cancel_clicked();
	void on_treeWidget_customContextMenuRequested(const QPoint &pos);
	void on_treeWidget_itemClicked(QTreeWidgetItem * item, int column);
	void on_treeWidget_itemDoubleClicked(QTreeWidgetItem * item, int column);
	//table
	void on_tableWidget_itemChanged(QTableWidgetItem * item) ;

	void on_m_AddRootNode_triggered();
	void on_m_ClearChildNode_triggered();
	void on_m_TaskNode_triggered();
	void on_m_ComposeNode_triggered();
	void on_m_LoopNode_triggered();
	void on_m_TimeNode_triggered();
	void on_m_ConditionNode_triggered();
	void on_m_BreakNode_triggered();
	void on_m_EndNode_triggered();
	void on_m_TriggerNode_triggered();
	void on_m_DeleteNode_triggered();
	void on_m_TriggerNumberUnlimited_triggered();
	void on_m_TriggerNumberSet_triggered();
	void on_m_TriggerSuccessUnInterrupt_triggered();
	void on_m_TriggerSuccessFinish_triggered();
	void on_m_TriggerSuccessCurrent_triggered();
	void on_m_TriggerSuccessNext_triggered();
	void on_m_TriggerSuccessReset_triggered();
	void on_m_TriggerFaileUnInterrupt_triggered();
	void on_m_TriggerFaileFinish_triggered();
	void on_m_TriggerFaileCurrent_triggered();
	void on_m_TriggerFaileNext_triggered();
	void on_m_TriggerFaileReset_triggered();

private:
	Ui::BehaviorTreeEditWdg ui;
	ByyCore &myCore;
	ByyAttributeEditMng& myATMng;
	ByyLocalEntity *myEntObj;
	ParameterList myEditParameters;
	BehaviorTreeTableWidgetDelegate *m_TableWdgDelegate;
	QString m_Path;
	QString m_Content;
	QString m_TaskName;
	QString m_TriggerName;
	QString m_TemplateHead;
	QString m_TskContent;
	QString m_TemplateEnd;

	QMenu *m_TreeRootMenu;
	QAction *m_DeleteNode;
	QMenu *m_TreeNodeMenu;
	QAction *m_AddRootNode;
	QAction *m_ClearChildNode;
	QAction *m_TaskNode;
	QAction *m_ComposeNode;
	QAction *m_LoopNode;
	QAction *m_TimeNode;
	QAction *m_ConditionNode;
	QAction *m_BreakNode;
	QAction *m_EndNode;
	QAction *m_TriggerNode;
	QMenu *m_TriggerNumberMenu;
	QAction *m_TriggerNumberUnlimited;
	QAction *m_TriggerNumberSet;
	QMenu *m_TriggerInterruptPolicyForSuccessMenu;
	QAction *m_TriggerSuccessUnInterrupt;
	QAction *m_TriggerSuccessFinish;
	QAction *m_TriggerSuccessCurrent;
	QAction *m_TriggerSuccessNext;
	QAction *m_TriggerSuccessReset;
	QMenu *m_TriggerInterruptPolicyForFaileMenu;
	QAction *m_TriggerFaileUnInterrupt;
	QAction *m_TriggerFaileFinish;
	QAction *m_TriggerFaileCurrent;
	QAction *m_TriggerFaileNext;
	QAction *m_TriggerFaileReset;
	QActionGroup *m_TriggerNuGroup;
	QActionGroup *m_TriggerInterruptPolicyForSuccessGroup;
	QActionGroup *m_TriggerInterruptPolicyForFaileGroup;
	QTreeWidgetItem  *m_CurItem;
	QTreeWidgetItem  *m_curItemRoot;
	QTreeWidgetItem  *m_CurItemNode;
	QTreeWidgetItem  *m_CurItemNode1;
	QTreeWidgetItem  *m_CurItemNode2;
	QTreeWidgetItem  *m_CurItemNode3;

};

#endif // BEHAVIORTREEEDITWDG_H
