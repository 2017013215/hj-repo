#ifndef VISUALIZERFACTORY_H
#define VISUALIZERFACTORY_H

#include "ByyTrieTree.h"

typedef QStringList ByyVisualizerDefinitions;

class ByyStateVisualizer;
class ByyStateVisualizerCreator;
class ByyEntityType;
class ByyEntityObject;
class ByyIG;

class ByyVisualizerFactory
{
public:
	~ByyVisualizerFactory();

	static ByyVisualizerFactory& instance();

	typedef QList<ByyStateVisualizer*> ByyStateVisualizerSet;

	ByyStateVisualizerSet createVisualizer(const ByyEntityType& entType,ByyEntityObject& entObj,ByyIG& ig);

	void addCreator(ByyStateVisualizerCreator *creator);

protected:
	ByyVisualizerFactory();

protected:
	ByyTrieTree<QStringList> myVisualizerDefs; 

	typedef QMap<QString,ByyStateVisualizerCreator*> ByyVisualizerCreatorMap;

	ByyVisualizerCreatorMap myVisualizerCreators;

};

#endif // VISUALIZERFACTORY_H
