#include "StdAfx.h"
#include "ByyQGis2DApdater.h"

#include "ByyApp.h"
#include "ByyCore.h"
#include "remoteObjectManager.h"
#include "localObjectManager.h"
#include "XFrame/XTest_GISWnd.h"
#include "mappingQuery.h"
#include <XGISCanvas.h>
#include "GIS-Extend\XShapeDef.h"
#include "ByyPolygon.h"
#include "ByyRectangle.h"
#include "ByyCircle.h"
#include "ByyEllipse.h"
#include "ByyRoute.h"
#include "ByyPhaseLine.h"
#include "ByyFineArrow.h"
#include "ByyAttackArrow.h"
#include "ByyDoubleArrow.h"
#include "ByyStraightArrow.h"
#include "ByyGatheringPlace.h"

#include "ByyDetectObject.h"
#include <osgEarth/GeoMath>

XCoordinateGeo vec3d2Xcoord(const osg::Vec3d& pnt)
{
	return XCoordinateGeo(pnt.x(),pnt.y(),pnt.z());
}

ByyQGis2DApdater::ByyQGis2DApdater(ByyApp& app,XTest_GISWnd* gisWnd,QObject *parent)
	: QObject(parent)
	,myApp(app)
	,myGisWnd(gisWnd)
{
	myGisWnd->canvas()->LoadEChartFile("./Data/S57Chart/C110408A.000");

	ByyRemoteObjectManager& remoteObjMgr=myApp.core().remoteObjectManager();
	ByyLocalObjectManager& localObjMgr=myApp.core().localObjectManager();

	connect(&remoteObjMgr,SIGNAL(entityAppend(ByyEntityObject*)),this,SLOT(appendEntObject(ByyEntityObject*)));
	connect(&remoteObjMgr,SIGNAL(entityRemoved(ByyEntityObject*)),this,SLOT(removeEntObject(ByyEntityObject*)));

	connect(&remoteObjMgr,SIGNAL(contrlObjectAppend(ByyControlObject*)),this,SLOT(appendContrlObject(ByyControlObject*)));
	connect(&remoteObjMgr,SIGNAL(contrlObjectRemoved(ByyControlObject*)),this,SLOT(removeContrlObject(ByyControlObject*)));

	connect(&localObjMgr,SIGNAL(entityAppend(ByyEntityObject*)),this,SLOT(appendEntObject(ByyEntityObject*)));
	connect(&localObjMgr,SIGNAL(entityRemoved(ByyEntityObject*)),this,SLOT(removeEntObject(ByyEntityObject*)));

	connect(&localObjMgr,SIGNAL(contrlObjectAppend(ByyControlObject*)),this,SLOT(appendContrlObject(ByyControlObject*)));
	connect(&localObjMgr,SIGNAL(contrlObjectRemoved(ByyControlObject*)),this,SLOT(removeContrlObject(ByyControlObject*)));


	connect(&remoteObjMgr,SIGNAL(detectAppend(ByyDetectObject*)),this,SLOT(appendDetectObject(ByyDetectObject*)));
	connect(&remoteObjMgr,SIGNAL(detectRemoved(ByyDetectObject*)),this,SLOT(removeDetectObject(ByyDetectObject*)));

	connect(&remoteObjMgr,SIGNAL(showRoleChanged(int)),this,SLOT(showRoleChanged(int)));

	QVector<XCoordinateGeo> _xLinePnts;
	_xLinePnts.push_back(XCoordinateGeo(120,30,0));

	_xLinePnts.push_back(XCoordinateGeo(150,20,0));

	XGIS::XPolygonShape* plygonShape=new XGIS::XPolygonShape(XGIS::XPolygonShape::ePolygon_Ellipses,_xLinePnts,"");

	plygonShape->SetPenStyle(Qt::SolidLine);
	plygonShape->SetPenClr(Qt::white);
	plygonShape->SetPenWidth(1.0);
	plygonShape->SetAlpha(0.7);
	plygonShape->SetVisible(true);

	//quint32 jk=myGisWnd->canvas()->AddShape(plygonShape);
}

ByyQGis2DApdater::~ByyQGis2DApdater()
{

}

static QColor forceTypeQtColor(ByyForceType ft)
{
	QColor rel=Qt::white;
	switch(ft)
	{
	case ByyForceOther:
		rel=Qt::white;
		break;
	case ByyForceFriendly:
		rel=Qt::red;
		break;
	case ByyForceOpposing:
		rel=Qt::blue;
		break;
	case ByyForceNeutral:
		rel=Qt::green;
		break;
	}

	return rel;
}

void ByyQGis2DApdater::appendEntObject( ByyEntityObject* entObj )
{
	JunBiaoId jbId=ByyMappingQuery::instance()->getEntityJunBiao(entObj->entityType().string().toLocal8Bit());

	if (jbId.libId==0&&jbId.markId==0)
	{
		jbId.libId=32;
		jbId.markId=16;
	}

	XCoordinateGeo xAirPlaneStart=vec3d2Xcoord(entObj->position());

	XGIS::XShapePointJB *m_pAirPlaneMark = new XGIS::XShapePointJB(xAirPlaneStart,
		jbId.libId,jbId.markId,entObj->name());
	m_pAirPlaneMark->SetHeading(entObj->orientation().x());
	
	m_pAirPlaneMark->SetUserClr(forceTypeQtColor(entObj->forceType()));

	myGisWnd->canvas()->AddShape(m_pAirPlaneMark);

	myEntityObjectList[entObj]=new ByyEntityJBUpdater(entObj,m_pAirPlaneMark,this);
}

void ByyQGis2DApdater::removeEntObject( ByyEntityObject* entObj )
{
	EntityObjectList::iterator it=myEntityObjectList.find(entObj);

	if (it!=myEntityObjectList.end())
	{
		myGisWnd->canvas()->RemoveShapeByID(it.value()->jbObj()->GetID());

		delete it.value();

		myEntityObjectList.erase(it);
	}
}

static QVector<XCoordinateGeo> myPoints2XFramePoints(const ByyVec3dArray& pnts)
{
	QVector<XCoordinateGeo> rel(pnts.size());

	for (int i=0;i!=rel.size();++i)
	{
		rel[i]=vec3d2Xcoord(pnts[i]);
	}
	return rel;
}

static QVector<XCoordinateGeo> myAreaObjPoint2XFrameObjPoint(ByyControlObject* obj)
{
	QVector<XCoordinateGeo> _xLinePnts;

	if (ByyRectangle::theType()==obj->type())
	{
		ByyRectangle *rectangle=dynamic_cast<ByyRectangle*>(obj);

		if (rectangle)
		{
			_xLinePnts.resize(2);

			ByyVec3dArray pnts=rectangle->points();

			osg::Vec3d minCoord=pnts.value(0);
			osg::Vec3d maxCoord=pnts.value(0);

			for (int i=0;i!=pnts.size();++i)
			{
				if (pnts[i].x()>maxCoord.x()) maxCoord.x()=pnts[i].x();
				else if (pnts[i].x()<minCoord.x()) minCoord.x()=pnts[i].x();

				if (pnts[i].y()>maxCoord.y()) maxCoord.y()=pnts[i].y();
				else if (pnts[i].y()<minCoord.y()) minCoord.y()=pnts[i].y();

				if (pnts[i].z()>maxCoord.z()) maxCoord.z()=pnts[i].z();
			}

			_xLinePnts[0]=XCoordinateGeo(minCoord.x(),maxCoord.y(),maxCoord.z());
			_xLinePnts[1]=XCoordinateGeo(maxCoord.x(),minCoord.y(),maxCoord.z());

		}
	}
	else if (ByyCircle::theType()==obj->type())
	{
		ByyCircle* circle=dynamic_cast<ByyCircle*>(obj);

		if (circle)
		{
			_xLinePnts.resize(2);

			_xLinePnts[0]=vec3d2Xcoord(circle->position());

			double clat, clon;
			osgEarth::GeoMath::destination( osg::DegreesToRadians(circle->position().y()),
				osg::DegreesToRadians(circle->position().x()), osg::DegreesToRadians(0.0),
				circle->radius(), clat, clon);

			_xLinePnts[1]=XCoordinateGeo(osg::RadiansToDegrees(clon),osg::RadiansToDegrees(clat),circle->position().z());
		}
	}
	else if (ByyEllipse::theType()==obj->type())
	{
		ByyEllipse* ellipse=dynamic_cast<ByyEllipse*>(obj);

		if (ellipse)
		{
			_xLinePnts.resize(2);

			double halfDiagonalDistance=sqrt(pow(ellipse->majorSemiAxis(),2)+pow(ellipse->minorSemiAxis(),2));

			double angleA=asin(ellipse->minorSemiAxis()/halfDiagonalDistance);

			double clat, clon;
			osgEarth::GeoMath::destination( osg::DegreesToRadians(ellipse->position().y()),
				osg::DegreesToRadians(ellipse->position().x()), osg::PI*2-angleA,
				halfDiagonalDistance, clat, clon);

			_xLinePnts[0]=XCoordinateGeo(osg::RadiansToDegrees(clon),osg::RadiansToDegrees(clat),ellipse->position().z());

			osgEarth::GeoMath::destination( osg::DegreesToRadians(ellipse->position().y()),
				osg::DegreesToRadians(ellipse->position().x()), osg::PI-angleA,
				halfDiagonalDistance, clat, clon);

			_xLinePnts[1]=XCoordinateGeo(osg::RadiansToDegrees(clon),osg::RadiansToDegrees(clat),ellipse->position().z());
		}
	}
	else
	{
		_xLinePnts=myPoints2XFramePoints(obj->points());
	}

	return _xLinePnts;
}

void ByyQGis2DApdater::appendContrlObject( ByyControlObject* obj )
{
	QVector<XCoordinateGeo> _xLinePnts=myAreaObjPoint2XFrameObjPoint(obj);

	XGIS::XPolygonShape::EPolygonType  _eType=XGIS::XPolygonShape::ePolygon_Unknown;

	if (ByyPolygon::theType()==obj->type())
	{
		_eType=XGIS::XPolygonShape::ePolygon_Polygon;
	}
	else if (ByyRectangle::theType()==obj->type())
	{
		_eType=XGIS::XPolygonShape::ePolygon_Rect;
	}
	else if (ByyCircle::theType()==obj->type())
	{
		_eType=XGIS::XPolygonShape::ePolygon_Circle;
	}
	else if (ByyEllipse::theType()==obj->type())
	{
		_eType=XGIS::XPolygonShape::ePolygon_Ellipses;
	}

	if (_eType!=XGIS::XPolygonShape::ePolygon_Unknown)
	{
		XGIS::XPolygonShape* plygonShape=new XGIS::XPolygonShape(_eType,_xLinePnts,obj->name());

		plygonShape->SetPenStyle(Qt::SolidLine);
		plygonShape->SetPenClr(Qt::white);
		plygonShape->SetPenWidth(1.0);
		plygonShape->SetAlpha(0.7);
		plygonShape->SetFillClr(forceTypeQtColor(obj->forceType()));
		plygonShape->SetVisible(true);

		myGisWnd->canvas()->AddShape(plygonShape);

		myGisWnd->canvas()->update();

		myControlObjectList[obj]=new ByyControlObjectUpdater(obj,plygonShape,this);

		return;
	}

	if (ByyRoute::theType()==obj->type()||ByyPhaseLine::theType()==obj->type())
	{
		XGIS::XLineShape* lineShape=new XGIS::XLineShape(_xLinePnts,obj->name());

		lineShape->SetPenStyle(Qt::SolidLine);
		lineShape->SetPenClr(forceTypeQtColor(obj->forceType()));
		lineShape->SetPenWidth(1.0);
		lineShape->SetAlpha(0.7);

		lineShape->SetVisible(true);

		myGisWnd->canvas()->AddShape(lineShape);

		myGisWnd->canvas()->update();

		myControlObjectList[obj]=new ByyControlObjectUpdater(obj,lineShape,this);

		return;
	}

	JunBiaoId jbId;

	if (ByyFineArrow::theType()==obj->type())
	{
		jbId.libId=32;
		jbId.markId=40500;
	}
	else if(ByyDoubleArrow::theType()==obj->type())
	{
		jbId.libId=32;
		jbId.markId=40700;
	}
	else if(ByyGatheringPlace::theType()==obj->type())
	{
		jbId.libId=32;
		jbId.markId=35500;
	}
	else if(ByyAttackArrow::theType()==obj->type())
	{
		jbId.libId=32;
		jbId.markId=40400;
		
	}
	else if(ByyStraightArrow::theType()==obj->type())
	{
		jbId.libId=32;
		jbId.markId=40403;
	}

	if (jbId.libId==0||jbId.markId==0) return;

	else
	{
		XGIS::XShapeLineJB* lineJB=new XGIS::XShapeLineJB(jbId.libId,jbId.markId,obj->name());
		
		lineJB->AddPnts(_xLinePnts);

		lineJB->SetPenStyle(Qt::SolidLine);
		lineJB->SetPenClr(forceTypeQtColor(obj->forceType()));
		lineJB->SetPenWidth(1.0);
		lineJB->SetAlpha(0.7);

		lineJB->SetVisible(true);

		myGisWnd->canvas()->AddShape(lineJB);

		myGisWnd->canvas()->update();

		myControlObjectList[obj]=new ByyControlObjectUpdater(obj,lineJB,this);

		return;
	}
}

void ByyQGis2DApdater::removeContrlObject( ByyControlObject* obj )
{
	ControlObjectList::iterator it=myControlObjectList.find(obj);

	if (it!=myControlObjectList.end())
	{
		myGisWnd->canvas()->RemoveShapeByID(it.value()->shape()->GetID());

		delete *it;

		myControlObjectList.erase(it);
	}
}

void ByyQGis2DApdater::appendDetectObject( ByyDetectObject* obj )
{
	JunBiaoId jbId=ByyMappingQuery::instance()->getEntityJunBiao(obj->entityType().string().toLocal8Bit());

	if (jbId.libId==0&&jbId.markId==0)
	{
		jbId.libId=32;
		jbId.markId=16;
	}

	XCoordinateGeo xAirPlaneStart(obj->position().x(),obj->position().y(),obj->position().z());

	XGIS::XShapePointJB *m_pAirPlaneMark = new XGIS::XShapePointJB(xAirPlaneStart,
		jbId.libId,jbId.markId,obj->name());
	m_pAirPlaneMark->SetHeading(obj->course());

	m_pAirPlaneMark->SetUserClr(forceTypeQtColor(obj->forceType()));

	myGisWnd->canvas()->AddShape(m_pAirPlaneMark);

	myDetectEntityObjectList[obj]=new ByyDetectEntityJBUpdater(obj,m_pAirPlaneMark,this);
}

void ByyQGis2DApdater::removeDetectObject( ByyDetectObject* obj )
{
	DetectEntityObjectList::iterator it=myDetectEntityObjectList.find(obj);

	if (it!=myDetectEntityObjectList.end())
	{
		myGisWnd->canvas()->RemoveShapeByID(it.value()->jbObj()->GetID());

		delete it.value();

		myDetectEntityObjectList.erase(it);
	}
}

void ByyQGis2DApdater::showRoleChanged( int rule )
{
	QList<ByyDetectObject*> waitRemovedList=myDetectEntityObjectList.keys();

	for (int i=0;i!=waitRemovedList.size();++i)
	{
		removeDetectObject(waitRemovedList[i]);
	}

	ByyRemoteObjectManager& remoteObjMgr=myApp.core().remoteObjectManager();

	QList<ByyDetectObject*>& allDetectList=remoteObjMgr.detectObjects();

	for (int i=0;i!=allDetectList.size();++i)
	{
		ByyDetectObject* obj=allDetectList[i];

		if ((rule==ByyRemoteObjectManager::Red&&obj->forceType()!=ByyForceFriendly)||
			(rule==ByyRemoteObjectManager::Blue&&obj->forceType()!=ByyForceOpposing))
		{
			appendDetectObject(obj);
		}
	}
}

ByyObjectUpdater::ByyObjectUpdater( ByyObject* obj,XGIS::XShape* shape,ByyQGis2DApdater* apdater )
	:myObj(obj)
	,myShape(shape)
	,myApdater(apdater)
{
	connect(myObj,SIGNAL(visibleChanged(bool)),this,SLOT(updateVisible(bool)));
	connect(myObj,SIGNAL(forceTypeChanged(ByyForceType)),this,SLOT(updateforceType(ByyForceType)));
	connect(myObj,SIGNAL(nameChanged(const QString&)),this,SLOT(updateName(const QString&)));
}

ByyObjectUpdater::~ByyObjectUpdater()
{

}

void ByyObjectUpdater::updateVisible( bool v )
{
	myShape->SetVisible(v);
}

void ByyObjectUpdater::updateName( const QString& name )
{
	myShape->SetName(name);
}

ByyEntityJBUpdater::ByyEntityJBUpdater( ByyEntityObject* entObj,XGIS::XShapePointJB* JB ,ByyQGis2DApdater* apdater)
	:ByyObjectUpdater(entObj,JB,apdater),myEntObj(entObj),myJB(JB)
{
	connect(entObj,SIGNAL(positionChanged(const osg::Vec3d&)),this,SLOT(updatePosition()));
	connect(entObj,SIGNAL(orientationChanged(const osg::Vec3d&)),this,SLOT(updatePosition()));

	connect(entObj,SIGNAL(damageStateChanged(ByyDamageState)),this,SLOT(updateDamageState(ByyDamageState)));
}

void ByyEntityJBUpdater::updatePosition()
{
	osg::Vec3d pos=myEntObj->position();

	XCoordinateGeo xAirPlaneStart(pos.x(),pos.y(),pos.z());
	myJB->SetPosGeo(xAirPlaneStart);
	myJB->SetHeading(myEntObj->orientation().x());
}

void ByyEntityJBUpdater::updateforceType( ByyForceType ft )
{
	myJB->SetUserClr(forceTypeQtColor(ft));
}

void ByyEntityJBUpdater::updateDamageState( ByyDamageState ds )
{

}

ByyControlObjectUpdater::ByyControlObjectUpdater( ByyControlObject* obj,XGIS::XShape* shape,ByyQGis2DApdater* apdater )
	:ByyObjectUpdater(obj,shape,apdater),myObj(obj)

{
	ByyLocalizedCtrlObject* localizedObj=dynamic_cast<ByyLocalizedCtrlObject*>(obj);

	if (localizedObj)
	{
		connect(localizedObj,SIGNAL(positionChanged(const osg::Vec3d&)),this,SLOT(rebuild()));
	}

	ByyPointSet* pointSet=dynamic_cast<ByyPointSet*>(obj);

	if (pointSet)
	{
		connect(pointSet,SIGNAL(pointsChanged()),this,SLOT(rebuild()));
	}

	ByyPhaseLine* phaseline=dynamic_cast<ByyPhaseLine*>(obj);

	if (phaseline)
	{
		connect(phaseline,SIGNAL(startChanged(const osg::Vec3d&)),this,SLOT(rebuild()));
		connect(phaseline,SIGNAL(endChanged(const osg::Vec3d&)),this,SLOT(rebuild()));
	}
}

void ByyControlObjectUpdater::updateforceType( ByyForceType ft )
{
	XGIS::XPolygonShape* plygonShape=dynamic_cast<XGIS::XPolygonShape*>(myShape);

	if (plygonShape)
	{
		plygonShape->SetFillClr(forceTypeQtColor(ft));
		return;
	}
}

void ByyControlObjectUpdater::rebuild()
{
	QVector<XCoordinateGeo> _xLinePnts=myAreaObjPoint2XFrameObjPoint(myObj);

	XGIS::XPolygonShape* polygonShape=dynamic_cast<XGIS::XPolygonShape*>(myShape);

	if (polygonShape)
	{
		polygonShape->SetPnts(_xLinePnts);

		return; 
	}

	XGIS::XLineShape* lineShape=dynamic_cast<XGIS::XLineShape*>(myShape);

	if (lineShape)
	{
		lineShape->SetPnts(_xLinePnts);
		return;
	}

	XGIS::XShapeLineJB* lineJB=dynamic_cast<XGIS::XShapeLineJB*>(myShape);

	if (lineJB)
	{
		lineJB->SetPnts(_xLinePnts);
		return;
	}
}

ByyDetectEntityJBUpdater::ByyDetectEntityJBUpdater( ByyDetectObject* entObj,XGIS::XShapePointJB* JB ,ByyQGis2DApdater* apdater)
	:ByyObjectUpdater(entObj,JB,apdater),myEntObj(entObj),myJB(JB)
{
	connect(entObj,SIGNAL(posChanged(osg::Vec3d)),this,SLOT(updatePosition()));
	connect(entObj,SIGNAL(courseChanged(double)),this,SLOT(updatePosition()));
}

void ByyDetectEntityJBUpdater::updatePosition()
{
	osg::Vec3d pos=myEntObj->position();
	XCoordinateGeo xAirPlaneStart(pos.x(),pos.y(),pos.z());
	myJB->SetPosGeo(xAirPlaneStart);
	myJB->SetHeading(myEntObj->course());
}

void ByyDetectEntityJBUpdater::updateforceType( ByyForceType ft )
{
	myJB->SetUserClr(forceTypeQtColor(ft));
}
