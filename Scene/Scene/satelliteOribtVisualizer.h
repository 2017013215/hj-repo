#ifndef SATELLITEORIBTVISUALIZER_H
#define SATELLITEORIBTVISUALIZER_H

#include "stateVisualizer.h"

class ByySatelliteOribtVisualizer : public ByyStateVisualizer
{
	Q_OBJECT
public:
	ByySatelliteOribtVisualizer(ByyEntityObject& entObj,ByyIG& ig);
	~ByySatelliteOribtVisualizer();

	static const QString& theType();
	virtual const QString& type();

public slots:
	void updateForceType();

private:
	osg::ref_ptr< osg::Geometry > pGeometry0;
	osg::ref_ptr<osg::Vec4Array>	colors0;
};

typedef ByyStateVisualizerCreatorTemplate<ByySatelliteOribtVisualizer> ByySatelliteOribtVisualizerCreator;

#endif // SATELLITEORIBTVISUALIZER_H
