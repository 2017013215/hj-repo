#include "XVItemRenderV2.h"
#include "XVItemManager.h"
#include "XVItemPoint.h"

#include <QPainter>
#include "XVItemLine.h"
#include "XVItemPolygon.h"

namespace XGIS
{
	XVItemRenderV2::XVItemRenderV2()
	: QgsFeatureRendererV2("VItemRender")
	{
		
	}

	XVItemRenderV2::~XVItemRenderV2()
	{

	}

	void XVItemRenderV2::startRender(QgsRenderContext& context, const QgsFields& fields)
	{
		int kk = 0;
	}

	void XVItemRenderV2::stopRender(QgsRenderContext& context)
	{
		int kk = 0;
	}

	QList<QString> XVItemRenderV2::usedAttributes()
	{
		return QList<QString>();
	}

	QgsFeatureRendererV2* XVItemRenderV2::clone() const
	{
		return new XVItemRenderV2();
	}

	bool XVItemRenderV2::renderFeature(QgsFeature& feature, QgsRenderContext& context, int layer /*= -1*/, bool selected /*= false*/, bool drawVertexMarker /*= false */)
	{
		if(false == feature.isValid())
		{
			return false;
		}

		quint64 unID = feature.attribute(0).toUInt();
		const XVItem* pItem = XVItemManager::GetInstance()->FindVItem(unID);
		if(nullptr == pItem)
		{
			return false;
		}

		XVItem::EType eItemType = pItem->GetType();
		QPainter* pPtr = context.painter();
		if(nullptr == pPtr)
		{
			return false;
		}
		QPen oldPen = pPtr->pen();
		QBrush oldBru = pPtr->brush();

		switch (eItemType)
		{
		case XVItem::E_Point:	// 点
			{
				const XVItemPoint* pPnt = dynamic_cast<const XVItemPoint*>(pItem);
				if(nullptr == pPnt)
				{
					Q_ASSERT(false);
					break;
				}

				QgsPoint pos = pPnt->GetPos();
				// 绘制点
				pPtr->setPen(QPen(pPnt->GetOutLineClr(),pPnt->GetOutLineWidth()));
				pPtr->setBrush(QBrush(pPnt->GetFillClr()));
				QgsPoint qgsPnt = context.mapToPixel().transform(pos);
				pPtr->drawEllipse(qgsPnt.toQPointF(),pPnt->GetSize(),pPnt->GetSize());
				pPtr->setPen(oldPen);
			}
			break;
		case XVItem::E_Line:	// 线
			{
				const XVItemLine* pLine = dynamic_cast<const XVItemLine*>(pItem);
				if(nullptr == pLine)
				{
					Q_ASSERT(false);
					break;
				}
				QgsPolyline lineData = pLine->GetLineData();
				int nCount = lineData.count();
				if(nCount < 2)
				{
					break;
				}

				pPtr->setPen(QPen(pLine->GetLineClr(),pLine->GetLineWidth(),pLine->GetLineStyle()));
				QPointF firstPt = context.mapToPixel().transform(lineData.first()).toQPointF();
				for (int nIdx=1;nIdx<nCount;nIdx++)
				{
					QPointF curPt = context.mapToPixel().transform(lineData[nIdx]).toQPointF();
					pPtr->drawLine(firstPt,curPt);
					firstPt = curPt;
 				}
			}break;
		case XVItem::E_Polygon:	// 多边形
			{
				const XVItemPolygon* pPolygon = dynamic_cast<const XVItemPolygon*>(pItem);
				if(nullptr == pPolygon)
				{
					Q_ASSERT(false);
					break;
				}
				QgsPolyline polyData = pPolygon->GetPolygonData();
				int nCount = polyData.count();
				if(nCount < 3)
				{
					break;
				}

				pPtr->setPen(QPen(pPolygon->GetLineClr(),pPolygon->GetLineWidth(),pPolygon->GetLineStyle()));
				pPtr->setBrush(QBrush(pPolygon->GetFillClr()));
				QPointF firstPt = context.mapToPixel().transform(polyData.first()).toQPointF();
				QPolygonF drawPolygonInfo;
				foreach(QgsPoint pnt, polyData)
				{
					drawPolygonInfo.append(context.mapToPixel().transform(pnt).toQPointF());
				}
				pPtr->drawPolygon(drawPolygonInfo);
			}break;
		default:
			break;
		}

		pPtr->setPen(oldPen);
		pPtr->setBrush(oldBru);
		
		pPtr = nullptr;
		return true;
	}

}