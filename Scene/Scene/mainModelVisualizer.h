#ifndef MAINMODELVISUALIZER_H
#define MAINMODELVISUALIZER_H

#include "stateVisualizer.h"

class ByyMainModelVisualizer : public ByyStateVisualizer
{
	Q_OBJECT
public:
	ByyMainModelVisualizer(ByyEntityObject& entObj,ByyIG& ig);
	~ByyMainModelVisualizer();

	static const QString& theType();
	const QString& type();
	
};

typedef ByyStateVisualizerCreatorTemplate<ByyMainModelVisualizer> ByyMainModelVisualizerCreator;

#endif // MAINMODELVISUALIZER_H
