#ifndef ROUTE_H
#define ROUTE_H

#include "ByyPointSet.h"

class SNIGCORE_EXPORT ByyRoute : public ByyPointSet
{
public:
	ByyRoute();
	virtual ~ByyRoute();

	virtual const QString& type()const;
	static const QString& theType();

	virtual ByyKeyValueList getConfig();
	virtual void fromConfig(ByyKeyValueList& keyValues);

	virtual QString getAttributeValueSet();
};

#endif // ROUTE_H
