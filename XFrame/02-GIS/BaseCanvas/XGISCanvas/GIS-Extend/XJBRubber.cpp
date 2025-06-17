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

#include "XJBRubber.h"
#include "qgsfeature.h"
#include "qgsgeometry.h"
#include "qgslogger.h"
#include "qgsmapcanvas.h"
#include "qgsmaprenderer.h"
#include "qgsvectorlayer.h"
#include <QPainter>
#include "Qt-JB\JBDef.h"
#include "Qt-JB\xjbdrawhelper.h"
#include "XJBItem.h"
#include "plotUtils.h"

/*!
\class XJBRubber
\brief The XJBRubber class provides a transparent overlay widget
for tracking the mouse while drawing polylines or polygons.
*/

namespace XGIS
{
	XJBRubber::XJBRubber( QgsMapCanvas* mapCanvas, QGis::GeometryType geometryType )
		: QgsMapCanvasItem( mapCanvas )
		, mTranslationOffsetX( 0.0 )
		, mTranslationOffsetY( 0.0 )
		, m_pCurJBItem(nullptr)
		, m_bIsUseUserClr(true)
		, m_cUserClr(Qt::red)
		, m_curMousePos(1000000,1000000)
		, m_mouseMovePnt(QPointF(COOR_FAILD_VAL,COOR_FAILD_VAL))
		, m_pCurOpItem(nullptr)
	{
		reset( geometryType );
	}

	XJBRubber::~XJBRubber()
	{
		foreach(XJBItem* pItem,m_cCurItems)
		{
			delete pItem;
			pItem = nullptr;
		}
		m_cCurItems.clear();
	}

	/*!
	Remove all points from the shape being created.
	*/
	void XJBRubber::reset()
	{
		mPoints.clear();
		//m_cCurItems.clear();
		updateRect();
		update();
	}

	void XJBRubber::reset( bool isPolygon )
	{
		mPoints.clear();
		//m_cCurItems.clear();
		updateRect();
		update();
	}

	/*!
	Add a point to the shape being created.
	*/
	void XJBRubber::addPoint( const QgsPoint & p, bool doUpdate /* = true */ )
	{
		mPoints.append(p);

		if ( doUpdate )
		{
			setVisible( true );
			updateRect();
			update();
		}
	}

	void XJBRubber::removeLastPoint( bool doUpdate/* = true*/ )
	{
		mPoints.removeLast();
		if ( doUpdate )
		{
			setVisible( true );
			updateRect();
			update();
		}
	}


	void XJBRubber::movePoint( int index, const QgsPoint& p )
	{
		if(nullptr == m_pCurOpItem)
		{
			m_mouseMovePnt = QPointF(COOR_FAILD_VAL,COOR_FAILD_VAL);
			return ;
		}

		//m_pCurOpItem->AddPoint(p.toQPointF());
		m_mouseMovePnt = p.toQPointF();

		setVisible(true);
		updateRect();
		update();
	}

	void XJBRubber::setToCanvasRectangle( QRect rect )
	{
		if ( !mMapCanvas )
		{
			return;
		}

		const QgsMapToPixel* transform = mMapCanvas->getCoordinateTransform();
		QgsPoint ll = transform->toMapCoordinates( rect.left(), rect.bottom() );
		QgsPoint lr = transform->toMapCoordinates( rect.right(), rect.bottom() );
		QgsPoint ul = transform->toMapCoordinates( rect.left(), rect.top() );
		QgsPoint ur = transform->toMapCoordinates( rect.right(), rect.top() );

		reset();
		addPoint( ll, false );
		addPoint( lr, false );
		addPoint( ur, false );
		addPoint( ul, true );
	}

	/*!
	Draw the shape in response to an update event.
	*/
	void XJBRubber::paint( QPainter* p )
	{
		foreach(XJBItem* pItem,m_cCurItems)
		{
			if(nullptr == pItem || pItem->m_cUserPnts.isEmpty())
			{
				continue;
			}
			QVector<QPointF> screenPnts;
			QPointF pntTemp;
			QPointF rubberPos = pos();
			if(rubberPos.x() < 0)
			{
				rubberPos.setX(0);
			}
			if(rubberPos.y() < 0)
			{
				rubberPos.setY(0);
			}

			// 获取特殊绘制信息
			if(nullptr == pItem->m_pExceptiveInfo)
			{
				foreach(QPointF pt,pItem->m_cUserPnts)
				{
					pntTemp = toCanvasCoordinates( QgsPoint( pt.x() + mTranslationOffsetX, pt.y() + mTranslationOffsetY ) ) - pos();
					screenPnts.append(pntTemp);
				}

				XJBDrawHelper::DrawJB(p,pItem,screenPnts);
			}else
			{
				// 根据特殊绘制规则，调用不同的绘制函数
				switch(pItem->m_pExceptiveInfo->eDwType)
				{
				case ExceptiveInfo::edwType_DoubleArrow:
					{
						drawDoubleArrow(p,pItem);
					}break;
				case ExceptiveInfo::edwType_FineArrow:
					{
						drawFineArrow(p,pItem);
					}break;
				case ExceptiveInfo::edwType_GatheringPlace:
					{
						drawGatheringPlace(p,pItem);
					}break;
				case ExceptiveInfo::edwType_AttackArrow:
					{
						drawAttackArrow(p,pItem);
					}break;
				case  ExceptiveInfo::edwType_StraightArrow:
					{
						drawStraightArrow(p,pItem);
					}break;
				default:
					break;
				}
			}
		}

		// 绘制临时对象
		if(nullptr != m_pCurOpItem)
		{
			// 根据特殊绘制规则，调用不同的绘制函数
			switch(m_pCurOpItem->m_pExceptiveInfo->eDwType)
			{
			case ExceptiveInfo::edwType_DoubleArrow:
				{
					drawDoubleArrow(p,m_pCurOpItem,true);
				}break;
			case ExceptiveInfo::edwType_FineArrow:
				{
					drawFineArrow(p,m_pCurOpItem,true);
				}break;
			case ExceptiveInfo::edwType_GatheringPlace:
				{
					drawGatheringPlace(p,m_pCurOpItem,true);
				}break;
			case ExceptiveInfo::edwType_AttackArrow:
				{
					drawAttackArrow(p,m_pCurOpItem,true);
				}break;
			case  ExceptiveInfo::edwType_StraightArrow:
				{
					drawStraightArrow(p,m_pCurOpItem,true);
				}break;
			default:
				break;
			}
		}
	}

	void XJBRubber::updateRect()
	{
		if ( m_cCurItems.empty() && m_pCurOpItem == nullptr )
		{
			setRect( QgsRectangle() );
			setVisible( false );
			return;
		}

// 		mPoints.clear();
// 		mPoints.append(QgsPoint(120,40));

		const QgsMapToPixel& m2p = *( mMapCanvas->getCoordinateTransform() );

		if(nullptr != m_pCurJBItem && 0 == m_pCurJBItem->snFlag)
		{
			qreal w = 60/2;
			QgsRectangle res;

			QList<QgsPoint>::const_iterator it = mPoints.constBegin(),
				itE = mPoints.constEnd();
			for ( ; it != itE; ++it )
			{
				QgsPoint p( it->x(), it->y());
				p = m2p.transform( p );
				QgsRectangle rect( p.x() - w, p.y() - w, p.x() + w, p.y() + w );

				if ( res.isEmpty() )
				{
					res = rect;
				}
				else
				{
					res.combineExtentWith( rect );
				}
			}

			QgsPoint topLeft = m2p.toMapPoint( res.xMinimum(), res.yMinimum() );
			QgsPoint rightBom = m2p.toMapPoint( res.xMaximum(), res.yMaximum() );
			QgsRectangle rect( topLeft, rightBom );

			cur = res;
			setRect( rect );
		}else
		{
			QgsRectangle rect(-180,90,180,-90);
			QgsRectangle canvasRect = mMapCanvas->extent();
			if(canvasRect.xMinimum() < rect.xMinimum())
			{
				rect.setXMinimum(canvasRect.xMinimum());
			}
			if(canvasRect.xMaximum() > rect.xMaximum())
			{
				rect.setXMaximum(canvasRect.xMaximum());
			}
			if(canvasRect.yMinimum() < rect.yMinimum())
			{
				rect.setYMinimum(canvasRect.yMinimum());
			}
			if(canvasRect.yMaximum() > rect.yMaximum())
			{
				rect.setYMaximum(canvasRect.yMaximum());
			}
			

			setRect(rect);
		}
		//QgsRectangle rect(115,42.5,125,37.5);

		
	}

	void XJBRubber::updatePosition()
	{
		// re-compute rectangle
		// See http://hub.qgis.org/issues/12392
		// NOTE: could be optimized by saving map-extent
		//       of rubberband and simply re-projecting
		//       that to device-rectange on "updatePosition"
		updateRect();
	}

	void XJBRubber::setTranslationOffset( double dx, double dy )
	{
		mTranslationOffsetX = dx;
		mTranslationOffsetY = dy;
		updateRect();
	}

	int XJBRubber::size() const
	{
		return mPoints.size();
	}

	int XJBRubber::numberOfVertices() const
	{
		return mPoints.count();
	}

	const QgsPoint *XJBRubber::getPoint( int i ) const
	{
		if ( i < mPoints.size() )
			return &mPoints[i];
		else
			return nullptr;
	}

	QgsGeometry *XJBRubber::asGeometry()
	{
		QgsGeometry *geom = nullptr;
		return geom;
	}

	void XJBRubber::ClearAll()
	{
		mPoints.clear();

		// 删除当前目标列表
		foreach(XJBItem* pItem,m_cCurItems)
		{
			delete pItem;
			pItem = nullptr;
		}
		m_cCurItems.clear();

		updateRect();
		update();
	}

	void XJBRubber::setCurJBItem(const Q_JBItem* _pCurItem)
	{
		m_pCurJBItem = _pCurItem;
	}

	void XJBRubber::AddItem(XJBItem* _pItem)
	{
		if(nullptr == _pItem)
		{
			return ;
		}
		m_cCurItems.append(_pItem);

		setVisible( true );
		updateRect();
		update();
	}

	void XJBRubber::SetTempOpItem(XJBItem* _pOpItem)
	{
		m_pCurOpItem = _pOpItem;
		m_mouseMovePnt = QPointF(COOR_FAILD_VAL,COOR_FAILD_VAL);
		setVisible( true );
		updateRect();
		update();
	}

	void XJBRubber::drawDoubleArrow(QPainter* _p, XJBItem* _pItem, bool _bIsTemp /*= false*/)
	{
		if(nullptr == _p || nullptr == _pItem)
		{
			return ;
		}

		_p->save();

		int md = 0;
		ByyVec3dArray datas;
		foreach(QPointF pt,_pItem->m_cUserPnts)
		{
			datas.append(QVector3D(pt.x(),pt.y(),0));
		}

		if(true == _bIsTemp)
		{
			// 跟随鼠标
			QPointF lastPt = toCanvasCoordinates( QgsPoint( datas.last().x() + mTranslationOffsetX, datas.last().y() + mTranslationOffsetY ) ) - pos();
			QPointF faildPt = QPointF(COOR_FAILD_VAL,COOR_FAILD_VAL);
			QPointF movePos = toCanvasCoordinates( QgsPoint( m_mouseMovePnt.x() + mTranslationOffsetX, m_mouseMovePnt.y() + mTranslationOffsetY ) ) - pos();
			if(m_mouseMovePnt != faildPt && m_mouseMovePnt != QPointF(0,0) 
				&& (qAbs(movePos.x()-lastPt.x()) > 4 || qAbs(movePos.y()-lastPt.y()) > 4))
			{
				datas.append(QVector3D(m_mouseMovePnt.x(),m_mouseMovePnt.y(),0));
			}
		}

		ByyVec3dArray res;
		res = ByyPlotUtils::generateDoubleArrow(datas);

		QPainterPath path;
		QPointF tempPt;

		if(res.isEmpty())
		{
			res = datas;
		}
		if(!res.isEmpty())
		{
			QPointF firstPt = res.first().toPointF();
			firstPt = toCanvasCoordinates( QgsPoint( firstPt.x() + mTranslationOffsetX, firstPt.y() + mTranslationOffsetY ) ) - pos();
			path.moveTo(firstPt);
			foreach(QVector3D pt3d,res)
			{
				tempPt = pt3d.toPointF();
				tempPt = toCanvasCoordinates( QgsPoint( tempPt.x() + mTranslationOffsetX, tempPt.y() + mTranslationOffsetY ) ) - pos();
				path.lineTo(tempPt);
			}
		}

		QPen pen;
		if(true == _bIsTemp)
		{
			pen.setColor(Qt::green);
			pen.setStyle(Qt::DashLine);
		}else
		{
			pen.setColor(Qt::red);
			pen.setStyle(Qt::SolidLine);
		}
		pen.setWidthF(2.0);
		_p->setPen(pen);
		_p->setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);
		_p->drawPath(path);

		_p->restore();
	}

	void XJBRubber::drawFineArrow(QPainter* _p, XJBItem* _pItem, bool _bIsTemp /*= false*/)
	{
		if(nullptr == _p || nullptr == _pItem)
		{
			return ;
		}

		_p->save();

		int md = 0;
		ByyVec3dArray datas;
		foreach(QPointF pt,_pItem->m_cUserPnts)
		{
			datas.append(QVector3D(pt.x(),pt.y(),0));
		}

		if(true == _bIsTemp)
		{
			// 跟随鼠标
			QPointF lastPt = toCanvasCoordinates( QgsPoint( datas.last().x() + mTranslationOffsetX, datas.last().y() + mTranslationOffsetY ) ) - pos();
			QPointF faildPt = QPointF(COOR_FAILD_VAL,COOR_FAILD_VAL);
			QPointF movePos = toCanvasCoordinates( QgsPoint( m_mouseMovePnt.x() + mTranslationOffsetX, m_mouseMovePnt.y() + mTranslationOffsetY ) ) - pos();
			if(m_mouseMovePnt != faildPt && m_mouseMovePnt != QPointF(0,0) 
				&& (qAbs(movePos.x()-lastPt.x()) > 4 || qAbs(movePos.y()-lastPt.y()) > 4))
			{
				datas.append(QVector3D(m_mouseMovePnt.x(),m_mouseMovePnt.y(),0));
			}
		}

		ByyVec3dArray res;
		res = ByyPlotUtils::generateFineArrow(datas);

		QPainterPath path;
		QPointF tempPt;

		if(res.isEmpty())
		{
			res = datas;
		}
		if(!res.isEmpty())
		{
			QPointF firstPt = res.first().toPointF();
			firstPt = toCanvasCoordinates( QgsPoint( firstPt.x() + mTranslationOffsetX, firstPt.y() + mTranslationOffsetY ) ) - pos();
			path.moveTo(firstPt);
			foreach(QVector3D pt3d,res)
			{
				tempPt = pt3d.toPointF();
				tempPt = toCanvasCoordinates( QgsPoint( tempPt.x() + mTranslationOffsetX, tempPt.y() + mTranslationOffsetY ) ) - pos();
				path.lineTo(tempPt);
			}
		}

		QPen pen;
		if(true == _bIsTemp)
		{
			pen.setColor(Qt::green);
			pen.setStyle(Qt::DashLine);
		}else
		{
			pen.setColor(Qt::red);
			pen.setStyle(Qt::SolidLine);
		}
		pen.setWidthF(2.0);
		_p->setPen(pen);
		_p->setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);
		_p->drawPath(path);

		_p->restore();
	}

	void XJBRubber::drawGatheringPlace(QPainter* _p, XJBItem* _pItem, bool _bIsTemp /*= false*/)
	{
		if(nullptr == _p || nullptr == _pItem)
		{
			return ;
		}

		_p->save();

		int md = 0;
		ByyVec3dArray datas;
		foreach(QPointF pt,_pItem->m_cUserPnts)
		{
			datas.append(QVector3D(pt.x(),pt.y(),0));
		}

		if(true == _bIsTemp)
		{
			// 跟随鼠标
			QPointF lastPt = toCanvasCoordinates( QgsPoint( datas.last().x() + mTranslationOffsetX, datas.last().y() + mTranslationOffsetY ) ) - pos();
			QPointF faildPt = QPointF(COOR_FAILD_VAL,COOR_FAILD_VAL);
			QPointF movePos = toCanvasCoordinates( QgsPoint( m_mouseMovePnt.x() + mTranslationOffsetX, m_mouseMovePnt.y() + mTranslationOffsetY ) ) - pos();
			if(m_mouseMovePnt != faildPt && m_mouseMovePnt != QPointF(0,0) 
				&& (qAbs(movePos.x()-lastPt.x()) > 4 || qAbs(movePos.y()-lastPt.y()) > 4))
			{
				datas.append(QVector3D(m_mouseMovePnt.x(),m_mouseMovePnt.y(),0));
			}
		}

		ByyVec3dArray res;
		res = ByyPlotUtils::generateGatheringPlace(datas);

		QPainterPath path;
		QPointF tempPt;

		if(res.isEmpty())
		{
			res = datas;
		}
		if(!res.isEmpty())
		{
			QPointF firstPt = res.first().toPointF();
			firstPt = toCanvasCoordinates( QgsPoint( firstPt.x() + mTranslationOffsetX, firstPt.y() + mTranslationOffsetY ) ) - pos();
			path.moveTo(firstPt);
			foreach(QVector3D pt3d,res)
			{
				tempPt = pt3d.toPointF();
				tempPt = toCanvasCoordinates( QgsPoint( tempPt.x() + mTranslationOffsetX, tempPt.y() + mTranslationOffsetY ) ) - pos();
				path.lineTo(tempPt);
			}
		}

		QPen pen;
		if(true == _bIsTemp)
		{
			pen.setColor(Qt::green);
			pen.setStyle(Qt::DashLine);
		}else
		{
			pen.setColor(Qt::red);
			pen.setStyle(Qt::SolidLine);
		}
		pen.setWidthF(2.0);
		_p->setPen(pen);
		_p->setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);
		_p->drawPath(path);

		_p->restore();
	}

	void XJBRubber::drawAttackArrow(QPainter* _p, XJBItem* _pItem, bool _bIsTemp /*= false*/)
	{
		if(nullptr == _p || nullptr == _pItem)
		{
			return ;
		}

		_p->save();

		int md = 0;
		ByyVec3dArray datas;
		foreach(QPointF pt,_pItem->m_cUserPnts)
		{
			datas.append(QVector3D(pt.x(),pt.y(),0));
		}

		if(true == _bIsTemp)
		{
			// 跟随鼠标
			QPointF lastPt = toCanvasCoordinates( QgsPoint( datas.last().x() + mTranslationOffsetX, datas.last().y() + mTranslationOffsetY ) ) - pos();
			QPointF faildPt = QPointF(COOR_FAILD_VAL,COOR_FAILD_VAL);
			QPointF movePos = toCanvasCoordinates( QgsPoint( m_mouseMovePnt.x() + mTranslationOffsetX, m_mouseMovePnt.y() + mTranslationOffsetY ) ) - pos();
			if(m_mouseMovePnt != faildPt && m_mouseMovePnt != QPointF(0,0) 
				&& (qAbs(movePos.x()-lastPt.x()) > 4 || qAbs(movePos.y()-lastPt.y()) > 4))
			{
				datas.append(QVector3D(m_mouseMovePnt.x(),m_mouseMovePnt.y(),0));
			}
		}

		ByyVec3dArray res;
		res = ByyPlotUtils::generateAttackArrow(datas);

		QPainterPath path;
		QPointF tempPt;

		if(res.isEmpty())
		{
			res = datas;
		}
		if(!res.isEmpty())
		{
			QPointF firstPt = res.first().toPointF();
			firstPt = toCanvasCoordinates( QgsPoint( firstPt.x() + mTranslationOffsetX, firstPt.y() + mTranslationOffsetY ) ) - pos();
			path.moveTo(firstPt);
			foreach(QVector3D pt3d,res)
			{
				tempPt = pt3d.toPointF();
				tempPt = toCanvasCoordinates( QgsPoint( tempPt.x() + mTranslationOffsetX, tempPt.y() + mTranslationOffsetY ) ) - pos();
				path.lineTo(tempPt);
			}
		}

		QPen pen;
		if(true == _bIsTemp)
		{
			pen.setColor(Qt::green);
			pen.setStyle(Qt::DashLine);
		}else
		{
			pen.setColor(Qt::red);
			pen.setStyle(Qt::SolidLine);
		}
		pen.setWidthF(2.0);
		_p->setPen(pen);
		_p->setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);
		_p->drawPath(path);

		_p->restore();
	}

	void XJBRubber::drawStraightArrow(QPainter* _p, XJBItem* _pItem, bool _bIsTemp /*= false*/)
	{
		if(nullptr == _p || nullptr == _pItem)
		{
			return ;
		}

		_p->save();

		int md = 0;
		ByyVec3dArray datas;
		foreach(QPointF pt,_pItem->m_cUserPnts)
		{
			datas.append(QVector3D(pt.x(),pt.y(),0));
		}

		if(true == _bIsTemp)
		{
			// 跟随鼠标
			QPointF lastPt = toCanvasCoordinates( QgsPoint( datas.last().x() + mTranslationOffsetX, datas.last().y() + mTranslationOffsetY ) ) - pos();
			QPointF faildPt = QPointF(COOR_FAILD_VAL,COOR_FAILD_VAL);
			QPointF movePos = toCanvasCoordinates( QgsPoint( m_mouseMovePnt.x() + mTranslationOffsetX, m_mouseMovePnt.y() + mTranslationOffsetY ) ) - pos();
			if(m_mouseMovePnt != faildPt && m_mouseMovePnt != QPointF(0,0) 
				&& (qAbs(movePos.x()-lastPt.x()) > 4 || qAbs(movePos.y()-lastPt.y()) > 4))
			{
				datas.append(QVector3D(m_mouseMovePnt.x(),m_mouseMovePnt.y(),0));
			}
		}

		ByyVec3dArray res;
		res = ByyPlotUtils::generateStraightArrow(datas);

		QPainterPath path;
		QPointF tempPt;

		if(res.isEmpty())
		{
			res = datas;
		}
		if(!res.isEmpty())
		{
			QPointF firstPt = res.first().toPointF();
			firstPt = toCanvasCoordinates( QgsPoint( firstPt.x() + mTranslationOffsetX, firstPt.y() + mTranslationOffsetY ) ) - pos();
			path.moveTo(firstPt);
			foreach(QVector3D pt3d,res)
			{
				tempPt = pt3d.toPointF();
				tempPt = toCanvasCoordinates( QgsPoint( tempPt.x() + mTranslationOffsetX, tempPt.y() + mTranslationOffsetY ) ) - pos();
				path.lineTo(tempPt);
			}
		}

		QPen pen;
		if(true == _bIsTemp)
		{
			pen.setColor(Qt::green);
			pen.setStyle(Qt::DashLine);
		}else
		{
			pen.setColor(Qt::red);
			pen.setStyle(Qt::SolidLine);
		}
		pen.setWidthF(2.0);
		_p->setPen(pen);
		_p->setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);
		_p->drawPath(path);

		_p->restore();
	}
}