#ifndef BYYEVENTPROCESSOR_H
#define BYYEVENTPROCESSOR_H

#include "scene_global.h"

class ByyChannel;

class SCENE_EXPORT ByyEventProcessor
{
public:
	ByyEventProcessor();
	virtual ~ByyEventProcessor();

	virtual bool processEvent(const osgGA::GUIEventAdapter& ea,ByyChannel &channel);
	virtual bool processOsgEvent(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);
};

template<class T>
class ByyHostEventProcessor : public ByyEventProcessor
{
public:
	ByyHostEventProcessor(T *host)
		:myHost(host)
	{

	}
	virtual ~ByyHostEventProcessor(){}

	bool processOsgEvent(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
	{
		return myHost->processEvent(ea,aa);
	}

protected:
	T* myHost;
};

#endif // BYYEVENTPROCESSOR_H
