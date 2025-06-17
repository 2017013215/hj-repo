#include "stdafx.h"
#include "altitudeCullingCallback.h"

ByyAltitudeCullingCallback::ByyAltitudeCullingCallback( const osgEarth::SpatialReference *srs,double altitude ) 
	: osg::NodeCallback(),
	_altitude(altitude),
	_visible    ( true ),
	_srs(srs)
{

}

ByyAltitudeCullingCallback::~ByyAltitudeCullingCallback()
{

}

void ByyAltitudeCullingCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
	if (nv->getVisitorType() == osg::NodeVisitor::CULL_VISITOR)
	{
		osgUtil::CullVisitor* cv = osgEarth::Culling::asCullVisitor(nv);

		osg::Vec3d eye = cv->getViewPoint();

		if (_prevEye != eye)
		{
			double alt = 0.0;

			if ( _srs && !_srs->isProjected() )
			{
				osgEarth::GeoPoint mapPoint;
				mapPoint.fromWorld( _srs.get(), eye );
				alt = mapPoint.z();
			}
			else
			{
				alt = eye.z();
			}

			_visible=alt<_altitude;

			_prevEye = eye;
		}

		if (_visible)
		{
			traverse(node, nv );
		}
	}
	else
	{
		traverse( node, nv );
	}
}
