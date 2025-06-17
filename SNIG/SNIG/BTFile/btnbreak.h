#ifndef BTNBREAK_H
#define BTNBREAK_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QList>
#include <QMap>
#include <QStringList>

struct RefAttr_Break
{
	QString idf;
	QString note;
	QString type;
	QString initVal;
	QStringList nameList;
	QStringList valueList;
	QMap<QString, QString> map;
	RefAttr_Break()
	{

	}
};
class BTNBreak
{
public:
	BTNBreak();
	~BTNBreak();

	void setObjectName(QString &name);
	QString getObjectName();
	void setNote(QString &note);
	QString getNote();
	void setList(QList<RefAttr_Break> &list);
	QList<RefAttr_Break> getList();
	void listAppend(RefAttr_Break &attr);
	void setValue(QString &note,QString &value);
	QString getContent();
private:
	QString m_ObjectName;
	QString m_Note;
	QList<RefAttr_Break> m_list_attrs;
public:
};
class BTNBreakManager : public QObject
{
	Q_OBJECT

public:
	static BTNBreakManager *instance();
	void readConfig();
	BTNBreak *getBBTNBreak(QString &breakname);
	QStringList getBreakNodeList();

	BTNBreakManager();
	~BTNBreakManager();
private:
	QMap<QString,BTNBreak*> m_BreakMap;
	QStringList m_BreakList;
};
#endif // BTNBREAK_H
