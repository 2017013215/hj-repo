/***************************************************************************
XJBRubber.cpp - Rubberband widget for drawing multilines and polygons
--------------------------------------
Date                 : 07-Jan-2006
Copyright            : (C) 2006 by Tom Elwertowski
Email                : telwertowski at users dot sourceforge dot net
***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include "XDrawRubber.h"
#include "qgsmapcanvas.h"
#include <QPainter>
#include "XGISCanvas.h"
#include "GIS-Extend\XShapeDef.h"
#include "XJBDrawHelper.h"
#include "QTimer"
#include "XShapeDoc.h"

namespace XGIS
{
	XDrawRubber::XDrawRubber( XGISCanvas* _pMapCanvas)
		: QgsMapCanvasItem( _pMapCanvas )
		, m_pUpTimer(nullptr)
	{
		Q_ASSERT(_pMapCanvas);
		m_pCanvas = _pMapCanvas;
		m_pxShapDoc = new XShapeDoc();
		updateRect();
	}

	XDrawRubber::~XDrawRubber()
	{
		ClearAllShape();
		if(nullptr != m_pxShapDoc)
		{
			delete m_pxShapDoc;
			m_pxShapDoc = nullptr;
		}
		m_pCanvas = nullptr;
	}

	void XDrawRubber::paint( QPainter* p )
	{
// 		p->save();
// 
// 		QRectF rct = rect().toRectF();
// 		QPointF ptLeftTop;
// 		QPointF ptRightBottom;
// 		m_pCanvas->GetScreenPosByGeo(rct.topLeft(),ptLeftTop);
// 		m_pCanvas->GetScreenPosByGeo(rct.bottomRight(),ptRightBottom);
// 
// 		p->setPen(QPen(Qt::red,5,Qt::DashLine));
// 		p->drawRect(QRectF(ptLeftTop,ptRightBottom));
// 		p->restore();
		p->save();

		QMap<quint32,XShape*> shapes = m_pxShapDoc->GetShapes();
		foreach(XShape* pShape, shapes)
		{
			if(nullptr == pShape || false == pShape->GetVisible())
			{
				continue;
			}

			XShape::EShapeType eType = pShape->GetType();
			switch(eType)
			{
			case XShape::eType_PointJB:
				{
					XShapePointJB* pPntJB = (XShapePointJB*)pShape;
					if(nullptr != pPntJB && pPntJB->IsValid())
					{

						QPointF qScreenPnt;
						bool bRes = m_pCanvas->GetScreenPosByGeo(pPntJB->GetPos(),qScreenPnt);
						// 绘制军标
						if(true == bRes)
						{
							XJBDrawHelper::DrawPointJB(p,pPntJB,qScreenPnt);
						}
						pPntJB = nullptr;
					}
				}break;
			case XShape::eType_LineJB:
				{
					XShapeLineJB* pLineJB = (XShapeLineJB*)pShape;
					if(nullptr != pLineJB && pLineJB->IsValid())
					{
						QList<QPointF> scrPnts;
						bool bRes = false;
						QPointF ptTemp = QPointF();
						QPolygonF lineGeoPnts = pLineJB->GetLinePnts();
						for (int nIdx=0;nIdx<lineGeoPnts.count(); ++nIdx)
						{
							bRes = m_pCanvas->GetScreenPosByGeo(lineGeoPnts[nIdx],ptTemp);
							if(false == bRes)
								break;
							scrPnts.append(ptTemp);
						}

						XJBDrawHelper::DrawLineJB(p,pLineJB,scrPnts);

						pLineJB = nullptr;
					}
				}break;
			case XShape::eType_Point:
				{
					XPointShape* pPntShape = (XPointShape*)pShape;
					if(nullptr != pPntShape && pPntShape->IsValid())
					{
						QPointF scrPnt;
						bool bRes = m_pCanvas->GetScreenPosByGeo(pPntShape->GetPos(),scrPnt);
						if(true == bRes)
						{
							// 设置画笔颜色
							p->save();

							QPen pen(pPntShape->GetPenClr());
							pen.setWidthF(pPntShape->GetPenWidth());
							pen.setCapStyle(pPntShape->GetPenCapStyle());
							p->setPen(pen);
							p->drawPoint(scrPnt);

							p->restore();
						}
						pPntShape = nullptr;
					}
				}break;
			case XShape::eType_Line:
				{
					XLineShape* pLineShape = (XLineShape*)pShape;
					if(nullptr != pLineShape && pLineShape->IsValid())
					{
						QVector<QPointF> scrPnts;
						bool bRes = false;
						QPointF ptTemp = QPointF();
						QPolygonF lineGeoPnts = pLineShape->GetLinePos();
						QPainterPath path;
						for (int nIdx=0;nIdx<lineGeoPnts.count(); ++nIdx)
						{
							bRes = m_pCanvas->GetScreenPosByGeo(lineGeoPnts[nIdx],ptTemp);
							if(false == bRes)
								break;
							if(nIdx > 0)
								path.lineTo(ptTemp);
							else
								path.moveTo(ptTemp);
						}

						// 设置画笔颜色
						p->save();

						QPen pen(pLineShape->GetPenClr());
						pen.setWidthF(pLineShape->GetPenWidth());
						pen.setCapStyle(pLineShape->GetPenCapStyle());
						pen.setJoinStyle(pLineShape->GetPenJoinStyle());
						pen.setStyle(pLineShape->GetPenStyle());

						p->setPen(pen);

						p->drawPath(path);

						p->restore();

						pLineShape = nullptr;
					}
				}break;
			case XShape::eType_Polygon:
				{
					XPolygonShape* pPolyShape = (XPolygonShape*)pShape;
					if(nullptr != pPolyShape )
					{
						XPolygonShape::EPolygonType ePolyType = pPolyShape->GetPolygonType();

						bool bRes = false;
						QPointF ptTemp = QPointF();
						QPainterPath path;
						QVector<XCoordinateGeo> lineGeoPnts = pPolyShape->GetLinePnts();
						if(true == lineGeoPnts.isEmpty())
						{
							pPolyShape = nullptr;
							break;
						}
						if(lineGeoPnts.count() < pPolyShape->GetMinPnts())
						{
							for(int nIdx =0; nIdx < lineGeoPnts.count(); ++nIdx)
							{
								bRes = m_pCanvas->GetScreenPosByGeo(QPointF(lineGeoPnts[nIdx].m_dLontitude,lineGeoPnts[nIdx].m_dLatitude),ptTemp);
								if(false == bRes)
									break;
								if(nIdx > 0)
									path.lineTo(ptTemp);
								else
									path.moveTo(ptTemp);
							}
						}else
						{
							switch(ePolyType)
							{
							case XPolygonShape::ePolygon_Polygon:
								{
									QPolygonF polygons;
									for(int nIdx =0; nIdx < lineGeoPnts.count(); ++nIdx)
									{
										bRes = m_pCanvas->GetScreenPosByGeo(QPointF(lineGeoPnts[nIdx].m_dLontitude,lineGeoPnts[nIdx].m_dLatitude),ptTemp);
										if(false == bRes)
											break;
										polygons.append(ptTemp);
									}
									polygons.append(polygons.first());
									path.addPolygon(polygons);
								}break;
							case XPolygonShape::ePolygon_Rect:
								{
									if(lineGeoPnts.count() == 2)
									{
										QPolygonF polygons;
										for(int nIdx =0; nIdx < lineGeoPnts.count(); ++nIdx)
										{
											bRes = m_pCanvas->GetScreenPosByGeo(QPointF(lineGeoPnts[nIdx].m_dLontitude,lineGeoPnts[nIdx].m_dLatitude),ptTemp);
											if(false == bRes)
												break;
											polygons.append(ptTemp);
										}
										QRectF rct( polygons.first(), polygons.last());
										path.addRect(rct);
									}
								}break;
							case XPolygonShape::ePolygon_Circle:
								{
									if(lineGeoPnts.count() == 2)
									{
										QPolygonF polygons;
										for(int nIdx =0; nIdx < lineGeoPnts.count(); ++nIdx)
										{
											bRes = m_pCanvas->GetScreenPosByGeo(QPointF(lineGeoPnts[nIdx].m_dLontitude,lineGeoPnts[nIdx].m_dLatitude),ptTemp);
											if(false == bRes)
												break;
											polygons.append(ptTemp);
										}
										double dRadius = sfnGetPntDis(polygons.first(),polygons.last());
										path.addEllipse(polygons.first(),dRadius,dRadius);
									}
								}break;
							case XPolygonShape::ePolygon_Ellipses:
								{
									if(lineGeoPnts.count() == 2)
									{
										QPolygonF polygons;
										for(int nIdx =0; nIdx < lineGeoPnts.count(); ++nIdx)
										{
											bRes = m_pCanvas->GetScreenPosByGeo(QPointF(lineGeoPnts[nIdx].m_dLontitude,lineGeoPnts[nIdx].m_dLatitude),ptTemp);
											if(false == bRes)
												break;
											polygons.append(ptTemp);
										}
										QRectF rct( polygons.first(), polygons.last());
										path.addEllipse(rct);
									}
								}break;
							default:
								break;
							}
						}

						if(true == path.isEmpty())
							break;

						// 设置画笔颜色
						p->save();

						QPen pen(pPolyShape->GetPenClr());
						pen.setWidthF(pPolyShape->GetPenWidth());
						pen.setCapStyle(pPolyShape->GetPenCapStyle());
						pen.setJoinStyle(pPolyShape->GetPenJoinStyle());
						pen.setStyle(pPolyShape->GetPenStyle());

						QBrush brush;
						brush.setColor(pPolyShape->GetFillClr());
						brush.setStyle(pPolyShape->GetFillStyle());
						if(Qt::TexturePattern == pPolyShape->GetFillStyle())
						{
							QImage qImg = pPolyShape->GetFillImg();
							if(false == qImg.isNull())
							{
								brush.setTextureImage(qImg);
							}
						}

						p->setPen(pen);
						p->setBrush(brush);
						p->drawPath(path);

						p->restore();

						pPolyShape = nullptr;
					}
				}break;
			default:
				break;
			}
		}

		p->restore();
	}

	void XDrawRubber::updateRect()
	{
		if(nullptr == m_pCanvas)
			return;

		QgsRectangle canvasRect = mMapCanvas->extent();
		setRect(canvasRect);
	}

	void XDrawRubber::updatePosition()
	{
		updateRect();
		update();
	}

	const quint32 XDrawRubber::AddShape(const XShape* _pxShape)
	{
		quint32 unID =  m_pxShapDoc->AddShape(_pxShape);

		update();
		return unID;
	}

	const XShape* XDrawRubber::GetShapeByID(const quint32 _unID) const
	{
		return m_pxShapDoc->GetShapeByID(_unID);
	}

	void XDrawRubber::ClearAllShape()
	{
		m_pxShapDoc->ClearAllShape();
		update();
	}

	const bool XDrawRubber::RemoveShapeByID(const quint32 _unID)
	{
		return m_pxShapDoc->RemoveShapeByID(_unID);
	}

	const bool XDrawRubber::StartAutoUpdate(const quint32 _unMSec)
	{
		if(_unMSec <= 0)
			return false;

		if(nullptr == m_pUpTimer)
		{
			m_pUpTimer = new QTimer(this);
			QObject::connect(m_pUpTimer, SIGNAL(timeout()), this, SLOT(UpdateRubber()));
		}

		m_pUpTimer->start(_unMSec);
		return true;
	}

	void XDrawRubber::StopAutoUpdate()
	{
		if(nullptr == m_pUpTimer)
			return ;

		m_pUpTimer->stop();
	}

	void XDrawRubber::UpdateRubber()
	{
		updateRect();
		update();
	}

	const bool XDrawRubber::SaveStateFile(const QString& _strFilePath)
	{
		if( 0 == m_pxShapDoc->SaveDocument(_strFilePath))
			return true;
		return false;
	}

	const bool XDrawRubber::OpenStateFile(const QString& _strFilePath)
	{
		if(0 == m_pxShapDoc->OpenDocument(_strFilePath))
		{
			UpdateRubber();
			return true;
		}

		return false;
	}

}