#ifndef ELEVATIONQUERY_H
#define ELEVATIONQUERY_H

#include <QObject>
#include "snigcore_global.h"

#include <osgEarth/MapNode>
#include <osgEarthSymbology/Geometry>
#include <osgEarth/ElevationQuery>

class SNIGCORE_EXPORT ByyElevationQuery : public QObject
{
	Q_OBJECT
public:
	ByyElevationQuery();
	virtual ~ByyElevationQuery();

	static ByyElevationQuery* instance();

	void init(osgEarth::MapNode *mapNode);
	void release();

	bool getElevation(const osgEarth::GeoPoint& point,double& out_elevation,double desiredResolution=0.0, double *out_actualResolution=0L);

	bool getForceAltitude(double x, double y,double& altitude);

	osgEarth::MapNode* getMapNode();

signals:
	void mapNodeChanged(osgEarth::MapNode*);

private:
	osg::observer_ptr<osgEarth::MapNode> d_mapNode;
	osg::observer_ptr<osgEarth::Map> d_map;
	std::vector<osg::ref_ptr<osgEarth::Symbology::Polygon>> d_forceAltitudeBoundList;
	std::vector<double> d_forceAltitudes;

	osgEarth::ElevationQuery* myElevationQuery;
};

#endif // ELEVATIONQUERY_H
