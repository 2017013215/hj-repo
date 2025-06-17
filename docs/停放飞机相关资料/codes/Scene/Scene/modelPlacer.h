#ifndef MODELPLACER_H
#define MODELPLACER_H

#include <QObject>
#include "ByyEventProcessor.h"
#include "scene_global.h"
#include "entityType.h"
#include <QCursor>

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
	static ByyModelPlacer* instance();
	bool setModelType(const ByyEntityType& type);
	void	enableDeploy(bool deploy);

	ByyEntityType modelType()const;

	bool processOsgEvent(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);

	void release();

	bool	checkTerrainObstacle(double lon,double lat,double len,double width);
	bool	checkTerrainObstacle(double lon,double lat);

	QCursor cursor(){return _cursor;}

signals:
	void push(const osgEarth::GeoPoint&, const osg::Vec3d&,int);
	void released();
public slots:

protected:
	ByyModelPlacer(ByyIG& ig);

protected:
	ByyIG& myIG;

	ByyEntityType myEntityType;

	osg::ref_ptr<osgEarth::Annotation::LocalGeometryNode> myPlaceNode;

	float myPushX;
	float myPushY;


	QCursor	_cursor;
	bool	_deploy;
};

#endif // MODELPLACER_H
