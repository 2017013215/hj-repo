#ifndef SCENECONTROLOBJECT_H
#define SCENECONTROLOBJECT_H

#include "sceneObject.h"

namespace osgEarth
{
	namespace Annotation
	{
		class LabelNode;
	}
}

class ByyControlObject;

class SCENE_EXPORT ByySceneControlObject : public ByySceneObject
{
	Q_OBJECT
public:
	ByySceneControlObject(ByyControlObject* ctrlObj,ByyIG& ig);
	virtual ~ByySceneControlObject();

	virtual void update();

public slots:
	virtual void setName(const QString& text);
	virtual void setForceType(ByyForceType side);
	virtual void setClampToTerrain(bool clamp);
	virtual void setCtrlObjLableVisible(bool);

protected:
	void updateClampStyle(bool clamp,osgEarth::Symbology::Style& style);

protected:
	osg::ref_ptr<osgEarth::Annotation::LabelNode> myLabelNode;
};

#endif // SCENECONTROLOBJECT_H
