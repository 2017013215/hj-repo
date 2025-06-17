#ifndef SCENEDOUBLEARROW_H
#define SCENEDOUBLEARROW_H

#include "scenePolygon.h"

class ByyDoubleArrow;

class ByySceneDoubleArrow : public ByyScenePolygon
{
public:
	ByySceneDoubleArrow(ByyDoubleArrow* doubleArrow,ByyIG& ig);
	~ByySceneDoubleArrow();

	static ByySceneControlObject* create(ByyControlObject* obj,ByyIG& ig);
	virtual	bool	isLineString(){return true;}
protected:
	ByyVec3dArray getPoints();

protected:
	//ByyDoubleArrow* myDoubleArrow;
	
};

#endif // SCENEDOUBLEARROW_H
