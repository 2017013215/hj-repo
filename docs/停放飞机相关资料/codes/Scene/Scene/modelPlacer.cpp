#include "StdAfx.h"
#include "modelPlacer.h"
#include "ByyIG.h"
#include "terrain.h"
#include "ByyInputController.h"
#include "ByyRenderer.h"
#include "utils.h"
#include "entitySettings.h"
#include "opdEntry.h"
#include "opd.h"

#include <osgEarth/ElevationQuery>
#include <osg/ComputeBoundsVisitor>

#include <Feature>
#include <FeatureSource>
#include <FeatureModelSource>
#include <Query>
#include <QMessageBox>
#include "../snigCore/coreUtils.h"
#include <QToolBar>
#include <QApplication>

using namespace osgEarth;
using namespace Features;
using namespace Symbology;

ByyModelPlacer* modelPlacer=0;
ByyModelPlacer* ByyModelPlacer::instance()
{
	return modelPlacer;
}

ByyModelPlacer::ByyModelPlacer( ByyIG& ig )
	:myIG(ig)
{
	modelPlacer=this;
	ig.inputController().addEventProcessor(this);

	_deploy=false;
}

ByyModelPlacer::~ByyModelPlacer()
{

}

ByyModelPlacer& ByyModelPlacer::instance( ByyIG& ig )
{
	static ByyModelPlacer placer(ig);
	return placer;
}

void ByyModelPlacer::enableDeploy(bool deploy)
{
	_deploy=deploy;
}

bool ByyModelPlacer::setModelType( const ByyEntityType& type)
{
	if(!_deploy)
		return false;
	osg::Node* node=loadEntityModel(type.string(),myIG);

	if (!myPlaceNode.valid())
	{
		myPlaceNode=new osgEarth::Annotation::LocalGeometryNode(myIG.terrain()->mapNode(),node);
		osgEarth::GeoPoint position;
		position.fromWorld(myIG.terrain()->srs(),osg::Vec3d(0,0,0));
		myPlaceNode->setPosition(position);
		myIG.renderer()->addNodeToRoot(myPlaceNode,ByyOsgRenderer::Entity);
	}
	else
		myPlaceNode->setNode(node);

	myEntityType=type;

	_cursor=QCursor(loadEntityIcon(myEntityType.string(),ByyForceNeutral));

	return true;
}

bool ByyModelPlacer::checkTerrainObstacle(double olon,double olat,double len,double width)
{
	if(ByyEntitySettings::instance().terrainDeployChecking()&&ByyEntitySettings::instance().terrainObstacleLayers().size()>0)
	{
		if(len>0&&width>0)
		{
			double lon,lat;
			double minlon,maxlon,minlat,maxlat;
			osgEarth::GeoMath::rhumbDestination(olat,olon,osg::DegreesToRadians(90.0),len/2,lat,lon);
			maxlon=osg::RadiansToDegrees(lon);
			osgEarth::GeoMath::rhumbDestination(olat,olon,osg::DegreesToRadians(-90.0),len/2,lat,lon);
			minlon=osg::RadiansToDegrees(lon);
			osgEarth::GeoMath::rhumbDestination(olat,olon,osg::DegreesToRadians(0.0),width/2,lat,lon);
			maxlat=osg::RadiansToDegrees(lat);
			osgEarth::GeoMath::rhumbDestination(olat,olon,osg::DegreesToRadians(180.0),width/2,lat,lon);
			minlat=osg::RadiansToDegrees(lat);


			ModelLayerVector layers;
			ByyIG::IGInstance()->terrain()->getMap()->getModelLayers(layers);
			for (ModelLayerVector::const_iterator it = layers.begin(); it != layers.end(); ++it)
			{
				ModelLayer* layer=*it;
				std::string name=layer->getName();
				if(ByyEntitySettings::instance().terrainObstacleLayers().contains(QString::fromStdString(layer->getName())))
				{
					ModelSource* source=layer->getModelSource();
					FeatureModelSource* fds=(FeatureModelSource*)source;
					FeatureSource* sr=fds->getFeatureSource();
					const FeatureProfile* profile= sr->getFeatureProfile();
					int size=sr->getFeatureCount();
					Query qr;
					qr.bounds()=Bounds(minlon,minlat,maxlon,maxlat);
					FeatureCursor* ft=sr->createFeatureCursor(qr);
					if(ft->hasMore())
					{
						return false;
					}
				}
			}
		}
	}
	return true;
}

bool ByyModelPlacer::checkTerrainObstacle(double lon,double lat)
{
	ByyOpdEntry* opd=ByyOpd::instance()->findOpdEntry(myEntityType);
	if(opd==0)
		return false;
	double len=opd->value("Length").toDouble();
	double width=opd->value("Width").toDouble();
	return checkTerrainObstacle(lon,lat,len,width);
}

bool ByyModelPlacer::processOsgEvent( const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa )
{
	if (!myPlaceNode.valid())
		return false;
	if(!_deploy)
		return false;
	switch(ea.getEventType())
	{
	case ea.SCROLL:
	case ea.MOVE:
	{
		osgViewer::View* view=dynamic_cast<osgViewer::View*>(&aa);

		osg::Vec3d out_world;

		if (myIG.terrain()->getWorldCoordsUnderMouse(view,ea.getX(),ea.getY(),out_world))
		{
			osgEarth::GeoPoint position=myPlaceNode->getPosition();
			position.fromWorld(position.getSRS(),out_world);

			if (position.alt()<0)
			{
				position.alt()=0;
			}

			myPlaceNode->setPosition(position);

			if(checkTerrainObstacle(osg::DegreesToRadians(position.x()),osg::DegreesToRadians(position.y())))
			{
				ByyIG::IGInstance()->sceneWidget()->setCursor(_cursor);
			}
			else
			{
				ByyIG::IGInstance()->sceneWidget()->setCursor(Qt::ForbiddenCursor);
			}
		}
		break;
	}
	case ea.PUSH:
	{
		myPushX=ea.getX();
		myPushY=ea.getY();
		break;
	}
	case ea.RELEASE:
		{
			if (ea.getX()!=myPushX||ea.getY()!=myPushY)
				break;

			if (ea.getButton()==ea.LEFT_MOUSE_BUTTON)
			{
				if(_deploy)
				{
				osgEarth::GeoPoint geoPt=myPlaceNode->getPosition();
				int crushObjId = -1;  //被碰撞对象
				int carryObjId = -1;  //被搭载对象
				osgEarth::GeoPoint crushObjPt;
				osg::Vec3d orientation;////被碰撞对象的朝向
				bool isCrush = false;
				if(ByyIG::IGInstance()->terrain()->crushNode(geoPt, crushObjId, orientation, crushObjPt))
				{
					float height,length,width;
					//是否具有可停靠位置
					if(ByyIG::IGInstance()->terrain()->getChildPlacer(crushObjId, height, length, width))
					{
						isCrush = true;
						//放置到指定位置
						//设置搭载关系
						//
						osg::ComputeBoundsVisitor boundVisitor;
						myPlaceNode->accept(boundVisitor);
						osg::BoundingBox parkingLotBoundingBox = boundVisitor.getBoundingBox();
						double myHeight = (parkingLotBoundingBox.zMax() - parkingLotBoundingBox.zMin()) / 10;
						double myLength = (parkingLotBoundingBox.xMax() - parkingLotBoundingBox.xMin()) / 10;
						height += myHeight;
						length -= myLength;
						////////
						double lon,lat;
						double latRad;
						double longRad;
						GeoMath::destination(osg::DegreesToRadians(crushObjPt.y()),
							osg::DegreesToRadians(crushObjPt.x()),
							osg::DegreesToRadians(orientation.x()),
							length,
							latRad,
							longRad);
						lon = osg::RadiansToDegrees(longRad);
						lat = osg::RadiansToDegrees(latRad);
						geoPt.set(geoPt.getSRS(), lon, lat, height, geoPt.altitudeMode());
						//被搭载对象
						carryObjId = crushObjId;
					}
				}

				//if(len>0&&width>0)
				//{
					if(!checkTerrainObstacle(osg::DegreesToRadians(geoPt.x()),osg::DegreesToRadians(geoPt.y())))
					{
						QMessageBox::information(0,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("存在障碍物，请选择其它地方部署！"));
						return false;
					}
				//}

				osgEarth::ElevationQuery eq(myIG.terrain()->getMap());

				double out_elevation=0;


				if (eq.getElevation(geoPt,out_elevation))
				{
					/*if (out_elevation<0)
					{
						out_elevation=0;
					}

					geoPt.alt()=out_elevation; zt*/
					if (!isCrush)
					{
						if (out_elevation<0)
						{
							out_elevation=0;
						}

						geoPt.alt()=out_elevation;
					}

					myPlaceNode->setPosition(geoPt);
				}

				emit push(myPlaceNode->getPosition(), orientation, carryObjId);
				}
			}
			else if (ea.getButton()==ea.RIGHT_MOUSE_BUTTON)
			{
				release();
			}
			ByyIG::IGInstance()->sceneWidget()->setCursor(Qt::ArrowCursor);
			break;
		}
	}

	return false;
}

ByyEntityType ByyModelPlacer::modelType() const
{
	return myEntityType;
}

void ByyModelPlacer::release()
{
	emit released();

	myIG.renderer()->removeNodeFromRoot(myPlaceNode,ByyOsgRenderer::Entity);

	myPlaceNode=0;
}
