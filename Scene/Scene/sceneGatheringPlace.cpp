#include "StdAfx.h"
#include "sceneGatheringPlace.h"
#include "ByyGatheringPlace.h"
#include "plotUtils.h"

ByySceneGatheringPlace::ByySceneGatheringPlace( ByyGatheringPlace* gatheringPlace,ByyIG& ig )
	:ByyScenePolygon(gatheringPlace,ig)
/*	,myGatheringPlace(gatheringPlace)*/
{

}

ByySceneGatheringPlace::~ByySceneGatheringPlace()
{

}

ByySceneControlObject* ByySceneGatheringPlace::create( ByyControlObject* obj,ByyIG& ig )
{
	ByyGatheringPlace* wp=dynamic_cast<ByyGatheringPlace*>(obj);

	if (wp)
	{
		return new ByySceneGatheringPlace(wp,ig);
	}

	return 0;
}

ByyVec3dArray ByySceneGatheringPlace::getPoints()
{
	return ByyPlotUtils::generateGatheringPlace(myPolygon->points());
}
