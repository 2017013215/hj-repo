#ifndef BYYRANGEOFVISIBILITYVISUALIZER_H
#define BYYRANGEOFVISIBILITYVISUALIZER_H

#include "stateVisualizer.h"
#include "scene_global.h"

namespace osgEarth
{
	namespace Annotation
	{
		class CircleNode;
	};
}

class ByyDescribeComponent;

class SCENE_EXPORT ByyRangeOfVisibilityVisualizer : public ByyStateVisualizer
{
	Q_OBJECT
public:
	ByyRangeOfVisibilityVisualizer(ByyEntityObject& entObj,ByyIG& ig);
	~ByyRangeOfVisibilityVisualizer();

	static const QString& theType();
	virtual const QString& type();

	void setComponent(const ByyDescribeComponent& component);

	const QString& getSensorTag();

public slots:
	void update(const osg::Vec3d& position);

protected:
	osg::ref_ptr<osgEarth::Annotation::CircleNode> myCircleNode;

	double	myRadius;
	float	myAzMin;
	float	myAzMax;
	float	myElevMin;
	float	myElevMax;
	float	myLocatedH;

	QString myTag;
};

#endif // BYYRANGEOFVISIBILITYVISUALIZER_H
