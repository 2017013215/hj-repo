#include "StdAfx.h"
#include "gatheringPlaceCreator.h"
#include "plotUtils.h"

ByyGatheringPlaceCreator::ByyGatheringPlaceCreator( ByyIG& ig )
	:ByyFixedPolygonTypeCreatorTemplate<ByyGatheringPlace>(ig)
{
}

ByyGatheringPlaceCreator::~ByyGatheringPlaceCreator()
{
	
}

ByyTacticalGraphicsCreator* ByyGatheringPlaceCreator::create( ByyIG& ig )
{
	return new ByyGatheringPlaceCreator(ig);
}

unsigned int ByyGatheringPlaceCreator::getFixedPointCount()
{
	return 3;
}

ByyVec3dArray ByyGatheringPlaceCreator::calcPlots( const ByyVec3dArray& srcPoints )
{
	return ByyPlotUtils::generateGatheringPlace(srcPoints);
}
