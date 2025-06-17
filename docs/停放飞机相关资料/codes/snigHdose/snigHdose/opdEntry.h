#ifndef OPDENTRY_H
#define OPDENTRY_H

#include "snighdose_global.h"

#include "ByyDescParamList.h"
#include "entityType.h"

class QDomElement;

class SNIGHDOSE_EXPORT ByyOpdEntry : public ByyDescParamList
{
public:
	ByyOpdEntry();
	~ByyOpdEntry();

	void fromConfig(QDomElement& element);

	const ByyEntityType& getEntityType();

protected:
	ByyEntityType myEntityType;
};

#endif // OPDENTRY_H
