#include "StdAfx.h"
#include "visualizerFactory.h"
#include "stateVisualizer.h"

#include "mainModelVisualizer.h"
#include "indicateVisualizer.h"
#include "ribbonVisualizer.h"
#include "sphereElectronicVisualizer.h"
#include "halfSphereElectronicVisualizer.h"
#include "sensorVisualizer.h"
#include "ByyRangeOfVisibilityVisualizer.h"
#include "satelliteOribtVisualizer.h"

#include "entityObject.h"
#include "ByyDescribeEntry.h"

ByyVisualizerFactory::ByyVisualizerFactory()
{
	QStringList entity;
	//entity.append("MainModel");
	entity.append("Indicate");
	//entity.append("MissileElectronic");

	myVisualizerDefs.root()->setData(entity);

	{
		QStringList airEnt(entity);
		airEnt.append("Ribbon");
		//airEnt.append("SphereElectronic");

		myVisualizerDefs.insert(ByyEntityType("1:2:-1:-1:-1:-1:-1"),airEnt);
	}

	{
		QStringList missileEnt(entity);
		missileEnt.append("Ribbon");
		//missileEnt.append("MissileElectronic");

		myVisualizerDefs.insert(ByyEntityType("2:-1:-1:-1:-1:-1:-1"),missileEnt);
	}
	{
		QStringList surface(entity);
		//surface.append("HalfSphereElectronic");

		myVisualizerDefs.insert(ByyEntityType("1:3:-1:-1:-1:-1:-1"),surface);
	}
	
	{
		QStringList space(entity);
		space.append("SatelliteOribt");

		myVisualizerDefs.insert(ByyEntityType("1:5:-1:-1:-1:-1:-1"),space);
	}

	addCreator(new ByyMainModelVisualizerCreator);
	addCreator(new ByyIndicateVisualizerCreator);
	addCreator(new ByyRibbonVisualizerCreator);
	addCreator(new ByySphereElectronicVisualizerCreator);
	addCreator(new ByyHalfSphereElectronicVisualizerCreator);
	addCreator(new ByySensorVisualizerCreator);
	addCreator(new ByySatelliteOribtVisualizerCreator);
}

ByyVisualizerFactory::~ByyVisualizerFactory()
{

}

ByyVisualizerFactory& ByyVisualizerFactory::instance()
{
	static ByyVisualizerFactory _instance;
	return _instance;
}

void ByyVisualizerFactory::addCreator( ByyStateVisualizerCreator *creator )
{
	myVisualizerCreators[creator->type()]=creator;
}

ByyVisualizerFactory::ByyStateVisualizerSet ByyVisualizerFactory::createVisualizer( const ByyEntityType& entType,ByyEntityObject& entObj,ByyIG& ig )
{
	QStringList visualizers=myVisualizerDefs.getData(entType);

	ByyStateVisualizerSet rel;

	for (int i=0;i!=visualizers.size();++i)
	{
		ByyVisualizerCreatorMap::iterator it=myVisualizerCreators.find(visualizers[i]);

		if (it!=myVisualizerCreators.end())
		{
			rel.append(it.value()->createVisualizer(entObj,ig));
		}
	}

	ByyDescribeEntry* descEntry=entObj.describeEntry();

	if (descEntry)
	{
		ByyDescribeEntry::ComponentList components=descEntry->getComponent("sensor");

		for (int i=0;i!=components.size();++i)
		{
			ByySensorVisualizer *sensorVisualizer=new ByySensorVisualizer(entObj,ig);

			sensorVisualizer->setComponent(components[i]);

			rel.append(sensorVisualizer);


			ByyRangeOfVisibilityVisualizer* rangeOfVisualizer=new ByyRangeOfVisibilityVisualizer(entObj,ig);

			rangeOfVisualizer->setComponent(components[i]);

			rel.append(rangeOfVisualizer);
		}
	}


	return rel;
}
