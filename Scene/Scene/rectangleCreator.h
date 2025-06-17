#ifndef RECTANGLECREATOR_H
#define RECTANGLECREATOR_H

#include "pointSetCreator.h"

class ByyRectangleCreator : public ByyFixedPointSetCreator
{
public:
	ByyRectangleCreator(ByyIG& ig);
	~ByyRectangleCreator();

	virtual ByyControlObject* accept();

	static ByyTacticalGraphicsCreator* create(ByyIG& ig);

	virtual unsigned int getFixedPointCount();

	virtual void update(const ByyVec3dArray& points);
	
private:
	osg::ref_ptr<osgEarth::Annotation::RectangleNode> myRectNode;

};

#endif // RECTANGLECREATOR_H
