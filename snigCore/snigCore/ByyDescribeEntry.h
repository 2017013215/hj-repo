#ifndef BYYDESCRIBEENTRY_H
#define BYYDESCRIBEENTRY_H

#include "ByyAbsDescribe.h"
#include "ByyDescribeComponent.h"
#include <QList>

class SNIGCORE_EXPORT ByyDescribeEntry : public ByyAbsDescribe
{
public:
	ByyDescribeEntry();
	virtual ~ByyDescribeEntry();

	void addComponent(const ByyDescribeComponent& component);

	typedef QList<ByyDescribeComponent> ComponentList;

	ComponentList getComponent(const QString& type);

	const ComponentList& getComponent();

protected:
	ComponentList myComponents;
};

#endif // BYYDESCRIBEENTRY_H
