#ifndef BYYFINEARROW_H
#define BYYFINEARROW_H

#include "ByyPolygon.h"

class SNIGCORE_EXPORT ByyFineArrow : public ByyPolygon
{
public:
	ByyFineArrow();
	virtual ~ByyFineArrow();

	virtual const QString& type()const;
	static const QString& theType();

	virtual QString getAttributeValueSet(){return QString();}

};

#endif // BYYFINEARROW_H
