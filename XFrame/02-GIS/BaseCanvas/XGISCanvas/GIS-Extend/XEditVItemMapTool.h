#pragma once
// 地图工具-添加线段

// 头文件导入
#include <XGISCanvasDef.h>

// QGIS
#include <qgsmaptool.h>

#include <QList>

namespace XGIS
{
	class XGISCanvas;
	class XShape;
	class XLineShape;
	class XPolygonShape;
	class XEditVItemMapTool : public QgsMapTool
	{
		Q_OBJECT

	public:
		enum EVItemType
		{
			EVItem_Point = 1,	// 点
			EVItem_Line = 2,	// 线
			EVItem_Polygon = 3,	// 多边形
			EVItem_Rectangle = 4,// 矩形
			EVItem_Circle = 5,	// 圆形
			EVItem_Ellipses = 6,// 椭圆
		};

	public:
		XEditVItemMapTool(const XGISCanvas* _pCanvas, EVItemType _eItemType);
		~XEditVItemMapTool(void);

		virtual Flags flags() const override { return QgsMapTool::AllowZoomRect; }

		virtual void canvasMoveEvent( QgsMapMouseEvent* e ) override;
		virtual void canvasPressEvent( QgsMapMouseEvent* e ) override;
		virtual void canvasReleaseEvent( QgsMapMouseEvent* e ) override;
		virtual void activate() override;
		virtual void deactivate() override;

	private:
		QgsPoint snapPoint( const QPoint& p );

	private:
		EVItemType m_eItemType;
		XGISCanvas* m_pCanvas;

		QList<XShape*> m_xCurShapes;

		XLineShape* m_pCurOpLine;
		XPolygonShape* m_pCurOpPolygon;
		bool m_bIsMoveAddPnt;
	};
}