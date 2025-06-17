#include "stdafx.h"
#include "screenRecordWidget.h"
#include "snapshoot.h"
#include "ByyIG.h"
#include "ByyInputController.h"

#define INTERVAL 300

ByyScreenRecordWidget::ByyScreenRecordWidget(ByyIG& ig,QMainWindow* m,QWidget *parent /*= 0*/ )
	: QWidget(parent,Qt::Window)
	,myIG(ig)
	,mainWindow(m)
	,_started(false)
{
	ui.setupUi(this);

	ui.pushButton->setIcon(QIcon(ICONPATH+"play.png"));
	ui.pushButton_2->setIcon(QIcon(ICONPATH+"pause.png"));
	ui.pushButton_3->setIcon(QIcon(ICONPATH+"stop.png"));

	ui.pushButton_2->setVisible(false);
	ui.pushButton_3->setVisible(false);

	connect(&timer,SIGNAL(timeout()),SLOT(timeout()));

	connect(ui.pushButton,SIGNAL(clicked()),SLOT(start()));
	connect(ui.pushButton_2,SIGNAL(clicked()),SLOT(pauseOrCoutiue()));
	connect(ui.pushButton_3,SIGNAL(clicked()),SLOT(stop()));
}

ByyScreenRecordWidget::~ByyScreenRecordWidget()
{

}

void ByyScreenRecordWidget::start()
{
	if (!_started)
	{
		elapsedTime=0;
		_started=true;

		QString file=QString("../data/snapshoots/")+QDateTime::currentDateTime().toString("yyyyMMddhhmmss")+".avi";
		myIG.inputController().currentChannel()->snapshoot()->startRecord(file.toStdString(),mainWindow->centralWidget()->width(),mainWindow->centralWidget()->height());

		timer.start(INTERVAL);

		ui.pushButton->setVisible(false);
		ui.pushButton_2->setVisible(true);
		ui.pushButton_3->setVisible(true);
		ui.pushButton_2->setText(QString::fromLocal8Bit("ÔÝÍ£"));
		ui.pushButton_2->setIcon(QIcon(ICONPATH+"pause.png"));
	}

}

void ByyScreenRecordWidget::pauseOrCoutiue()
{
	myIG.inputController().currentChannel()->snapshoot()->pauseOrCountiue();

	if (timer.isActive())
	{
		timer.stop();
		ui.pushButton_2->setText(QString::fromLocal8Bit("¼ÌÐø"));
		ui.pushButton_2->setIcon(QIcon(ICONPATH+"play.png"));
	}
	else
	{
		timer.start(INTERVAL);
		ui.pushButton_2->setText(QString::fromLocal8Bit("ÔÝÍ£"));
		ui.pushButton_2->setIcon(QIcon(ICONPATH+"pause.png"));
	}

}

void ByyScreenRecordWidget::stop()
{
	if (_started)
	{
		myIG.inputController().currentChannel()->snapshoot()->stopRecord();

		timer.stop();
		_started=false;

		ui.pushButton->setVisible(true);
		ui.pushButton_2->setVisible(false);
		ui.pushButton_3->setVisible(false);
	}
}

void ByyScreenRecordWidget::timeout()
{
	elapsedTime+=INTERVAL;

	qint64 elapsed=elapsedTime;

	int day=elapsed/(24*60*60*1000);

	elapsed=elapsed%(24*60*60*1000);

	int hour=elapsed/(60*60*1000);

	elapsed=elapsed%(60*60*1000);

	int minute=elapsed/(60*1000);

	elapsed=elapsed%(60*1000);

	int sec=elapsed/(1000);

	elapsed=elapsed%(1000);

	ui.label->setText(QString("%1:%2:%3:%4.%5").arg(day)
		.arg(hour,2,10,QLatin1Char('0'))
		.arg(minute,2,10,QLatin1Char('0'))
		.arg(sec,2,10,QLatin1Char('0'))
		.arg(elapsed,3,10,QLatin1Char('0')));
}
