#ifndef BYYSTRAIGHTARROW_H
#define BYYSTRAIGHTARROW_H

#include "ByyPointSet.h"

class SNIGCORE_EXPORT ByyStraightArrow: public ByyPointSet
{
public:
	ByyStraightArrow();
	~ByyStraightArrow();

	virtual const QString& type()const;
	static const QString& theType();

	virtual QString getAttributeValueSet();
	virtual ByyKeyValueList getConfig();
	virtual void fromConfig(ByyKeyValueList& keyValues);

private:
	
};

#endif // BYYSTRAIGHTARROW_H
