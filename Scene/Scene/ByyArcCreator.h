#pragma once
#include "circlecreator.h"
class ByyArcCreator : public ByyCircleCreator
{
public:
	ByyArcCreator(ByyIG& ig);
	~ByyArcCreator(void);

	virtual ByyControlObject* accept();

	virtual unsigned int getFixedPointCount();

	static ByyTacticalGraphicsCreator* create(ByyIG& ig);

	virtual void update(const ByyVec3dArray& points);

};

