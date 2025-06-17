#ifndef POLYGONCREATOR_H
#define POLYGONCREATOR_H

#include "polygonTypeCreator.h"
#include "ByyPolygon.h"

class ByyPolygonCreator : public ByyPolygonTypeCreatorTemplate<ByyPolygon>
{
public:
	ByyPolygonCreator(ByyIG& ig);
	~ByyPolygonCreator();

	static ByyTacticalGraphicsCreator* create(ByyIG& ig);
	virtual unsigned int getMinPointCount();
	
};

#endif // POLYGONCREATOR_H
