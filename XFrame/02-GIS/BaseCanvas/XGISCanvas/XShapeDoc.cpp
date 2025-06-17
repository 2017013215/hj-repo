#include "XShapeDoc.h"
#include "GIS-Extend\XShapeDef.h"

namespace XGIS
{
	quint32 XShapeDoc::m_unShapeIdx=0;

	xIMPLEMENT_SERIAL_VIRTURL(XShapeDoc,XDocument,XVER_1_0)
	XShapeDoc::XShapeDoc(void)
		:XDocument()
	{
		m_pQuickShapes.clear();
	}

	XShapeDoc::~XShapeDoc(void)
	{
		// 清除内存
		ClearAllShape();
	}

	void XShapeDoc::Serialize(XArchive& _xArchive)
	{
		if (!IsSerialize())
			return;

		int nClassVersion = 0xFFFF;
		if (_xArchive.IsStoring())
		{
			nClassVersion = GetRuntimeClass()->GetClassVersion();
			if (nClassVersion >= XVER_1_0)
			{
				// 写入
				_xArchive<<m_pQuickShapes.count();
				foreach(XShape* pShape, m_pQuickShapes)
				{
					_xArchive.WriteObject(pShape);
				}
			}
		}
		else if (_xArchive.IsLoading())
		{
			nClassVersion = _xArchive.GetObjectVersion();
			if (nClassVersion >= XVER_1_0)
			{
				// 读取
				ClearAllShape();
				int nShapeCount = 0;
				_xArchive>>nShapeCount;
				for (int nIdx=0;nIdx<nShapeCount; ++nIdx)
				{
					XShape* pShape = (XShape*)_xArchive.ReadObject();
					Q_ASSERT(pShape);
					if(nullptr != pShape)
					{
						m_pQuickShapes.insert(pShape->GetID(),pShape);
						pShape = nullptr;
					}
				}
				m_unShapeIdx = nShapeCount;
			}
		}
	}

	const quint32 XShapeDoc::AddShape(const XShape* _pxShapePtr)
	{
		if(nullptr == _pxShapePtr)
		{
			return 0;
		}

		((XShape*)_pxShapePtr)->m_unID = ++m_unShapeIdx;
		m_pQuickShapes[_pxShapePtr->GetID()] = ((XShape*)_pxShapePtr);

		return m_unShapeIdx;
	}

	const bool XShapeDoc::RemoveShapeByID(quint32 _unID)
	{
		bool rel=false;
		if(true == m_pQuickShapes.contains(_unID))
		{
			XShape* pShape = m_pQuickShapes[_unID];
			m_pQuickShapes.remove(_unID);
			delete pShape;
			pShape = nullptr;
			rel=true;
		}

		return rel;
	}

	void XShapeDoc::ClearAllShape()
	{
		m_pQuickShapes.clear();
		foreach(XShape* pShape,m_pQuickShapes)
		{
			if(nullptr != pShape)
			{
				delete pShape;
				pShape = nullptr;
			}
		}
		m_pQuickShapes.clear();
		m_unShapeIdx = 0;
	}

	const XShape* XShapeDoc::GetShapeByID(const quint32 _unID) const
	{
		if(true == m_pQuickShapes.contains(_unID))
		{
			return m_pQuickShapes[_unID];
		}

		return nullptr;
	}

	const QMap<quint32,XShape*>& XShapeDoc::GetShapes()
	{
		return m_pQuickShapes;
	}

}