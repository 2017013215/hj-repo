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
#ifndef XJB_RUBBER_H
#define XJB_RUBBER_H

#include "XGISCanvasDef.h"

#include "qgsmapcanvasitem.h"
#include "qgsgeometry.h"
#include <QList>
class QgsVectorLayer;
class QPaintEvent;

/** \ingroup gui
* A class for drawing transient features (e.g. digitising lines) on the map.
* It may be used
*/

namespace XGIS
{
	class XJBItem;
	class Q_JBItem;
	class XJBRubber: public QgsMapCanvasItem
	{
		friend class XAddJBMapTool;
	public:
		/**
		* Creates a new RubberBand.
		*  @param mapCanvas The map canvas to draw onto. It's CRS will be used map points onto screen coordinates.
		*  @param geometryType Defines how the data should be drawn onto the screen. (Use QGis::Line, QGis::Polygon or QGis::Point)
		*/
		XJBRubber( QgsMapCanvas* mapCanvas, QGis::GeometryType geometryType = QGis::Line );
		~XJBRubber();

		void ClearAll();

		void AddItem(XJBItem* _pItem);

		void SetTempOpItem(XJBItem* _pOpItem);

	private:
		void setCurJBItem(const Q_JBItem* _pCurItem);
	public:

		/**
		* Clears all the geometries in this rubberband.
		* Sets the representation type according to geometryType.
		*  @param geometryType Defines how the data should be drawn onto the screen. (Use QGis::Line, QGis::Polygon or QGis::Point)
		*/
		void reset();

		/**
		* @deprecated Use the reset method which takes QGis::GeometryType as second argument instead
		* Clears all the geometries in this rubberband.
		* Sets the representation type according to isPolygon.
		*  @param isPolygon true: draw as (multi-)polygon, false draw as (multi-)linestring
		*/
		Q_DECL_DEPRECATED void reset( bool isPolygon );

		/**
		* Add a vertex to the rubberband and update canvas.
		* The rendering of the vertex depends on the current GeometryType and icon.
		* If adding more points consider using update=false for better performance
		*  @param p             The vertex/point to add
		*  @param doUpdate      Should the map canvas be updated immediately?
		*  @param geometryIndex The index of the feature part (in case of multipart geometries)
		*/
		void addPoint( const QgsPoint & p, bool doUpdate = true );

		/**
		* Removes the last point. Most useful in connection with undo operations
		*/
		void removeLastPoint( bool doUpdate = true );

		/**
		* Moves the rubber band point specified by index. Note that if the rubber band is
		* not used to track the last mouse position, the first point of the rubber band has two vertices
		*/
		void movePoint( int index, const QgsPoint& p );

		/**
		* Sets this rubber band to a map canvas rectangle
		*  @param rect rectangle in canvas coordinates
		*/
		void setToCanvasRectangle( QRect rect );

		/**
		* Adds translation to original coordinates (all in map coordinates)
		*  @param dx  x translation
		*  @param dy  y translation
		*/
		void setTranslationOffset( double dx, double dy );

		/**
		* Returns number of geometries
		*  @return number of geometries
		*/
		int size() const;

		/**
		* Returns count of vertices in all lists of mPoint
		*  @return The total number of vertices
		*/
		int numberOfVertices() const;

		/**
		* Return vertex
		*  @param i   The geometry index
		*  @param j   The vertex index within geometry i
		*/
		const QgsPoint *getPoint( int i ) const;

		/**
		* Returns the rubberband as a Geometry.
		*  @return A geometry object which reflects the current state of the rubberband.
		*/
		QgsGeometry* asGeometry();

		virtual void updatePosition() override;

	protected:
		virtual void paint( QPainter* p ) override;

		//! recalculates needed rectangle
		void updateRect();

	private:
		// 绘制双箭头-钳击
		void drawDoubleArrow(QPainter* _p, XJBItem* _pItem, bool _bIsTemp = false);
		// 绘制细直箭标-突击
		void drawFineArrow(QPainter* _p, XJBItem* _pItem, bool _bIsTemp = false);
		// 绘制腰子形区域-集结地域
		void drawGatheringPlace(QPainter* _p, XJBItem* _pItem, bool _bIsTemp = false);
		// 绘制攻击箭头
		void drawAttackArrow(QPainter* _p, XJBItem* _pItem, bool _bIsTemp = false);
		// 绘制细直箭头
		void drawStraightArrow(QPainter* _p, XJBItem* _pItem, bool _bIsTemp = false);
	private:
		const Q_JBItem* m_pCurJBItem;
		QVector<XJBItem*> m_cCurItems;
		bool m_bIsUseUserClr;
		QColor m_cUserClr;
		XJBItem* m_pCurOpItem;
		QPointF m_mouseMovePnt;

		QList<QgsPoint> mPoints;
		
		QgsPoint m_curMousePos;

		double mTranslationOffsetX;
		double mTranslationOffsetY;

		QgsRectangle cur;
	};
}

#endif
