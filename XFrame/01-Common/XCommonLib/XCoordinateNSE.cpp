#include "XCoordinateNSE.h"
#include "XCoordinateGeo.h"
#include "XCoordinateCentric.h"

namespace XSpace_Common
{
	/************************************************************************/
	/* 北天东坐标系                                                         */
	/************************************************************************/
	// 序列化宏定义
	xIMPLEMENT_SERIAL(XCoordinateNSE,XObject,XCOMLIB_VERSION_CURRENT)
	// 构造函数
	XCoordinateNSE::XCoordinateNSE( XCoordinateGeo _xOriginGeo )
	{
		m_xOriginGeo = _xOriginGeo;
		m_dN = 0.0;
		m_dS = 0.0;
		m_dE = 0.0;
	}
	// 构造函数
	XCoordinateNSE::XCoordinateNSE(  XCoordinateGeo _xOriginGeo, double _dN, double _dS, double _dE )
	{
		m_xOriginGeo = _xOriginGeo;
		m_dN	= _dN;
		m_dS	= _dS;
		m_dE	= _dE;
	}
	// 构造函数
	XCoordinateNSE::XCoordinateNSE()
	{
		m_xOriginGeo = XCoordinateGeo();
		m_dN = m_dS = m_dE = 0.0;
	}
	// 析构函数
	XCoordinateNSE::~XCoordinateNSE()
	{
		m_xOriginGeo = XCoordinateGeo();
		m_dN = m_dS = m_dE = 0.0;
	}
	// 判断数据有效性
	const bool XCoordinateNSE::isValid() const
	{
		bool bRes = m_xOriginGeo.isValid();
		
		return bRes;
	}
	// 转换至地心坐标系
	const bool XCoordinateNSE::toCentric(XCoordinateCentric& _xRefCoordinateCentric) const
	{
		bool bRes = false;
		
		if (isValid() == false)
			return bRes;

		// 计算
		XCoordinateCentric xOriginCentric;
		bRes = m_xOriginGeo.toCentric(xOriginCentric);
		if( false == bRes )
			return bRes;

		double dSinL,dSinB,dCosL,dCosB;

		double dLon = m_xOriginGeo.m_dLontitude * xDEG_RAD;
		double dLat = m_xOriginGeo.m_dLatitude * xDEG_RAD;

		dSinL = sin(dLon);
		dCosL = cos(dLon);
		dSinB = sin(dLat);
		dCosB = cos(dLat);

		_xRefCoordinateCentric.m_dX = xOriginCentric.m_dX - dSinB*dCosL*m_dN + dCosB*dCosL*m_dS - dSinL*m_dE;
		_xRefCoordinateCentric.m_dY = xOriginCentric.m_dY - dSinB*dSinL*m_dN + dCosB*dCosL*m_dS + dCosL*m_dE;
		_xRefCoordinateCentric.m_dZ = xOriginCentric.m_dZ + dCosB*m_dN + dSinB*m_dS;

		bRes = _xRefCoordinateCentric.isValid();
		if (false == bRes)
			return bRes;

		return true;
	}
	// 转换至地理坐标系
	const bool XCoordinateNSE::toGeo(XCoordinateGeo& _xRefCoordinateGeo) const
	{
		bool bRes = false;

		if (isValid() == false)
			return bRes;

		XCoordinateCentric xNSECentricCGCS2000;
		bRes = this->toCentric(xNSECentricCGCS2000);
		if ( false == bRes )
			return bRes;

		bRes = xNSECentricCGCS2000.toGeo(_xRefCoordinateGeo);
		if ( false == bRes )
			return bRes;

		return true;
	}
	// 转换至北天东坐标系
	const bool XCoordinateNSE::toNSE(XCoordinateNSE& _xRefCoordinateNSE, const XCoordinateGeo& _xCenterGeo) const
	{
		bool bRes = false;

		if (isValid() == false)
			return bRes;

		XCoordinateCentric xCoordinateCentricThis;
		bRes = this->toCentric(xCoordinateCentricThis);
		if (false == bRes)
			return bRes;

		bRes = xCoordinateCentricThis.toNSE(_xRefCoordinateNSE, _xCenterGeo);
		if ( false == bRes )
			return bRes;

		return true;
	}
	// 序列化函数
	void XCoordinateNSE::Serialize( XArchive& _xArchive )
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
				_xArchive<<m_xOriginGeo;
				_xArchive<<m_dN;
				_xArchive<<m_dS;
				_xArchive<<m_dE;
			}
		}else if( _xArchive.IsLoading() )
		{
			nClassVersion = _xArchive.GetObjectVersion();
			if (nClassVersion >= XCOMLIB_VERSION_1)
			{
				XObject::Serialize(_xArchive);
				_xArchive>>m_xOriginGeo;
				_xArchive>>m_dN;
				_xArchive>>m_dS;
				_xArchive>>m_dE;
			}
		}
	}
}
