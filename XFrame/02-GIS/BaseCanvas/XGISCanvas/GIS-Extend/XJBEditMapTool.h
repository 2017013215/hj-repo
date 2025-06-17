#pragma once
// 地图工具-添加线段

// 头文件导入
#include <XGISCanvasDef.h>

// QGIS
#include <qgsmaptool.h>

namespace XGIS
{
	class XGISCanvas;
	class XShape;
	class XShapeLineJB;
	class XJBEditMapTool : public QgsMapTool
	{
		friend class XJBWidget;
		Q_OBJECT
	public:
		XJBEditMapTool(const XGISCanvas* _pCanvas);
		~XJBEditMapTool(void);

		virtual Flags flags() const override { return QgsMapTool::AllowZoomRect; }
		virtual void activate() override;
		virtual void deactivate() override;
		virtual void canvasMoveEvent( QgsMapMouseEvent* e ) override;
		virtual void canvasPressEvent( QgsMapMouseEvent* e ) override;
		virtual void canvasReleaseEvent( QgsMapMouseEvent* e ) override;

	public slots:
		void onJBWndClose();

	private:
		QgsPoint snapPoint( const QPoint& p );

	private:
		// 标绘选取窗口
		XJBWidget* m_pJBWnd;
		QPoint m_cJBWndLastPos;
		quint32 m_unCurSelLibID;
		quint32 m_unCurSelItemID;
		XGISCanvas* m_pCanvas;

		QList<XShape*> m_pCurAddShape;
		XShapeLineJB* m_pCurOpLineJB;
		bool m_bIsMoveAddPnt;
	};
}