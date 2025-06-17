#include "XCoordinateCentric.h"
#include "XCoordinateGeo.h"
#include "XCoordinateNSE.h"

namespace XSpace_Common
{
	// 序列化宏实现
	xIMPLEMENT_SERIAL(XCoordinateCentric, XObject, XCOMLIB_VERSION_CURRENT)
	// 构造函数
	XCoordinateCentric::XCoordinateCentric()
	{
		m_dX	= 0.0;
		m_dY	= 0.0;
		m_dZ	= 0.0;
	}
	// 构造函数
	XCoordinateCentric::XCoordinateCentric( const double& _dX , const double& _dY, const double& _dZ )
	{
		m_dX	= _dX;
		m_dY	= _dY;
		m_dZ	= _dZ;
	}
	// 析构函数
	XCoordinateCentric::~XCoordinateCentric()
	{
		m_dX	= 0.0;
		m_dY	= 0.0;
		m_dZ	= 0.0;
	}
	// 判断数据有效性
	const bool XCoordinateCentric::isValid() const
	{
		return(true);
	}
	// 转换至经纬度坐标系
	const bool XCoordinateCentric::toGeo(XCoordinateGeo& _xCoordinateGeo) const
	{
		double dLon,dLat,dAlt;
		dLon = dLat = dAlt = 0.0;

		if ( m_dX == 0 && m_dY == 0 )
		{
			_xCoordinateGeo.m_dLontitude = 0.0;
			if ( m_dZ > 0 )
			{
				_xCoordinateGeo.m_dLatitude = 90.0;
				_xCoordinateGeo.m_dAltitude = m_dZ - xEARTH_R_B*1000;
			}else
			{
				_xCoordinateGeo.m_dLatitude = -90.0;
				_xCoordinateGeo.m_dAltitude = -m_dZ - xEARTH_R_B*1000;
			}
			return true;
		}else if ( m_dX == 0.0 )
		{
			if ( m_dY > 0.0 )
				dLon = xPI / 2.0;
			else
				dLon = -xPI / 2.0;
		}else
			dLon = atan( m_dY/m_dX );

		if ( m_dX < 0.0 )
		{
			if ( dLon > 0.0 )
				dLon -= xPI;
			else
				dLon += xPI;
		}

		// 计算纬度
		double dN,dH,dB;
		dN = dH = dB = 0.0;
		dN = xEARTH_R_A;
		dAlt = sqrt(m_dX*m_dX + m_dY*m_dY + m_dZ*m_dZ) - sqrt(xEARTH_R_A*xEARTH_R_B);
		dLat = atan( m_dZ / (sqrt(m_dX*m_dX + m_dY*m_dY) * (1 - (xEARTH_E2 * dN
			/ (dN+_xCoordinateGeo.m_dAltitude)))) );
		for (int n=0;n<20;n++)
		{
			dH = dAlt;
			dB = dLat;
			dN = xEARTH_R_A / sqrt(1-xEARTH_E2*sin(dLat)*sin(dLat));
			dAlt = sqrt( m_dX*m_dX + m_dY*m_dY )/ cos(dLat) - dN;
			dLat = atan ( m_dZ / (sqrt(m_dX*m_dX+m_dY*m_dY)*(1-(xEARTH_E2*dN/(dN+dAlt)))) );
			if ( fabs(dH - dAlt)<xMIN_FLOAT && fabs(dB-dLat)<xMIN_FLOAT )
				break;
		}

		// 弧度转换成度
		_xCoordinateGeo.m_dLontitude = dLon * xRAD_DEG;
		_xCoordinateGeo.m_dLatitude  = dLat * xRAD_DEG;
		_xCoordinateGeo.m_dAltitude	 = dAlt;

		return true;
	}
	// 转换至北天东坐标系
	const bool XCoordinateCentric::toNSE(XCoordinateNSE& _xRefCoordinateNSE, const XCoordinateGeo& _xNSECenterGeo) const
	{
		XCoordinateCentric xNSECenterCGCS2000;
		bool bRes = _xNSECenterGeo.toCentric(xNSECenterCGCS2000);
		if (false == bRes)
			return false;

		double dSinL, dSinB, dCosL, dCosB;
		dSinL = dSinB = dCosL = dCosB = 0.0;

		double dLon,dLat,dAlt;
		dLon = dLat = dAlt = 0.0;

		dLon = _xNSECenterGeo.m_dLontitude * xDEG_RAD;
		dLat = _xNSECenterGeo.m_dLatitude * xDEG_RAD;
		dSinL = sin(dLon);
		dCosL = cos(dLon);
		dSinB = sin(dLat);
		dCosB = cos(dLat);

		double dX,dY,dZ;
		dX = dY = dZ = 0.0;

		dX = m_dX - xNSECenterCGCS2000.m_dX;
		dY = m_dY - xNSECenterCGCS2000.m_dY;
		dZ = m_dZ - xNSECenterCGCS2000.m_dZ;

		_xRefCoordinateNSE.m_xOriginGeo	= _xNSECenterGeo;
		_xRefCoordinateNSE.m_dN			= -dSinB*dCosL*dX - dSinB*dSinL*dY + dCosB*dZ;
		_xRefCoordinateNSE.m_dS			= dCosB*dCosL*dX + dCosB*dSinL*dY + dSinB*dZ;
		_xRefCoordinateNSE.m_dE			= -dSinL*dX + dCosL*dY;

		if (!_xRefCoordinateNSE.isValid())
			return false;

		return true;
	}
	// 序列化函数
	void XCoordinateCentric::Serialize(XArchive& _xArchive)
	{
		if (!IsSerialize())
			return;

		int nClassVersion = 0xFFFF;
		if (_xArchive.IsStoring())
		{
			nClassVersion = GetRuntimeClass()->GetClassVersion();
			if (nClassVersion >= XCOMLIB_VERSION_1)
			{
				XObject::Serialize(_xArchive);
				_xArchive << m_dX;
				_xArchive << m_dY;
				_xArchive << m_dZ;
			}
		}
		else if (_xArchive.IsLoading())
		{
			nClassVersion = _xArchive.GetObjectVersion();
			if (nClassVersion >= XCOMLIB_VERSION_1)
			{
				XObject::Serialize(_xArchive);
				_xArchive >> m_dX;
				_xArchive >> m_dY;
				_xArchive >> m_dZ;
			}
		}
	}
}
