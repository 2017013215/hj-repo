#ifndef SCENEFINEARROW_H
#define SCENEFINEARROW_H

#include "scenePolygon.h"

class ByyFineArrow;

class ByySceneFineArrow : public ByyScenePolygon
{
	Q_OBJECT
public:
	ByySceneFineArrow(ByyFineArrow* fineArrow,ByyIG& ig);
	virtual ~ByySceneFineArrow();

	static ByySceneControlObject* create(ByyControlObject* obj,ByyIG& ig);
	virtual	bool	isLineString(){return true;}
protected:
	ByyVec3dArray getPoints();

protected:
	/*ByyFineArrow* myFineArrow;*/

};

#endif // SCENEFINEARROW_H
