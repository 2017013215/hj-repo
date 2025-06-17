#ifndef BYYSCENEDETECT_H
#define BYYSCENEDETECT_H

#include "sceneObject.h"
#include "scene_global.h"

class ByyDetectObject;

namespace osg
{
	class Image;
}


namespace osgEarth
{
	namespace Annotation
	{
		class LocalGeometryNode;
		class PlaceNode;
		class FeatureNode;
	}
	namespace Symbology
	{
		class Style;
		class LineString;
	}

	class PixelAutoTransform;
}

class ByyDetectIndicateVisualizer;

class SCENE_EXPORT ByySceneDetectObject : public ByySceneObject
{
	Q_OBJECT
public:
	ByySceneDetectObject(ByyDetectObject* entObj,ByyIG& ig);
	virtual ~ByySceneDetectObject();

	void initiate();

	osg::Vec3d center();

	double radius();

	osg::Node* modelNode();

	ByyDetectObject* detectObject();

	osg::Vec3d worldPosition();

	void setName(const QString& name){}

public slots:
	void update();
	void setForceType(ByyForceType side);
	void setAutoScaleEnabled(bool e);
	void updatePositionAndOri();

protected slots:
	void set2D(bool is2d);

protected:
	ByyDetectObject* myDetectObject;

	osg::ref_ptr<osg::Group> myLocalizedGroup; 

	osg::ref_ptr<osgEarth::Annotation::LocalGeometryNode> myLocalNode;
	osg::observer_ptr<osg::MatrixTransform> myLocalNodeMatrixTransfrom;

	osg::ref_ptr<osgEarth::PixelAutoTransform> myAutoScaleTransform;

	osg::ref_ptr<osg::Node> myModel;

	ByyDetectIndicateVisualizer* myIndicateVisualizer;
};

#endif // BYYSCENEDETECT_H
