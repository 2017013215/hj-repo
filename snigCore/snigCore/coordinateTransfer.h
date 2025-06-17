#ifndef COORDINATETRANSFER_H
#define COORDINATETRANSFER_H

#include "snigcore_global.h"

#include <osg/CoordinateSystemNode>

class SNIGCORE_EXPORT ByyCoordinateTransfer
{
public:
	virtual ~ByyCoordinateTransfer();

	static ByyCoordinateTransfer* instance();

	osg::Matrixd computeLocalToWorld(double latitude, double longitude, double height);

private:
	ByyCoordinateTransfer();

private:
	osg::ref_ptr<osg::CoordinateSystemNode> csn;
};

#endif // COORDINATETRANSFER_H
