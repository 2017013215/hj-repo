#ifndef BYYOSGEVENTCONTROLLER_H
#define BYYOSGEVENTCONTROLLER_H

#include <QObject>
#include "scene_global.h"

class ByyIG;
class ByyChannel;
class ByyObserver;
class ByyEventProcessor;

class SCENE_EXPORT ByyInputController : public QObject
{
	Q_OBJECT

public:
	ByyInputController(ByyIG& ig,QObject *parent=0);
	~ByyInputController();

	void addEventProcessor(ByyEventProcessor* eventProcessor);
	void removeEventProcessor(ByyEventProcessor* eventProcessor);

	bool  processEvent(const osgGA::GUIEventAdapter &ea, ByyChannel &channel); 

	ByyObserver* getDefaultObserver();
	ByyObserver* createObserver();
	inline ByyObserver* currentObserver();
	inline ByyChannel* currentChannel();
	void setCurrentChannel(ByyChannel* channel);

	typedef QList<ByyEventProcessor*> EventProcessorList;
	typedef QMap<QString,ByyObserver*> ObserversMap; 

	friend class ByySceneEventProcessor;

protected slots:
	void addChannel(ByyChannel* channel);

signals:
	void currrentObserverChanged(ByyObserver*);
	void requestContextMenu();
	void requsetDoubleClick();

protected:
	ByyIG& myIG;
	EventProcessorList myEventProcessorList;
	ObserversMap myObservers;
	ByyObserver* myCurrentObserver;
	ByyChannel* myCurrentChannel;
};

#endif // BYYOSGEVENTCONTROLLER_H
