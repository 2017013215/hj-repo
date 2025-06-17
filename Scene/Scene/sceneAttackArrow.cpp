#include "StdAfx.h"
#include "sceneAttackArrow.h"
#include "ByyAttackArrow.h"
#include "plotUtils.h"

ByySceneAttackArrow::ByySceneAttackArrow(ByyAttackArrow* attackArrow,ByyIG& ig)
	:ByyScenePolygon(attackArrow,ig)
	/*,myAttackArrow(attackArrow)*/
{

}

ByySceneAttackArrow::~ByySceneAttackArrow()
{

}

ByySceneControlObject* ByySceneAttackArrow::create( ByyControlObject* obj,ByyIG& ig )
{
	ByyAttackArrow* wp=dynamic_cast<ByyAttackArrow*>(obj);

	if (wp)
	{
		return new ByySceneAttackArrow(wp,ig);
	}

	return 0;
}

ByyVec3dArray ByySceneAttackArrow::getPoints()
{
	return ByyPlotUtils::generateAttackArrow(myPolygon->points());
}
