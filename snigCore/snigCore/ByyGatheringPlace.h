#ifndef BYYGATHERINGPLACE_H
#define BYYGATHERINGPLACE_H

#include "ByyPolygon.h"

class SNIGCORE_EXPORT ByyGatheringPlace : public ByyPolygon
{
public:
	ByyGatheringPlace();
	~ByyGatheringPlace();

	virtual const QString& type()const;
	static const QString& theType();

	virtual QString getAttributeValueSet(){return QString();}
	
};

#endif // BYYGATHERINGPLACE_H
