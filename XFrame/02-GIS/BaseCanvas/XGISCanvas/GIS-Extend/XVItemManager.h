#pragma once

#include <QtCore/QMutex>
#include <QtCore/QMap>
#include "XVItem.h"


namespace XGIS
{
	class XVItemManager
	{
	private:
		XVItemManager(void);
		~XVItemManager(void);

	public:
		static XVItemManager* GetInstance();

		XVItem* CreateVItem(XVItem::EType _eItemType);

		const XVItem* FindVItem(quint64 _unID);

		void ClearAllItem();

	private:
		static XVItemManager ms_Instance;
		quint64 m_unItemIndex;				// 下标从1000000开始
		QMutex m_cSynLock;

		QMap<quint64,XVItem*> m_cItemMap;
	};
}