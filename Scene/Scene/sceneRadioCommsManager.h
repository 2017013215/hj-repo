#ifndef SCENERADIOCOMMSMANAGER_H
#define SCENERADIOCOMMSMANAGER_H

#include <QObject>
#include <QMap>

class ByyIG;
class ByyEntityObject;
class ByyRadioCommsVisualizer;
class ByyTrackVisualizer;
class ByyGuidanceVisualizer;
class ByyRoutePlanVisualizer;

class ByySceneRadioCommsManager : public QObject
{
	Q_OBJECT

public:
	ByySceneRadioCommsManager(ByyIG& ig);
	~ByySceneRadioCommsManager();

public slots:
	void commAdded(int senderId,int targetId);
	void commRemoved(int senderId,int targetId);

	void trackAdded(int senderId,int targetId);
	void trackRemoved(int senderId,int targetId);

	void guidanceAdded(int senderId,int targetId);
	void guidanceRemoved(int senderId,int targetId);

	void routePlanningAdded(int entId,const QVector<osg::Vec3d>& pnts);
	void routePlanningRemoved(int entId);

protected:
	ByyIG& myIG;

	typedef QPair<int,int> CommObjects;

	typedef QMap<CommObjects,ByyRadioCommsVisualizer*> CommVisualizers;

	typedef QMap<CommObjects,ByyRadioCommsVisualizer*>::iterator Iter;

	CommVisualizers myCommVisualizers;

	QMap<CommObjects,ByyTrackVisualizer*> myTrackVisualizers;

	QMap<CommObjects,ByyGuidanceVisualizer*> myGuidanceVisualizers;

	QMultiMap<int,ByyRoutePlanVisualizer*> myRoutePlanVisualizers;
};

#endif // SCENERADIOCOMMSMANAGER_H
