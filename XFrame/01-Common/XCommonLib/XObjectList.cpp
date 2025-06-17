#include "XObjectList.h"

namespace XSpace_Common
{
	xIMPLEMENT_SERIAL(XObjectList, XObject, XCOMLIB_VERSION_1)

	XObjectList::XObjectList(void)
	{
	}


	XObjectList::~XObjectList(void)
	{
		// 清除内存占用
		ResetList();
	}

	const bool XObjectList::AddObject( XObject* _pObject )
	{
		if (_pObject == NULL)
			return false;

		if (m_xObjectList.contains(_pObject))
			return false;

		m_qObjListLock.lock();
		m_xObjectList.push_back(_pObject);
		m_qObjListLock.unlock();

		return true;
	}

	const bool XObjectList::RemoveObject(XObject* _pObject)
	{
		if (_pObject == NULL)
			return false;

		m_qObjListLock.lock();
		if( ! m_xObjectList.contains(_pObject) )
		{
			m_qObjListLock.unlock();
			return true;
		}
		
		m_xObjectList.removeOne(_pObject);
		m_qObjListLock.unlock();

		delete _pObject;
		_pObject = NULL;

		return true;
	}

	int XObjectList::GetObjectCount()
	{
		int nObjCount = 0;

		m_qObjListLock.lock();
		nObjCount = m_xObjectList.count();
		m_qObjListLock.unlock();

		return nObjCount;
	}

	XObject* XObjectList::GetObjectByIndex( int _nObjIndex )
	{
		XObject* pRetObj = NULL;

		m_qObjListLock.lock();
		if( _nObjIndex >= 0 && _nObjIndex < m_xObjectList.count() )
			pRetObj = m_xObjectList.at(_nObjIndex);
		m_qObjListLock.unlock();

		return pRetObj;
	}

	const bool XObjectList::RemoveObjectByIndex(int _nObjIndex)
	{
		m_qObjListLock.lock();
		if( _nObjIndex < 0 || _nObjIndex >= m_xObjectList.count() )
		{
			m_qObjListLock.unlock();
			return true;
		}
		
		XObject* pRemoveObj = m_xObjectList.at(_nObjIndex);
		if( pRemoveObj )
			delete pRemoveObj;
		pRemoveObj = NULL;

		m_xObjectList.removeAt(_nObjIndex);

		m_qObjListLock.unlock();

		return true;
	}

	bool XObjectList::IsContainObject( XObject* _pObject )
	{
		bool bRet = false;

		if( _pObject == NULL )
			return bRet;

		m_qObjListLock.lock();
		bRet = m_xObjectList.contains(_pObject);
		m_qObjListLock.unlock();

		return bRet;
	}

	void XObjectList::ResetList()
	{
		m_qObjListLock.lock();

		QList<XObject*>::iterator itr = m_xObjectList.begin();
		while(itr != m_xObjectList.end())
		{
			XObject* pObj = *itr;
			if (pObj)
				delete pObj;

			itr = m_xObjectList.erase(itr);
			pObj = NULL;
		}

		m_qObjListLock.unlock();
	}

	void XObjectList::Serialize( XArchive& _xArchive )
	{
		if( !IsSerialize() )
			return;
		int nClassVersion = 0xFFFF;
		if( _xArchive.IsStoring() )
		{
			nClassVersion = GetRuntimeClass()->GetClassVersion();
			if( nClassVersion >= XCOMLIB_VERSION_1 )
			{
				XObject::Serialize(_xArchive);
				m_qObjListLock.lock();
				int nObjectCount = m_xObjectList.count();
				_xArchive<<nObjectCount;
				for (int nIndex=0; nIndex<nObjectCount; nIndex++)
				{
					XObject* pObject = m_xObjectList.at(nIndex);
					if( pObject )
						_xArchive<<pObject;
				}
				m_qObjListLock.unlock();
			}
		}else if( _xArchive.IsLoading() )
		{
			nClassVersion = _xArchive.GetObjectVersion();
			if (nClassVersion >= XCOMLIB_VERSION_1)
			{
				XObject::Serialize(_xArchive);
				m_qObjListLock.lock();
				int nObjectCount = 0;
				_xArchive>>nObjectCount;
				for (int nIndex=0; nIndex<nObjectCount; nIndex++)
				{
					XObject* pObject = NULL;
					_xArchive>>pObject;
					m_xObjectList.push_back(pObject);
				}
				m_qObjListLock.unlock();
			}
		}
	}

}