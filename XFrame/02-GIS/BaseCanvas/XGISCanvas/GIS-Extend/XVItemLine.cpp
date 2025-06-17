#include "XVItemLine.h"

#include <qgsgeometry.h>
#include "XCommonLib.h"

namespace XGIS
{
	XVItemLine::XVItemLine(void)
		: XVItem(XVItem::E_Line)
		,m_dLineWidth(1.0)
		,m_cLineClr(QColor(0,196,196))
		,m_dAlpha(0.75)
		,m_eLineStyle(Qt::SolidLine)
	{
		m_cLineData.clear();
	}

	XVItemLine::~XVItemLine(void)
	{
		m_cLineData.clear();
	}

	const bool XVItemLine::IsValid() const
	{
		if(E_Line != GetType() || 0 == GetID())
		{
			return false;
		}

		if(m_cLineData.count() < 2)
		{
			return false;
		}

		return true;
	}

	QgsGeometry* XVItemLine::GetGeom() const
	{
		if(false == IsValid())
		{
			return nullptr;
		}
		
		return QgsGeometry::fromPolyline(m_cLineData);
	}

	const bool XVItemLine::AddLineData(QgsPolyline& _cRefData)
	{
		if(_cRefData.count() < 2)
		{
			return false;
		}

		m_cLineData << _cRefData;
		UpdateLayerGeometry();
		return true;
	}

	void XVItemLine::AddLineData(QgsPoint _cLinePnt)
	{
		m_cLineData.append(_cLinePnt);
		UpdateLayerGeometry();
	}

	const QgsPolyline& XVItemLine::GetLineData() const
	{
		return m_cLineData;
	}

	double XVItemLine::GetLineWidth() const
	{
		return m_dLineWidth;
	}

	QColor XVItemLine::GetLineClr() const
	{
		QColor retClr = m_cLineClr;
		retClr.setAlphaF(m_dAlpha);
		return retClr;
	}

	Qt::PenStyle XVItemLine::GetLineStyle() const
	{
		return m_eLineStyle;
	}

	void XVItemLine::SetLineStyle(Qt::PenStyle _eStyle)
	{
		m_eLineStyle = _eStyle;
	}

	void XVItemLine::SetLineClr(QColor _cLineClr)
	{
		m_cLineClr = _cLineClr;
	}

	void XVItemLine::SetLineWidht(double _dWidth)
	{
		if(_dWidth <= 0)
		{
			return ;
		}
		m_dLineWidth = _dWidth;
	}

	void XVItemLine::SetLineAlpha(double _dAlpha)
	{
		if(_dAlpha < 0 )
		{
			_dAlpha = 0;
		}
		if(_dAlpha > 1)
			_dAlpha = 1;

		m_dAlpha = _dAlpha;
	}

}