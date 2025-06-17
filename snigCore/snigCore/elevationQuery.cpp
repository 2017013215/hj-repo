#include "elevationQuery.h"

#include <osgEarth/GeoData>
#include <osgEarthFeatures/GeometryUtils>

ByyElevationQuery::ByyElevationQuery()
	:myElevationQuery(NULL)
{

}

ByyElevationQuery::~ByyElevationQuery()
{
	if (myElevationQuery)
	{
		delete myElevationQuery;
		myElevationQuery=0;
	}
}

ByyElevationQuery* ByyElevationQuery::instance()
{
	static ByyElevationQuery _instace;
	return &_instace;
}

void ByyElevationQuery::init( osgEarth::MapNode *mapNode )
{
	release();

	d_mapNode=mapNode;

	emit mapNodeChanged(mapNode);

	d_map=mapNode->getMap();

	myElevationQuery=new osgEarth::ElevationQuery(d_map.get());

	const osgEarth::Config& masksConf = mapNode->externalConfig().child("masks");

	if (!masksConf.empty())
	{
		const osgEarth::ConfigSet& children=masksConf.children("mask");

		for(osgEarth::ConfigSet::const_iterator i = children.begin(); i != children.end(); ++i )
		{
			if (!i->empty()&&i->hasChild("forcealtitude"))
			{
				osgEarth::Symbology::Geometry* geom=osgEarth::Features::GeometryUtils::geometryFromWKT(i->value("geometry"));

				osgEarth::Symbology::Polygon *polygon=geom==0?0:dynamic_cast<osgEarth::Symbology::Polygon*>(geom);

				if (polygon)
				{
					d_forceAltitudeBoundList.push_back(polygon);
				}

				double d_alt=0;
				i->getIfSet("forcealtitude",d_alt);
				d_forceAltitudes.push_back(d_alt);
			}
		}
	}

	const osgEarth::Config& altAreasConf=mapNode->externalConfig().child("altareas");

	if (!altAreasConf.empty())
	{
		const osgEarth::ConfigSet& children=altAreasConf.children("altarea");
	}
}

void ByyElevationQuery::release()
{
	delete myElevationQuery;
	myElevationQuery=NULL;
	d_map=NULL;
	d_mapNode=NULL;
	d_forceAltitudeBoundList.clear();
	d_forceAltitudes.clear();
}

bool ByyElevationQuery::getElevation( const osgEarth::GeoPoint& point,double& out_elevation,double desiredResolution/*=0.0*/, double *out_actualResolution/*=0L*/ )
{
	if (!d_map.valid()||!myElevationQuery)
	{
		return false;
	}

	return myElevationQuery->getElevation( point,out_elevation,desiredResolution,out_actualResolution);
	
}

bool ByyElevationQuery::getForceAltitude( double x, double y,double& altitude )
{
	for (int i=0;i!=d_forceAltitudeBoundList.size();++i)
	{
		if (d_forceAltitudeBoundList[i]->contains2D(x,y))
		{
			altitude=d_forceAltitudes[i];
			
			return true;
		}
	}
	return false;
}

osgEarth::MapNode* ByyElevationQuery::getMapNode()
{
	return d_mapNode.get();
}
