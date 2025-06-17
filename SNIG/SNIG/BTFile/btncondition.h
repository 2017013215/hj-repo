#ifndef BTNCONDITION_H
#define BTNCONDITION_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QList>
#include <QMap>
#include <QStringList>

class ForceTask;
class BTNCompose;
class BTNTime;
class BTNTrigger;

struct RefAttr_Condition
{
	QString idf;
	QString note;
	QString type;
	QString initVal;
	QStringList nameList;
	QStringList valueList;
	QMap<QString, QString> map;
	RefAttr_Condition()
	{

	}
};
class BTNCondition
{
public:
	BTNCondition();
	~BTNCondition();

	void setObjectName(QString &name);
	QString getObjectName();
	void setNote(QString &note);
	QString getNote();
	void setList(QList<RefAttr_Condition> &list);
	QList<RefAttr_Condition> getList();
	void listAppend(RefAttr_Condition &attr);
	void setValue(QString &note,QString &value);
	QString getContent();
private:
	QString m_ObjectName;
	QString m_Note;
	QList<RefAttr_Condition> m_list_attrs;
public:
	//触发器列表
	QList<BTNTrigger*> m_TriggerList;
	//复合节点
	QList<BTNCompose*> m_ComposeList;
	//时间节点
	QList<BTNTime*> m_TimeList;
	//任务列表
	QList<ForceTask*> m_TaskList;

};

class BTNConditionManager : public QObject
{
	Q_OBJECT

public:
	static BTNConditionManager *instance();
	void readConfig();
	BTNCondition *getBBTNCondition(QString &conditionname);
	QStringList getConditionNodeList();

	BTNConditionManager();
	~BTNConditionManager();
private:
	QMap<QString,BTNCondition*> m_ConditionMap;
	QStringList m_ConditionList;
};
#endif // BTNCONDITION_H
