#include "XVItemManager.h"
#include "XVItemPoint.h"
#include "XVItemLine.h"
#include "XVItemPolygon.h"


namespace XGIS
{
	XVItemManager XVItemManager::ms_Instance;
	XVItemManager::XVItemManager(void)
		:m_unItemIndex(1000000)
	{
		m_cItemMap.clear();
	}

	XVItemManager::~XVItemManager(void)
	{
		// Çå³ýÄÚ´æ
	}

	XVItemManager* XVItemManager::GetInstance()
	{
		return &ms_Instance;
	}

	XVItem* XVItemManager::CreateVItem(XVItem::EType _eItemType)
	{
		XVItem* pRet = nullptr;

		switch (_eItemType)
		{
		case XVItem::E_Point:
			pRet = new XVItemPoint();
			m_cSynLock.lock();
			pRet->m_unID = m_unItemIndex++;
			m_cItemMap[pRet->m_unID] = pRet;
			m_cSynLock.unlock();
			break;
		case XVItem::E_Line:
			pRet = new XVItemLine();
			m_cSynLock.lock();
			pRet->m_unID = m_unItemIndex++;
			m_cItemMap[pRet->m_unID] = pRet;
			m_cSynLock.unlock();
			break;
		case XVItem::E_Polygon:
			pRet = new XVItemPolygon();
			m_cSynLock.lock();
			pRet->m_unID = m_unItemIndex++;
			m_cItemMap[pRet->m_unID] = pRet;
			m_cSynLock.unlock();
			break;
		default:
			break;
		}

		return pRet;
	}

	const XVItem* XVItemManager::FindVItem(quint64 _unID)
	{
		XVItem* pRet = nullptr;
		m_cSynLock.lock();
		if(false == m_cItemMap.contains(_unID))
		{
			m_cSynLock.unlock();
			return nullptr;
		}

		pRet = m_cItemMap[_unID];

		m_cSynLock.unlock();
		return pRet;
	}

	void XVItemManager::ClearAllItem()
	{
		m_cSynLock.lock();

		QMap<quint64,XVItem*>::iterator itr = m_cItemMap.begin();
		while(itr != m_cItemMap.end())
		{
			delete itr.value();
			++itr;
		}
		m_cItemMap.clear();
		m_cSynLock.unlock();
	}
}