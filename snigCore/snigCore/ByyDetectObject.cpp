#include "ByyDetectObject.h"

ByyDetectObject::ByyDetectObject()
	: ByyObject()
{

}

ByyDetectObject::~ByyDetectObject()
{

}

void ByyDetectObject::setUpdateTime( double t )
{
	if (m_UpdateTime!=t)
	{
		m_UpdateTime=t;

		emit updateTimeChanged(m_UpdateTime);
	}
}

double ByyDetectObject::updateTime()
{
	return m_UpdateTime;
}

void ByyDetectObject::setContinueTime(double t)
{
	m_ContinueTime = t;
}

double ByyDetectObject::getContinueTime()
{
	return m_ContinueTime;
}

void ByyDetectObject::setBatchNo( long val )
{
	if (m_BatchNo!=val)
	{
		m_BatchNo=val;

		emit batchNoChanged(m_BatchNo);
	}
}

long ByyDetectObject::batchNo()
{
	return m_BatchNo;
}

void ByyDetectObject::setTargetID( long id )
{
	setId(id);
}

long ByyDetectObject::targetID()
{
	return id();
}

void ByyDetectObject::setTargetName( const QString& name )
{
	setName(name);
}

QString ByyDetectObject::targetName()
{
	return name();
}

void ByyDetectObject::setCID( long cid )
{
	if (m_CID!=cid)
	{
		m_CID=cid;

		emit CIDChanged(m_CID);
	}
}

long ByyDetectObject::getCID()
{
	return m_CID;
}

void ByyDetectObject::setEntityType( const ByyEntityType& et )
{
	if (m_EntityType!=et)
	{
		m_EntityType=et;

		emit  entityTypeChanged(m_EntityType);
	}
}

const ByyEntityType& ByyDetectObject::entityType()
{
	return m_EntityType;
}

void ByyDetectObject::setThreatLevel( long level )
{
	if (m_ThreatLevel!=level)
	{
		m_ThreatLevel=level;

		emit threatLevelChanged(m_ThreatLevel);
	}
}

long ByyDetectObject::threatLevel()
{
	return m_ThreatLevel;
}

void ByyDetectObject::setPosition( const osg::Vec3d& pos )
{
	if (m_Pos!=pos)
	{
		m_Pos=pos;

		emit posChanged(m_Pos);
	}
}

const osg::Vec3d& ByyDetectObject::position()
{
	return m_Pos;
}

void ByyDetectObject::setVelocity( double val )
{
	if (m_Velocity!=val)
	{
		m_Velocity=val;

		emit velocityChanged(m_Velocity);
	}
}

double ByyDetectObject::velocity()
{
	return m_Velocity;
}

void ByyDetectObject::setCourse( double val )
{
	if (m_Course!=val)
	{
		m_Course=val;

		emit courseChanged(m_Course);
	}
}

double ByyDetectObject::course()
{
	return m_Course;
}

void ByyDetectObject::setDistance( double val )
{
	if (m_Distance!=val)
	{
		m_Distance=val;

		emit distanceChanged(m_Distance);
	}
}

double ByyDetectObject::distance()
{
	return m_Distance;
}

void ByyDetectObject::setEl( double val )
{
	if (m_El!=val)
	{
		m_El=val;

		emit elChanged(m_El);
	}
}

double ByyDetectObject::El()
{
	return m_El;
}

void ByyDetectObject::setAz( double val )
{
	if (m_Az!=val)
	{
		m_Az=val;

		emit azChanged(m_Az);
	}
}

double ByyDetectObject::Az()
{
	return m_Az;
}

void ByyDetectObject::setRadiationType( long val )
{
	if (m_RadiationType!=val)
	{
		m_RadiationType=val;

		emit radiationTypeChanged(m_RadiationType);
	}
}

long ByyDetectObject::radiationType()
{
	return m_RadiationType;
}

void ByyDetectObject::setRadiationPt( double val )
{
	if (m_RadiationPt!=val)
	{
		m_RadiationPt=val;

		emit radiationPtChanged(m_RadiationPt);
	}
}

double ByyDetectObject::radiationPt()
{
	return m_RadiationPt;
}

void ByyDetectObject::setRadiationFreq( double val )
{
	if (m_RadiationFreq!=val)
	{
		m_RadiationFreq=val;

		emit radiationFreqChanged(m_RadiationFreq);
	}
}

double ByyDetectObject::radiationFreq()
{
	return m_RadiationFreq;
}

void ByyDetectObject::setRadiationTu( double val )
{
	if (m_RadiationTu!=val)
	{
		m_RadiationTu=val;

		emit radiationTuChanged(m_RadiationTu);
	}
}

double ByyDetectObject::radiationTu()
{
	return m_RadiationTu;
}

void ByyDetectObject::setRadiationPRF( double val )
{
	if (m_RadiationPRF!=val)
	{
		m_RadiationPRF=val;

		emit radiationPRFChanged(m_RadiationPRF);
	}
}

double ByyDetectObject::radiationPRF()
{
	return m_RadiationPRF;
}

void ByyDetectObject::setRadiationSignalType( double val )
{
	if (m_RadiationSignalType!=val)
	{
		m_RadiationSignalType=val;

		emit radiationSignalTypeChanged(m_RadiationSignalType);
	}
}

double ByyDetectObject::radiationSignalType()
{
	return m_RadiationSignalType;
}

void ByyDetectObject::setRadiationPulseType( double val )
{
	if (m_RadiationPulseType!=val)
	{
		m_RadiationPulseType=val;

		emit radiationPulseTypeChanged(m_RadiationPulseType);
	}
}

double ByyDetectObject::radiationPulseType()
{
	return m_RadiationPulseType;
}

void ByyDetectObject::setDeviceName( const QString& device )
{
	m_DeviceName=device;
}

QString ByyDetectObject::deviceName()
{
	return m_DeviceName;
}
