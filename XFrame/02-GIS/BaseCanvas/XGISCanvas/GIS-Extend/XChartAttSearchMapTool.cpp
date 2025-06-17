#include "XChartAttSearchMapTool.h"

#include <qgsmapcanvas.h>
#include <qgsrubberband.h>
#include <qgslogger.h>
#include <qgscursors.h>
#include <qgsmessagelog.h>

#include <XGISCanvas.h>
#include "xchartsearchreswnd.h"

namespace XGIS
{
	XChartAttSearchMapTool::XChartAttSearchMapTool(const XGISCanvas* _pCanvas)
		: QgsMapTool( (QgsMapCanvas*)_pCanvas )
	{
		if(nullptr == _pCanvas)
		{
			Q_ASSERT(false);
			return;
		}

		m_pCanvas = (XGISCanvas*)_pCanvas;

 		m_pSearchResWnd = new XChartSearchResWnd(m_pCanvas->topLevelWidget());
 		m_pSearchResWnd->setWindowTitle(QString::fromLocal8Bit("海图查询结果"));
 		m_pSearchResWnd->hide();
 		m_pSearchResWnd->setMinimumSize(200,250);
 		m_pSearchResWnd->resize(200,250);

		// 鼠标形状
		QPixmap myCrossHairQPixmap = QPixmap(( const char ** ) cross_hair_cursor );
		mCursor = QCursor( myCrossHairQPixmap, 8, 8 );
	}


	XChartAttSearchMapTool::~XChartAttSearchMapTool(void)
	{
		if(m_pSearchResWnd)
		{
			m_pSearchResWnd->hide();
			delete m_pSearchResWnd;
			m_pSearchResWnd = nullptr;
		}
		m_pCanvas = nullptr;
	}

	void XChartAttSearchMapTool::canvasMoveEvent(QgsMapMouseEvent* e)
	{
		Q_UNUSED( e );
	}

	void XChartAttSearchMapTool::canvasPressEvent(QgsMapMouseEvent* e)
	{
		Q_UNUSED( e );
	}

	void XChartAttSearchMapTool::canvasReleaseEvent(QgsMapMouseEvent* e)
	{
		switch (e->button())
		{
		case Qt::LeftButton:
			{
				QgsPoint point = snapPoint( e->pos() );

				// 查询该点海图数据
				QStringList strDetail = m_pCanvas->GetChartInfoByPt(point.toQPointF());
				if(m_pSearchResWnd->isHidden())
				{
					m_pSearchResWnd->ShowThis();
				}
				m_pSearchResWnd->UpdateInfo(strDetail);
			}break;
		default:
			break;
		}
	}

	void XChartAttSearchMapTool::activate()
	{
		 QgsMapTool::activate();
	}

	void XChartAttSearchMapTool::deactivate()
	{
		m_pSearchResWnd->HideThis();
		QgsMapTool::deactivate();
	}

	QgsPoint XChartAttSearchMapTool::snapPoint(const QPoint& p)
	{
		QgsPointLocator::Match m = mCanvas->snappingUtils()->snapToMap( p );
		return m.isValid() ? m.point() : mCanvas->getCoordinateTransform()->toMapCoordinates( p );
	}
}