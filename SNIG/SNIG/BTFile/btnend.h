#ifndef BTNEND_H
#define BTNEND_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QList>
#include <QMap>
#include <QStringList>

struct RefAttr_End
{
	QString idf;
	QString note;
	QString type;
	QString initVal;
	QStringList nameList;
	QStringList valueList;
	QMap<QString, QString> map;
	RefAttr_End()
	{

	}
};
class BTNEnd
{
public:
	BTNEnd();
	~BTNEnd();

	void setObjectName(QString &name);
	QString getObjectName();
	void setNote(QString &note);
	QString getNote();
	void setList(QList<RefAttr_End> &list);
	QList<RefAttr_End> getList();
	void listAppend(RefAttr_End &attr);
	void setValue(QString &note,QString &value);
	QString getContent();
private:
	QString m_ObjectName;
	QString m_Note;
	QList<RefAttr_End> m_list_attrs;
public:
};
class BTNEndManager : public QObject
{
	Q_OBJECT

public:
	static BTNEndManager *instance();
	void readConfig();
	BTNEnd *getBBTNEnd(QString &endname);
	QStringList getEndNodeList();

	BTNEndManager();
	~BTNEndManager();
private:
	QMap<QString,BTNEnd*> m_EndMap;
	QStringList m_EndList;
};
#endif // BTNEND_H
