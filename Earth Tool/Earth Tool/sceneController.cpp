#include "stdafx.h"
#include "sceneController.h"

#include <QString>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthDrivers/gdal/GDALOptions>
#include <osgEarth/ImageLayer>
#include <osgEarthUtil/Sky>
#include <osgEarthUtil/Ocean>
#include <osgEarth/Registry>
#include <osgEarthUtil/Controls>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/StateSetManipulator>

ByySceneController::ByySceneController()
{
	viewer=new osgViewer::Viewer;
	viewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);
	root=new osg::Group;
	root->addChild(osgEarth::Util::Controls::ControlCanvas::getOrCreate(viewer));
	viewer->setSceneData(root);
	viewer->getDatabasePager()->setUnrefImageDataAfterApplyPolicy(false,false);
	viewer->getCamera()->setNearFarRatio(0.00001);
	viewer->getCamera()->setSmallFeatureCullingPixelSize(-1.0f);
	viewer->addEventHandler(new osgViewer::StatsHandler);
	viewer->addEventHandler(new osgGA::StateSetManipulator(viewer->getCamera()->getStateSet()));
	clear();
}

ByySceneController::~ByySceneController()
{
	
}

osg::ref_ptr<osgViewer::Viewer> ByySceneController::getViewer()
{
	return viewer;
}

osgEarth::MapNode* ByySceneController::getMapNode()
{
	return mapNode.get();
}

osg::ref_ptr<osgEarth::Map> ByySceneController::getMap()
{
	return _map;
}

void ByySceneController::newMap()
{
	readFile("../data/earthfile/default.earth");
}

bool ByySceneController::readFile( const QString &fileName )
{
	if (earthNode.valid())
	{
		root->removeChild(earthNode);
		earthNode = 0L;
	}
	earthNode = osgDB::readNodeFile(fileName.toStdString());
	if (!earthNode.valid())
	{
		earthNode = new osgEarth::MapNode(new osgEarth::Map());
	}
	else
	{
		mapNode = osgEarth::MapNode::findMapNode( earthNode );
		if (mapNode.valid())
		{
			_map = mapNode->getMap();
		}
	}

	root->insertChild(0,earthNode);

	osgEarth::Util::EarthManipulator *em=new osgEarth::Util::EarthManipulator;
	viewer->setCameraManipulator(em);

	osgEarth::Viewpoint vp=em->getViewpoint();
#if OSGEARTH_MINOR_VERSION==6
	vp.x()=119;
	vp.y()=30;
#else if OSGEARTH_MINOR_VERSION==7
	vp.focalPoint()->x()=119;
	vp.focalPoint()->y()=30;
#endif
	

	em->setHomeViewpoint(vp);
	em->setViewpoint(vp);

	viewer->getCamera()->setClearColor(osg::Vec4(0.2,0.2,0.4,1.0));

	osg::ref_ptr<osgDB::Options> dbOptions = osgEarth::Registry::instance()->cloneOrCreateOptions();
	const osgEarth::Config& externals = mapNode->externalConfig();

	return true;
}

bool ByySceneController::writeFile( const QString &fileName )
{
	osgDB::Registry::instance()->writeNode(*earthNode,fileName.toStdString(),osgDB::Registry::instance()->getOptions());
	return true;
}

void ByySceneController::clear()
{
	viewer->getCamera()->setClearColor(osg::Vec4(0.37,0.37,0.37,1.0));
	root->removeChild(earthNode);
	earthNode=NULL;
	mapNode=NULL;
	_map=NULL;
}

osgEarth::Viewpoint ByySceneController::getViewpoint()
{
	osgEarth::Util::EarthManipulator *em=dynamic_cast<osgEarth::Util::EarthManipulator*>(viewer->getCameraManipulator());
	if (em)
	{
		return em->getViewpoint();
	}
	return osgEarth::Viewpoint();
}

void ByySceneController::changeViewpoint(osgEarth::Viewpoint& viewpoint )
{
	osgEarth::Util::EarthManipulator *em=dynamic_cast<osgEarth::Util::EarthManipulator*>(viewer->getCameraManipulator());
	if (em)
	{
		em->setViewpoint(viewpoint);
	}
}

void ByySceneController::changeViewpoint( osg::Vec3d& v )
{
	osgEarth::Util::EarthManipulator *em=dynamic_cast<osgEarth::Util::EarthManipulator*>(viewer->getCameraManipulator());
	if (em)
	{
#if OSGEARTH_MINOR_VERSION==6
		osgEarth::Viewpoint vp;
		vp.x()=v.x();
		vp.y()=v.y();
		vp.setHeading(0);
		vp.setPitch(-89);
		vp.setRange(v.z());
#else if OSGEARTH_MINOR_VERSION==7
		osgEarth::Viewpoint vp("vp",v.x(),v.y(),0,0,-89,v.z());
#endif
		em->setViewpoint(vp);
	}
}

osg::ref_ptr<osg::Group> ByySceneController::getRootNode()
{
	return root;
}

void ByySceneController::addChild( osg::Node* node )
{
	root->addChild(node);
}

void ByySceneController::removeChild( osg::Node* node )
{
	root->removeChild(node);
}