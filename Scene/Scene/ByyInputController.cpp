#include "StdAfx.h"
#include "ByyInputController.h"
#include "ByyOsgEventHandler.h"
#include "ByyEventProcessor.h"
#include "ByyObserver.h"
#include "ByyChannel.h"
#include "ByyDisplaySignaler.h"
#include "ByySceneEventProcessor.h"
#include "ByyEntityInfoEventProcessor.h"

ByyInputController::ByyInputController( ByyIG& ig,QObject *parent/*=0*/ ) 
	: QObject(parent)
	,myIG(ig)
	,myCurrentObserver(0)
	,myCurrentChannel(0)
{
	myCurrentObserver=createObserver();

	addEventProcessor(new ByySceneEventProcessor(myIG));
	addEventProcessor(new ByyEntityInfoEventProcessor(myIG));

	connect(ByyDisplaySignaler::instance(),SIGNAL(channelCreated(ByyChannel*)),this,SLOT(addChannel(ByyChannel*)));
}

ByyInputController::~ByyInputController()
{

}

bool ByyInputController::processEvent( const osgGA::GUIEventAdapter &ea, ByyChannel &channel )
{
	/*if (channel.getObserver()!=myCurrentObserver&&ea.getEventType()==ea.PUSH)
	{
		myCurrentChannel=&channel;
		myCurrentObserver=channel.getObserver();
		emit currrentObserverChanged(myCurrentObserver);
	}*/

	for (int i=0;i!=myEventProcessorList.size();++i)
	{
		if (myEventProcessorList[i]->processEvent(ea,channel))
		{
			return true;
		}
	}

	return false;
}

void ByyInputController::addEventProcessor( ByyEventProcessor* eventProcessor )
{
	myEventProcessorList.append(eventProcessor);
}

void ByyInputController::removeEventProcessor( ByyEventProcessor* eventProcessor )
{
	myEventProcessorList.removeOne(eventProcessor);
}

void ByyInputController::addChannel( ByyChannel* channel )
{
	ByyOsgEventHandler *eventHandler=new ByyOsgEventHandler(*this,*channel);
}

ByyObserver* ByyInputController::getDefaultObserver()
{
	if (myObservers.isEmpty())
	{
		createObserver();
	}

	return myObservers.begin().value();
}

ByyObserver* ByyInputController::createObserver()
{
	static int index=1;

	QString observerName=QString::fromLocal8Bit("²Ù×÷Æ÷ %1").arg(index++);

	ByyObserver* observer=new ByyObserver;
	observer->reset();

	myObservers[observerName]=observer;

	if (myCurrentObserver==0)
	{
		myCurrentObserver=observer;
	}

	return observer;
}

ByyObserver* ByyInputController::currentObserver()
{
	return myCurrentObserver;
}

ByyChannel* ByyInputController::currentChannel()
{
	return myCurrentChannel;
}

void ByyInputController::setCurrentChannel( ByyChannel* channel )
{
	myCurrentChannel=channel;
}
