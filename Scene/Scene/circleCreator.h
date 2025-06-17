#ifndef CIRCLECREATOR_H
#define CIRCLECREATOR_H

#include "pointSetCreator.h"

class ByyCircleCreator : public ByyFixedPointSetCreator
{
public:
	ByyCircleCreator(ByyIG& ig);
	~ByyCircleCreator();

	virtual ByyControlObject* accept();

	virtual unsigned int getFixedPointCount();

	static ByyTacticalGraphicsCreator* create(ByyIG& ig);

	virtual void update(const ByyVec3dArray& points);

protected:
	osg::ref_ptr<osgEarth::Annotation::CircleNode> myCircleNode;

};

#endif // CIRCLECREATOR_H
