#ifndef PHASELINECREATOR_H
#define PHASELINECREATOR_H

#include "lineStringTypeCreator.h"
#include "ByyPhaseLine.h"

class ByyPhaseLineCreator : public ByyFixedLineStringTypeCreatorTemplate<ByyPhaseLine>
{
public:
	ByyPhaseLineCreator(ByyIG& ig);
	~ByyPhaseLineCreator();

	virtual unsigned int getFixedPointCount();

	static ByyTacticalGraphicsCreator* create(ByyIG& ig);
};

#endif // PHASELINECREATOR_H
