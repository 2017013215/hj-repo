#include "StdAfx.h"
#include "sceneFineArrow.h"
#include "ByyFineArrow.h"
#include "plotUtils.h"

ByySceneFineArrow::ByySceneFineArrow( ByyFineArrow* fineArrow,ByyIG& ig )
	:ByyScenePolygon(fineArrow,ig)
	/*,myFineArrow(fineArrow)*/
{
	
}

ByySceneFineArrow::~ByySceneFineArrow()
{

}

ByySceneControlObject* ByySceneFineArrow::create( ByyControlObject* obj,ByyIG& ig )
{
	ByyFineArrow* wp=dynamic_cast<ByyFineArrow*>(obj);

	if (wp)
	{
		return new ByySceneFineArrow(wp,ig);
	}

	return 0;
}

ByyVec3dArray ByySceneFineArrow::getPoints()
{
	return ByyPlotUtils::generateFineArrow(myPolygon->points());
}
