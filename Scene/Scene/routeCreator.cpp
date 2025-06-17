#include "StdAfx.h"
#include "routeCreator.h"

ByyRouteCreator::ByyRouteCreator( ByyIG& ig ) 
	: ByyLineStringTypeCreatorTemplate<ByyRoute>(ig)
{
	
}

ByyRouteCreator::~ByyRouteCreator()
{
	
}

ByyTacticalGraphicsCreator* ByyRouteCreator::create( ByyIG& ig )
{
	return new ByyRouteCreator(ig);
}

unsigned int ByyRouteCreator::getMinPointCount()
{
	return 2;
}
