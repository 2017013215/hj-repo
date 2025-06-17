#include "StdAfx.h"
#include "ByyLogCanvas.h"
#include "ByyChannel.h"

ByyLogCanvas::ByyLogCanvas(ByyChannel* channel,QObject *parent)
	: QObject(parent)
{
	myOutputControl=new osgEarth::Util::Controls::LabelControl("");
	myOutputControl->setFont(osgText::readFontFile("../data/fonts/msyh.ttf"));
	myOutputControl->setEncoding(osgText::String::ENCODING_UTF8);
	myOutputControl->setSize(300,500);
	myOutputControl->setFontSize(12);
	myOutputControl->setAbsorbEvents(true);
	myOutputControl->setAlign(osgEarth::Util::Controls::Control::ALIGN_LEFT,osgEarth::Util::Controls::Control::ALIGN_BOTTOM);

	channel->addToCanvas(myOutputControl);
}

ByyLogCanvas::~ByyLogCanvas()
{

}

void ByyLogCanvas::setSource( QObject* sender,const char* signal )
{
	connect(sender,signal,this,SLOT(appendLog(double,QString)));
}

void ByyLogCanvas::appendLog( double simTime,QString log )
{
	if (myLogList.size()>15)
	{
		myLogList.removeFirst();
	}

	int second=((int)simTime)%60;
	int minute=((int)simTime)/60;
	if(minute>=60)	minute%=60;
	int hour=(((int)simTime)/60)/60;
	if(hour==24)	hour%=24;

	QTime t(hour,minute,second);
	QString str=t.toString(Qt::ISODate)+" "+log;

	myLogList.push_back(str);

	myOutputControl->setText(myLogList.join("").toUtf8().data());
}

void ByyLogCanvas::setVisible( bool visible )
{
	myOutputControl->setVisible(visible);
}
