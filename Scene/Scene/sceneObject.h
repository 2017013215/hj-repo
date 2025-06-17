#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <QObject>
#include "enums.h"
#include "scene_global.h"

class ByyIG;
class ByyObject;

typedef QVector<osg::Vec3d> ByyVec3dArray;

namespace osgEarth
{
	namespace Symbology
	{
		class Style;
	}
}

class SCENE_EXPORT ByySceneObject : public QObject
{
	Q_OBJECT
public:
	ByySceneObject(ByyObject* obj,ByyIG& ig);
	virtual ~ByySceneObject();

	virtual void initiate()=0;

	virtual void update()=0;

	virtual osg::Group* root();

	virtual osg::Vec3d center()=0;

	virtual double radius()=0;

	osgEarth::MapNode* mapNode();
	const osgEarth::SpatialReference* srs();

	bool visible();

public slots:
	virtual void setVisible(bool b);
	virtual void setName(const QString& name)=0;
	virtual void setForceType(ByyForceType side)=0;

protected:
	void updateTextStyle(ByyForceType side,osgEarth::Symbology::Style& style);
	void updateLineStyle(ByyForceType side,osgEarth::Symbology::Style& style);
	void updatePolygonStyle(ByyForceType side,osgEarth::Symbology::Style& style);

protected:
	ByyIG& myIG;

	osg::ref_ptr<osg::Group> myRoot;

	osg::observer_ptr<osgEarth::MapNode> myMapNode;
	osg::observer_ptr<const osgEarth::SpatialReference> mySrs;
};

#endif // SCENEOBJECT_H
