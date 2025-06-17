#include "StdAfx.h"
#include "sceneRectangle.h"
#include "ByyRectangle.h"

ByySceneRectangle::ByySceneRectangle( ByyRectangle* re,ByyIG& ig ) 
	: ByyScenePolygon(re,ig)
	,myRectangle(re)
{
	
}

ByySceneRectangle::~ByySceneRectangle()
{

}

ByySceneControlObject* ByySceneRectangle::create( ByyControlObject* obj,ByyIG& ig )
{
	ByyRectangle* wp=dynamic_cast<ByyRectangle*>(obj);

	if (wp)
	{
		return new ByySceneRectangle(wp,ig);
	}

	return 0;
}
