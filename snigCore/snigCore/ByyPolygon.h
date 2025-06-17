#ifndef POLYGON_H
#define POLYGON_H

#include "ByyPointSet.h"

class SNIGCORE_EXPORT ByyPolygon : public ByyPointSet
{
public:
	ByyPolygon();
	virtual ~ByyPolygon();

	virtual const QString& type()const;
	static const QString& theType();

	virtual ByyKeyValueList getConfig();
	virtual void fromConfig(ByyKeyValueList& keyValues);

	virtual QString getAttributeValueSet();

};

#endif // POLYGON_H
