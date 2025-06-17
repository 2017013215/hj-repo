#include "ByyUserOperateLog.h"

ByyUserOperateLog::ByyUserOperateLog()
	: QObject()
{

}

ByyUserOperateLog::~ByyUserOperateLog()
{

}

void ByyUserOperateLog::addLog( const QString& log )
{
	Log log_st={QTime::currentTime(),log};

	myLoglist.append(log_st);

	emit logAdded(log_st.time,log);
}

ByyUserOperateLog& ByyUserOperateLog::instance()
{
	static ByyUserOperateLog _instance;
	return _instance;
}

ByyUserOperateLog::LogList& ByyUserOperateLog::logList()
{
	return myLoglist;
}
