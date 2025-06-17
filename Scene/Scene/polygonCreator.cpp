#include "StdAfx.h"
#include "polygonCreator.h"
#include "utils.h"
#include "ByyPolygon.h"
#include "ByyIG.h"
#include "terrain.h"

ByyPolygonCreator::ByyPolygonCreator( ByyIG& ig )
	: ByyPolygonTypeCreatorTemplate<ByyPolygon>(ig)
{
	
}

ByyPolygonCreator::~ByyPolygonCreator()
{
	
}

ByyTacticalGraphicsCreator* ByyPolygonCreator::create( ByyIG& ig )
{
	return new ByyPolygonCreator(ig);
}

unsigned int ByyPolygonCreator::getMinPointCount()
{
	return 3;
}