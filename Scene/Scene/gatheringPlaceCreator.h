#ifndef GATHERINGPLACECREATOR_H
#define GATHERINGPLACECREATOR_H

#include "polygonTypeCreator.h"
#include "ByyGatheringPlace.h"

class ByyGatheringPlaceCreator : public ByyFixedPolygonTypeCreatorTemplate<ByyGatheringPlace>
{
public:
	ByyGatheringPlaceCreator(ByyIG& ig);
	~ByyGatheringPlaceCreator();

	static ByyTacticalGraphicsCreator* create(ByyIG& ig);

	virtual unsigned int getFixedPointCount();

	virtual ByyVec3dArray calcPlots(const ByyVec3dArray& srcPoints);
	
};

#endif // GATHERINGPLACECREATOR_H
