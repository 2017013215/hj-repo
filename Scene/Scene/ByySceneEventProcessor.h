#ifndef BYYSCENEEVENTPROCESSOR_H
#define BYYSCENEEVENTPROCESSOR_H

#include "ByyEventProcessor.h"

class ByyIG;

class ByySceneEventProcessor : public ByyEventProcessor
{
public:
	ByySceneEventProcessor(ByyIG& ig);
	~ByySceneEventProcessor();

	bool processOsgEvent(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);

protected:
	bool hasSelection(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa, int& out_select_type,unsigned int& out_select_id);

protected:
	ByyIG& myIG;	
};

#endif // BYYSCENEEVENTPROCESSOR_H
