#ifndef BYYUSEROPERATELOG_H
#define BYYUSEROPERATELOG_H

#include <QObject>
#include <QTime>
#include "snigcore_global.h"

class SNIGCORE_EXPORT ByyUserOperateLog : public QObject
{
	Q_OBJECT

public:
	~ByyUserOperateLog();

	struct Log
	{
		QTime time;
		QString log;
	};

	typedef QList<Log> LogList;

	void addLog(const QString& log);

	LogList& logList();

	static ByyUserOperateLog& instance();

private:
	ByyUserOperateLog();

signals:
	void logAdded(QTime time,QString log);

protected:
	LogList myLoglist;
};

#endif // BYYUSEROPERATELOG_H
