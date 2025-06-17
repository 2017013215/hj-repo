#ifndef ROUTECREATOR_H
#define ROUTECREATOR_H

#include "lineStringTypeCreator.h"
#include "ByyRoute.h"

class ByyRouteCreator : public ByyLineStringTypeCreatorTemplate<ByyRoute>
{
public:
	ByyRouteCreator(ByyIG& ig);
	~ByyRouteCreator();

	static ByyTacticalGraphicsCreator* create(ByyIG& ig);

	virtual unsigned int getMinPointCount();
};

#endif // ROUTECREATOR_H
