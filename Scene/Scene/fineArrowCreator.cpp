#include "StdAfx.h"
#include "fineArrowCreator.h"
#include "plotUtils.h"

ByyFineArrowCreator::ByyFineArrowCreator( ByyIG& ig )
	:ByyFixedPolygonTypeCreatorTemplate<ByyFineArrow>(ig)
{

}

ByyFineArrowCreator::~ByyFineArrowCreator()
{
	
}


ByyTacticalGraphicsCreator* ByyFineArrowCreator::create( ByyIG& ig )
{
	return new ByyFineArrowCreator(ig);
}

unsigned int ByyFineArrowCreator::getFixedPointCount()
{
	return 2;
}

ByyVec3dArray ByyFineArrowCreator::calcPlots( const ByyVec3dArray& srcPoints )
{
	return ByyPlotUtils::generateFineArrow(srcPoints);
}
