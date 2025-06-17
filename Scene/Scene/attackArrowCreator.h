#ifndef ATTACKARROWCREATOR_H
#define ATTACKARROWCREATOR_H

#include "polygonTypeCreator.h"
#include "ByyAttackArrow.h"

class ByyAttackArrowCreator : public ByyPolygonTypeCreatorTemplate<ByyAttackArrow>
{
public:
	ByyAttackArrowCreator(ByyIG& ig);
	~ByyAttackArrowCreator();

	static ByyTacticalGraphicsCreator* create(ByyIG& ig);
	virtual unsigned int getMinPointCount();
	virtual ByyVec3dArray calcPlots(const ByyVec3dArray& srcPoints);
};

#endif // ATTACKARROWCREATOR_H
