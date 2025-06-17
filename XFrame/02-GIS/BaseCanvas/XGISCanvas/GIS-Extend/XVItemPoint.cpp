#include "XVItemPoint.h"

#include <qgsgeometry.h>
#include "XCommonLib.h"

namespace XGIS
{
	XVItemPoint::XVItemPoint(void)
		: XVItem(XVItem::E_Point)
		,m_cPos(QPointF(0,0))
		,m_cPntClr(QColor(196,128,0))
		,m_dPntSize(4.0)
		,m_dAlpha(0.75)
		,m_cOutLineClr(QColor(0,0,0))
		,m_dOutLineAlpha(0.75)
		,m_dOutLineWidth(1.0)
	{
	}

	XVItemPoint::XVItemPoint(QPointF _pos, QColor _cFillClr, QColor _cOutLineClr,double _dPntSize, double _dOutLineWidth,double _dOutLineAlpha /*= 1.0*/,double _dAlpha /*= 1.0*/)
		: XVItem(XVItem::E_Point)
		,m_cPos(_pos)
		,m_cPntClr(_cFillClr)
		,m_cOutLineClr(_cOutLineClr)
		,m_dPntSize(_dPntSize)
		,m_dAlpha(_dAlpha)
		,m_dOutLineAlpha(_dOutLineAlpha)
		,m_dOutLineWidth(_dOutLineWidth)
	{

	}

	XVItemPoint::~XVItemPoint(void)
	{

	}

	const bool XVItemPoint::IsValid() const
	{
		if(E_Point != GetType() || 0 == GetID())
		{
			return false;
		}

		if(m_dPntSize <= 0)
		{
			return false;
		}

		return true;
	}

	void XVItemPoint::SetPos(QgsPoint _pos)
	{
		m_cPos = _pos;
		UpdateLayerGeometry();
	}

	const QgsPoint XVItemPoint::GetPos() const
	{
		return m_cPos;
	}

	QgsGeometry* XVItemPoint::GetGeom() const
	{
		if(false == IsValid())
		{
			return nullptr;
		}
		return QgsGeometry::fromPoint(m_cPos);
	}

	QColor XVItemPoint::GetFillClr() const
	{
		QColor retClr = m_cPntClr;
		retClr.setAlphaF(m_dAlpha);
		return retClr;
	}

	QColor XVItemPoint::GetOutLineClr() const
	{
		QColor retClr = m_cOutLineClr;
		retClr.setAlphaF(m_dOutLineAlpha);
		return retClr;
	}

	double XVItemPoint::GetSize() const
	{
		return m_dPntSize;
	}

	double XVItemPoint::GetOutLineWidth() const
	{
		return m_dOutLineWidth;
	}

	void XVItemPoint::SetSize(double _dPntSize)
	{
		if(0 == _dPntSize)
		{
			return ;
		}
		if(true == xDoubleNear(_dPntSize,m_dPntSize))
		{
			return ;
		}

		m_dPntSize = _dPntSize;
	}

	void XVItemPoint::SetFillClr(QColor _cFillClr)
	{
		m_cPntClr = _cFillClr;
	}
}