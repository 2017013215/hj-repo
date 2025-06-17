#ifndef HALFSPHEREELECTRONICVISUALIZER_H
#define HALFSPHEREELECTRONICVISUALIZER_H

#include "sphereElectronicVisualizer.h"

class ByyHalfSphereElectronicVisualizer : public ByySphereElectronicVisualizer
{
public:
	ByyHalfSphereElectronicVisualizer(ByyEntityObject& entObj,ByyIG& ig);
	~ByyHalfSphereElectronicVisualizer();

	static const QString& theType();
	virtual const QString& type();

private:
	
};

typedef ByyStateVisualizerCreatorTemplate<ByyHalfSphereElectronicVisualizer> ByyHalfSphereElectronicVisualizerCreator;

#endif // HALFSPHEREELECTRONICVISUALIZER_H
