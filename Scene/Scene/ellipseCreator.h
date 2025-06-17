#ifndef ELLIPSECREATEOR_H
#define ELLIPSECREATEOR_H

#include "pointSetCreator.h"

class ByyEllipseCreator : public ByyFixedPointSetCreator
{
public:
	ByyEllipseCreator(ByyIG& ig);
	~ByyEllipseCreator();

	virtual ByyControlObject* accept();

	virtual unsigned int getFixedPointCount();

	static ByyTacticalGraphicsCreator* create(ByyIG& ig);

	virtual void update(const ByyVec3dArray& points);

protected:
	osg::ref_ptr<osgEarth::Annotation::EllipseNode> myEllipseNode;

};

#endif // ELLIPSECREATEOR_H
