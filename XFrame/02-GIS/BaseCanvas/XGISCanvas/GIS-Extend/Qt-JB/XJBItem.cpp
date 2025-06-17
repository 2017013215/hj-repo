#include "XJBItem.h"
#include "JBDef.h"
#include "XJBInfoManager.h"


namespace XGIS
{
	XJBItem::XJBItem(quint32 _unLibID, quint32 _unJBID)
		: m_cCenterPnt(QPointF(COOR_FAILD_VAL,COOR_FAILD_VAL))
		, m_strJBName(QString::null)
		, m_pJBDrawInfo(nullptr)
		, m_cSize(QSize(30,30))
		, m_dAngle(0)
		, m_pExceptiveInfo(nullptr)
	{
		Q_JBItem* pDrawItem = XJBInfoManager::GetInstance()->CloneItem(_unLibID,_unJBID);
		if(nullptr == pDrawItem)
		{
			Q_ASSERT(false);
			return;
		}

		m_pJBDrawInfo = pDrawItem;
		m_strJBName = pDrawItem->strName;
		
		m_pExceptiveInfo = XJBInfoManager::GetInstance()->GetItemExceptiveInfo(_unLibID,_unJBID);

		pDrawItem = nullptr;
	}

	XJBItem::~XJBItem(void)
	{
		if(nullptr != m_pJBDrawInfo)
		{
			delete m_pJBDrawInfo;
			m_pJBDrawInfo = nullptr;
		}
		m_pExceptiveInfo = nullptr;
	}

	void XJBItem::SetSize(int _nW,int _nH)
	{
		if(_nW < 10 || _nH < 10)
		{
			return; 
		}

		m_cSize = QSize(_nW,_nH);
	}

	void XJBItem::SetCenterPnt(QPointF _cPnt)
	{
		m_cCenterPnt = _cPnt;
	}

	void XJBItem::AddPoint(QPointF _cGeoPos)
	{
		m_cUserPnts.append(_cGeoPos);
	}

	void XJBItem::SetAngle(double _dAngle)
	{
		if(_dAngle > 360 || m_dAngle < 0)
		{
			m_dAngle = 0;
		}

		m_dAngle = _dAngle;
	}

	const quint32 XJBItem::GetPtCount() const
	{
		return m_cUserPnts.count();
	}

	ExceptiveInfo* XJBItem::GetExceptiveInfo() const
	{
		return m_pExceptiveInfo;
	}

}