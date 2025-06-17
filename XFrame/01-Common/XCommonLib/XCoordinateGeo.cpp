#include "XCoordinateGeo.h"
#include "XCoordinateCentric.h"

namespace XSpace_Common
{
	// 序列化宏
	xIMPLEMENT_SERIAL(XCoordinateGeo, XObject, XCOMLIB_VERSION_CURRENT)
	// 构造函数
	XCoordinateGeo::XCoordinateGeo()
	{
		m_dLontitude	= xCOOR_GEO_INVALID_VALUE;
		m_dLatitude		= xCOOR_GEO_INVALID_VALUE;
		m_dAltitude		= xCOOR_ALT_INVALID_VALUE;
	}
	// 构造函数
	XCoordinateGeo::XCoordinateGeo( double _dLon, double _dLat, double _dAlt )
	{
		m_dLontitude	= _dLon;
		m_dLatitude		= _dLat;
		m_dAltitude		= _dAlt;
	}
	// 析构函数
	XCoordinateGeo::~XCoordinateGeo()
	{
		m_dLontitude	= 0.0;
		m_dLatitude		= 0.0;
		m_dAltitude		= 0.0;
	}
	// 判断数据有效性
	const bool XCoordinateGeo::isValid() const
	{
		if( (m_dLontitude <= 180.0 && m_dLontitude >= -180.0)
			&& (m_dLatitude <= 90.0 && m_dLatitude >= -90.0)
			&& m_dAltitude > (xCOOR_ALT_INVALID_VALUE-1))
			return(true);
		else
			return(false);
	}
	// 转换至地心坐标系
	const bool XCoordinateGeo::toCentric(XCoordinateCentric& _xRefCoordinateCentric) const
	{
		bool bIsValid = isValid();
		if (bIsValid == false)
			return false;

		// 转换为弧度
		double dLon = m_dLontitude	* xDEG_RAD;
		double dLat = m_dLatitude	* xDEG_RAD;
		double dAlt = m_dAltitude	/ 1000.0;

		// N为纬度为B时的卯酉圈的曲率半径，单位：千米
		double dN	= xEARTH_R_A / sqrt( 1 - xEARTH_E2 * sin(dLat) * sin(dLat) );

		double dX = ( dN + dAlt ) * cos(dLat) * cos(dLon);
		double dY = ( dN + dAlt ) * cos(dLat) * sin(dLon);
		double dZ = ( dN * ( 1 - xEARTH_E2 ) + dAlt ) * sin(dLat);

		_xRefCoordinateCentric = XCoordinateCentric(dX*1000,dY*1000,dZ*1000);

		bIsValid = _xRefCoordinateCentric.isValid();
		if (bIsValid == false)
			return false;

		return true;
	}
	// 转换至北天东坐标系
	const bool XCoordinateGeo::toNSE(XCoordinateNSE& _xRefCoordinateNSE, const XCoordinateGeo& _xNSECenterGeo) const
	{
		bool bRes = false;

		if (isValid() == false)
			return false;

		XCoordinateCentric xGeoCentric;
		bRes = this->toCentric(xGeoCentric);
		if (false == bRes)
			return bRes;
		

		bRes = xGeoCentric.toNSE(_xRefCoordinateNSE, _xNSECenterGeo);
		if (false == bRes)
			return bRes;

		return true;
	}
	// 序列化函数
	void XCoordinateGeo::Serialize( XArchive& _xArchive )
	{
		if( !IsSerialize() )
			return;

		int nClassVersion = 0xFFFF;
		if(_xArchive.IsStoring())
		{
			nClassVersion = GetRuntimeClass()->GetClassVersion();
			if (nClassVersion >= XCOMLIB_VERSION_1)
			{
				XObject::Serialize(_xArchive);
				_xArchive<<m_dLontitude;
				_xArchive<<m_dLatitude;
				_xArchive<<m_dAltitude;
			}
		}else if( _xArchive.IsLoading() )
		{
			nClassVersion = _xArchive.GetObjectVersion();
			if (nClassVersion >= XCOMLIB_VERSION_1)
			{
				XObject::Serialize(_xArchive);
				_xArchive>>m_dLontitude;
				_xArchive>>m_dLatitude;
				_xArchive>>m_dAltitude;
			}
		}
	}

}
