#include "StdAfx.h"
#include "ByyEventProcessor.h"
#include "ByyChannel.h"

ByyEventProcessor::ByyEventProcessor()
{
	
}

ByyEventProcessor::~ByyEventProcessor()
{

}

bool ByyEventProcessor::processEvent( const osgGA::GUIEventAdapter& ea,ByyChannel &channel )
{
	return processOsgEvent(ea,*channel.view());
}

bool ByyEventProcessor::processOsgEvent( const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa )
{
	return false;
}
