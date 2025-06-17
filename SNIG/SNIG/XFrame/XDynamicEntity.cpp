#include "stdafx.h"
#include "XDynamicEntity.h"


XDynamicEntity::XDynamicEntity(void)
	:m_bIsInit(false)
	,m_unStepIdx(0)
	,m_strName(QString::null)
	,m_xCurPos()
{
	m_xActionPoints.clear();

}


XDynamicEntity::~XDynamicEntity(void)
{
	m_xActionPoints.clear();
	m_bIsInit = false;
	m_unStepIdx = 0;
	m_strName.clear();
	m_xCurPos = XCoordinateGeo();
}

const int XDynamicEntity::InitEntity(quint32 _unMarkLibID, quint32 _unMarkItemID, const QString& _strName)
{
	return 0;
}

const bool XDynamicEntity::SetActionPoints(const QList<XCoordinateGeo>& _xActionPoints)
{

	return true;
}

const bool XDynamicEntity::Step()
{
	if(m_unStepIdx >= m_xActionPoints.count())
	{
		return false; // 已经推进结束
	}

	m_xCurPos = m_xActionPoints[m_unStepIdx];
	return true;
}

const XCoordinateGeo& XDynamicEntity::GetPos() const
{
	return m_xCurPos;
}
