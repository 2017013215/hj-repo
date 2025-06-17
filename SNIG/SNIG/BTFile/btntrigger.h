#ifndef BTNTRIGGER_H
#define BTNTRIGGER_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QList>
#include <QMap>
#include <QStringList>

class ForceTask;
class BTNCompose;
class BTNTime;

struct RefAttr_Trigger
{
	QString idf;
	QString note;
	QString type;
	QString initVal;
	QStringList nameList;
	QStringList valueList;
	QMap<QString, QString> map;
	RefAttr_Trigger()
	{

	}
};
class BTNTrigger
{
public:
	BTNTrigger();
	~BTNTrigger();

	void setObjectName(QString &name);
	QString getObjectName();
	void setNote(QString &note);
	QString getNote();
	void setList(QList<RefAttr_Trigger> &list);
	QList<RefAttr_Trigger> getList();
	void listAppend(RefAttr_Trigger &attr);
	void setValue(QString &note,QString &value);
	QString getContent();
private:
	QString m_ObjectName;
	QString m_Note;
	QList<RefAttr_Trigger> m_list_attrs;
public:
	QString m_Count;//次数
	QString m_InterruptPolicyForSuccess;//触发节点执行成功后策略
	QString m_InterruptPolicyForFaile;//触发节点执行失败后策略
	//触发器列表
	QList<BTNTrigger*> m_TriggerList;
	//复合节点
	QList<BTNCompose*> m_ComposeList;
	//时间节点
	QList<BTNTime*> m_TimeList;
	//任务列表
	QList<ForceTask*> m_TaskList;
	
};

class BTNTriggerManager : public QObject
{
	Q_OBJECT

public:
	static BTNTriggerManager *instance();
	void readConfig();
	BTNTrigger *getBBTNTrigger(QString &triggername);
	QMap<QString,BTNTrigger*> getTriggerMap();
	QStringList getTriggerNodeList();

	BTNTriggerManager();
	~BTNTriggerManager();
private:
	QMap<QString,BTNTrigger*> m_TriggerMap;
	QStringList m_TriggerList;
};
#endif // BTNTRIGGER_H
