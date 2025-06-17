#ifndef VERDICTBASEDATA_H
#define VERDICTBASEDATA_H

#include <QObject>
#include <QVariant>
#include <QVector>

//整体设计上，为了快速查找，不保留顺序
typedef QVariantMap KeyValues;

// 武器命中的遍历逻辑
//Item -> HitProbTable -> corrFactor -> facValue
typedef QList<KeyValues> FacValues;
//管理碰撞概率
struct CorrFactor
{
	KeyValues			attrs;
	FacValues			facValues;			//KEY:线性表
	KeyValues			typeValues;			//KEY:kv
	double factorByDistance(double distance) const
	{
		if(!facValues.isEmpty())
		{
			FacValues::const_iterator iter = facValues.constBegin();
			while(iter != facValues.constEnd())
			{
				double max = iter->value("max",0.0).toDouble();
				double min = iter->value("min",0.0).toDouble();
				double corrFac = iter->value("corrFac",0.0).toDouble();
				if((min <= distance) && (max >= distance))
				{
					return corrFac;
				}
				iter++;
			}
			return facValues.last().value("corrFac",0.0).toDouble();
		}
		return 0.0;
	}

	double factorByType(int type) const;
};
//命中因素
struct HitProbTable
{
	KeyValues				attrs;
	QMap<QString, CorrFactor> corrFactors;	//KEY:FacTag	
};
//管理命中类型
struct WeaponHitProbItem
{
	KeyValues				attrs;
	QMap<QString, HitProbTable> hitProbTables;	//KEY:%Targetkind.%Targetdomain
};
typedef QMap<QString, WeaponHitProbItem>  WeaponHitProbItems;	//Key:EntClsName

//使用方法 实体类型（%EntClsName） - 命中类型（%Targetkind-%Targetdomain） - 因素标识（%FacTag） - 获取碰撞概率（距离|类型）
//WeaponHitProbItems.value("CYJ83K_DL8Entity").value("1-3").value("LchRange").factorByDistance(100.0)
//WeaponHitProbItems.value("CYJ83K_DL8Entity").value("1-3").value("TgtJamType").factorByType(1)

// 武器毁伤的遍历逻辑
//Item -> WarheadProbTable -> ImpactProbTable -> DmgProb -> Prob
struct DmgProb {
	double max;
	double min;
	double tntKg;
	QVector<double> azDeg;
	QVector<double> elDeg;
	QVector<double> pt;
	int length;
};
//管理基础距离
struct ImpactProbTable{
	KeyValues		attrs;
	QList<DmgProb>	dmgProbs;	//KEY:FacTag	
	double getImpactProb(double distance, double az, double el, double& tnt) const;
};

//管理 接触方式
struct WarheadProbTable{
	KeyValues				attrs;
	QMap<QString, ImpactProbTable>  impactProbTables;		
	double getProbInfo(const QString& impactType, double distance, double az, double el, double& tnt)
	{
		double ret = 0.0;
		tnt=0.0;
		if(impactProbTables.contains(impactType))
		{
			ret = impactProbTables[impactType].getImpactProb(distance, az, el, tnt);
		}
		return ret;
	}
};

//管理 弹头
struct EntityDamageProbItem
{
	KeyValues				attrs;
	QMap<QString, WarheadProbTable> warheadProbTables;	//KEY:%WarheadType
};
typedef QMap<QString, EntityDamageProbItem>  EntityDamageProbItems;	//Key:EntClsName
//使用说明 实体（%EntClsName） -> 战斗部类型（%WarheadType） -> 接触方式（%ImpactType） -> 计算损伤值(length,az,el,tnth)
//EntityDamageProbItems.value("Zero72Ship").value("1001").value("1001").value(100.0, 100.0,)
//EntityDamageProbItems.value("Zero72Ship").value("1001").value("1001").value(100.0, 100.0,,)

class VerdictBaseData : public QObject
{
	Q_OBJECT

public:
	VerdictBaseData(QObject *parent = nullptr);
	~VerdictBaseData();
	void initEntityDamageProb(const QString& file);
	void initWeaponHitProb(const QString& file);

	const EntityDamageProbItems& getEntityDamageProbItems() { return m_entityDamageProbItems; }
	const WeaponHitProbItems& getWeaponHitProbItems() { return m_weaponHitProbItems; }

private:
	
	EntityDamageProbItems m_entityDamageProbItems;
	WeaponHitProbItems	  m_weaponHitProbItems;
};

#endif // VERDICTBASEDATA_H
