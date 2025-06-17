#include "StdAfx.h"
#include "simTimeWidget.h"
#include "ByyApp.h"
#include "ByyHdose.h"
#include "simuController.h"
#include "../snigHdose/scenarioManager.h"
#include "../snigHdose/scenarioV2.h"

ByySimTimeWidget::ByySimTimeWidget( ByyApp &app,QWidget *parent /*= 0*/ ) 
	: QWidget(parent)
	,myApp(app)
{
	ui.setupUi(this);

	ui.lcdNumber->display("0000:00:00  00:00:00");
	ui.lcdNumber->setNumDigits(20);
	connect(&myTimer,SIGNAL(timeout()),SLOT(update()));
	myTimer.setInterval(400);

	connect(&myApp.hdose().simuController(),SIGNAL(signal_StatusChanged(int)),SLOT(onSimStatusChanged()));

}

ByySimTimeWidget::~ByySimTimeWidget()
{

}


void ByySimTimeWidget::update()
{
	double simTime=myApp.hdose().simTime();
	double realTime=osg::Timer::instance()->time_s();

	/*int second=((int)simTime)%60;
	int minute=((int)simTime)/60;
	if(minute>=60)	minute%=60;
	int hour=(((int)simTime)/60)/60;
	if(hour==24)	hour%=24;

	QTime t(hour,minute,second);*/
	//EpochTimeStruct start,end;
	//double simTime;
	//bool ret=GetBackScnTime(start, end, simTime);

	//GetBackScnTime()
	QDateTime curTime;//myApp.hdose().scenarioManager().scenario()->getScnInfo().myStartDateTime.addSecs(simTime);
	if(myApp.hdose().simDateTime(curTime))
	{
		ui.lcdNumber->display(curTime.toString("yyyy:MM:dd  hh:mm:ss"));
		//curTime=curTime.addSecs(simTime);
	}


	if (myLastSimTime!=-1)
	{
		double multiplier=(simTime-myLastSimTime)/(realTime-myLastRealTime);

		ui.lcdNumber_2->display(multiplier);
	}
	
	myLastSimTime=simTime;
	myLastRealTime=realTime;;
}

void ByySimTimeWidget::onSimStatusChanged()
{
	if (myApp.hdose().simuController().isRunning())
	{
		myLastSimTime=-1;
		myLastRealTime=-1;
		myTimer.start();
	}
	else
		myTimer.stop();

	if (myApp.hdose().simuController().status()==myApp.hdose().simuController().S_NULL)
	{
		ui.lcdNumber->display("0000:00:00  00:00:00");
		ui.lcdNumber_2->display(0);
	}
}
