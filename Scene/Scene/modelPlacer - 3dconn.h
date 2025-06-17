#ifndef MODELPLACER_H
#define MODELPLACER_H

#include <QObject>
#include "ByyEventProcessor.h"
#include "scene_global.h"
#include "entityType.h"

namespace osgEarth
{
	namespace Annotation
	{
		class LocalGeometryNode;
	}
}

class ByyIG;

class SCENE_EXPORT ByyModelPlacer : public QObject ,public ByyEventProcessor
{
	Q_OBJECT

public:
	~ByyModelPlacer();

	static ByyModelPlacer& instance(ByyIG& ig);

	bool setModelType(const ByyEntityType& type);

	ByyEntityType modelType()const;

	bool processOsgEvent(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);

	void release();

	void connexion3D(long TX,long TY,long TZ,long RX,long RY,long RZ);

signals:
	void push(const osgEarth::GeoPoint&);
	void released();

protected:
	ByyModelPlacer(ByyIG& ig);

protected:
	ByyIG& myIG;

	ByyEntityType myEntityType;

	osg::ref_ptr<osgEarth::Annotation::LocalGeometryNode> myPlaceNode;

	float myPushX;
	float myPushY;

	float myRotate;
};

#endif // MODELPLACER_H
