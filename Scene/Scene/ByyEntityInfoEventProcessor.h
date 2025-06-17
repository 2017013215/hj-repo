#ifndef BYYENTITYINFOEVENTPROCESSOR_H
#define BYYENTITYINFOEVENTPROCESSOR_H

#include "ByyEventProcessor.h"

class ByyIG;
class ByyEntityObject;

class ByyEntityInfoEventProcessor : public QObject ,public ByyEventProcessor
{
	Q_OBJECT
public:
	ByyEntityInfoEventProcessor(ByyIG& ig);
	~ByyEntityInfoEventProcessor();

	bool processOsgEvent(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);

protected:
	void update();

protected slots:
	void onEntityRemoved(ByyEntityObject *entObj);

protected:
	ByyIG& myIG;	
	ByyEntityObject* myEntObj;
};

#endif // BYYENTITYINFOEVENTPROCESSOR_H
