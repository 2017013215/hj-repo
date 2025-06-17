#include "StdAfx.h"
#include "ByyEntityInfoEventProcessor.h"
#include "entityInfoWidget.h"
#include "entitySettings.h"
#include "ByyIG.h"
#include "ByyNodeBank.h"
#include "entityObject.h"
#include "ByyObjectIdManager.h"

#include "ByyCore.h"
#include "remoteObjectManager.h"
#include "localObjectManager.h"
#include "terrain.h"

ByyEntityInfoEventProcessor::ByyEntityInfoEventProcessor( ByyIG& ig ) 
	: ByyEventProcessor()
	,myIG(ig)
	,myEntObj(0)
{
	ByyRemoteObjectManager& remoteObjMgr=ig.core().remoteObjectManager();
	ByyLocalObjectManager& localObjMgr=ig.core().localObjectManager();

	connect(&remoteObjMgr,SIGNAL(entityRemoved(ByyEntityObject*)),this,SLOT(onEntityRemoved(ByyEntityObject*)));
	connect(&localObjMgr,SIGNAL(entityRemoved(ByyEntityObject*)),this,SLOT(onEntityRemoved(ByyEntityObject*)));
}

ByyEntityInfoEventProcessor::~ByyEntityInfoEventProcessor()
{

}

bool ByyEntityInfoEventProcessor::processOsgEvent( const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa )
{
	if (!ByyEntitySettings::instance().entityInfoEnabled())
		return false;

	if (myEntObj&&ea.getEventType()==ea.FRAME)
	{
		static int frame=0;

		if (++frame==20)
		{
			update();

			frame=0;
		}
	}

	if (ea.getEventType() != ea.MOVE)
		return false;
	
	ByyNodeBank::PlaceNodeList& placeNodeList=ByyNodeBank::instance().getPlaceNodeList();

	if (placeNodeList.size()==0)
		return false;

	osgViewer::View *view=dynamic_cast<osgViewer::View*>(aa.asView());

	const osg::Camera * cam = view->getCamera();
	const osg::Viewport * viewport = cam->getViewport();
	osg::Matrixd windowMatrix = viewport->computeWindowMatrix();
	osg::Matrixd viewProjectionWindowMatrix = cam->getViewMatrix() * cam->getProjectionMatrix() * windowMatrix;

	osg::Vec3d position;

	osg::Vec3d v3;

	osg::Vec2d v2;

	osg::Vec2d mousePos(ea.getX(),ea.getY());

	ByyNodeBank::PlaceNodeList::iterator it=placeNodeList.begin();

	bool finded=false;

	for (;it!=placeNodeList.end();++it)
	{
		//if (it.key()->getNodeMask()==0||!it.key()->getPosition().toWorld(position))
		//	continue;

		if( it.key()->getNodeMask()==0 )
			continue;
		osgEarth::GeoPoint gp=it.key()->getPosition();
		if(gp.altitudeMode()==osgEarth::ALTMODE_RELATIVE)
		{
			gp.makeAbsolute(myIG.terrain()->mapNode()->getTerrain());
		}
		if(!gp.toWorld(position))
		{
			continue;
		}

		v3=position * viewProjectionWindowMatrix;


		bool inScreen=viewport->x() < v3.x() && v3.x() < viewport->x() + viewport->width() &&
			viewport->y() < v3.y() && v3.y() < viewport->y() + viewport->height();

		if(!inScreen)
			continue;

		v2.set(v3.x(),v3.y());

		if ((v2-mousePos).length()>16)
			continue;

		myEntObj=dynamic_cast<ByyEntityObject*>(ByyObjectIdManager::instance().getObject(it.value()));

		if (myEntObj)
		{
			ByyEntityInfoWidget::instance(myIG).setVisible(true);

			update();

			finded=true;

		}
		
		break;
		
	}


	if (!finded)
	{
		ByyEntityInfoWidget::instance(myIG).setVisible(false);
	}

	return false;
}

void ByyEntityInfoEventProcessor::update()
{
	osg::Vec4 color(1,1,1,0.5);

	switch(myEntObj->forceType())
	{
	case ByyForceFriendly:
		color=osg::Vec4(1,0,0,0.7);
		break;
	case ByyForceOpposing:
		color=osg::Vec4(0,0,1,0.7);
		break;
	case ByyForceNeutral:
		color=osg::Vec4(0,1,0,0.7);
		break;
	default:
		color=osg::Vec4(1,1,1,0.7);

	}
	ByyEntityInfoWidget::instance(myIG).setBackColor(color);

	osg::EllipsoidModel em;

	osg::Vec3d worldPos;

	em.convertLatLongHeightToXYZ(osg::DegreesToRadians(myEntObj->position().y()),
		osg::DegreesToRadians(myEntObj->position().x()),myEntObj->position().z(),worldPos.x(),worldPos.y(),worldPos.z());

	ByyEntityInfoWidget::instance(myIG).setPosition(worldPos);
	/// add 20220506
	int damageValue=100;
	switch(myEntObj->damageState())
	{
	case ByyDamageNone: 
		damageValue=100;
		break;
	case ByyDamageSlight: 
		damageValue=80;
		break;
	case ByyDamageModerate: 
		damageValue=40;
		break;
	case ByyDamageDestroyed: 
		damageValue=0;
		break;
	default:
		damageValue=100;
		break;
	}
	/////
	QString text=QString::fromLocal8Bit("名称：    %1\n经度：    %2\n纬度：    %3\n高度：    %4\n速度：    %5\n航向：    %6\n生命：    %7")
		.arg(myEntObj->name())
		.arg(myEntObj->position().x(),0,'f')
		.arg(myEntObj->position().y(),0,'f')
		.arg(myEntObj->position().z(),0,'f')
		.arg(myEntObj->speed(),0,'f')
		.arg(myEntObj->orientation().x(),0,'f')
		.arg(damageValue);

	ByyEntityInfoWidget::instance(myIG).setText(text);
}

void ByyEntityInfoEventProcessor::onEntityRemoved( ByyEntityObject *entObj )
{
	if (entObj==myEntObj)
	{
		myEntObj=0;
		ByyEntityInfoWidget::instance(myIG).setVisible(false);
	}
}
