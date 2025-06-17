#ifndef DOUBLEARROWCREATOR_H
#define DOUBLEARROWCREATOR_H

#include "polygonTypeCreator.h"
#include "ByyDoubleArrow.h"

class ByyDoubleArrowCreator : public ByyFixedPolygonTypeCreatorTemplate<ByyDoubleArrow>
{
public:
	ByyDoubleArrowCreator(ByyIG& ig);
	~ByyDoubleArrowCreator();

	static ByyTacticalGraphicsCreator* create(ByyIG& ig);

	virtual unsigned int getFixedPointCount();

	virtual ByyVec3dArray calcPlots(const ByyVec3dArray& srcPoints);
	
};

#endif // DOUBLEARROWCREATOR_H
