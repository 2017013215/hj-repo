#pragma once
#include "XVItem.h"
#include <QtCore/QPointF>
#include <QtGui/QColor>
#include <qgsgeometry.h>

namespace XGIS
{
	class XVItemPolygon :
		public XVItem
	{
	public:
		XVItemPolygon(void);
		virtual ~XVItemPolygon(void);

		virtual const bool IsValid() const;
		virtual QgsGeometry* GetGeom() const;

		void AddPolygonData(QgsPolyline& _cRefData);
/*		void AddPolygonData(QgsPoint _cLinePnt);*/
		void AddPolygonData(QgsPoint& _cRefData);

		const QgsPolyline& GetPolygonData() const;
		void SetLineWidht(double _dWidth);
		double GetLineWidth() const;
		void SetLineClr(QColor _cLineClr);
		QColor GetLineClr() const;
		Qt::PenStyle GetLineStyle() const;
		void SetLineStyle(Qt::PenStyle _eStyle);
		void SetLineAlpha(double _dAlpha);

		void SetFillClr(QColor _cFillClr, double _dAlpha);
		QColor GetFillClr() const;

	private:
		QgsPolyline m_cPolygonData;

		double m_dLineWidth;
		QColor m_cLineClr;
		double m_dLineAlpha;
		Qt::PenStyle m_eLineStyle;

		double m_dFillAlpha;
		QColor m_cFillClr;
	};
}