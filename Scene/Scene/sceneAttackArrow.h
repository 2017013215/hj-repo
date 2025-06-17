#ifndef SCENEATTACKARROW_H
#define SCENEATTACKARROW_H

#include "scenePolygon.h"

class ByyAttackArrow;

class ByySceneAttackArrow : public ByyScenePolygon
{
public:
	ByySceneAttackArrow(ByyAttackArrow* attackArrow,ByyIG& ig);
	~ByySceneAttackArrow();

	static ByySceneControlObject* create(ByyControlObject* obj,ByyIG& ig);
	virtual	bool	isLineString(){return true;}
protected:
	ByyVec3dArray getPoints();

protected:
	//ByyAttackArrow* myAttackArrow;
	
};

#endif // SCENEATTACKARROW_H
