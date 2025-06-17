#ifndef BYYDOUBLEARROW_H
#define BYYDOUBLEARROW_H

#include "ByyPolygon.h"

class SNIGCORE_EXPORT ByyDoubleArrow : public ByyPolygon
{
public:
	ByyDoubleArrow();
	~ByyDoubleArrow();

	virtual const QString& type()const;
	static const QString& theType();

	virtual QString getAttributeValueSet(){return QString();}
	
};

#endif // BYYDOUBLEARROW_H
