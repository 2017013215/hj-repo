#ifndef XVITEM_H
#define XVITEM_H
#include "XGISCanvasDef.h"
#include <QObject>
#include <QMap>

class QgsGeometry;
namespace XGIS
{
	class XGISVLayer;
	class XVItem : public QObject
	{
		Q_OBJECT
		friend class XVItemManager;
	public:
		enum EType
		{
			E_None = 0,		// 无效
			E_Point = 1,	// 点
			E_Line = 2,		// 线
			E_Polygon = 3,	// 多边形
		};
	public:
		XVItem(EType _eType);
		virtual ~XVItem();

		const EType GetType() const;

		const quint64 GetID() const;

		virtual const bool IsValid() const = 0;

		virtual QgsGeometry* GetGeom() const = 0;

		const bool TouchLayer(XGISVLayer* _pLayer);

		const bool LeaveLayer(XGISVLayer* _pLayer);

		void UpdateLayerGeometry();

	private:
		EType m_eType;
		quint64 m_unID;

		QMap<XGISVLayer*,quint64> m_pBindLayers;
	};
}

#endif // XVITEM_H
