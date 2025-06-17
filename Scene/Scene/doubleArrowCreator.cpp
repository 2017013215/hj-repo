#include "StdAfx.h"
#include "doubleArrowCreator.h"
#include "plotUtils.h"

ByyDoubleArrowCreator::ByyDoubleArrowCreator( ByyIG& ig )
	:ByyFixedPolygonTypeCreatorTemplate<ByyDoubleArrow>(ig)
{

}

ByyDoubleArrowCreator::~ByyDoubleArrowCreator()
{
	
}

ByyTacticalGraphicsCreator* ByyDoubleArrowCreator::create( ByyIG& ig )
{
	return new ByyDoubleArrowCreator(ig);
}

unsigned int ByyDoubleArrowCreator::getFixedPointCount()
{
	return 4;
}

ByyVec3dArray ByyDoubleArrowCreator::calcPlots( const ByyVec3dArray& srcPoints )
{
	return ByyPlotUtils::generateDoubleArrow(srcPoints);
}
