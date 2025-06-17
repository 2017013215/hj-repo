#ifndef BYYDETECTOBJECT_H
#define BYYDETECTOBJECT_H

#include "ByyObject.h"
#include "entityType.h"

class SNIGCORE_EXPORT ByyDetectObject : public ByyObject
{
	Q_OBJECT
public:
	ByyDetectObject();
	~ByyDetectObject();

	void setUpdateTime(double t);
	double updateTime();

	void setContinueTime(double t);
	double getContinueTime();

	void setBatchNo(long val);
	long batchNo();

	void setTargetID(long id);
	long targetID();

	void setTargetName(const QString& name);
	QString targetName();

	void setCID(long cid);
	long getCID();

	void setEntityType(const ByyEntityType& et);
	const ByyEntityType& entityType();

	void setThreatLevel(long level);
	long threatLevel();

	void setPosition(const osg::Vec3d& pos);
	const osg::Vec3d& position();

	void setVelocity(double val);
	double velocity();

	void setCourse(double val);
	double course();

	void setDistance(double val);
	double distance();

	void setEl(double val);
	double El();

	void setAz(double val);
	double Az();

	void setRadiationType(long val);
	long radiationType();

	void setRadiationPt(double val);
	double radiationPt();

	void setRadiationFreq(double val);
	double radiationFreq();

	void setRadiationTu(double val);
	double radiationTu();

	void setRadiationPRF(double val);
	double radiationPRF();

	void setRadiationSignalType(double val);
	double radiationSignalType();

	void setRadiationPulseType(double val);
	double radiationPulseType();

	QString getAttributeValueSet(){return QString::null;}

	void setDeviceName(const QString& device);
	QString deviceName();

signals:
	void updateTimeChanged(double);
	void batchNoChanged(long);
	void CIDChanged(long);

	void entityTypeChanged(ByyEntityType);
	void threatLevelChanged(long);
	void posChanged(osg::Vec3d);
	void velocityChanged(double);
	void courseChanged(double);
	void distanceChanged(double);
	void elChanged(double);
	void azChanged(double);
	void radiationTypeChanged(long);
	void radiationPtChanged(double);
	void radiationFreqChanged(double);
	void radiationTuChanged(double);
	void radiationPRFChanged(double);
	void radiationSignalTypeChanged(double);
	void radiationPulseTypeChanged(double);

private:
	double	m_UpdateTime/*更新时间*/;
	double	m_ContinueTime/*持续探测时间s*/;
	long	m_BatchNo/*目标批号*/;
	//long	m_TargetID/*目标ID*/;
	//QString	m_TargetName/*目标名称*/;
	long	m_CID/*目标识别等级(0未发现,1发现,2分类,3识别,4掌握)*/;
	//long	m_Side/*敌我属性*/;
	ByyEntityType	m_EntityType/*目标类型*/;
	long	m_ThreatLevel/*威胁等级*/;
	osg::Vec3d m_Pos/*目标位置*/;
	double	m_Velocity/*速度大小（m/s）*/;
	double	m_Course/*航向(deg,正北为0)*/;
	double	m_Distance/*目标距离(m)*/;
	double	m_El/*高低角(deg)*/;
	double	m_Az/*方位角(deg)*/;
	long	m_RadiationType/*辐射源类型*/;
	double	m_RadiationPt/*辐射源功率(KW)*/;
	double	m_RadiationFreq/*辐射源频率(Hz)*/;
	double	m_RadiationTu/*辐射源脉宽(s)*/;
	double	m_RadiationPRF/*辐射源重频(Hz)*/;
	double	m_RadiationSignalType/*辐射源信号类型*/;
	double	m_RadiationPulseType/*辐射源信号脉冲调制类型*/;

	QString m_DeviceName;
};

#endif // BYYDETECTOBJECT_H
