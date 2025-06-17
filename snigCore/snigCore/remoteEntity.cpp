#include "remoteEntity.h"
#include "stateRepository.h"

ByyRemoteEntity::ByyRemoteEntity()
	:myDamageState(ByyDamageNone)
	,myLevel(1)
{

}

ByyRemoteEntity::~ByyRemoteEntity()
{
	qDeleteAll(myExtendEsrMap);
}

void ByyRemoteEntity::setSuperiorName( const QString& superiorName )
{
	mySuperiorName=superiorName;
}

QString ByyRemoteEntity::superiorName()
{
	return mySuperiorName;
}

void ByyRemoteEntity::setHealthPoint(double healthPoint)
{
	m_healthPoint = healthPoint;
}

double ByyRemoteEntity::getHealthPoint()
{
	return m_healthPoint;
}

void ByyRemoteEntity::setPosition( const osg::Vec3d& pos )
{
	if(pos!=myPosition)
	{
		myPosition=pos;
		emit positionChanged(myPosition);
	}
}

osg::Vec3d ByyRemoteEntity::position()
{
	return myPosition;
}

void ByyRemoteEntity::setOrientation( const osg::Vec3d& ori )
{
	if (myOrientation!=ori)
	{
		myOrientation=ori;
		emit orientationChanged(myOrientation);
	}
}

osg::Vec3d ByyRemoteEntity::orientation()
{
	return myOrientation;
}

void ByyRemoteEntity::setSpeed( double speed )
{
	if (mySpeed!=speed)
	{
		mySpeed=speed;
	}
}

double ByyRemoteEntity::speed()
{
	return mySpeed;
}

void ByyRemoteEntity::setVelocity( const osg::Vec3d& vel )
{
	if(myVelocity!=vel)
	{
		myVelocity=vel;
	}
}

osg::Vec3d ByyRemoteEntity::velocity()
{
	return myVelocity;
}

void ByyRemoteEntity::setLevel( int l )
{
	if (l!=myLevel)
	{
		myLevel=l;
	}
}

int ByyRemoteEntity::level() 
{
	return myLevel;
}

bool ByyRemoteEntity::isLocal() const
{
	return false;
}

void ByyRemoteEntity::addExtendEsr( ByyStateRepository* esr )
{
	myExtendEsrMap.insert(esr->type(),esr);
}

ByyStateRepository* ByyRemoteEntity::getExtendEsr( const QString& name )
{
	ByyExtendEsrMap::iterator it=myExtendEsrMap.find(name);

	ByyStateRepository *rel=0;

	if (it!=myExtendEsrMap.end())
	{
		rel=it.value();
	}

	return rel;
}
