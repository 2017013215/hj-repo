#include "XAddJBMapTool.h"

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
#include "JB\JBWindow\QtDrawJBWidget.h"
#include "Qt-JB\XJBInfoManager.h"
#include "Qt-JB\JBDef.h"
#include "XJBRubber.h"
#include "XJBWidget.h"
#include "XJBItem.h"


namespace XGIS
{
	XAddJBMapTool::XAddJBMapTool(const XGISCanvas* _pCanvas)
		: QgsMapTool( (QgsMapCanvas*)_pCanvas )
		,m_unLineWidth(2)
		,m_cLineColor(QColor(255,128,0))
		,m_dLineAlpha(0.8)
		,m_eLineStyle(Qt::DotLine)
		,m_unPointSize(5)
		,m_cPointColor(QColor(255,128,0))
		,m_dPointAlpha(0.9)
		,m_cFillClr(QColor(255,128,64))
		,m_dFillAlpha(0.6)
		,m_pCurAddLayer(nullptr)
		,m_pCurOpLineItem(nullptr)
	{
		if(nullptr == _pCanvas)
		{
			Q_ASSERT(false);
			return;
		}

		m_pCanvas = (XGISCanvas*)_pCanvas;

		m_pJBWnd = new XJBWidget(m_pCanvas->topLevelWidget());
		m_pJBWnd->setWindowTitle(QString::fromLocal8Bit("标绘窗口"));
		m_pJBWnd->hide();
		m_pJBWnd->setMinimumSize(664,472);
		m_pJBWnd->resize(664,472);
		m_cJBWndLastPos = QPoint(-1,-1);

		// 更新临时形状设置
		updateRubber();
		// New
		m_pJBRubber = new XJBRubber(m_pCanvas);

		// 鼠标形状
		QPixmap myCrossHairQPixmap = QPixmap(( const char ** ) cross_hair_cursor );
		mCursor = QCursor( myCrossHairQPixmap, 8, 8 );

		// 数据初始化
		m_cPoints.clear();
		m_bIsDone = true;

		// 信号链接
		QObject::connect( m_pCanvas, SIGNAL( destinationCrsChanged() ),
			this, SLOT( updateSettings() ) );

		QObject::connect(m_pJBWnd, SIGNAL(slgCloseWnd()),this, SLOT(onJBWndClose()));
	}


	XAddJBMapTool::~XAddJBMapTool(void)
	{
		if(m_pJBRubber)
		{
			delete m_pJBRubber;
			m_pJBRubber = nullptr;
		}
	}

	void XAddJBMapTool::updateRubber()
	{
		QColor ptClr = m_cPointColor;
		QColor liClr = m_cLineColor;
		QColor aeClr = m_cFillClr;
		liClr.setAlphaF(m_dLineAlpha);
		ptClr.setAlphaF(m_dPointAlpha);
		aeClr.setAlphaF(m_dFillAlpha);
		mToolName = QString::fromLocal8Bit("添加态势标绘");
	}

	void XAddJBMapTool::updateSettings()
	{
		if (m_cDestinationCrs != mCanvas->mapSettings().destinationCrs() )
		{
			QList<QgsPoint> points = m_cPoints;
			bool lastDone = m_bIsDone;

			m_bIsDone = lastDone;
			QgsCoordinateTransform ct( m_cDestinationCrs, mCanvas->mapSettings().destinationCrs() );

			Q_FOREACH ( const QgsPoint& previousPoint, points )
			{
				try
				{
					QgsPoint point = ct.transform( previousPoint );

					m_cPoints.append( point );
				}
				catch ( QgsCsException &cse )
				{
					QgsMessageLog::logMessage( QString( "Transform error caught at the MeasureTool: %1" ).arg( cse.what() ) );
				}
			}

			m_pJBRubber->updatePosition();
			m_pJBRubber->update();
		}
		m_cDestinationCrs = mCanvas->mapSettings().destinationCrs();
	}

	void XAddJBMapTool::restart()
	{
		m_cPoints.clear();

		//m_pJBRubber->ClearAll();
		
		m_bIsDone = false;
	}

	void XAddJBMapTool::addPoint(const QgsPoint &point)
	{
		QgsDebugMsg( "point=" + point.toString() );

		if ( !m_cPoints.isEmpty() && m_cPoints.last() == point )
		{
			return;
		}

		QgsPoint pnt( point );
		m_cPoints.append( pnt );
		m_pJBRubber->addPoint(pnt);
	}

	void XAddJBMapTool::canvasMoveEvent(QgsMapMouseEvent* e)
	{
		if ( ! m_bIsDone )
		{
			QgsPoint point = snapPoint( e->pos() );

			if(nullptr != m_pCurOpLineItem)
			{
				m_pJBRubber->movePoint(-1,point);
 			}
		}
	}

	void XAddJBMapTool::canvasPressEvent(QgsMapMouseEvent* e)
	{
		Q_UNUSED( e );
	}

	void XAddJBMapTool::canvasReleaseEvent(QgsMapMouseEvent* e)
	{
		if(false == m_pJBWnd->IsCheckedItem())
		{
			if(true == m_pJBWnd->isHidden())
			{
				m_pJBWnd->move(m_cJBWndLastPos);
				m_pJBWnd->show();
			}
			m_pJBWnd->activateWindow();
			return;
		}

		if(false == m_pJBWnd->isHidden() || m_unCurSelLibID == 0 || m_unCurSelItemID == 0)
		{
			m_cJBWndLastPos = m_pJBWnd->pos();
			m_pJBWnd->hide();
			bool bRes = m_pJBWnd->GetCheckItem(m_unCurSelLibID,m_unCurSelItemID);
			if(false == bRes)
			{
				Q_ASSERT(false);
				return;
			}
		}


		// 判断当前添加的目标类型---点or线
		QgsPoint point = snapPoint( e->pos() );
		if(true == XJBInfoManager::GetInstance()->ItemIsPointType(m_unCurSelLibID,m_unCurSelItemID))
		{
			// 点目标
			switch (e->button())
			{
			case Qt::LeftButton:
				{
					// 创建点目标，并加入JB对象管理器
					XJBItem* pItem = new XJBItem(m_unCurSelLibID,m_unCurSelItemID);
					pItem->AddPoint(point.toQPointF());
					pItem->SetSize(60,60);
					pItem->SetAngle(rand()%360);
					// 添加进军标管理器

					// 添加到当前Rubber列表
					m_pJBRubber->AddItem(pItem);
					m_bIsDone = true;
					pItem = nullptr;
				}
				break;
			case Qt::RightButton:
				{
					m_unCurSelLibID = m_unCurSelItemID = 0;
					m_pJBWnd->move(m_cJBWndLastPos);
					m_pJBWnd->show();
					m_pJBWnd->activateWindow();
				}break;
			default:
				break;
			}
		}else
		{
			switch (e->button())
			{
			case Qt::LeftButton:
				{
					// 创建目标，并加入JB对象管理器
					if(nullptr == m_pCurOpLineItem)
					{
						m_pCurOpLineItem = new XJBItem(m_unCurSelLibID,m_unCurSelItemID);
						m_pCurOpLineItem->AddPoint(point.toQPointF());
						m_pJBRubber->SetTempOpItem(m_pCurOpLineItem);
					}
					else
					{
						m_pCurOpLineItem->AddPoint(point.toQPointF());
						m_pJBRubber->updateRect();
						m_pJBRubber->update();
					}
					
					quint32 unPts = m_pCurOpLineItem->GetPtCount();
					if(unPts == m_pCurOpLineItem->GetExceptiveInfo()->unMaxPoint)
					{
						// 数据点达到最大数据点数，取消当前图形编辑并加入绘制列表
						m_pJBRubber->AddItem(m_pCurOpLineItem);
						m_pJBRubber->SetTempOpItem(nullptr);
						m_pCurOpLineItem = nullptr;
						m_pJBRubber->updateRect();
						m_pJBRubber->update();
					}


					m_bIsDone = false;
					//pItem->SetSize(60,60);
					//pItem->SetAngle(rand()%360);
					// 添加进军标管理器

					// 添加到当前Rubber列表
					//m_pJBRubber->AddItem(m_pCurOpLineItem);
				}
				break;
			case Qt::RightButton:
				{
// 					m_pJBRubber->AddItem(m_pCurOpLineItem);
// 					m_pJBRubber->SetTempOpItem(nullptr);
// 					m_pCurOpLineItem = nullptr;
// 					m_unCurSelLibID = m_unCurSelItemID = 0;
// 					m_pJBWnd->move(m_cJBWndLastPos);
// 					m_pJBWnd->show();
// 					m_pJBWnd->activateWindow();
					if(nullptr != m_pCurOpLineItem)
					{
						quint32 unPts = m_pCurOpLineItem->GetPtCount();
						if(unPts+1 >= m_pCurOpLineItem->GetExceptiveInfo()->unMinPoint)
						{
							// 数据点达到最小数据点数，取消当前图形编辑并加入绘制列表
							m_pCurOpLineItem->AddPoint(point.toQPointF());
							m_pJBRubber->AddItem(m_pCurOpLineItem);
							m_pJBRubber->SetTempOpItem(nullptr);
							m_pCurOpLineItem = nullptr;
							m_pJBRubber->updateRect();
							m_pJBRubber->update();

							m_unCurSelLibID = m_unCurSelItemID = 0;
							m_pJBWnd->move(m_cJBWndLastPos);
							m_pJBWnd->show();
							m_pJBWnd->activateWindow();
						}
					}else
					{
						m_unCurSelLibID = m_unCurSelItemID = 0;
						m_pJBWnd->move(m_cJBWndLastPos);
						m_pJBWnd->show();
						m_pJBWnd->activateWindow();
					}
					

					m_bIsDone = true;
				}break;
			default:
				break;
			}
		}
	}

	void XAddJBMapTool::activate()
	{
		 QgsMapTool::activate();
		  updateSettings();
		  if ( mCanvas->mapSettings().destinationCrs().geographicFlag() &&
			  ( mCanvas->extent().height() > 360 ||
			  mCanvas->extent().width() > 720 ) )
		  {
			  Q_ASSERT(false);
		  }

		  if(m_cJBWndLastPos.x() >= 0 && m_cJBWndLastPos.y() >= 0)
		  {
			  m_pJBWnd->move(m_cJBWndLastPos);
		  }
		  m_pJBWnd->show();
		  m_pJBRubber->setVisible(true);
		  m_pJBRubber->updateRect();
		  m_pJBRubber->update();
	}

	void XAddJBMapTool::deactivate()
	{
		restart();
		m_cJBWndLastPos = m_pJBWnd->pos();
		m_pJBWnd->hide();
		//m_pJBRubber->hide();
		//m_pJBRubber->ClearAll();
		QgsMapTool::deactivate();
	}

	void XAddJBMapTool::keyPressEvent(QKeyEvent* e)
	{
		int curKey = e->key();
		switch(curKey)
		{
		case Qt::Key_Backspace:
		case Qt::Key_Delete:
			if ( !m_bIsDone )
			{
				undo();
			}break;
		case Qt::Key_Escape:
			{
				restart();
			}break;
		case Qt::Key_Enter:
		case Qt::Key_Return:
			{
				if(true == m_bIsDone)
				{
					// 添加至图层
				}
				// 重新显示标绘窗口
				m_pJBWnd->move(m_cJBWndLastPos);
				m_pJBWnd->show();
				//restart();
			}
		default:
			break;
		}
	}

	QgsPoint XAddJBMapTool::snapPoint(const QPoint& p)
	{
		QgsPointLocator::Match m = mCanvas->snappingUtils()->snapToMap( p );
		return m.isValid() ? m.point() : mCanvas->getCoordinateTransform()->toMapCoordinates( p );
	}

	void XAddJBMapTool::undo()
	{
		if ( m_pJBRubber )
		{
			if ( m_cPoints.size() < 1 )
			{
				return;
			}

			if ( m_cPoints.size() == 1 )
			{
				//removing first point, so restart everything
				restart();
				//mDialog->restart();
			}
			else
			{
				m_cPoints.removeLast();
				m_pJBRubber->removeLastPoint();
			}

		}
	}

	bool XAddJBMapTool::SetOpLayer(XGISVLayer* _pVLayer)
	{
		if(nullptr == _pVLayer)
		{
			return false;
		}
		
		m_pCurAddLayer = _pVLayer;
		return true;
	}

	void XAddJBMapTool::SetLineClr(QColor _cLineClr, double _dAlpha)
	{
		if(nullptr != m_pJBRubber)
		{
			m_cLineColor = _cLineClr;
			m_dFillAlpha = _dAlpha;
		}
	}

	void XAddJBMapTool::SetLineStyle(Qt::PenStyle _eLineStyle)
	{
		if(nullptr != m_pJBRubber)
		{
			m_eLineStyle = _eLineStyle;
		}
	}

	void XAddJBMapTool::SetLineWidth(quint8 _unLineWidth)
	{
		if(nullptr != m_pJBRubber)
		{
			m_unLineWidth = _unLineWidth;
		}
	}

	void XAddJBMapTool::SetPointClr(QColor _cPntClr, double _dAlpha)
	{
		m_cPointColor = _cPntClr;
		m_dPointAlpha = _dAlpha;
	}

	void XAddJBMapTool::SetPointSize(quint8 _unSize)
	{
		m_unPointSize = _unSize;
	}

	void XAddJBMapTool::SetFillClr(QColor _cFillClr, double _dAlpha)
	{
		m_cFillClr = _cFillClr;
		m_dFillAlpha = _dAlpha;
	}

	void XAddJBMapTool::onJBWndClose()
	{
		m_cJBWndLastPos = m_pJBWnd->pos();
	}

}