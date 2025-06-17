#pragma once
#include "XVItem.h"
#include <QtCore/QPointF>
#include <QtGui/QColor>
#include <qgspoint.h>

namespace XGIS
{
	class XVItemPoint :
		public XVItem
	{
	public:
		XVItemPoint(void);
		XVItemPoint(QPointF _pos, QColor _cFillClr, QColor _cOutLineClr,double _dPntSize, double _dOutLineWidth,double _dOutLineAlpha=1.0,double _dAlpha = 1.0);
		virtual ~XVItemPoint(void);

		virtual const bool IsValid() const;
		virtual QgsGeometry* GetGeom() const;

		void SetPos(QgsPoint _pos);
		const QgsPoint GetPos() const;

		QColor GetFillClr() const;
		QColor GetOutLineClr() const;
		double GetSize() const;
		double GetOutLineWidth() const;

		void SetSize(double _dPntSize);
		void SetFillClr(QColor _cFillClr);

	private:
		QgsPoint m_cPos;
		QColor m_cPntClr;
		double m_dAlpha;	
		double m_dPntSize;

		QColor m_cOutLineClr;
		double m_dOutLineAlpha;
		double m_dOutLineWidth;
	};
}