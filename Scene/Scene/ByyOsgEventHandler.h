#ifndef BYYOSGEVENTHANDLER_H
#define BYYOSGEVENTHANDLER_H

#include <osgGA/GUIEventHandler>

class ByyInputController;
class ByyChannel;

class ByyOsgEventHandler : public osgGA::GUIEventHandler
{
public:
	ByyOsgEventHandler(ByyInputController& controller,ByyChannel& channel);
	~ByyOsgEventHandler();

	bool  handle (const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa) ;

protected:
	ByyInputController& myController;
	ByyChannel& myChannel;
};

#endif // BYYOSGEVENTHANDLER_H
