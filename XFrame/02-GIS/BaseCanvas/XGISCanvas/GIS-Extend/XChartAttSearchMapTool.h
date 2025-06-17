#pragma once
// 地图工具-添加线段

// 头文件导入
#include <XGISCanvasDef.h>

// QGIS
#include <qgsmaptool.h>

class QgsMapCanvas;

namespace XSpace_Plugins
{
	class XS57LayerAdapter;
}using namespace XSpace_Plugins;

namespace XGIS
{
	class XGISCanvas;
	class XChartSearchResWnd;
	class XChartAttSearchMapTool : public QgsMapTool
	{
		Q_OBJECT
	public:
		XChartAttSearchMapTool(const XGISCanvas* _pCanvas);
		~XChartAttSearchMapTool(void);

		virtual Flags flags() const override { return QgsMapTool::AllowZoomRect; }

		virtual void canvasMoveEvent( QgsMapMouseEvent* e ) override;
		virtual void canvasPressEvent( QgsMapMouseEvent* e ) override;
		virtual void canvasReleaseEvent( QgsMapMouseEvent* e ) override;
		virtual void activate() override;
		virtual void deactivate() override;

	private:
		QgsPoint snapPoint( const QPoint& p );
	private:
		XGISCanvas* m_pCanvas;
		XChartSearchResWnd* m_pSearchResWnd;
	};
}