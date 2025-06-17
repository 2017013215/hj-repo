#ifndef BTNROOT_H
#define BTNROOT_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QList>
#include <QMap>
#include <QStringList>

class BTNTrigger;
class ForceTask;
class BTNCompose;
class BTNTime;
class BTNCondition;
class BTNLoop;
class BTNBreak;
class BTNEnd;

struct RefAttr_Root
{
	QString idf;
	QString note;
	QString type;
	QString initVal;
	QStringList nameList;
	QStringList valueList;
	QMap<QString, QString> map;
	RefAttr_Root()
	{

	}
};
class BTNRoot
{
public:
	BTNRoot();
	~BTNRoot();

	void setObjectName(QString &name);
	QString getObjectName();
	void setNote(QString &note);
	QString getNote();
	void setList(QList<RefAttr_Root> &list);
	QList<RefAttr_Root> getList();
	void listAppend(RefAttr_Root &attr);
	void setValue(QString &note,QString &value);
	QString getContent();
private:
	QString m_ObjectName;
	QString m_Note;
	QList<RefAttr_Root> m_list_attrs;
public:
	//触发器列表
	QList<BTNTrigger*> m_TriggerList;
	//复合节点
	QList<BTNCompose*> m_ComposeList;
	//时间节点
	QList<BTNTime*> m_TimeList;
	//任务列表
	QList<ForceTask*> m_TaskList;
	//条件节点
	QList<BTNCondition*> m_ConditionList;
	//循环节点
	QList<BTNLoop*> m_LoopList;
	//跳出
	QList<BTNBreak*> m_BreakList;
	//结束
	QList<BTNEnd*> m_EndList;
};

class BTNRootManager : public QObject
{
	Q_OBJECT

public:
	static BTNRootManager *instance();
	void readConfig();
	BTNRoot *getBTNRoot(QString &rootname);
	//QMap<QString,BTNRoot*> getRootMap();
	//QList<BTNRoot*> getRootList();
	//QStringList getRootNodeList();

	BTNRootManager();
	~BTNRootManager();
private:
	QMap<QString,BTNRoot*> m_RootMap;
};
#endif // BTNROOT_H
