#ifndef BYYSENSORREPOSITORY_H
#define BYYSENSORREPOSITORY_H

#include "stateRepository.h"
#include "snigcore_global.h"
#include <QMap>
#include <QVector>
#include <osg/Vec3d>
#include "entityType.h"

class ByyDetectObject;

class SNIGCORE_EXPORT ByySensorSR : public ByyStateRepository
{
	Q_OBJECT

public:
	ByySensorSR(ByyRemoteEntity* ent);
	~ByySensorSR();

	const QString& type();

	static const QString& theType();

	bool getDevicePower(const QString& device);

	void setDevicePower(const QString& device,bool onOrFalse);

	bool switchPower(const QString& device);

	void setAntennaAz(const QString& device, double az);
	void setBeamWidth(const QString& device, float bw);

	//是否被跟踪
	bool isTracked(const QString& targetName, const QString& device);

	struct DetectData 
	{
		double	m_UpdateTime/*更新时间*/;
		double	m_ContinueTime/*持续探测时间s*/;
		long	m_BatchNo/*目标批号*/;
		long	m_TargetID/*目标ID*/;
		QString	m_TargetName/*目标名称*/;
		long	m_CID/*目标识别等级(0未发现,1发现,2分类,3识别,4掌握)*/;
		long	m_Side/*敌我属性*/;
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

		bool operator==(const DetectData& other)
		{
			return m_UpdateTime==other.m_UpdateTime&&
				m_BatchNo==other.m_BatchNo&&
				m_TargetID==other.m_TargetID&&
				m_TargetName==other.m_TargetName&&
				m_CID==other.m_CID&&
				m_Side==other.m_Side&&
				m_EntityType==other.m_EntityType&&
				m_ThreatLevel==other.m_ThreatLevel&&
				m_Pos==other.m_Pos&&
				m_Velocity==other.m_Velocity&&
				m_Course==other.m_Course&&
				m_Distance==other.m_Distance&&
				m_El==other.m_El&&
				m_Az==other.m_Az&&
				m_RadiationType==other.m_RadiationType&&
				m_RadiationPt==other.m_RadiationPt&&
				m_RadiationFreq==other.m_RadiationFreq&&
				m_RadiationTu==other.m_RadiationTu&&
				m_RadiationPRF==other.m_RadiationPRF&&
				m_RadiationSignalType==other.m_RadiationSignalType&&
				m_RadiationPulseType==other.m_RadiationPulseType;
		}
	};

	typedef QVector<DetectData> DetectDataList;

	typedef QMap<long,ByyDetectObject*> DetectObjectMap;

	void setDeviceDetectData(const QString& device,const DetectDataList& detData);

	struct DeviceState
	{
		DeviceState()
		{
			myPowerOn=false;
		}

		bool myPowerOn;
		double myAntennaAz;
		float myBeamWidth;

		DetectObjectMap myDetectObjList;
	};

	typedef QMap<QString,DeviceState> DeviceStateList;

	DeviceStateList* getDeviceList(); // 20230912,xks

	DeviceState* getDeviceState(const QString& device);

protected:
	void updateDetectObject(ByyDetectObject* obj,const DetectData& detect_data);

private:
	DeviceStateList myDeviceStateList;
	bool sensorOpr;
};

class SNIGCORE_EXPORT ByySensorSRSignaler : public QObject
{
	Q_OBJECT
public:
	static ByySensorSRSignaler& instance();

	void emit_discoverDetect(ByyDetectObject* obj,ByySensorSR* sensor);
	void emit_detectRemoved(ByyDetectObject* obj,ByySensorSR* sensor);

	void emit_powerSwitch(ByyRemoteEntity* ent,ByySensorSR* sensor,QString device,bool power);

signals:
	void discoverDetect(ByyDetectObject* obj,ByySensorSR* sensor);
	void detectRemoved(ByyDetectObject* obj,ByySensorSR* sensor);
	void powerSwitch(ByyRemoteEntity* ent,ByySensorSR* sensor,QString device,bool power);
};

#endif // BYYSENSORREPOSITORY_H
