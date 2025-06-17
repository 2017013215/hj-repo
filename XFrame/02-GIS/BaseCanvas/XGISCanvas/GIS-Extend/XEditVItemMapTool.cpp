#include "XEditVItemMapTool.h"

#include <qgsmapcanvas.h>
#include <qgsrubberband.h>
#include <qgslogger.h>
#include <qgscursors.h>
#include <qgsmessagelog.h>

#include <XGISVLayer.h>
#include <XGISCanvas.h>
#include "XVItemPoint.h"
#include "XVItemManager.h"
#include "XVItemLine.h"
#include "XVItemPolygon.h"
#include "GIS-Extend\XShapeDef.h"

namespace XGIS
{
	XEditVItemMapTool::XEditVItemMapTool(const XGISCanvas* _pCanvas, EVItemType _eItemType)
		: QgsMapTool( (QgsMapCanvas*)_pCanvas )
		,m_eItemType(_eItemType)
		,m_pCurOpLine(nullptr)
		,m_pCurOpPolygon(nullptr)
		,m_bIsMoveAddPnt(false)
	{
		if(nullptr == _pCanvas)
		{
			Q_ASSERT(false);
			return;
		}

		m_pCanvas = (XGISCanvas*)_pCanvas;

		// 鼠标形状
		QPixmap myCrossHairQPixmap = QPixmap(( const char ** ) cross_hair_cursor );
		mCursor = QCursor( myCrossHairQPixmap, 8, 8 );
	}


	XEditVItemMapTool::~XEditVItemMapTool(void)
	{
		m_pCanvas = nullptr;
		m_pCurOpLine = nullptr;
		m_pCurOpPolygon = nullptr;
		m_bIsMoveAddPnt = false;
	}

	void XEditVItemMapTool::canvasMoveEvent(QgsMapMouseEvent* e)
	{
		switch(m_eItemType)
		{
		case EVItem_Line:
			{
				if(nullptr != m_pCurOpLine)
				{
					QgsPoint point = snapPoint(e->pos());
					if(true == m_bIsMoveAddPnt)
					{
						m_pCurOpLine->AddPnt(XCoordinateGeo(point.x(),point.y(),0));
						m_bIsMoveAddPnt = false;
					}else
					{
						m_pCurOpLine->ReplaceLast(XCoordinateGeo(point.x(),point.y(),0));
					}
					m_pCanvas->update();
				}
			}break;
		case EVItem_Polygon:
		case EVItem_Rectangle:
		case EVItem_Circle:
		case EVItem_Ellipses:
			{
				if(nullptr != m_pCurOpPolygon)
				{
					QgsPoint point = snapPoint(e->pos());
					if(true == m_bIsMoveAddPnt)
					{
						m_pCurOpPolygon->AddPnt(XCoordinateGeo(point.x(),point.y(),0));
						m_bIsMoveAddPnt = false;
					}else
					{
						m_pCurOpPolygon->ReplaceLast(XCoordinateGeo(point.x(),point.y(),0));
					}
					m_pCanvas->update();
				}
			}
			break;
		}
	}

	void XEditVItemMapTool::canvasPressEvent(QgsMapMouseEvent* e)
	{
		Q_UNUSED( e );
	}

	void XEditVItemMapTool::canvasReleaseEvent(QgsMapMouseEvent* e)
	{
		QgsPoint point = snapPoint( e->pos() );

		switch (m_eItemType)
		{
		case EVItem_Point:
			{
				if(Qt::LeftButton == e->button())
				{
					XPointShape* pPntShape = new XPointShape(XCoordinateGeo(point.x(),point.y(),0));
					pPntShape->SetPenClr(Qt::red);
					pPntShape->SetPenCapStyle(Qt::RoundCap);
					//pPntShape->SetPenWidth(25);
					quint32 unShapeID = m_pCanvas->AddShape(pPntShape);
					Q_ASSERT(unShapeID);
					m_xCurShapes.append(pPntShape);
					pPntShape = nullptr;
				}
			}
			break;
		case EVItem_Line:
			{
				if(Qt::LeftButton == e->button())
				{
					if(nullptr == m_pCurOpLine)
					{
						// 创建线
						m_pCurOpLine = new XLineShape();
						m_pCurOpLine->SetPenStyle(Qt::DashLine);
						m_pCurOpLine->SetPenClr(Qt::red);
						//m_pCurOpLine->SetPenWidth(2.0);
						m_pCurOpLine->SetAlpha(0.7);
						m_pCurOpLine->AddPnt(XCoordinateGeo(point.x(),point.y(),0));
						m_pCanvas->AddShape(m_pCurOpLine);
						m_bIsMoveAddPnt = true;
					}else
					{
						m_pCurOpLine->ReplaceLast(XCoordinateGeo(point.x(),point.y(),0));
						m_bIsMoveAddPnt = true;
					}
				}else if(Qt::RightButton == e->button())
				{
					if(nullptr != m_pCurOpLine)
					{
						m_pCurOpLine->ReplaceLast(XCoordinateGeo(point.x(),point.y(),0));
						m_pCurOpLine->SetPenStyle(Qt::SolidLine);
						m_pCurOpLine->SetPenClr(Qt::red);
						m_pCurOpLine->SetAlpha(1.0);
						m_pCanvas->update();
						m_pCurOpLine = nullptr;
						m_bIsMoveAddPnt = false;
					}else
						m_bIsMoveAddPnt = false;
				}
			}break;
		case EVItem_Polygon:
		case EVItem_Rectangle:
		case EVItem_Circle:
		case EVItem_Ellipses:
			{
				if(Qt::LeftButton == e->button())
				{
					if(nullptr == m_pCurOpPolygon)
					{
						// 创建多边形
						switch(m_eItemType)
						{
						case EVItem_Polygon:
							m_pCurOpPolygon = new XPolygonShape(XPolygonShape::ePolygon_Polygon);
							break;
						case EVItem_Rectangle:
							m_pCurOpPolygon = new XPolygonShape(XPolygonShape::ePolygon_Rect);
							break;
						case EVItem_Circle:
							m_pCurOpPolygon = new XPolygonShape(XPolygonShape::ePolygon_Circle);
							break;
						case EVItem_Ellipses:
							m_pCurOpPolygon = new XPolygonShape(XPolygonShape::ePolygon_Ellipses);
							break;
						default:
							break;
						}
						if(nullptr == m_pCurOpPolygon)
						{
							Q_ASSERT(false);
							break;
						}
						m_pCurOpPolygon->SetPenStyle(Qt::DashLine);
						m_pCurOpPolygon->SetPenClr(Qt::red);
						m_pCurOpPolygon->SetPenWidth(1.0);
						m_pCurOpPolygon->SetAlpha(0.7);
						m_pCurOpPolygon->AddPnt(XCoordinateGeo(point.x(),point.y(),0));
						m_pCanvas->AddShape(m_pCurOpPolygon);
						m_bIsMoveAddPnt = true;
					}else
					{
						m_pCurOpPolygon->ReplaceLast(XCoordinateGeo(point.x(),point.y(),0));
						if(m_pCurOpPolygon->GetMaxPnts() == m_pCurOpPolygon->GetPntCount())
						{
							// 达到最大数据点数，结束绘制
							m_pCurOpPolygon->SetPenStyle(Qt::SolidLine);
							m_pCurOpPolygon->SetAlpha(1.0);
							m_pCanvas->update();

							m_pCurOpPolygon = nullptr;
							m_bIsMoveAddPnt = false;
						}else
							m_bIsMoveAddPnt = true;
					}

				}else if(Qt::RightButton == e->button())
				{
					if(nullptr != m_pCurOpPolygon)
					{
						if(m_pCurOpPolygon->GetMinPnts() <= m_pCurOpPolygon->GetPntCount())
						{
							// 达到最大数据点数，结束绘制
							m_pCurOpPolygon->SetPenStyle(Qt::SolidLine);
							m_pCanvas->update();

							m_pCurOpPolygon = nullptr;
							m_bIsMoveAddPnt = false;
						}else
						{
							// 移除当前对象
							m_pCanvas->RemoveShapeByID(m_pCurOpPolygon->GetID());
							m_pCurOpPolygon = nullptr;
							m_bIsMoveAddPnt = false;
						}
					}else
						m_bIsMoveAddPnt = false;
				}
			}
			break;
		default:
			break;
		}
	}

	void XEditVItemMapTool::activate()
	{
		m_xCurShapes.clear();
		 QgsMapTool::activate();
	}

	void XEditVItemMapTool::deactivate()
	{
		m_xCurShapes.clear();
		QgsMapTool::deactivate();
	}

	QgsPoint XEditVItemMapTool::snapPoint(const QPoint& p)
	{
		QgsPointLocator::Match m = mCanvas->snappingUtils()->snapToMap( p );
		return m.isValid() ? m.point() : mCanvas->getCoordinateTransform()->toMapCoordinates( p );
	}
}