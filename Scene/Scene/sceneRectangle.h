#ifndef SCENERECTANGLE_H
#define SCENERECTANGLE_H

#include "scenePolygon.h"

class ByyRectangle;

class SCENE_EXPORT ByySceneRectangle : public ByyScenePolygon
{
public:
	ByySceneRectangle(ByyRectangle* re,ByyIG& ig);
	~ByySceneRectangle();

	static ByySceneControlObject* create(ByyControlObject* obj,ByyIG& ig);

private:
	ByyRectangle *myRectangle;
};

#endif // SCENERECTANGLE_H
