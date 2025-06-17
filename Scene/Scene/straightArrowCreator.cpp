#include "StdAfx.h"
#include "straightArrowCreator.h"
#include "ByyStraightArrow.h"
#include "plotUtils.h"

ByyStraightArrowCreator::ByyStraightArrowCreator(ByyIG& ig)
	:ByyFixedLineStringTypeCreatorTemplate<ByyStraightArrow>(ig)
{
	
}

ByyStraightArrowCreator::~ByyStraightArrowCreator()
{
	
}

unsigned int ByyStraightArrowCreator::getFixedPointCount()
{
	return 2;
}

ByyTacticalGraphicsCreator* ByyStraightArrowCreator::create( ByyIG& ig )
{
	return new ByyStraightArrowCreator(ig);
}

ByyVec3dArray ByyStraightArrowCreator::calcPlots( const ByyVec3dArray& srcPoints )
{
	return ByyPlotUtils::generateStraightArrow(srcPoints);
}
