#ifndef SCENEGATHERINGPLACE_H
#define SCENEGATHERINGPLACE_H

#include "scenePolygon.h"

class ByyGatheringPlace;

class ByySceneGatheringPlace : public ByyScenePolygon
{
public:
	ByySceneGatheringPlace(ByyGatheringPlace* gatheringPlace,ByyIG& ig);
	~ByySceneGatheringPlace();

	static ByySceneControlObject* create(ByyControlObject* obj,ByyIG& ig);

protected:
	ByyVec3dArray getPoints();

protected:
	//ByyGatheringPlace* myGatheringPlace;
	
};

#endif // SCENEGATHERINGPLACE_H
