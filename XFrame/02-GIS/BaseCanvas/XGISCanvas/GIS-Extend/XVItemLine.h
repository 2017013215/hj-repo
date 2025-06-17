#pragma once
#include "XVItem.h"
#include <QtCore/QPointF>
#include <QtGui/QColor>
#include <qgsgeometry.h>

namespace XGIS
{
	class XVItemLine :
		public XVItem
	{
	public:
		XVItemLine(void);
		//XVItemLine(QPointF _pos, QColor _cFillClr, QColor _cOutLineClr,double _dPntSize, double _dOutLineWidth,double _dOutLineAlpha=1.0,double _dAlpha = 1.0);
		virtual ~XVItemLine(void);

		virtual const bool IsValid() const;
		virtual QgsGeometry* GetGeom() const;

		const bool AddLineData(QgsPolyline& _cRefData);
		void AddLineData(QgsPoint _cLinePnt);
		const QgsPolyline& GetLineData() const;
		void SetLineWidht(double _dWidth);
		double GetLineWidth() const;
		void SetLineClr(QColor _cLineClr);
		QColor GetLineClr() const;
		Qt::PenStyle GetLineStyle() const;
		void SetLineStyle(Qt::PenStyle _eStyle);
		void SetLineAlpha(double _dAlpha);

	private:
		QgsPolyline m_cLineData;

		double m_dLineWidth;
		QColor m_cLineClr;
		double m_dAlpha;
		Qt::PenStyle m_eLineStyle;
	};
}