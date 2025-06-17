#ifndef STRAIGHTARROWCREATOR_H
#define STRAIGHTARROWCREATOR_H

#include "lineStringTypeCreator.h"
#include "ByyStraightArrow.h"

class ByyStraightArrowCreator : public ByyFixedLineStringTypeCreatorTemplate<ByyStraightArrow>
{
public:
	ByyStraightArrowCreator(ByyIG& ig);
	~ByyStraightArrowCreator();

	virtual unsigned int getFixedPointCount();

	static ByyTacticalGraphicsCreator* create(ByyIG& ig);

	virtual ByyVec3dArray calcPlots(const ByyVec3dArray& srcPoints);
};

#endif // STRAIGHTARROWCREATOR_H
