#include "StdAfx.h"
#include "ByyDisplaySignaler.h"

ByyDisplaySignaler::ByyDisplaySignaler()
	: QObject()
{

}

ByyDisplaySignaler::~ByyDisplaySignaler()
{

}

ByyDisplaySignaler* ByyDisplaySignaler::instance()
{
	static ByyDisplaySignaler _instance;
	return &_instance;
}

void ByyDisplaySignaler::emitWindowCreated( ByyOsgWindow* window )
{
	emit windowCreated(window);
}

void ByyDisplaySignaler::emitWindowToBeDestroyed( ByyOsgWindow* window )
{
	emit windowToBeDestroyed(window);
}

void ByyDisplaySignaler::emitChannelCreated( ByyChannel* channel )
{
	emit channelCreated(channel);
}

void ByyDisplaySignaler::emitChannelToBeDestroyed( ByyChannel* channel )
{
	emit channelToBeDestroyed(channel);
}
