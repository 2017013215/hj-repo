#include "StdAfx.h"
#include "ByyOutputWidget.h"
#include "ByyHdose.h"
#include "ByyUserOperateLog.h"

ByyOutputWidget::ByyOutputWidget(ByyHdose& hdose,QWidget *parent)
	: QWidget(parent)
	,myHdose(hdose)
{
	ui.setupUi(this);

	connect(myHdose.simEventLogDatabase(),SIGNAL(cleard()),
		this,SLOT(cleatSimEventLog()),Qt::QueuedConnection);
}

ByyOutputWidget::~ByyOutputWidget()
{

}

void ByyOutputWidget::showEvent( QShowEvent *e )
{
	ui.hdose_plainTextEdit->setPlainText(ByyHdoseLogDatabase::instance().logList().join("\n"));

	ByyHdoseSimEventLogDatabase::LogList& logList=myHdose.simEventLogDatabase()->logList();

	ByyHdoseSimEventLogDatabase::LogList::iterator it=logList.begin();

	for (;it!=logList.end();++it)
	{
		appendSimEventLog(it->simTime,it->log);
	}

	ByyUserOperateLog::LogList& userLogList=ByyUserOperateLog::instance().logList();

	ByyUserOperateLog::LogList::iterator userIt=userLogList.begin();

	for(;userIt!=userLogList.end();++userIt)
	{
		appendUserLog(userIt->time,userIt->log);
	}

	connect(&ByyHdoseLogDatabase::instance(),SIGNAL(logAdded(QString)),this,SLOT(appendHdoseLog(QString)),Qt::QueuedConnection);
	connect(&ByyHdoseCollisionDetectionLogDatabase::instance(),SIGNAL(appendCollisionDetectionLogAdded(QString)),this,SLOT(appendCollisionDetectionLog(QString)),Qt::QueuedConnection);

	connect(myHdose.simEventLogDatabase(),SIGNAL(logAdded(double,QString)),
		this,SLOT(appendSimEventLog(double,QString)),Qt::QueuedConnection);

	connect(&ByyUserOperateLog::instance(),SIGNAL(logAdded(QTime,QString)),
		this,SLOT(appendUserLog(QTime,QString)),Qt::QueuedConnection);

	__super::showEvent(e);
}

void ByyOutputWidget::hideEvent( QHideEvent *e )
{
	ui.simEvent_plainTextEdit->clear();
	ui.hdose_plainTextEdit->clear();
	ui.user_plainTextEdit->clear();

	disconnect(&ByyHdoseLogDatabase::instance(),SIGNAL(logAdded(QString)),this,SLOT(appendHdoseLog(QString)));
	disconnect(myHdose.simEventLogDatabase(),SIGNAL(logAdded(double,QString)),this,SLOT(appendSimEventLog(double,QString)));
	disconnect(&ByyUserOperateLog::instance(),SIGNAL(logAdded(QTime,QString)),this,SLOT(appendUserLog(QTime,QString)));
	disconnect(&ByyHdoseCollisionDetectionLogDatabase::instance(),SIGNAL(appendCollisionDetectionLogAdded(QString)),this,SLOT(appendCollisionDetectionLog(QTime,QString)));

	__super::hideEvent(e);
}

void ByyOutputWidget::appendHdoseLog( QString log )
{
	ui.hdose_plainTextEdit->appendPlainText(log);
}

void ByyOutputWidget::appendSimEventLog(double simTime,QString log)
{
	int second=((int)simTime)%60;
	int minute=((int)simTime)/60;
	if(minute>=60)	minute%=60;
	int hour=(((int)simTime)/60)/60;
	if(hour==24)	hour%=24;

	QTime t(hour,minute,second);
	QString str=t.toString(Qt::ISODate)+" "+log;

	ui.simEvent_plainTextEdit->appendPlainText(str);
}

void ByyOutputWidget::cleatSimEventLog()
{
	ui.simEvent_plainTextEdit->clear();
}

void ByyOutputWidget::appendUserLog( QTime time,QString log )
{
	QString str=time.toString(Qt::ISODate)+" "+log+"\n";

	ui.user_plainTextEdit->appendPlainText(str);
}

void ByyOutputWidget::appendCollisionDetectionLog(QString log)
{
	ui.collisionDetection_plainTextEdit->appendPlainText(log);
}

QSize ByyOutputWidget::sizeHint() const
{
	return QSize(700,250);
}
