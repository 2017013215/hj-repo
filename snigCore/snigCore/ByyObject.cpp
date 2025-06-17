#include "ByyObject.h"
#include "ByyObjectIdManager.h"
#include <QDomElement>

ByyObject::ByyObject() 
	:myForceType(ByyForceOther)
	,myLocalObj(false)
	,myVisible(true)
{
	ByyObjectIdManager::instance().registerObject(this);
}

ByyObject::~ByyObject()
{
	ByyObjectIdManager::instance().unRegisterObject(this);
}

void ByyObject::setId( int id )
{
	myId=id;
}

int ByyObject::id() const
{
	return myId;
}

void ByyObject::setObjectId( unsigned int id )
{
	myObjectId=id;
}

unsigned int ByyObject::objectId() const
{
	return myObjectId;
}

void ByyObject::setForceType( ByyForceType side )
{
	if (side!=myForceType)
	{
		myForceType=side;
		emit forceTypeChanged(myForceType);
	}
}

ByyForceType ByyObject::forceType()
{
	return myForceType;
}

void ByyObject::setName( const QString& name )
{
	if (name!=myName)
	{
		QString old=myName;
		myName=name;
		emit nameChanged(myName);
		emit nameChangedWithOld(name,old);
	}
}

QString ByyObject::name()
{
	return myName;
}
void ByyObject::setLocal( bool b )
{
	myLocalObj=b;
}

bool ByyObject::isLocal() const
{
	return myLocalObj;
}

void ByyObject::setVisible( bool b )
{
	if (b!=myVisible)
	{
		myVisible=b;

		emit visibleChanged(myVisible);
	}
}

bool ByyObject::visible() const
{
	return myVisible;
}

void ByyObject::setItemClass( const QString& cls )
{
	if (cls!=myItemClass)
	{
		myItemClass=cls;
	}
}

QString ByyObject::itemClass() const
{
	return myItemClass;
}

ByyKeyValueList ByyObject::getConfig()
{
	ByyKeyValueList keyValues;
	keyValues.add("Side",myForceType);
	return keyValues;
}

void ByyObject::fromConfig( const ByyKeyValueList& keyValues )
{
	keyValues.get("Side",(int&)myForceType);
}

void ByyObject::setDriverName( const QString& n )
{
	myDriverName=n;
}

QString ByyObject::driverName()
{
	return myDriverName;
}
