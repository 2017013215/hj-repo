#include "StdAfx.h"
#include "attackArrowCreator.h"
#include "plotUtils.h"

ByyAttackArrowCreator::ByyAttackArrowCreator( ByyIG& ig )
	:ByyPolygonTypeCreatorTemplate<ByyAttackArrow>(ig)
{
	
}

ByyAttackArrowCreator::~ByyAttackArrowCreator()
{
	
}

ByyTacticalGraphicsCreator* ByyAttackArrowCreator::create( ByyIG& ig )
{
	return new ByyAttackArrowCreator(ig);
}

unsigned int ByyAttackArrowCreator::getMinPointCount()
{
	return 3;
}

ByyVec3dArray ByyAttackArrowCreator::calcPlots( const ByyVec3dArray& srcPoints )
{
	return ByyPlotUtils::generateAttackArrow(srcPoints);
}
