#include "ByyWeaponSysSR.h"

ByyWeaponSysSR::ByyWeaponSysSR(ByyRemoteEntity* ent)
	: ByyStateRepository(ent)
{

}

ByyWeaponSysSR::~ByyWeaponSysSR()
{

}

const QString& ByyWeaponSysSR::type()
{
	return theType();
}

const QString& ByyWeaponSysSR::theType()
{
	static QString _type("WeaponSys");
	return _type;
}

const ByyWeaponSysSR::WeaponRecSmpDataList& ByyWeaponSysSR::WeaponSystem::getWeaponRecSmpData()
{
	return myWeponDataList;
}

void ByyWeaponSysSR::WeaponSystem::setWeaponRecSmpData( const WeaponRecSmpDataList& data )
{
	myWeponDataList=data;
}

void ByyWeaponSysSR::setSystemList( const WeaponSystemMap& sysList )
{
	if (sysList!=mySystemList)
	{
		mySystemList=sysList;
		emit dataChanged();
	}
}

bool operator==( const ByyWeaponSysSR::WeaponSystem& s1,const ByyWeaponSysSR::WeaponSystem& s2 )
{
	return s1.myWeponDataList==s2.myWeponDataList&&
		s1.m_WeaponCount==s2.m_WeaponCount&&
		s1.m_WeaponRecCount==s2.m_WeaponRecCount;

}
