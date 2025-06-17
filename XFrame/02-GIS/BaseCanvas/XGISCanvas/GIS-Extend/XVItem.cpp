#include "XVItem.h"
#include <XGISVLayer.h>

namespace XGIS
{
	XVItem::XVItem(EType _eType)
		: QObject(nullptr)
		,m_eType(_eType)
		,m_unID(0)
	{

	}

	XVItem::~XVItem()
	{
		QMap<XGISVLayer*,quint64>::const_iterator itr = m_pBindLayers.begin();
		bool bRes = false;
		while(itr != m_pBindLayers.end())
		{
			bRes = LeaveLayer(itr.key());
			Q_ASSERT(bRes);
			++itr;
		}
		m_pBindLayers.clear();
	}

	const XVItem::EType XVItem::GetType() const
	{
		return m_eType;
	}

	const quint64 XVItem::GetID() const
	{
		return m_unID;
	}

	const bool XVItem::TouchLayer(XGISVLayer* _pLayer)
	{
		if(nullptr == _pLayer || false == _pLayer->isValid())
		{
			return false;
		}

		if(m_pBindLayers.contains(_pLayer) == true)
		{
			return false;
		}

		quint64 unFeatureID = _pLayer->AddVItem(this);
		if(0 == unFeatureID)
		{
			Q_ASSERT(false);
			return false;
		}
		m_pBindLayers[_pLayer] = unFeatureID;
		return true;
	}

	void XVItem::UpdateLayerGeometry()
	{
		if(false == IsValid())
		{
			return ;
		}

		QMap<XGISVLayer*,quint64>::const_iterator itr = m_pBindLayers.constBegin();

		while(itr != m_pBindLayers.constEnd())
		{
			QgsGeometry* pGeom = GetGeom();
			if(nullptr == pGeom)
			{
				Q_ASSERT(false);
			}

			bool bRes = itr.key()->UpdateGeometry(itr.value(),pGeom);
			if(false == bRes)
			{
				delete pGeom;
				pGeom = nullptr;
				Q_ASSERT(false);
			}
			pGeom = nullptr;
			++itr;
		}
	}

	const bool XVItem::LeaveLayer(XGISVLayer* _pLayer)
	{
		if(nullptr == _pLayer || false == _pLayer->isValid())
		{
			return false;
		}

		if(m_pBindLayers.contains(_pLayer) == false)
		{
			return false;
		}

		bool bRes = _pLayer->RemoveVItem(m_pBindLayers[_pLayer]);
		Q_ASSERT(bRes);
		return bRes;
	}

}