#include "entityObject.h"
#include "taskManager.h"
#include "ByyDescribeEntry.h"
#include "coreUtils.h"
#include <QStringList>

bool isRed(ByyEntityObject* entity)
{
	if(!entity) return false;
	ByyEntityType entType = entity->entityType();
	if ( entity->forceType() == ByyForceFriendly)
	{
		return true;
	}
	return false;
}

bool isBlue(ByyEntityObject* entity)
{
	if(!entity) return false;
	ByyEntityType entType = entity->entityType();
	if (entity->forceType() == ByyForceOpposing)
	{
		return true;
	}
	return false;
}

bool isAircraft(ByyEntityObject* entity)
{
	if(!entity) return false;
	ByyEntityType entType = entity->entityType();
	if (entType.kind==1 && entType.domain==2 )
	{
		return true;
	}
	return false;
}

bool isLandcraft(ByyEntityObject* entity)
{
	if(!entity) return false;
	ByyEntityType entType = entity->entityType();
	if (entType.kind==1 && entType.domain==1 )
	{
		return true;
	}
	return false;
}

bool isShip(ByyEntityObject* entity)
{
	if(!entity) return false;
	ByyEntityType entType = entity->entityType();
	if (entType.kind==1 && entType.domain==3)
	{
		return true;
	}
	return false;
}

bool isSubmarine(ByyEntityObject* entity)
{
	if(!entity) return false;
	ByyEntityType entType = entity->entityType();
	if (entType.kind == 1 && entType.domain == 4)
	{
		return true;
	}
	return false;
}

bool isMissile(ByyEntityObject* entity)
{
	if(!entity) return false;
	ByyEntityType entType = entity->entityType();
	if ((2 == entType.kind) &&
		((6 == entType.domain) || (11 == entType.domain) || (1 == entType.domain)))//2 )
	{
		return true;
	}
	return false;
}

bool isTorpedo(ByyEntityObject* entity)
{
	if(!entity) return false;
	ByyEntityType entType = entity->entityType();
	if ((2 == entType.kind) && (((6 == entType.domain) && (1 == entType.category) && entity->position().z() < 0) ||
		((7 == entType.domain) && (1 == entType.category))))
	{
		return true;
	}
	return false;
}

bool isSameEntity(ByyEntityObject* entity1 ,ByyEntityObject* entity2)
{
	if(!entity1 || !entity2) return false;
	if (entity1->name() == entity2->name())
	{
		return true;
	}
	return false;
}

QString getPartyName(ByyEntityObject* entity)
{
	if(!entity) return "";
	if(isRed(entity))
	{
		return RED_SIDE;
	}
	else if(isBlue(entity))
	{
		return BLUE_SIDE;
	}
	return "";
}

QString getAntiPartyName(ByyEntityObject* entity)
{
	if(!entity) return "";
	if(isBlue(entity))
	{
		return RED_SIDE;
	}
	else if(isRed(entity))
	{
		return BLUE_SIDE;
	}
	return "";
}

QString getAntiPartyName(const QString& partyname)
{
	if(RED_SIDE == partyname)
	{
		return BLUE_SIDE;
	}
	else if(BLUE_SIDE == partyname)
	{
		return RED_SIDE;
	}
	return "";
}

EntitySimpleObjectType typeJudgement(const ByyEntityType& typeInfo, double z)
{
	//ÓãÀ×
	if((2 == typeInfo.kind) && (((6 == typeInfo.domain) && (1 == typeInfo.category) && z < 0) ||
		((7 == typeInfo.domain) && (1 == typeInfo.category))))
	{
		return TorpedoObject;
	}
	//µ¼µ¯
	else if((2 == typeInfo.kind) &&
		((6 == typeInfo.domain) || (11 == typeInfo.domain) || (1 == typeInfo.domain)))
	{
		return MissileObject;
	}
	//Ç±Í§
	else if((1 == typeInfo.kind) && 
		(4 == typeInfo.domain))
	{
		return SubmarineObject;
	}
	//½¢Í§
	else if((1 == typeInfo.kind) && 
		(3 == typeInfo.domain))
	{
		return ShipEntityObject;
	}
	//ÉîË®Õ¨µ¯
	else if((2 == typeInfo.kind) && 
		(7 == typeInfo.domain) &&
		(2 == typeInfo.category))
	{
		return DepthChargeObject;
	}
	//Â½µØÄ¿±ê
	else if((1 == typeInfo.kind) && 
		(1 == typeInfo.domain))
	{
		return DepthChargeObject;
	}
	return InvalidObject;
}

ByyEntityObject::ByyEntityObject() 
	:ByyObject()
	,myDamageState(ByyDamageNone)
	,myTaskManager(new ByyTaskManager)
	,myDescribeEntry(0)
{

}

ByyEntityObject::~ByyEntityObject()
{
	if (myDescribeEntry)
	{
		delete myDescribeEntry;
	}

	delete myTaskManager;
}

void ByyEntityObject::setEntityType( const ByyEntityType& entType )
{
	if (entType!=myEntityType)
	{
		myEntityType=entType;
	}
}

const ByyEntityType& ByyEntityObject::entityType()
{
	return myEntityType;
}

void ByyEntityObject::setTargetName(const QString& targetName)
{
	myTargetName = targetName;
}

QString ByyEntityObject::getTargetName(void)
{
	return myTargetName;
}

bool ByyEntityObject::complete()
{
	return (!name().isEmpty())&&myEntityType.valid();
}

void ByyEntityObject::setDamageState( ByyDamageState ds )
{
	if (ds!=myDamageState)
	{
		myDamageState=ds;
		emit damageStateChanged(myDamageState);
	}
}

ByyDamageState ByyEntityObject::damageState() const
{
	return myDamageState;
}

double ByyEntityObject::getHealthPoint()
{
	return 100.0;
}

ByyTaskManager& ByyEntityObject::taskManager()
{
	return *myTaskManager;
}

/*
void ByyEntityObject::updateParam()
{
	ParameterList::iterator it=myParameters.begin();

	for (;it!=myParameters.end();++it)
	{
		if ((*it)->Name=="Level")
		{
			setLevel((*it)->getValue().toInt());
		}
		else if ((*it)->Name=="Side")
		{
			setForceType((ByyForceType)(*it)->getValue().toInt());
		}
		else if ((*it)->Name=="EntityName")
		{
			setName((*it)->getValue());
		}
		else if ((*it)->Name=="Position")
		{
			osg::Vec3d p;
			QTextStream(&(*it)->getValue())>>p;
			setPosition(p);
		}
		else if ((*it)->Name=="Superior")
		{
			setSuperiorName((*it)->getValue());
		}
		else if ((*it)->Name=="Velocity")
		{
			setSpeed((*it)->getValue().toDouble());
		}
		else if ((*it)->Name=="Heading")
		{
			setOrientation(osg::Vec3d((*it)->getValue().toDouble(),myOrientation.y(),myOrientation.z()));
		}
	}
}*/

QString ByyEntityObject::getAttributeValueSet()
{
	QStringList attList;

	attList.push_back(QString::number(forceType()));
	attList.push_back(name());
	attList.push_back(QString::number(level()));
	attList.push_back(name());
	attList.push_back(vec3d2Str(position()));

	return attList.join(";");
}

ByyArticulatedPartCollection* ByyEntityObject::artPartList()
{
	return &myArtPartList;
}

ByyDescribeEntry* ByyEntityObject::describeEntry()
{
	return myDescribeEntry;
}

void ByyEntityObject::setDescribeEntry( const ByyDescribeEntry& descEntry )
{
	if (myDescribeEntry==0)
	{
		myDescribeEntry=new ByyDescribeEntry;
	}

	*myDescribeEntry=descEntry;
}
