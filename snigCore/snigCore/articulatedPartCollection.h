#ifndef ARTICULATEDPARTCOLLECTION_H
#define ARTICULATEDPARTCOLLECTION_H

#include "snigcore_global.h"
#include <QMap>

class SNIGCORE_EXPORT ByyArticulatedPart
{
public:
	typedef int ParameterMetric;

	void getParameterMetrics(QVector<ParameterMetric> &metrics)const;
	float getParameterValue (ParameterMetric parameterMetric)const;
	void setParameter (ParameterMetric parameterMetric,float value);

protected:
	typedef QMap<ParameterMetric,float> Paramters;

	Paramters myValues;
};

class SNIGCORE_EXPORT ByyArticulatedPartCollection
{
public:
	ByyArticulatedPartCollection();
	~ByyArticulatedPartCollection();

	typedef QSet<unsigned int>  PartTypeSet;

	void getPartTypes(PartTypeSet &aSet);

	ByyArticulatedPart& getPart(unsigned int partType);

	ByyArticulatedPart* findPart(unsigned int partType);

	int partCount()const;

protected:
	typedef QMap<unsigned int,ByyArticulatedPart*> PartMap;
	PartMap myPartMap;
};

#endif // ARTICULATEDPARTCOLLECTION_H
