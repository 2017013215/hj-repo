#include "XVItemPolygon.h"

#include <qgsgeometry.h>
#include "XCommonLib.h"

namespace XGIS
{
	XVItemPolygon::XVItemPolygon(void)
		: XVItem(XVItem::E_Polygon)
		,m_dLineWidth(1.0)
		,m_cLineClr(QColor(0,196,196))
		,m_dLineAlpha(0.9)
		,m_eLineStyle(Qt::SolidLine)
		,m_cFillClr(QColor(255,128,128))
		,m_dFillAlpha(0.3)
	{
		m_cPolygonData.clear();
	}

	XVItemPolygon::~XVItemPolygon(void)
	{
		m_cPolygonData.clear();
	}

	void XVItemPolygon::SetFillClr(QColor _cFillClr, double _dAlpha)
	{
		m_cFillClr = _cFillClr;
		m_dFillAlpha = _dAlpha;
	}

	const bool XVItemPolygon::IsValid() const
	{
		if(E_Polygon != GetType() || 0 == GetID())
		{
			return false;
		}

		if(m_cPolygonData.count() < 3)
		{
			return false;
		}

		return true;
	}

	QgsGeometry* XVItemPolygon::GetGeom() const
	{
		if(false == IsValid())
		{
			return nullptr;
		}
		
		QgsPolygon polygon;
		polygon.append(m_cPolygonData);
		return QgsGeometry::fromPolygon(polygon);
	}

	void XVItemPolygon::AddPolygonData(QgsPolyline& _cRefData)
	{
		if(_cRefData.count() < 2)
		{
			return ;
		}

		m_cPolygonData << _cRefData;
		UpdateLayerGeometry();
	}

	void XVItemPolygon::AddPolygonData(QgsPoint& _cPolygonPnt)
	{
		m_cPolygonData.append(_cPolygonPnt);
		UpdateLayerGeometry();
	}

	const QgsPolyline& XVItemPolygon::GetPolygonData() const
	{
		return m_cPolygonData;
	}

	QColor XVItemPolygon::GetFillClr() const
	{
		QColor clr = m_cFillClr;
		clr.setAlphaF(m_dFillAlpha);
		return clr;
	}

	double XVItemPolygon::GetLineWidth() const
	{
		return m_dLineWidth;
	}

	QColor XVItemPolygon::GetLineClr() const
	{
		QColor retClr = m_cLineClr;
		retClr.setAlphaF(m_dLineAlpha);
		return retClr;
	}

	Qt::PenStyle XVItemPolygon::GetLineStyle() const
	{
		return m_eLineStyle;
	}

	void XVItemPolygon::SetLineStyle(Qt::PenStyle _eStyle)
	{
		m_eLineStyle = _eStyle;
	}

	void XVItemPolygon::SetLineClr(QColor _cLineClr)
	{
		m_cLineClr = _cLineClr;
	}

	void XVItemPolygon::SetLineWidht(double _dWidth)
	{
		if(_dWidth <= 0)
		{
			return ;
		}
		m_dLineWidth = _dWidth;
	}

	void XVItemPolygon::SetLineAlpha(double _dAlpha)
	{
		if(_dAlpha < 0 )
		{
			_dAlpha = 0;
		}
		if(_dAlpha > 1)
			_dAlpha = 1;

		m_dLineAlpha = _dAlpha;
	}

}