#pragma once
// 地图工具-添加线段

// 头文件导入
#include <XGISCanvasDef.h>

// QGIS
#include <qgsmaptool.h>

class QgsDistanceArea;
class QgsMapCanvas;
class QgsMeasureDialog;
class QgsRubberBand;

namespace XGIS
{
	class XGISCanvas;
	class XGISVLayer;
	class XJBRubber;
	class XJBItem;
	class Q_JBItem;
	class XJBWidget;
	struct ExceptiveInfo;
	class XAddJBMapTool : public QgsMapTool
	{
		friend XJBWidget;
		Q_OBJECT
	public:
		XAddJBMapTool(const XGISCanvas* _pCanvas);
		~XAddJBMapTool(void);

		virtual Flags flags() const override { return QgsMapTool::AllowZoomRect; }
		bool done() { return m_bIsDone; }

		//! Reset and start new
		void restart();

		//! Add new point
		void addPoint( const QgsPoint &point );

		//! Returns reference to array of the points
		const QList<QgsPoint>& points();

		// Inherited from QgsMapTool

		//! Mouse move event for overriding
		virtual void canvasMoveEvent( QgsMapMouseEvent* e ) override;

		//! Mouse press event for overriding
		virtual void canvasPressEvent( QgsMapMouseEvent* e ) override;

		//! Mouse release event for overriding
		virtual void canvasReleaseEvent( QgsMapMouseEvent* e ) override;

		//! called when set as currently active map tool
		virtual void activate() override;

		//! called when map tool is being deactivated
		virtual void deactivate() override;

		virtual void keyPressEvent( QKeyEvent* e ) override;

		// 设置要添加的图层
		bool SetOpLayer(XGISVLayer* _pVLayer);

		// 设置线颜色
		void SetLineClr(QColor _cLineClr, double _dAlpha);
		// 设置线样式
		void SetLineStyle(Qt::PenStyle _eLineStyle);
		// 设置线宽
		void SetLineWidth(quint8 _unLineWidth);

		// 设置点颜色
		void SetPointClr(QColor _cPntClr, double _dAlpha);
		// 设置点大下-像素
		void SetPointSize(quint8 _unSize);

		// 设置填充颜色
		void SetFillClr(QColor _cFillClr, double _dAlpha);

	public slots:
		//! updates the projections we're using
		void updateSettings();

		void onJBWndClose();

	private:
		QgsPoint snapPoint( const QPoint& p );

		void undo();

		void updateRubber();

	private:
		// 标绘选取窗口
		XJBWidget* m_pJBWnd;
		QPoint m_cJBWndLastPos;
		quint32 m_unCurSelLibID;
		quint32 m_unCurSelItemID;
		XJBRubber* m_pJBRubber;
		XJBItem* m_pCurOpLineItem;
		
		QColor m_cFillClr;
		double m_dFillAlpha;
		quint8 m_unLineWidth;
		QColor m_cLineColor;
		double m_dLineAlpha;
		quint8 m_unPointSize;
		QColor m_cPointColor;
		double m_dPointAlpha;
		Qt::PenStyle m_eLineStyle;

		bool m_bIsDone;
		QgsCoordinateReferenceSystem m_cDestinationCrs;
		QList<QgsPoint> m_cPoints;

		XGISVLayer* m_pCurAddLayer;
		XGISCanvas* m_pCanvas;
	};
}