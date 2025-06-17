#include "stdafx.h"
#include "terrain.h"
#include "earthManipulator.h"
#include "terrainEffect.h"
#include "observerSettings.h"
#include "ByyNodeBank.h"

#include "localObjectManager.h"
#include "ByyCore.h"
#include "ByyIG.h"
#include "entityObject.h" 
#include "keyValueList.h"
#include "opd.h"

#include <osgEarthUtil/ContourMap>
#include <osgEarthUtil/LODBlending>
#include <osgEarthUtil/VerticalScale>
#include <osgEarthDrivers/gdal/GDALOptions>
#include <osgEarth/FileUtils>
#include <osgDB/FileNameUtils>
#include <osgEarth/CullingUtils>

#include <osg/Stencil>
#include <osg/Depth>

const char* vertexShader =
	"attribute vec4  oe_terrain_attr; \n"
	"varying float height;\n"

	"void setupContour(inout vec4 VertexModel) \n"
	"{ \n"
	"   height = oe_terrain_attr[3]; \n"
	"} \n";

const char* source_vertProxy =
	"#version " GLSL_VERSION_STR "\n"
	GLSL_DEFAULT_PRECISION_FLOAT "\n"

	"vec2 ocean_xyz_to_spherical(in vec3 xyz) \n"
	"{ \n"
	"    float r = length(xyz); \n"
	"    float lat = acos(xyz.z/r); \n"
	"    float lon = atan(xyz.y, xyz.x); \n"
	"    return vec2(lon,lat); \n"
	"} \n"

	"uniform mat4 osg_ViewMatrixInverse; \n"

	"varying float ocean_v_msl; \n"                    // elevation (MSL) of camera             // distance from camera to current vertex
	"varying vec2 lonlat; \n"  
	"void oe_ocean_vertex(inout vec4 VertexVIEW) \n"
	"{ \n"
	"   vec4 eye = osg_ViewMatrixInverse * vec4(0,0,0,1); \n"
	"   ocean_v_msl = length(eye.xyz/eye.w) - 6378137.0 ; \n"
	"   vec4 worldVertex = osg_ViewMatrixInverse * VertexVIEW; \n"
	"   lonlat = ocean_xyz_to_spherical( worldVertex.xyz/worldVertex.w ); \n"
	"} \n";

const char* fragmentShader =
	"varying float  height;\n"
	"varying float  ocean_v_msl; \n" 
	"varying vec2 lonlat; \n"  
	"void colorContour( inout vec4 color ) \n"
	"{ \n"
	"	if(ocean_v_msl<50000.0) \n"
	"	{ \n"
	"		if(height<=0.0) \n"
	"		{ \n"
	"			discard;\n"
	"		} \n"
	"	} \n"
	"} \n";

ByyTerrain::ByyTerrain()
{
	//osgEarth::LODScaleGroup *scaleGroup=new osgEarth::LODScaleGroup;
	//scaleGroup->setLODScaleFactor(0.5);

	//d_root=scaleGroup;

	myRoot=new osg::Group;
	myEffect=new ByyTerrainEffect(this);

	connect(&ByyObserverSettings::instance(),SIGNAL(graticuleVisibleChanged(bool)),this,SLOT(setGraticuleVisible(bool)));
}

ByyTerrain::~ByyTerrain()
{
	unload();
	delete myEffect;
}

bool ByyTerrain::load( const std::string& filename )
{
	if (d_mapNode.valid())
	{
		unload();
	}
	osg::Node* earthNode = osgDB::readNodeFile(filename);

	if (!earthNode)
	{
		return false;
	}

	d_mapNode = osgEarth::MapNode::findMapNode(earthNode);
	d_map =d_mapNode->getMap();

	parseMapNode(d_mapNode);

	myRoot->addChild(d_mapNode);

	d_filename=filename;

	setGraticuleVisible(ByyObserverSettings::instance().graticuleVisible());

	emit terrainLoaded(d_mapNode);
	myEffect->setTerrainEffect();
	return true;
}

bool ByyTerrain::unload()
{
	if (d_mapNode.valid())
	{
		myRoot->removeChildren(0,myRoot->getNumChildren());
		d_mapNode=NULL;
		d_map=NULL;
	}
	
	return true;
}

void ByyTerrain::save()
{
	saveAs(d_filename);
}

void ByyTerrain::saveAs( const std::string& filename )
{
	osgDB::Registry::instance()->writeNode(*d_mapNode,filename,osgDB::Registry::instance()->getOptions());
}

osgEarth::MapNode* ByyTerrain::mapNode()
{
	return d_mapNode;
}

osgEarth::ImageLayer* ByyTerrain::addImageLayer( const std::string& filePath )
{
	osgEarth::ImageLayer* newLayer=NULL;

	if (d_map.valid())
	{
		osgEarth::Drivers::GDALOptions layerOpt;
		layerOpt.url()=filePath;
		std::string fileName = osgDB::getSimpleFileName(filePath);
		newLayer = new osgEarth::ImageLayer(osgEarth::ImageLayerOptions(fileName, layerOpt));
		d_map->addImageLayer(newLayer);
	}

	return newLayer;
}

osgEarth::ElevationLayer* ByyTerrain::addElevatonLayer( const std::string& filePath )
{
	osgEarth::ElevationLayer* newLayer=NULL;
	if (d_map.valid())
	{
		osgEarth::Drivers::GDALOptions layerOpt;
		layerOpt.url()=filePath;
		std::string fileName = osgDB::getSimpleFileName(filePath);
		newLayer = new osgEarth::ElevationLayer(osgEarth::ElevationLayerOptions(fileName, layerOpt));

		d_map->addElevationLayer(newLayer);
	}

	return newLayer;
}

void ByyTerrain::parseMapNode( osgEarth::MapNode* mapNode )
{
	osg::ref_ptr<osgDB::Options> dbOptions = osgEarth::Registry::instance()->cloneOrCreateOptions();
	const osgEarth::Config& externals = mapNode->externalConfig();

	const osgEarth::Config& declutterConf   = externals.child("decluttering");
	//osgEarth::Config        viewpointsConf  = externals.child("viewpoints");

	const osgEarth::Config& normalMapConf   = externals.child("normal_map");
	const osgEarth::Config& detailTexConf   = externals.child("detail_texture");
	const osgEarth::Config& lodBlendingConf = externals.child("lod_blending");
	const osgEarth::Config& vertScaleConf   = externals.child("vertical_scale");
	const osgEarth::Config& contourMapConf  = externals.child("contour_map");

	/*
	const osgEarth::ConfigSet& old_viewpoints = externals.children("viewpoint");
	for(osgEarth::ConfigSet::const_iterator i = old_viewpoints.begin(); i != old_viewpoints.end(); ++i )
		viewpointsConf.add( *i );

	if ( !viewpointsConf.empty() )
	{	
		const osgEarth::ConfigSet& children = viewpointsConf.children();
		if ( children.size() > 0 )
		{
			for( osgEarth::ConfigSet::const_iterator i = children.begin(); i != children.end(); ++i )
			{
				viewpoints.push_back( osgEarth::Viewpoint(*i) );
			}
		}
	}
	*/

	// Install elevation morphing
	if ( !lodBlendingConf.empty() )
	{
		mapNode->getTerrainEngine()->addEffect( new osgEarth::Util::LODBlending(osgEarth::Util::LODBlendingOptions(lodBlendingConf)) );
	}

	// Install vertical scaler
	if ( !vertScaleConf.empty() )
	{
		mapNode->getTerrainEngine()->addEffect( new osgEarth::Util::VerticalScale(vertScaleConf) );
	}

	// Install a contour map effect.
	if ( !contourMapConf.empty() )
	{
		mapNode->getTerrainEngine()->addEffect( new osgEarth::Util::ContourMap(contourMapConf) );
	}

	const osgEarth::Config& masksConf = externals.child("masks");
	
	if (!masksConf.empty())
	{
		const osgEarth::ConfigSet& children=masksConf.children("mask");

		for(osgEarth::ConfigSet::const_iterator i = children.begin(); i != children.end(); ++i )
		{
			if (!i->empty())
			{
				bool enable=false;
				i->getIfSet("enable",enable);

				if (enable)
				{
					addMask(mapNode,*i);
				}
			}
		}
	}


}

osgEarth::Map* ByyTerrain::getMap()
{
	return d_map;
}

void ByyTerrain::addMask( osgEarth::MapNode *mapNode,const osgEarth::Config& conf )
{
	osgEarth:: Config maskConf("mask");
	maskConf.add("driver","feature");
	maskConf.add("name","mask");
	maskConf.add("min_level",15);
	osgEarth::Config featuresConf("features");
	featuresConf.add("driver","ogr");
	featuresConf.add("geometry",conf.value("geometry"));

	maskConf.add(featuresConf);

	maskConf.add("profile","global-geodetic");


	osgEarth::MaskLayerOptions options(maskConf);
	options.name() = maskConf.value( "name" );
	options.driver() = osgEarth::MaskSourceOptions(options);

	mapNode->getMap()->addTerrainMaskLayer( new osgEarth::MaskLayer(options) );
}

bool ByyTerrain::isLoad()
{
	return d_mapNode.valid();
}

void ByyTerrain::setChanged()
{
	
}

ByyTerrainEffect* ByyTerrain::effect()
{
	return myEffect;
}

bool ByyTerrain::getWorldCoordsUnderMouse( osg::View* view, float x, float y, osg::Vec3d& out_coords ) const
{
	if (d_mapNode.valid())
	{
		return d_mapNode->getTerrain()->getWorldCoordsUnderMouse(view,x,y,out_coords);
	}
	else
		return false;
}

bool ByyTerrain::crushNode(const osgEarth::GeoPoint& point, int& opdId, osg::Vec3d& orientation, osgEarth::GeoPoint& crushObjPt)
{
	bool ret = false;
	ByyNodeBank::PlaceNodeList& nodesList = ByyNodeBank::instance().getPlaceNodeList();
	ByyNodeBank::PlaceNodeList::iterator it=nodesList.begin();

	//qDebug() << __FUNCTION__ << __LINE__ << point.x() << point.y() << point.z() << "yyyyyyyyyyyyyyyyyyy";
	for (;it!=nodesList.end();++it)
	{
		if( it.key()->getNodeMask()==0 )
			continue;

		osgEarth::GeoPoint tPoint = it.key()->getPosition();
		//在一定范围内，自动选择
		if((qAbs(tPoint.x() - point.x()) < 0.001) && (qAbs(tPoint.y() - point.y()) < 0.001))
		{
			crushObjPt = tPoint;
			int tmpOpdId = it.value();
			ByyEntityObject* obj = ByyIG::IGInstance()->core().localObjectManager().entityObjects().value(tmpOpdId, nullptr);
			qDebug() << __FUNCTION__ << __LINE__ << obj << tmpOpdId << "aaaaaaaaaaaaaaaaaaaaaa" << obj->entityType().domain;
			//目前只有水上目标可以搭载目标
			if( (nullptr == obj) || (obj && (3 != obj->entityType().domain)))
			{
				continue;
			}
			orientation = obj->orientation();
			opdId = tmpOpdId;
			//qDebug() << __FUNCTION__ << __LINE__ << crushObjPt.x() << crushObjPt.y() << crushObjPt.z() << opdId << "xxxxxxxxxxxxx";
			return true;
		}
	}
	return ret;
}


bool ByyTerrain::getChildPlacer(const int& opdId, float& height, float& length, float& width)
{
	bool ret = false;
	ByyEntityObject* obj = ByyIG::IGInstance()->core().localObjectManager().entityObjects().value(opdId, nullptr);
	if(obj)
	{
		ByyOpdEntry* opd=ByyOpd::instance()->findOpdEntry(obj->entityType());

		height = opd->value("StopHeight").toFloat();
		length = opd->value("StopLength").toFloat();
		width = opd->value("StopWidth").toFloat();
		QString name = opd->value("Name");
		//qDebug() << __FUNCTION__ << __LINE__ << height << length << width << name << opdId << obj->entityType().string() <<"zzzzzzzzzzzzzzzzzzzzzzzz";
		return true;
	}
	return ret;
}

const osgEarth::SpatialReference* ByyTerrain::srs()
{
	if (d_mapNode)
		return d_mapNode->getMapSRS();
	else
		return 0;
}

void ByyTerrain::setGraticuleVisible( bool v )
{
	if(v&&!myGraticule.valid())
	{
		myGraticule=new osgEarth::Util::GeodeticGraticule(mapNode());

		osgEarth::Util::GeodeticGraticuleOptions options=myGraticule->getOptions();
 		osgEarth::Util::GeodeticGraticuleOptions destOptions;
 		destOptions.lineStyle()=options.lineStyle();
 		destOptions.textStyle()=options.textStyle();
 		std::vector<osgEarth::Util::GeodeticGraticuleOptions::Level> lvls=options.levels();
 		
 
 		std::vector<osgEarth::Util::GeodeticGraticuleOptions::Level>::const_iterator it=lvls.begin();
 		for (;it!=lvls.end();++it)
 		{
 			osgEarth::Util::GeodeticGraticuleOptions::Level lvl=*it;
 			lvl._subdivisionFactor=2;
 			destOptions.addLevel(lvl._maxRange,lvl._minRange,3);
 		}
 
 		myGraticule->setOptions(destOptions);
 
 		it=myGraticule->getOptions().levels().begin();
 		for (;it!=myGraticule->getOptions().levels().end();++it)
 		{
 			osgEarth::Util::GeodeticGraticuleOptions::Level lvl=*it;
 			int fac=lvl._subdivisionFactor;
 			int test=0;
 		}
		//options.levels()=lvls;


		myRoot->addChild(myGraticule);
	}
	else if (!v&&myGraticule.valid())
	{
		myRoot->removeChild(myGraticule);
		myGraticule=0;
	}
}
