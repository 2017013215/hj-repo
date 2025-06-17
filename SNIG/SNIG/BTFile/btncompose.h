#ifndef BTNCOMPOSE_H
#define BTNCOMPOSE_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QList>
#include <QMap>
#include <QStringList>

class BTNTrigger;
class ForceTask;
class BTNTime;

struct RefAttr_Compose
{
	QString idf;
	QString note;
	QString type;
	QString initVal;
	QStringList nameList;
	QStringList valueList;
	QMap<QString, QString> map;
	RefAttr_Compose()
	{

	}
};
class BTNCompose
{
public:
	BTNCompose();
	~BTNCompose();

	void setObjectName(QString &name);
	QString getObjectName();
	void setNote(QString &note);
	QString getNote();
	void setList(QList<RefAttr_Compose> &list);
	QList<RefAttr_Compose> getList();
	void listAppend(RefAttr_Compose &attr);
	void setValue(QString &note,QString &value);
	QString getContent();
private:
	QString m_ObjectName;
	QString m_Note;
	QList<RefAttr_Compose> m_list_attrs;
public:
	//触发器列表
	QList<BTNTrigger*> m_TriggerList;
	//复合节点列表
	QList<BTNCompose*> m_ComposeList;
	//时间节点
	QList<BTNTime*> m_TimeList;
	//任务列表
	QList<ForceTask*> m_TaskList;
	
};

class BTNComposeManager : public QObject
{
	Q_OBJECT

public:
	static BTNComposeManager *instance();
	void readConfig();
	BTNCompose *getBTNCompose(QString &composename);
	//QMap<QString,BTNRoot*> getRootMap();
	//QList<BTNRoot*> getRootList();
	//QStringList getRootNodeList();

	BTNComposeManager();
	~BTNComposeManager();
private:
	QMap<QString,BTNCompose*> m_ComposeMap;
};
#endif // BTNCOMPOSE_H
