#include "vrlDriver.h"

#include <vl/exerciseConnInitializer.h>
#include <vl/exerciseConn.h>
#include <vl/reflectedEntityList.h>
#include <vl/reflectedEnvironmentProcess.h>
#include <vl/reflectedEnvironmentProcessList.h>
#include <vl/environmentalPointRecord.h>
#include <vlutil/vlTime.h>
#include <vlpi/articulatedPartCollection.h>
#include <vlpi/articulatedPart.h>

#include "remoteEntity.h"
#include "remoteObjectManager.h"
#include "ByyRoute.h"

static DtEnvironmentTypeRecord RouteType=DtEnvironmentTypeRecord(17,0,0,2,0,0,0);

ByyVrlDriver::ByyVrlDriver( ByyRemoteObjectManager& objectManager ) 
#if DtDIS
	:ByyDriver(objectManager,"MAK DIS")
#elif DtHLA_1516
	:ByyDriver(objectManager,"MAK HLA1516")
#else
	:ByyDriver(objectManager,"MAK HLA13")
#endif
	,exConn(0)
	,relEntList(0)
	,relEnvirList(0)
	,clock(0)
{

}

ByyVrlDriver::~ByyVrlDriver()
{
	onStop();
}

bool ByyVrlDriver::onStart()
{
	try
	{
#ifdef DtDIS

		DtExerciseConnInitializer exConnInitializer;
		exConnInitializer.setExerciseId(1);
		exConnInitializer.setMulticastTtl(-1);
		exConnInitializer.setPort(3000);
		exConnInitializer.setReceiveBufferSize(1048576);
		exConnInitializer.setSendBufferSize(-1);
		exConnInitializer.setSiteId(2);
		exConnInitializer.setUseAsynchIO(true);
		exConn=new DtExerciseConn(exConnInitializer);
#else
		exConn=new DtExerciseConn("VR-Link","SNIG");
#endif
	}
	catch(...)
	{
		return false;
	}
	

	relEntList=new DtReflectedEntityList(exConn);
	relEntList->addEntityAdditionCallback(entityAddedCallback,this);
	relEntList->addEntityRemovalCallback(entityRemovalCallback,this);
	relEntList->discoverOnlyWhenEntityTypeKnown (true);


	//relEnvirList=new DtReflectedEnvironmentProcessList(exConn);
	//relEnvirList->addEnvironmentProcessAdditionCallback(environmentProcessAdditionCallback,this);
	//relEnvirList->addEnvironmentProcessRemovalCallback(environmentProcessRemovalCallback,this);
	//relEnvirList->setDiscoveryCondition(discoveryConditionCallback,this);

	clock=exConn->clock();

	return true;
}

bool ByyVrlDriver::onStop()
{
	DtDELETE(relEnvirList);
	DtDELETE(relEntList);
	DtDELETE(exConn);

	{
		ByyEntityMap::iterator it=myEntityList.begin();

		while (it!=myEntityList.end())
		{
			myObjectManager.removeEntity(*it);
			delete *it;
			it=myEntityList.erase(it);
		}
	}

	{
		ByyCtrlObjMap::iterator it=myCtrlObjList.begin();

		while (it!=myCtrlObjList.end())
		{
			myObjectManager.removeContrlObject(*it);
			delete *it;
			it=myCtrlObjList.erase(it);
		}
	}
	return true;
}

bool ByyVrlDriver::onTick()
{
	if (exConn)
	{
		clock->setSimTime(clock->elapsedRealTime());
		exConn->drainInput();

		ByyEntityMap::iterator it=myEntityList.begin();
		for (;it!=myEntityList.end();++it)
		{
			processEntityUpdate(it.key());
		}
	}

	return true;
}

void ByyVrlDriver::entityAddedCallback( DtReflectedEntity *obj, void *userData )
{
	if (obj->entityStateRep()->entityType().kind()!=12)
	{
		ByyVrlDriver *c=static_cast<ByyVrlDriver*>(userData);
		if (c)
		{
			c->processEntityAdded(obj);
		}
	}
}

void ByyVrlDriver::entityRemovalCallback( DtReflectedEntity *obj, void *userData )
{
	ByyVrlDriver *c=static_cast<ByyVrlDriver*>(userData);

	if (c)
	{
		c->processEntityRemoved(obj);
	}
}

void ByyVrlDriver::processEntityAdded( DtReflectedEntity *obj )
{
	ByyRemoteEntity *entObj=new ByyRemoteEntity;

	entObj->setId((int)obj);

	entObj->setDriverName(instanceName());

	DtEntityType makType=obj->esr()->entityType();

	ByyEntityType myEntityType;
	
	myEntityType.kind=makType.DtEntityKind;
	myEntityType.domain=makType.DtDomain;
	myEntityType.country=makType.DtCountry;
	myEntityType.category=makType.DtCategory;
	myEntityType.subCategory=makType.DtSubCategory;
	myEntityType.specific=makType.DtSpecific;
	myEntityType.extra=makType.DtExtra;

	entObj->setEntityType(myEntityType);

	processEntityUpdate(obj);

	myObjectManager.appendEntity(entObj);

	myEntityList.insert(obj,entObj);

//	obj->addPostUpdateCallback(entityUpdateCallback,this);
}

void ByyVrlDriver::processEntityRemoved( DtReflectedEntity *obj )
{
	ByyEntityMap::iterator it=myEntityList.find(obj);

	if (it!=myEntityList.end())
	{
		myObjectManager.removeEntity(*it);
		delete *it;
		myEntityList.erase(it);
	}
}

void ByyVrlDriver::entityUpdateCallback( DtReflectedEntity *obj, void *userData )
{
	ByyVrlDriver *c=static_cast<ByyVrlDriver*>(userData);

	if (c)
	{
		c->processEntityUpdate(obj);
	}
}

void ByyVrlDriver::processEntityUpdate( DtReflectedEntity *obj)
{
	ByyEntityMap::iterator it=myEntityList.find(obj);

	if (it==myEntityList.end())return;

	DtEntityStateRepository *esr=it.key()->esr();
	ByyEntityObject* localEntity=it.value();

	static DtCoordTransform geocToTopo;
	static DtTaitBryan topoEuler;
	static osg::Vec3d pos;
	static osg::Vec3d ori;
	static osg::Vec3d vel;

	localEntity->setName(esr->markingText());

	ByyForceType forceType=(ByyForceType)esr->forceId();

	switch(esr->forceId())
	{
	case DtForceFriendly:
		forceType=ByyForceOpposing;
		break;
	case DtForceOpposing:
		forceType=ByyForceFriendly;
		break;
	default:
		forceType=(ByyForceType)esr->forceId();
	}

	localEntity->setForceType(forceType);

	localEntity->setDamageState((ByyDamageState)esr->damageState());

	geod.setGeocentric(esr->location());

	pos.x()=DtRad2Deg(geod.lon());
	pos.y()=DtRad2Deg(geod.lat());
	pos.z()=geod.alt();

	localEntity->setPosition(pos);

	DtGeocToTopoTransform(geod.lat(),geod.lon(),&geocToTopo);

	geocToTopo.eulerTrans(esr->orientation(), &topoEuler);

	ori.x()=DtRad2Deg(topoEuler.psi());
	ori.y()=DtRad2Deg(topoEuler.theta());
	ori.z()=DtRad2Deg(topoEuler.phi());

	if (ori.x()<0)
	{
		ori.x()+=360;
	}

	localEntity->setOrientation(ori);

	vel.x()=esr->velocity().x();
	vel.y()=esr->velocity().y();
	vel.z()=esr->velocity().z();

	localEntity->setVelocity(vel);

	/*DtArticulatedPartCollection *parts=esr->artPartList();
	ByyArticulatedPartCollection* localParts=localEntity->artPartList();

	for(DtArticulatedPartCollection::const_iterator artPartIter = parts->begin();artPartIter != parts->end();++artPartIter)
	{
		int currentPartType = artPartIter->first;
		DtArticulatedPart* currentPart = artPartIter->second;

		ByyArticulatedPart& curLocalPart=localParts->getPart(currentPartType);


		std::vector<DtArticulatedPart::ParameterMetric> parameterMetrics;
		currentPart->getParameterMetrics(parameterMetrics);

		std::vector<DtArticulatedPart::ParameterMetric>::const_iterator paramMetricIter = parameterMetrics.begin();
		std::vector<DtArticulatedPart::ParameterMetric>::const_iterator paramMetricEnd = parameterMetrics.end();
		for(;paramMetricIter != paramMetricEnd; ++paramMetricIter)
		{
			int paramMetric = *paramMetricIter;
			DtArticulatedPart::Parameter parameter;
			currentPart->getParameter(paramMetric, parameter);

			curLocalPart.setParameter(paramMetric,parameter.value);
		} 
	}*/
}

void ByyVrlDriver::environmentProcessAdditionCallback( DtReflectedEnvironmentProcess *obj, void *userData )
{
	ByyVrlDriver *c=static_cast<ByyVrlDriver*>(userData);

	if (c)
	{
		c->processEnvironmentAdded(obj);
	}
}

void ByyVrlDriver::environmentProcessRemovalCallback( DtReflectedEnvironmentProcess *obj, void *userData )
{
	ByyVrlDriver *c=static_cast<ByyVrlDriver*>(userData);

	if (c)
	{
		c->processEnvironmentRemoved(obj);
	}
}

void ByyVrlDriver::processEnvironmentAdded( DtReflectedEnvironmentProcess *obj )
{
	if (obj->epsr()->environmentType()==RouteType)
	{
		ByyRoute *route=new ByyRoute;

		myCtrlObjList.insert(obj,route);

		obj->addPostUpdateCallback(environmentProcessUpdateCallback,this);

		processEnvironmentUpdate(obj);

		myObjectManager.appendContrlObject(route);
	}
}

void ByyVrlDriver::processEnvironmentRemoved( DtReflectedEnvironmentProcess *obj )
{
	ByyCtrlObjMap::iterator it=myCtrlObjList.find(obj);

	if (it!=myCtrlObjList.end())
	{
		myObjectManager.removeContrlObject(*it);
		delete *it;
		myCtrlObjList.erase(it);
	}
}

void ByyVrlDriver::environmentProcessUpdateCallback( DtReflectedEnvironmentProcess *obj, void *userData )
{
	ByyVrlDriver *c=static_cast<ByyVrlDriver*>(userData);

	if (c)
	{
		c->processEnvironmentUpdate(obj);
	}
}

void ByyVrlDriver::processEnvironmentUpdate( DtReflectedEnvironmentProcess *obj )
{
	ByyCtrlObjMap::iterator it=myCtrlObjList.find(obj);

	if (it==myCtrlObjList.end())return;

	ByyControlObject* ctrlObj=it.value();

	QString name;

#ifdef DtDIS
	std::ostringstream os;
	os << obj->globalId().site() << ':' << obj->globalId().host() << ':' << obj->globalId().entityNum();
	name=QString::fromStdString(os.str());
#else
	name=obj->globalId().string();
#endif

	ctrlObj->setName(name);

	if (obj->epsr()->environmentType()==RouteType)
	{
		ByyRoute *route=dynamic_cast<ByyRoute*>(ctrlObj);

		if(route)
		{
			QVector<osg::Vec3d> myTempPoints;

			for (int i=0;i!=obj->epsr()->numberOfGeometryRecords();++i)
			{
				if (obj->epsr()->geometryRecordType(i)==DtPointRecord1Type)
				{

					DtEnvironmentalPointRecord geometryRecord;
					obj->epsr()->getGeometryRecord(i,geometryRecord);

					
					geod.setGeocentric(geometryRecord.location());

					myTempPoints.append(osg::Vec3d(DtRad2Deg(geod.lon()),DtRad2Deg(geod.lat()),geod.alt()));
				}
			}

			route->setPoints(myTempPoints);
		}
	}
}

bool ByyVrlDriver::discoveryConditionCallback( DtReflectedObject *refObj, void *usr )
{
	DtReflectedEnvironmentProcess *obj=dynamic_cast<DtReflectedEnvironmentProcess*>(refObj);

	if (obj&&obj->epsr()->environmentType()==DtEnvironmentTypeRecord(0,0,0,0,0,0,0))
	{
		return false;
	}
	return true;
}

bool ByyVrlDriver::isVRLink() const
{
	return true;
}
