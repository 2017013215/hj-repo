#ifndef BYYATTACKARROW_H
#define BYYATTACKARROW_H

#include "ByyPolygon.h"

class SNIGCORE_EXPORT ByyAttackArrow : public ByyPolygon
{
public:
	ByyAttackArrow();
	~ByyAttackArrow();

	virtual const QString& type()const;
	static const QString& theType();

	virtual QString getAttributeValueSet(){return QString();}
	
};

#endif // BYYATTACKARROW_H
