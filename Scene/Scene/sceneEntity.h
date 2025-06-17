#ifndef SCENEENTITY_H
#define SCENEENTITY_H

#include "sceneObject.h"
#include "scene_global.h"

class ByyEntityObject;

namespace osg
{
	class Image;
}

namespace osgSim
{
	class DOFTransform;
	class MultiSwitch;
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

namespace osgAudio
{
	class SoundState;
	class SoundUpdateCB;
}

class ByyRibbonModel;

class ByyStateVisualizer;

class SCENE_EXPORT ByySceneEntity : public ByySceneObject
{
	Q_OBJECT
public:
	ByySceneEntity(ByyEntityObject* entObj,ByyIG& ig);
	virtual ~ByySceneEntity();

	void initiate();

	osg::Vec3d center();

	double radius();

	osg::Node* modelNode();

	ByyEntityObject* entityObject();

	osg::Vec3d worldPosition();

	void setName(const QString& name){}

	QList<ByyStateVisualizer*>& getStateVisualizerList();

public slots:
	void update();
	void setForceType(ByyForceType side);
	void setAutoScaleEnabled(bool e);
	void enableSound(bool enable);
	void setSoundVolume(float value);
	void setSoundRange(int range);
	void setPosition(const osg::Vec3d& pos);
	void setOrientation(const osg::Vec3d& ori);
	void setDamageState(ByyDamageState ds);
	void setWake(); //xks

protected slots:
	void set2D(bool is2d);

protected:
	void updateArtParts();
	void updateMoveState();

	osg::ref_ptr<osgAudio::SoundState> createSoundState(const std::string& file);

protected:
	ByyEntityObject* myEntityObject;

	osg::ref_ptr<osg::Group> myLocalizedGroup; 

	osg::ref_ptr<osgEarth::Annotation::LocalGeometryNode> myLocalNode;
	osg::observer_ptr<osg::MatrixTransform> myLocalNodeMatrixTransfrom;

	osg::ref_ptr<osgEarth::PixelAutoTransform> myAutoScaleTransform;

	osg::ref_ptr<osg::Node> myModel;

	osg::ref_ptr<osgSim::MultiSwitch> damageSwitch;
	osg::ref_ptr<osgSim::MultiSwitch> healthSwitch;
	osg::ref_ptr<osgSim::MultiSwitch> movingSwitch;
	QMultiMap<int,osgSim::DOFTransform*> artMaps;


	osg::ref_ptr<osgAudio::SoundState> soundState;
	osg::ref_ptr< osgAudio::SoundUpdateCB > soundCB;

	osg::ref_ptr<osg::Node> mySmoke;
	osg::ref_ptr<osg::Node> myFlames;
	osg::ref_ptr<osg::Node> myWake; // xks

	QList<ByyStateVisualizer*> myStateVisualizerList;
};

#endif // SCENEENTITY_H
