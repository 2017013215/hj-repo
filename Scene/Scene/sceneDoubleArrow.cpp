#include "StdAfx.h"
#include "sceneDoubleArrow.h"
#include "ByyDoubleArrow.h"
#include "plotUtils.h"

ByySceneDoubleArrow::ByySceneDoubleArrow(ByyDoubleArrow* doubleArrow,ByyIG& ig)
	:ByyScenePolygon(doubleArrow,ig)
/*	,myDoubleArrow(doubleArrow)*/
{

}

ByySceneDoubleArrow::~ByySceneDoubleArrow()
{

}

ByySceneControlObject* ByySceneDoubleArrow::create( ByyControlObject* obj,ByyIG& ig )
{
	ByyDoubleArrow* wp=dynamic_cast<ByyDoubleArrow*>(obj);

	if (wp)
	{
		return new ByySceneDoubleArrow(wp,ig);
	}

	return 0;
}

ByyVec3dArray ByySceneDoubleArrow::getPoints()
{
	return ByyPlotUtils::generateDoubleArrow(myPolygon->points());
}
