#include "XJBEditMapTool.h"

#include <qgsmapcanvas.h>
#include <qgsrubberband.h>
#include <qgslogger.h>
#include <qgscursors.h>
#include <qgsmessagelog.h>

#include <XGISVLayer.h>
#include <XGISCanvas.h>
#include "Qt-JB\XJBInfoManager.h"
#include "Qt-JB\JBDef.h"
#include "XJBWidget.h"
#include "GIS-Extend\XShapeDef.h"


namespace XGIS
{
	XJBEditMapTool::XJBEditMapTool(const XGISCanvas* _pCanvas)
		: QgsMapTool( (QgsMapCanvas*)_pCanvas )
		, m_pCurOpLineJB(nullptr)
		, m_bIsMoveAddPnt(false)
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

		m_pCurAddShape.clear();

		// 鼠标形状
		QPixmap myCrossHairQPixmap = QPixmap(( const char ** ) cross_hair_cursor );
		mCursor = QCursor( myCrossHairQPixmap, 8, 8 );

		QObject::connect(m_pJBWnd, SIGNAL(slgCloseWnd()),this, SLOT(onJBWndClose()));
	}


	XJBEditMapTool::~XJBEditMapTool(void)
	{
		m_pCurAddShape.clear();
	}
	
	void XJBEditMapTool::canvasMoveEvent(QgsMapMouseEvent* e)
	{
		if(nullptr != m_pCurOpLineJB)
		{
			QgsPoint point = snapPoint( e->pos() );
			if(m_bIsMoveAddPnt)
			{
				m_pCurOpLineJB->AddPnt(XCoordinateGeo(point.x(),point.y(),0));
				m_bIsMoveAddPnt = false;
			}
			else
				m_pCurOpLineJB->ReplaceLast(XCoordinateGeo(point.x(),point.y(),0));
			m_pCanvas->update();
		}
	}

	void XJBEditMapTool::canvasPressEvent(QgsMapMouseEvent* e)
	{
		Q_UNUSED( e );
	}

	void XJBEditMapTool::canvasReleaseEvent(QgsMapMouseEvent* e)
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
					// 创建点目标
					XShapePointJB* pPntJB = new XShapePointJB(XCoordinateGeo(point.x(),point.y(),0),m_unCurSelLibID,m_unCurSelItemID);
					pPntJB->SetSize(QSize(30,30));
					m_pCurAddShape.append(pPntJB);
					quint32 unID = m_pCanvas->AddShape(pPntJB);
					if(0 == unID)
					{
						Q_ASSERT(false);
						delete pPntJB;
					}
					pPntJB = nullptr;
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
					if(nullptr == m_pCurOpLineJB)
					{
						// 创建线JB
						m_pCurOpLineJB = new XShapeLineJB(m_unCurSelLibID,m_unCurSelItemID);
						m_pCurOpLineJB->SetPenStyle(Qt::DashLine);
						m_pCurOpLineJB->SetPenClr(Qt::red);
						m_pCurOpLineJB->SetPenWidth(2.0);
						m_pCurOpLineJB->SetAlpha(0.7);
						m_pCurOpLineJB->AddPnt(XCoordinateGeo(point.x(),point.y(),0));
						m_pCanvas->AddShape(m_pCurOpLineJB);
						m_bIsMoveAddPnt = true;
					}else
					{
						m_pCurOpLineJB->ReplaceLast(XCoordinateGeo(point.x(),point.y(),0));
						quint32 unPts = m_pCurOpLineJB->GetPntCount();
						const ExceptiveInfo* pExInfo = m_pCurOpLineJB->GetExceptiveInfo();
						Q_ASSERT(pExInfo);
						if(nullptr != pExInfo && unPts == pExInfo->unMaxPoint)
						{
							// 数据点达到最大数据点数，取消当前图形编辑并加入绘制列表
							m_pCurOpLineJB->SetPenStyle(Qt::SolidLine);
							m_pCurOpLineJB->SetPenClr(Qt::red);
							m_pCurOpLineJB->SetAlpha(1.0);
							m_pCanvas->update();
							m_pCurOpLineJB = nullptr;
							m_bIsMoveAddPnt = false;
						}else
							m_bIsMoveAddPnt = true;
					}
				}
				break;
			case Qt::RightButton:
				{
					if(nullptr != m_pCurOpLineJB)
					{
						quint32 unPts = m_pCurOpLineJB->GetPntCount();
						const ExceptiveInfo* pExInfo = m_pCurOpLineJB->GetExceptiveInfo();
						Q_ASSERT(pExInfo);
						if(nullptr != pExInfo && unPts >= pExInfo->unMinPoint)
						{
							// 数据点达到最小数据点数，取消当前图形编辑并加入绘制列表
							m_pCurOpLineJB->ReplaceLast(XCoordinateGeo(point.x(),point.y(),0));
							m_pCurOpLineJB->SetPenStyle(Qt::SolidLine);
							m_pCurOpLineJB->SetPenClr(Qt::red);
							m_pCurOpLineJB->SetAlpha(1.0);
							m_pCanvas->update();
							m_pCurOpLineJB = nullptr;
							m_bIsMoveAddPnt = false;

						}else if(nullptr != pExInfo && unPts+1 < pExInfo->unMinPoint)
						{
							// 删除改对象
							m_pCanvas->RemoveShapeByID(m_pCurOpLineJB->GetID());
							m_pCurOpLineJB = nullptr;
							m_bIsMoveAddPnt = false;
							m_unCurSelLibID = m_unCurSelItemID = 0;
							m_pJBWnd->move(m_cJBWndLastPos);
							m_pJBWnd->show();
							m_pJBWnd->activateWindow();
						}
					}else
					{
						m_unCurSelLibID = m_unCurSelItemID = 0;
						m_bIsMoveAddPnt = false;
						m_pJBWnd->move(m_cJBWndLastPos);
						m_pJBWnd->show();
						m_pJBWnd->activateWindow();
					}
				}break;
			default:
				break;
			}
		}
	}

	void XJBEditMapTool::activate()
	{
		 m_pCurAddShape.clear();
		 if(m_cJBWndLastPos.x() >= 0 && m_cJBWndLastPos.y() >= 0)
		 {
			 m_pJBWnd->move(m_cJBWndLastPos);
		 }
		 m_pJBWnd->show();
		 QgsMapTool::activate();
	}

	void XJBEditMapTool::deactivate()
	{
		m_cJBWndLastPos = m_pJBWnd->pos();
		m_pJBWnd->hide();
		QgsMapTool::deactivate();
	}

	QgsPoint XJBEditMapTool::snapPoint(const QPoint& p)
	{
		QgsPointLocator::Match m = mCanvas->snappingUtils()->snapToMap( p );
		return m.isValid() ? m.point() : mCanvas->getCoordinateTransform()->toMapCoordinates( p );
	}

	void XJBEditMapTool::onJBWndClose()
	{
		m_cJBWndLastPos = m_pJBWnd->pos();
	}
}