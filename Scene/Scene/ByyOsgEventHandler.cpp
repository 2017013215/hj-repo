#include "StdAfx.h"
#include "ByyOsgEventHandler.h"
#include "ByyInputController.h"
#include "ByyChannel.h"

ByyOsgEventHandler::ByyOsgEventHandler(ByyInputController& controller,ByyChannel& channel)
	:myController(controller)
	,myChannel(channel)
{
	channel.addEventHandler(this);
}

ByyOsgEventHandler::~ByyOsgEventHandler()
{

}

bool ByyOsgEventHandler::handle( const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa )
{
	return  myController.processEvent(ea,myChannel);
}
