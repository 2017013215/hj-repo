#ifndef BTNLOOP_H
#define BTNLOOP_H

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

struct RefAttr_Loop
{
	QString idf;
	QString note;
	QString type;
	QString initVal;
	QStringList nameList;
	QStringList valueList;
	QMap<QString, QString> map;
	RefAttr_Loop()
	{

	}
};
class BTNLoop
{
public:
	BTNLoop();
	~BTNLoop();

	void setObjectName(QString &name);
	QString getObjectName();
	void setNote(QString &note);
	QString getNote();
	void setList(QList<RefAttr_Loop> &list);
	QList<RefAttr_Loop> getList();
	void listAppend(RefAttr_Loop &attr);
	void setValue(QString &note,QString &value);
	QString getContent();
private:
	QString m_ObjectName;
	QString m_Note;
	QList<RefAttr_Loop> m_list_attrs;
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
class BTNLoopManager : public QObject
{
	Q_OBJECT

public:
	static BTNLoopManager *instance();
	void readConfig();
	BTNLoop *getBBTNLoop(QString &loopname);
	QStringList getLoopNodeList();

	BTNLoopManager();
	~BTNLoopManager();
private:
	QMap<QString,BTNLoop*> m_LoopMap;
	QStringList m_LoopList;
};
#endif // BTNLOOP_H
