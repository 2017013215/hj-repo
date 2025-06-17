#ifndef BYYDESCRIBECOMPONENT_H
#define BYYDESCRIBECOMPONENT_H

#include "ByyAbsDescribe.h"
#include "ByyDescParamList.h"

class SNIGCORE_EXPORT ByyDescribeComponent : public ByyAbsDescribe
{
public:
	ByyDescribeComponent();
	virtual ~ByyDescribeComponent();

	void setParamList(const ByyDescParamList& paramList);

	const ByyDescParamList& getParamList()const;

protected:
	ByyDescParamList myParamList;
};

#endif // BYYDESCRIBECOMPONENT_H
