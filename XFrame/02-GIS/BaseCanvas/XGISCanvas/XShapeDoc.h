#pragma once

#include <XDocument.h>
#include "XGISCanvasDef.h"

using namespace XSpace_Common;
namespace XGIS
{
	class XShape;
	class XShapeDoc : public XDocument
	{
		xDECLARE_SERIAL_DLL(XShapeDoc)
	public:
		XShapeDoc(void);
		~XShapeDoc(void);

		// 重载文件序列化函数
		virtual void Serialize(XArchive& _xArchive);

		// 获取Shapes列表
		const QMap<quint32,XShape*>& GetShapes();

		// 添加XShape
		const quint32 AddShape(const XShape* _pxShapePtr);

		// 根据ID获取Shape
		const XShape* GetShapeByID(const quint32 _unID) const;

		// 删除Shape
		const bool RemoveShapeByID(quint32 _unID);

		// 删除所有Shape对象
		void ClearAllShape();

	private:
		QMap<quint32,XShape*> m_pQuickShapes;
		static quint32 m_unShapeIdx;
	};
}