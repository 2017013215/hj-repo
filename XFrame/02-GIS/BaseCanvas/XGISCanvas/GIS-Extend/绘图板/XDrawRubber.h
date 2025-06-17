/***************************************************************************
XJBRubber.h - Rubberband widget for drawing multilines and polygons
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
#ifndef XDRAW_RUBBER_H
#define XDRAW_RUBBER_H

#include "XGISCanvasDef.h"
#include "qgsmapcanvasitem.h"
#include <QMap>

class QTimer;
namespace XGIS
{
	class XGISCanvas;
	class XShape;
	class XShapeDoc;
	class XDrawRubber: public QObject,public QgsMapCanvasItem
	{
		Q_OBJECT
	public:
		XDrawRubber( XGISCanvas* _pMapCanvas );
		~XDrawRubber();

		const quint32 AddShape(const XShape* _pxShape);

		const XShape* GetShapeByID(const quint32 _unID) const;

		const bool RemoveShapeByID(const quint32 _unID);

		void ClearAllShape();

		// 开启动态更新
		const bool StartAutoUpdate(const quint32 _unMSec);
		void StopAutoUpdate();

		// 存储态势文件
		const bool SaveStateFile(const QString& _strFilePath);

		// 打开态势文件
		const bool OpenStateFile(const QString& _strFilePath);

	private slots:
		virtual void updatePosition() override;
		virtual void UpdateRubber();

	protected:
		virtual void paint( QPainter* p ) override;
		void updateRect();

	private:
		XGISCanvas* m_pCanvas;

		// 图形数据档
		XShapeDoc* m_pxShapDoc;
		//QMap<quint32,XShape*> m_cShapes;
		//quint32 m_unCurIdx;	// 当前索引值 每添加一次改值会加1 不会减少

		QTimer* m_pUpTimer;
	};
}

#endif
