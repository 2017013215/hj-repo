#include "coordinateTransfer.h"

ByyCoordinateTransfer::ByyCoordinateTransfer()
{
	csn = new osg::CoordinateSystemNode;
	csn->setEllipsoidModel(new osg::EllipsoidModel());
}

ByyCoordinateTransfer::~ByyCoordinateTransfer()
{

}

ByyCoordinateTransfer* ByyCoordinateTransfer::instance()
{
	static ByyCoordinateTransfer _instance;
	return &_instance;
}

osg::Matrixd ByyCoordinateTransfer::computeLocalToWorld( double latitude, double longitude, double height )
{
	osg::Matrixd matrix;
	csn->getEllipsoidModel()->computeLocalToWorldTransformFromLatLongHeight(latitude,longitude,height, matrix);
	return matrix;
}
