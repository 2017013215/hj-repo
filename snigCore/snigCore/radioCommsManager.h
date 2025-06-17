#ifndef RADIOCOMMSMANAGER_H
#define RADIOCOMMSMANAGER_H

#include <QObject>
#include <QMultiMap>
#include <QVector>
#include <osg/Vec3d>
#include "snigcore_global.h"

class ByyCore;
class ByyEntityObject;

class SNIGCORE_EXPORT DtRadioCommsManager : public QObject
{
	Q_OBJECT

public:
	DtRadioCommsManager(ByyCore& core);
	~DtRadioCommsManager();

public slots:

	void addComm(int senderId,int targetId,bool connected);

	void addTrack(int senderId,int targetId,bool connected);

	void addGuidance(int senderId,int targetId,bool connected);

	void addRoutePlanning(int entId,QVector<osg::Vec3d> pnts);

	void clear();

	void removeEntObject(ByyEntityObject* entObj);

signals:
	void commAdded(int,int);
	void commRemoved(int,int);

	void trackAdded(int,int);
	void trackRemoved(int,int);

	void guidanceAdded(int,int);
	void guidanceRemoved(int,int);

	void routePlanningAdded(int entId,const QVector<osg::Vec3d>& pnts);
	void routePlanningRemoved(int entId);

protected:
	ByyCore& myCore;

	typedef QMultiMap<int,int> CommObjects;
	typedef QMap<int,int>::iterator Iter;

	CommObjects myCommObjects;
	CommObjects myTrackObjects;
	CommObjects myGuidanceObjects;
};

#endif // RADIOCOMMSMANAGER_H
