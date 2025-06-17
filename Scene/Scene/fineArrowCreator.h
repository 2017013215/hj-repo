#ifndef FINEARROWCREATOR_H
#define FINEARROWCREATOR_H

#include "polygonTypeCreator.h"
#include "ByyFineArrow.h"

class ByyFineArrowCreator : public ByyFixedPolygonTypeCreatorTemplate<ByyFineArrow>
{
public:
	ByyFineArrowCreator(ByyIG& ig);
	~ByyFineArrowCreator();

	static ByyTacticalGraphicsCreator* create(ByyIG& ig);

	virtual unsigned int getFixedPointCount();

	virtual ByyVec3dArray calcPlots(const ByyVec3dArray& srcPoints);

};

#endif // FINEARROWCREATOR_H
