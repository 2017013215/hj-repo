#ifndef BYYSATELLITEESR_H
#define BYYSATELLITEESR_H

#include "stateRepository.h"

class SNIGCORE_EXPORT ByySatelliteSR : public ByyStateRepository
{
	Q_OBJECT
public:
	ByySatelliteSR(ByyRemoteEntity* ent);
	~ByySatelliteSR();

	const QString& type();

	static const QString& theType();

	struct ByyOrbitData /*轨道参数*/
	{
		double	m_h/*角动量AngularMomentum(Km^2/s)*/;
		double	m_a/*半长轴(Km)*/;
		double	m_e/*偏心率(0圆,0-1椭圆,>1双曲线)*/;
		double	m_Incl/*轨道倾角,弹道平面与赤道面的夹角(度)*/;
		double	m_RA/*升交点赤经(度)*/;
		double	m_w/*近地点中心角(Argument of perigee)(度)*/;
		double	m_TA/*真近点角(True Anomaly)(度)*/;
		double	m_EA/*偏近点角,Eccentric Anomaly(度)*/;
		double	m_Tp/*过近地点的时刻*/;

		bool operator!=(const ByyOrbitData& orther)
		{
			return orther.m_h!=this->m_h||
				orther.m_a!=this->m_a||
				orther.m_e!=this->m_e||
				orther.m_Incl!=this->m_Incl||
				orther.m_RA!=this->m_RA||
				orther.m_w!=this->m_w||
				orther.m_TA!=this->m_TA||
				orther.m_EA!=this->m_EA||
				orther.m_Tp!=this->m_Tp;
		}
	};

	ByyOrbitData orbitData(){return myOrbitData;}

	void setOrbitData(const ByyOrbitData& od);

signals:
	void orbitChanged(const ByyOrbitData&);

protected:
	ByyOrbitData myOrbitData;
};

#endif // BYYSATELLITEESR_H
