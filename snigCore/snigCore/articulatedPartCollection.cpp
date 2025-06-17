#include "articulatedPartCollection.h"
#include <QSet>
#include <QVector>

void ByyArticulatedPart::getParameterMetrics( QVector<ParameterMetric> &metrics ) const
{
	metrics=myValues.keys().toVector();
}

float ByyArticulatedPart::getParameterValue( ParameterMetric parameterMetric ) const
{
	Paramters::iterator it=myValues.find(parameterMetric);

	if (it!=myValues.end())
	{
		return it.value();
	}

	return 0;
}

void ByyArticulatedPart::setParameter( ParameterMetric parameterMetric, float value )
{
	myValues[parameterMetric]=value;
}

ByyArticulatedPartCollection::ByyArticulatedPartCollection()
{

}

ByyArticulatedPartCollection::~ByyArticulatedPartCollection()
{
	qDeleteAll(myPartMap);
}

void ByyArticulatedPartCollection::getPartTypes( PartTypeSet &aSet )
{
	aSet=myPartMap.keys().toSet();
}

ByyArticulatedPart * ByyArticulatedPartCollection::findPart( unsigned int partType )
{
	PartMap::iterator it=myPartMap.find(partType);

	if (it!=myPartMap.end())
	{
		return it.value();
	}

	return 0;
}

ByyArticulatedPart& ByyArticulatedPartCollection::getPart( unsigned int partType )
{
	PartMap::iterator it=myPartMap.find(partType);

	if (it!=myPartMap.end())
	{
		return *it.value();
	}

	ByyArticulatedPart* aPart=new ByyArticulatedPart;

	myPartMap.insert(partType,aPart);

	return *aPart;
}

int ByyArticulatedPartCollection::partCount() const
{
	return myPartMap.size();
}
