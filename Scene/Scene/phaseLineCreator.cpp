#include "StdAfx.h"
#include "phaseLineCreator.h"

ByyPhaseLineCreator::ByyPhaseLineCreator( ByyIG& ig)
	:ByyFixedLineStringTypeCreatorTemplate<ByyPhaseLine>(ig)
{
	
}

ByyPhaseLineCreator::~ByyPhaseLineCreator()
{
	
}

ByyTacticalGraphicsCreator* ByyPhaseLineCreator::create( ByyIG& ig)
{
	return new ByyPhaseLineCreator(ig);
}

unsigned int ByyPhaseLineCreator::getFixedPointCount()
{
	return 2;
}
