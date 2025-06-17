#ifndef FORCETASK_H
#define FORCETASK_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QList>
#include <QMap>
#include <QStringList>

struct RefAttr_Task
{
	QString idf;
	QString note;
	QString type;
	QString options;
	QString initVal;
	QString range;
	QString unit;
	QString editEnable;
	RefAttr_Task()
	{

	}
};

class ForceTask
{
public:
	ForceTask();
	~ForceTask();

	void setObjectName(QString &name);
	QString getObjectName();
	void setNote(QString &note);
	QString getNote();
	void setList(QList<RefAttr_Task> &list);
	QList<RefAttr_Task> getList();
	void listAppend(RefAttr_Task &attr);
	void setValue(QString &note,QString &value);
	QString getContent();
private:
	QString m_ObjectName;
	QString m_Note;
	QList<RefAttr_Task> m_list_attrs;
};

class ForceTaskManager : public QObject
{
	Q_OBJECT

public:
	static ForceTaskManager *instance();
	void readConfig();
	ForceTask *getForceTask(QString &taskname);
	QMap<QString,ForceTask*> getTaskMap();
	QList<ForceTask*> getTaskList();
	QStringList getTaskNodeList();

	ForceTaskManager();
	~ForceTaskManager();
private:
	QMap<QString,ForceTask*> m_TaskMap;
	QList<ForceTask*> m_TaskList;
	QStringList m_TaskNodeList;
};

#endif // FORCETASK_H
