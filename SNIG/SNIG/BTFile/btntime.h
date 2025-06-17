#ifndef BTNTIME_H
#define BTNTIME_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QList>
#include <QMap>
#include <QStringList>

class BTNTrigger;
class ForceTask;
class BTNCompose;

struct RefAttr_Time
{
	QString idf;
	QString note;
	QString type;
	QString initVal;
	QStringList nameList;
	QStringList valueList;
	QMap<QString, QString> map;
	RefAttr_Time()
	{

	}
};
class BTNTime
{
public:
	BTNTime();
	~BTNTime();

	void setObjectName(QString &name);
	QString getObjectName();
	void setNote(QString &note);
	QString getNote();
	void setList(QList<RefAttr_Time> &list);
	QList<RefAttr_Time> getList();
	void listAppend(RefAttr_Time &attr);
	void setValue(QString &note,QString &value);
	QString getContent();
private:
	QString m_ObjectName;
	QString m_Note;
	QList<RefAttr_Time> m_list_attrs;
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

class BTNTimeManager : public QObject
{
	Q_OBJECT

public:
	static BTNTimeManager *instance();
	void readConfig();
	BTNTime *getBTNTime(QString &timename);
	//QMap<QString,BTNRoot*> getRootMap();
	//QList<BTNRoot*> getRootList();
	//QStringList getRootNodeList();

	BTNTimeManager();
	~BTNTimeManager();
private:
	QMap<QString,BTNTime*> m_TimeMap;
};
#endif // BTNTIME_H
