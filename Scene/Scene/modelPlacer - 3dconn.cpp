#include "StdAfx.h"
#include "modelPlacer.h"
#include "ByyIG.h"
#include "terrain.h"
#include "ByyInputController.h"
#include "ByyRenderer.h"
#include "ByyObserver.h"
#include "utils.h"

#include <osgEarth/ElevationQuery>

ByyModelPlacer::ByyModelPlacer( ByyIG& ig )
	:myIG(ig)
{
	ig.inputController().addEventProcessor(this);
}

ByyModelPlacer::~ByyModelPlacer()
{

}

ByyModelPlacer& ByyModelPlacer::instance( ByyIG& ig )
{
	static ByyModelPlacer placer(ig);
	return placer;
}

bool ByyModelPlacer::setModelType( const ByyEntityType& type )
{
	osg::Node* node=loadEntityModel(type.string(),myIG);

	if (!myPlaceNode.valid())
	{

		osgEarth::Viewpoint vp=myIG.inputController().currentObserver()->getViewpoint();

		myPlaceNode=new osgEarth::Annotation::LocalGeometryNode(myIG.terrain()->mapNode(),node);
		//osgEarth::GeoPoint position;
		//position.fromWorld(myIG.terrain()->srs(),osg::Vec3d(0,0,0));
		myPlaceNode->setPosition(vp.focalPoint().get());
		myIG.renderer()->addNodeToRoot(myPlaceNode,ByyOsgRenderer::Entity);
	}
	else
		myPlaceNode->setNode(node);

	myEntityType=type;

	myRotate=0;

	return true;
}

bool ByyModelPlacer::processOsgEvent( const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa )
{
	if (!myPlaceNode.valid())
		return false;

	switch(ea.getEventType())
	{
	case ea.SCROLL:
	case ea.MOVE:
	{
		/*osgViewer::View* view=dynamic_cast<osgViewer::View*>(&aa);

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
		}
		break;*/
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
				osgEarth::ElevationQuery eq(myIG.terrain()->getMap());

				double out_elevation=0;

				osgEarth::GeoPoint geoPt=myPlaceNode->getPosition();

				if (eq.getElevation(geoPt,out_elevation))
				{
					if (out_elevation<0)
					{
						out_elevation=0;
					}

					geoPt.alt()=out_elevation;

					myPlaceNode->setPosition(geoPt);
				}

				emit push(myPlaceNode->getPosition());
			}
			else if (ea.getButton()==ea.RIGHT_MOUSE_BUTTON)
			{
				release();
			}
			else if (ea.getButton()==ea.MIDDLE_MOUSE_BUTTON)
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
					myPlaceNode->setLocalRotation(osg::Quat());
					myRotate=0;
				}
			}
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

void ByyModelPlacer::connexion3D( long TX,long TY,long TZ,long RX,long RY,long RZ )
{
	if (!myPlaceNode.valid())
		return;

//	printf("%d,%d,%d,%d,%d,%d\n",TX,TY,TZ,RX,RY,RZ);

	osgEarth::GeoPoint geoPt=myPlaceNode->getPosition();

	osgEarth::Viewpoint vp=myIG.inputController().currentObserver()->getViewpoint();

	double range=vp.range().get().getValue();

	geoPt.x()+=range*0.000000001*TX;
	geoPt.y()+=range*0.000000001*TY;
	geoPt.alt()+=range*0.000003*TZ;

	geoPt.altitudeMode()=osgEarth::AltitudeMode::ALTMODE_ABSOLUTE;

	myRotate+=0.0001*RZ;

	myPlaceNode->setPosition(geoPt);
	myPlaceNode->setLocalRotation(osg::Quat(myRotate,osg::Z_AXIS));
}
