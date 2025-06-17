#ifndef MISSILEELECTRONICVISUALIZER_H
#define MISSILEELECTRONICVISUALIZER_H

#include "stateVisualizer.h"
#include "scene_global.h"
#include "ByySensorSR.h"

class ByyDescribeComponent;
class ByyEllipsoidArcModel;

namespace osg
{
	class MatrixTransform;
}

class SCENE_EXPORT ByySensorVisualizer : public ByyStateVisualizer
{
	Q_OBJECT
public:
	ByySensorVisualizer(ByyEntityObject& entObj,ByyIG& ig);
	~ByySensorVisualizer();

	void setComponent(const ByyDescribeComponent& component);

	static const QString& theType();
	virtual const QString& type();

	virtual bool isLocalized()const;

	const QString& getSensorTag();

protected:
	void init();

protected slots:
	void update();
	void hasPowerSwitch(ByyRemoteEntity* ent,ByySensorSR* sensor,QString device,bool power);

protected:
	osg::ref_ptr<ByyEllipsoidArcModel> mySensorRange;
	osg::ref_ptr<osg::MatrixTransform> myPlane;

	double	myRadius;
	float	myAzMin;
	float	myAzMax;
	float	myElevMin;
	float	myElevMax;
	float	myLocatedH;

	QString myTag;
	
	ByySensorSR::DeviceState* myRunningState;
};

typedef ByyStateVisualizerCreatorTemplate<ByySensorVisualizer> ByySensorVisualizerCreator;

#endif // MISSILEELECTRONICVISUALIZER_H
