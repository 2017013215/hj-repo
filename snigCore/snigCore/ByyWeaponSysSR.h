#ifndef BYYWEAPONSYSREPOSITORY_H
#define BYYWEAPONSYSREPOSITORY_H

#include <QVector>
#include <QMap>
#include "stateRepository.h"

class SNIGCORE_EXPORT ByyWeaponSysSR : public ByyStateRepository
{
	Q_OBJECT

public:
	ByyWeaponSysSR(ByyRemoteEntity* ent);
	~ByyWeaponSysSR();

	const QString& type();

	static const QString& theType();

	struct CWeaponRecSmpData
	{
		long	m_WeaponRecID/*武器记录编号（从0开始）*/;
		QString	m_Name;/*武器名称*/;
		long	m_Count/*当前数量*/;
		bool	m_Available/*该武器是否可用*/;
		double	m_TimeToFire/*当前发射准备时间(s)*/;

		bool operator==(const CWeaponRecSmpData& other)
		{
			return m_WeaponRecID==other.m_WeaponRecID&&
				m_Name==other.m_Name&&
				m_Count==other.m_Count&&
				m_Available==other.m_Available&&
				m_TimeToFire==other.m_TimeToFire;
		}
	};

	typedef QVector<CWeaponRecSmpData> WeaponRecSmpDataList;

	struct SNIGCORE_EXPORT WeaponSystem
	{
		long	m_WeaponCount/*当前总可用弹量*/;

		long	m_WeaponRecCount/*总装弹种类数量*/;

		WeaponRecSmpDataList myWeponDataList;	

		const WeaponRecSmpDataList& getWeaponRecSmpData();

		void setWeaponRecSmpData(const WeaponRecSmpDataList& data);

		long getWeaponCount() const { return m_WeaponCount; }
		void setWeaponCount(long val) { m_WeaponCount = val; }

		long getWeaponRecCount() const { return m_WeaponRecCount; }
		void setWeaponRecCount(long val) { m_WeaponRecCount = val; }

		bool operator==(const WeaponSystem& other)
		{
			return myWeponDataList==other.myWeponDataList&&
				m_WeaponCount==other.m_WeaponCount&&
				m_WeaponRecCount==other.m_WeaponRecCount;
		}
	};

	typedef QMap<QString,WeaponSystem> WeaponSystemMap;

	void setSystemList(const WeaponSystemMap& sysList);

	const WeaponSystemMap& getSystemList(){return mySystemList;}

signals:
	void dataChanged();

protected:
	WeaponSystemMap mySystemList;
};

bool operator==(const ByyWeaponSysSR::WeaponSystem& s1,const ByyWeaponSysSR::WeaponSystem& s2);

#endif // BYYWEAPONSYSREPOSITORY_H
