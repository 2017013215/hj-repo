#ifndef EARTHCOMPONENT_H
#define EARTHCOMPONENT_H

#include <QObject>
#include "scene_global.h"
#include <QVector>

namespace osgEarth
{
	class MapNode;
	class Map;
	class ImageLayer;
	class ElevationLayer;
	class SpatialReference;
	class Config;
	class GeoPoint;

	namespace Util
	{
		class GeodeticGraticule;
	}
}

namespace osgViewer
{
	class Viewer;
}

class ByyModel;
class ByyTerrainEffect;
class ByyEntityObject;

class SCENE_EXPORT ByyTerrain : public QObject
{
	Q_OBJECT
public:
	ByyTerrain();
	~ByyTerrain();

	bool load(const std::string& filename);
	bool unload();

	bool isLoad();

	void save();
	void saveAs(const std::string& filename);
	
	osgEarth::MapNode* mapNode();
	osgEarth::Map* getMap();
	const osgEarth::SpatialReference* srs();

	ByyTerrainEffect* effect();

	void setChanged();

	osg::Group* root(){return myRoot;}

	bool getWorldCoordsUnderMouse(osg::View* view, float x, float y, osg::Vec3d& out_coords ) const;
	bool crushNode(const osgEarth::GeoPoint& point, int& opdId, osg::Vec3d& orientation, osgEarth::GeoPoint& crushObjPt);
	bool getChildPlacer(const int& opdId, float& height, float& length, float& width);

public slots:
	osgEarth::ImageLayer* addImageLayer( const std::string& filePath );
	osgEarth::ElevationLayer* addElevatonLayer( const std::string& filePath );
	void setGraticuleVisible(bool v);

signals:
	void terrainLoaded(osgEarth::MapNode*);
	void  terrainCleared();

private:
	void parseMapNode(osgEarth::MapNode* mapNode);
	void addMask(osgEarth::MapNode *mapNode,const osgEarth::Config& conf);

private:
	osg::ref_ptr<osgEarth::MapNode> d_mapNode;
	osg::ref_ptr<osgEarth::Map> d_map;

	std::string d_filename;

	osg::ref_ptr<osg::Group> myRoot;

	osg::ref_ptr<osgEarth::Util::GeodeticGraticule> myGraticule;

	ByyTerrainEffect* myEffect;
};

#endif // EARTHCOMPONENT_H
