#include "GIS-Extend/XShapeDef.h"
#include "XJBInfoManager.h"
#include "JBDef.h"

namespace XGIS
{
#pragma region 图形基类
	xIMPLEMENT_SERIAL_VIRTURL(XShape,XObject,XVER_1_0)
	XShape::XShape(const QString& _strName /*= QString::null*/, EShapeType _eShapeType /*= eType_None*/)
		: m_unID(0)
		, m_eType(_eShapeType)
		, m_strName(_strName)
		, m_bVisible(true)
		, m_dAlpha(1.0)
		, m_bIsShowName(false)
		, m_qNameClr(Qt::black)
		, m_qNameFont(QFont(QString::fromLocal8Bit("宋体"),10))
		, m_dPenWidth(1.0)
		, m_ePenStyle(Qt::SolidLine)
		, m_ePenCapStyle(Qt::SquareCap)
		, m_ePenJoinStyle(Qt::BevelJoin)
	{
	}

	XShape::~XShape(void)
	{
		m_unID = 0;
		m_eType = eType_None;
		m_strName.clear();
		m_dAlpha = 0;
		m_bIsShowName = false;
		m_qNameClr = QColor();
		m_qNameFont = QFont();
		m_bVisible = false;

		m_dPenWidth = 0.0;
		m_ePenStyle = Qt::SolidLine;
		m_ePenCapStyle = Qt::SquareCap;
		m_ePenJoinStyle = Qt::BevelJoin;
	}

	const quint32 XShape::GetID() const
	{
		return m_unID;
	}

	const XShape::EShapeType XShape::GetType() const
	{
		return m_eType;
	}

	const QString& XShape::GetName() const
	{
		return m_strName;
	}

	void XShape::SetName(const QString& _strName)
	{
		m_strName = _strName;
	}

	const double XShape::GetAlpha() const
	{
		return m_dAlpha;
	}

	void XShape::SetAlpha(double _dAlpha)
	{
		if(_dAlpha < 0.0)
			m_dAlpha = 0.0;
		else if(_dAlpha > 1.0)
			m_dAlpha = 1.0;

		m_dAlpha = _dAlpha;
	}

	const bool XShape::GetIsShowName() const
	{
		return m_bIsShowName;
	}

	const QColor XShape::GetNameColor() const
	{
		QColor retClr = m_qNameClr;
		retClr.setAlphaF(m_dAlpha);
		return retClr;
	}

	const QFont& XShape::GetNameFont() const
	{
		return m_qNameFont;
	}

	void XShape::SetIsShowName(bool _bIsShow)
	{
		m_bIsShowName = _bIsShow;
	}

	void XShape::SetNameClr(QColor _qNameClr)
	{
		m_qNameClr = _qNameClr;
	}

	void XShape::SetNameFont(const QFont& _qNameFont)
	{
		m_qNameFont = _qNameFont;
	}

	void XShape::SetPenClr(QColor _qPenClr)
	{
		m_qPenClr = _qPenClr;
	}

	const bool XShape::SetPenWidth(double _dWidth)
	{
		if(_dWidth <= 0 )
			return false;

		m_dPenWidth = _dWidth;
		return true;
	}

	void XShape::SetPenStyle(Qt::PenStyle _ePenStyle)
	{
		m_ePenStyle = _ePenStyle;
	}

	void XShape::SetPenCapStyle(Qt::PenCapStyle _ePenCapStyle)
	{
		m_ePenCapStyle = _ePenCapStyle;
	}

	void XShape::SetPenJoinStyle(Qt::PenJoinStyle _ePenJoinStyle)
	{
		m_ePenJoinStyle = _ePenJoinStyle;
	}

	void XShape::SetVisible(const bool _bIsVisible)
	{
		m_bVisible = _bIsVisible;
	}

	const bool XShape::GetVisible() const
	{
		return m_bVisible;
	}

	const QColor XShape::GetPenClr() const
	{
		QColor retClr = m_qPenClr;
		retClr.setAlphaF(m_dAlpha);
		return retClr;
	}

	const double XShape::GetPenWidth() const
	{
		return m_dPenWidth;
	}

	Qt::PenStyle XShape::GetPenStyle() const
	{
		return m_ePenStyle;
	}

	Qt::PenCapStyle XShape::GetPenCapStyle() const
	{
		return m_ePenCapStyle;
	}

	Qt::PenJoinStyle XShape::GetPenJoinStyle() const
	{
		return m_ePenJoinStyle;
	}

	void XShape::Serialize(XArchive& _xArchive)
	{
		if (!IsSerialize())
			return;

		int nClassVersion = 0xFFFF;
		if (_xArchive.IsStoring())
		{
			nClassVersion = GetRuntimeClass()->GetClassVersion();
			if (nClassVersion >= XVER_1_0)
			{
				// 写入
				_xArchive << m_unID;
				_xArchive << m_eType;
				_xArchive << m_strName;
				_xArchive << m_bVisible;
				_xArchive << m_dAlpha;
				_xArchive << m_bIsShowName;
				_xArchive << m_qNameClr;
				_xArchive << m_qNameFont;
				_xArchive << m_qPenClr;
				_xArchive << m_dPenWidth;
				_xArchive << m_ePenStyle;
				_xArchive << m_ePenCapStyle;
				_xArchive << m_ePenJoinStyle;
			}
		}
		else if (_xArchive.IsLoading())
		{
			nClassVersion = _xArchive.GetObjectVersion();
			if (nClassVersion >= XVER_1_0)
			{
				// 读取
				int nEnum = 0;
				_xArchive >> m_unID;
				_xArchive >> nEnum;
				m_eType = EShapeType(nEnum);
				_xArchive >> m_strName;
				_xArchive >> m_bVisible;
				_xArchive >> m_dAlpha;
				_xArchive >> m_bIsShowName;
				_xArchive >> m_qNameClr;
				_xArchive >> m_qNameFont;
				_xArchive >> m_qPenClr;
				_xArchive >> m_dPenWidth;
				_xArchive >> nEnum;
				m_ePenStyle = Qt::PenStyle(nEnum);
				_xArchive >> nEnum;
				m_ePenCapStyle = Qt::PenCapStyle(nEnum);
				_xArchive >> nEnum;
				m_ePenJoinStyle = Qt::PenJoinStyle(nEnum);
			}
		}
	}

#pragma endregion

#pragma region 图形-点
	xIMPLEMENT_SERIAL(XPointShape,XShape,XVER_1_0)
	XPointShape::XPointShape(QPointF _qPointGeo /*= QPointF()*/, const QString& _strName /*= QString::null*/)
		: XShape(_strName,XShape::eType_Point)
		, m_xPointPos(_qPointGeo.x(),_qPointGeo.y(),0)
	{
		SetPenClr(Qt::darkGreen);
		SetPenWidth(5.0);
	}

	XPointShape::XPointShape(const XCoordinateGeo& _xPointGeo,const QString& _strName /*= QString::null*/)
		: XShape(_strName,XShape::eType_Point)
		, m_xPointPos(_xPointGeo)
	{
		SetPenClr(Qt::darkGreen);
		SetPenWidth(5.0);
	}

	XPointShape::~XPointShape()
	{
		m_xPointPos = XCoordinateGeo();
	}

	const bool XPointShape::IsValid() const
	{
		return m_xPointPos.isValid();
	}

	const XCoordinateGeo& XPointShape::GetPoint() const
	{
		return m_xPointPos;
	}

	const QPointF XPointShape::GetPos() const
	{
		return QPointF(m_xPointPos.m_dLontitude,m_xPointPos.m_dLatitude);
	}

	const bool XPointShape::SetPoint(const QPointF& _qPos)
	{
		XCoordinateGeo tempPos(_qPos.x(),_qPos.y(),0);
		if(false == tempPos.isValid())
			return false;
		m_xPointPos = tempPos;
		return true;
	}

	const bool XPointShape::SetPoint(const XCoordinateGeo& _qPos)
	{
		if(false == _qPos.isValid())
			return false;
		m_xPointPos = _qPos;
		return true;
	}

	void XPointShape::Serialize(XArchive& _xArchive)
	{
		if (!IsSerialize())
			return;

		int nClassVersion = 0xFFFF;
		if (_xArchive.IsStoring())
		{
			nClassVersion = GetRuntimeClass()->GetClassVersion();
			if (nClassVersion >= XVER_1_0)
			{
				// 写入
				XShape::Serialize(_xArchive);
				_xArchive.WriteObject(m_xPointPos);
			}
		}
		else if (_xArchive.IsLoading())
		{
			nClassVersion = _xArchive.GetObjectVersion();
			if (nClassVersion >= XVER_1_0)
			{
				// 读取
				XShape::Serialize(_xArchive);
				_xArchive.ReadObject(m_xPointPos);
			}
		}
	}

#pragma endregion

#pragma region 图形-线
	xIMPLEMENT_SERIAL(XLineShape,XShape,XVER_1_0)
	XLineShape::XLineShape(const QPolygonF& _qLinePnts /*= QPolygonF()*/,const QString& _strName /*= QString::null*/)
		: XShape(_strName,XShape::eType_Line)
	{
		SetPenClr(Qt::red);
		SetPenWidth(1.0);
		foreach(QPointF qPnt, _qLinePnts)
		{
			m_xLinePnts.append(XCoordinateGeo(qPnt.x(),qPnt.y(),0));
		}
	}

	XLineShape::XLineShape(const QVector<XCoordinateGeo>& _xLinePnts,const QString& _strName /*= QString::null*/)
		: XShape(_strName,XShape::eType_Line)
		, m_xLinePnts(_xLinePnts)
	{
		SetPenClr(Qt::red);
		SetPenWidth(1.0);
	}

	XLineShape::~XLineShape()
	{
		m_xLinePnts.clear();
	}

	const bool XLineShape::IsValid() const
	{
		if(m_xLinePnts.size() < 2)
			return false;
		int nPntCount = m_xLinePnts.count();
		for (int nIdx=0;nIdx<nPntCount;nIdx++)
		{
			if(false == m_xLinePnts[nIdx].isValid())
			{
				nPntCount = 0;
				return false;
			}
		}
		nPntCount = 0;
		return true;
	}

	const QPolygonF XLineShape::GetLinePos() const
	{
		QPolygonF qRet;

		int nPntCount = m_xLinePnts.count();
		for (int nIdx=0;nIdx<nPntCount;nIdx++)
		{
			qRet.append(QPointF(m_xLinePnts[nIdx].m_dLontitude,m_xLinePnts[nIdx].m_dLatitude));
		}
		nPntCount = 0;

		return qRet;
	}

	const QVector<XCoordinateGeo>& XLineShape::GetLinePnts() const
	{
		return m_xLinePnts;
	}

	const bool XLineShape::AddPnts(const QPolygonF& _qLinePnts)
	{
		if(true == _qLinePnts.isEmpty())
			return false;

		int nCount = _qLinePnts.size();
		QVector<XCoordinateGeo> tempPos;
		XCoordinateGeo xGeoPnt;
		for (int nIdx = 0; nIdx < nCount; nIdx ++)
		{
			xGeoPnt = XCoordinateGeo(_qLinePnts[nIdx].x(),_qLinePnts[nIdx].y(),0);
			if(false == xGeoPnt.isValid())
			{
				tempPos.clear();
				nCount = 0;
				return false;
			}
			tempPos.append(xGeoPnt);
		}

		m_xLinePnts << tempPos;
		tempPos.clear();
		nCount = 0;
		return true;
	}

	const bool XLineShape::AddPnts(const QVector<XCoordinateGeo>& _xLinePnts)
	{
		if(true == _xLinePnts.isEmpty())
			return false;

		int nCount = _xLinePnts.count();
		for (int nIdx =0; nIdx < nCount; nIdx ++)
		{
			if(false == _xLinePnts[nIdx].isValid())
			{
				nCount = 0;
				return false;
			}
		}

		m_xLinePnts << _xLinePnts;
		nCount = 0;
		return true;
	}

	const quint32 XLineShape::GetPntCount() const
	{
		return m_xLinePnts.count();
	}

	const bool XLineShape::ReplacePnt(const quint32 _unIdx, const XCoordinateGeo& _xNewGeo)
	{
		if(_unIdx >= m_xLinePnts.count())
			return false;

		m_xLinePnts[_unIdx] = _xNewGeo;
		return true;
	}

	const bool XLineShape::ReplaceLast(const XCoordinateGeo& _xNewGeo)
	{
		if(true == m_xLinePnts.isEmpty())
			return false;

		m_xLinePnts[m_xLinePnts.count()-1] = _xNewGeo;
		return true;
	}

	const bool XLineShape::AddPnt(const XCoordinateGeo& _xPntGeo)
	{
		if(false == _xPntGeo.isValid())
			return false;
		m_xLinePnts.append(_xPntGeo);
		return true;
	}

	void XLineShape::Serialize(XArchive& _xArchive)
	{
		if (!IsSerialize())
			return;

		int nClassVersion = 0xFFFF;
		if (_xArchive.IsStoring())
		{
			nClassVersion = GetRuntimeClass()->GetClassVersion();
			if (nClassVersion >= XVER_1_0)
			{
				// 写入
				XShape::Serialize(_xArchive);
				_xArchive<<m_xLinePnts.count();
				for(int nIdx=0;nIdx<m_xLinePnts.count();++nIdx)
				{
					_xArchive.WriteObject(m_xLinePnts[nIdx]);
				}
			}
		}
		else if (_xArchive.IsLoading())
		{
			nClassVersion = _xArchive.GetObjectVersion();
			if (nClassVersion >= XVER_1_0)
			{
				// 读取
				XShape::Serialize(_xArchive);
				int nPntCount = 0;
				_xArchive>>nPntCount;
				m_xLinePnts.resize(nPntCount);
				for (int nIdx=0;nIdx<nPntCount;++nIdx)
				{
					_xArchive.ReadObject(m_xLinePnts[nIdx]);
				}
			}
		}
	}

	const bool XLineShape::SetPnts(const QVector<XCoordinateGeo>& _xLinePnts)
	{
		if(true == _xLinePnts.isEmpty())
			return false;

		int nCount = _xLinePnts.count();
		for (int nIdx =0 ; nIdx < nCount; nIdx ++)
		{
			if(false == _xLinePnts[nIdx].isValid())
			{
				nCount = 0;
				return false;
			}
		}

		m_xLinePnts.clear();
		m_xLinePnts << _xLinePnts;
		nCount = 0;
		return true;
	}

#pragma endregion

#pragma region 图形-点状JB
	xIMPLEMENT_SERIAL_VIRTURL(XShapePointJB,XShape,XVER_1_0)
	XShapePointJB::XShapePointJB(const XCoordinateGeo& _xPos /*= XCoordinateGeo()*/, quint32 _unLibID /*= 0*/, quint32 _unItemID /*= 0*/, const QString& _strName /*= QString::null*/)
		: XShape(_strName,XShape::eType_PointJB)
		, m_unLibID(_unLibID)
		, m_unItemID(_unItemID)
		, m_xPosGeo(_xPos)
		, m_qSize(60,60)
		, m_dHeading(0.0)
	{

	}

	XShapePointJB::~XShapePointJB()
	{
		m_xPosGeo = XCoordinateGeo();
		m_unLibID = m_unItemID = 0;
		m_qSize = QSize();
		m_dHeading = 0.0;
		m_qUserClr = QColor();
	}

	const bool XShapePointJB::IsValid() const
	{
		if(m_unLibID == 0 || m_unItemID == 0)
			return false;

		if(false == m_xPosGeo.isValid())
			return false;

		return true;
	}

	const bool XShapePointJB::SetPos(const QPointF& _qPos)
	{
		XCoordinateGeo tempPos(_qPos.x(),_qPos.y(),0);
		if(false == tempPos.isValid())
			return false;
		m_xPosGeo = tempPos;
		return true;
	}

	const bool XShapePointJB::SetPosGeo(const XCoordinateGeo& _xPosGeo)
	{
		if(false == _xPosGeo.isValid())
			return false;
		m_xPosGeo = _xPosGeo;
		return true;
	}

	const double XShapePointJB::GetHeading() const
	{
		return m_dHeading;
	}

	void XShapePointJB::SetHeading(double _dHeading)
	{
		if(_dHeading > 360.0)
			_dHeading = 360.0;
		else if(_dHeading < 0.0)
			_dHeading = 0.0;

		m_dHeading = _dHeading;
	}

	const quint32 XShapePointJB::GetMarkLibID() const
	{
		return m_unLibID;
	}

	const quint32 XShapePointJB::GetMarkItemID() const
	{
		return m_unItemID;
	}

	const QSize XShapePointJB::GetSize() const
	{
		 return m_qSize;
	}

	const bool XShapePointJB::SetSize(const QSize& _qSize)
	{
		if(false == _qSize.isValid())
			return false;

		m_qSize = _qSize;
		return true;
	}

	const QPointF XShapePointJB::GetPos() const
	{
		return QPointF(m_xPosGeo.m_dLontitude,m_xPosGeo.m_dLatitude);
	}

	void XShapePointJB::SetUserClr(const QColor _qUserClr /*= QColor()*/)
	{
		SetPenClr(_qUserClr);
		m_qUserClr = _qUserClr;
	}

	const QColor XShapePointJB::GetUserClr() const
	{
		QColor resClr = m_qUserClr;
		resClr.setAlphaF(GetAlpha());
		return resClr;
	}

	void XShapePointJB::Serialize(XArchive& _xArchive)
	{
		if (!IsSerialize())
			return;

		int nClassVersion = 0xFFFF;
		if (_xArchive.IsStoring())
		{
			nClassVersion = GetRuntimeClass()->GetClassVersion();
			if (nClassVersion >= XVER_1_0)
			{
				// 写入
				XShape::Serialize(_xArchive);
				_xArchive<<m_unLibID;
				_xArchive<<m_unItemID;
				_xArchive.WriteObject(m_xPosGeo);
				_xArchive<<m_dHeading;
				_xArchive<<m_qSize;
				_xArchive<<m_qUserClr;
			}
		}
		else if (_xArchive.IsLoading())
		{
			nClassVersion = _xArchive.GetObjectVersion();
			if (nClassVersion >= XVER_1_0)
			{
				// 读取
				XShape::Serialize(_xArchive);
				_xArchive>>m_unLibID;
				_xArchive>>m_unItemID;
				_xArchive.WriteObject(m_xPosGeo);
				_xArchive>>m_dHeading;
				_xArchive>>m_qSize;
				_xArchive>>m_qUserClr;
			}
		}
	}

#pragma endregion

#pragma region 图形-线JB
	xIMPLEMENT_SERIAL(XShapeLineJB,XShape,XVER_1_0)
	XShapeLineJB::XShapeLineJB(quint32 _unLibID /*= 0*/, quint32 _unItemID /*= 0*/, const QString& _strName /*= QString::null*/)
		: XShape(_strName,XShape::eType_LineJB)
		, m_unLibID(_unLibID)
		, m_unItemID(_unItemID)
		, m_pExceptiveInfo(nullptr)

	{
		SetPenWidth(2.0);
		m_xLinePnts.clear();
		m_pExceptiveInfo = XJBInfoManager::GetInstance()->GetItemExceptiveInfo(_unLibID,_unItemID);
		Q_ASSERT(m_pExceptiveInfo);
	}

	XShapeLineJB::~XShapeLineJB()
	{
		m_unLibID = m_unItemID = 0;
		m_xLinePnts.clear();
		m_pExceptiveInfo = nullptr;
	}

	const bool XShapeLineJB::IsValid() const
	{
		if(0 == m_unLibID || 0 == m_unItemID)
			return false;
		if(nullptr == m_pExceptiveInfo)
			return false;

// 		if(m_xLinePnts.count() < m_pExceptiveInfo->unMinPoint || m_xLinePnts.count() > m_pExceptiveInfo->unMaxPoint)
// 		{
// 			return false;
// 		}

		return true;
	}

	const bool XShapeLineJB::AddPnts(const QVector<XCoordinateGeo>& _xLinePnts)
	{
		if(true == _xLinePnts.isEmpty())
			return false;

		int nCount = _xLinePnts.count();
		for (int nIdx =0 ; nIdx < nCount; nIdx ++)
		{
			if(false == _xLinePnts[nIdx].isValid())
			{
				nCount = 0;
				return false;
			}
		}

		m_xLinePnts << _xLinePnts;
		nCount = 0;
		return true;
	}

	const QPolygonF XShapeLineJB::GetLinePnts() const
	{
		QPolygonF poly;

		for (int nIdx=0;nIdx<m_xLinePnts.count(); ++nIdx)
		{
			poly.append(QPointF(m_xLinePnts[nIdx].m_dLontitude,m_xLinePnts[nIdx].m_dLatitude));
		}

		return poly;
	}

	const bool XShapeLineJB::ReplacePnt(const quint32 _unIdx, const XCoordinateGeo& _xNewGeo)
	{
		if(_unIdx >= m_xLinePnts.count())
			return false;

		m_xLinePnts[_unIdx] = _xNewGeo;
		return true;
	}

	const bool XShapeLineJB::AddPnt(const XCoordinateGeo& _xPntGeo)
	{
		if(false == _xPntGeo.isValid())
			return false;

		m_xLinePnts.append(_xPntGeo);
		return true;
	}

	const bool XShapeLineJB::ReplaceLast(const XCoordinateGeo& _xNewGeo)
	{
		if(true == m_xLinePnts.isEmpty())
			return false;

		m_xLinePnts[m_xLinePnts.count()-1] = _xNewGeo;
		return true;
	}

	const quint32 XShapeLineJB::GetPntCount() const
	{
		return m_xLinePnts.count();
	}

	const ExceptiveInfo* XShapeLineJB::GetExceptiveInfo() const
	{
		return m_pExceptiveInfo;
	}

	void XShapeLineJB::Serialize(XArchive& _xArchive)
	{
		if (!IsSerialize())
			return;

		int nClassVersion = 0xFFFF;
		if (_xArchive.IsStoring())
		{
			nClassVersion = GetRuntimeClass()->GetClassVersion();
			if (nClassVersion >= XVER_1_0)
			{
				// 写入
				XShape::Serialize(_xArchive);
				_xArchive<<m_unLibID;
				_xArchive<<m_unItemID;
				_xArchive<<m_xLinePnts.count();
				for(int nIdx=0;nIdx<m_xLinePnts.count();++nIdx)
				{
					_xArchive.WriteObject(m_xLinePnts[nIdx]);
				}
			}
		}
		else if (_xArchive.IsLoading())
		{
			nClassVersion = _xArchive.GetObjectVersion();
			if (nClassVersion >= XVER_1_0)
			{
				// 读取
				XShape::Serialize(_xArchive);
				_xArchive>>m_unLibID;
				_xArchive>>m_unItemID;
				int nPntCount = 0;
				_xArchive>>nPntCount;
				m_xLinePnts.resize(nPntCount);
				for (int nIdx=0;nIdx<nPntCount;++nIdx)
				{
					_xArchive.ReadObject(m_xLinePnts[nIdx]);
				}
				m_pExceptiveInfo = XJBInfoManager::GetInstance()->GetItemExceptiveInfo(m_unLibID,m_unItemID);
			}
		}
	}

	const bool XShapeLineJB::SetPnts(const QVector<XCoordinateGeo>& _xLinePnts)
	{
		if(true == _xLinePnts.isEmpty())
			return false;

		int nCount = _xLinePnts.count();
		for (int nIdx =0 ; nIdx < nCount; nIdx ++)
		{
			if(false == _xLinePnts[nIdx].isValid())
			{
				nCount = 0;
				return false;
			}
		}

		m_xLinePnts.clear();
		m_xLinePnts << _xLinePnts;
		nCount = 0;
		return true;
	}

#pragma endregion

#pragma region 图形-多边形
	xIMPLEMENT_SERIAL(XPolygonShape,XShape,XVER_1_0)
	XPolygonShape::XPolygonShape(EPolygonType _eType, const QVector<XCoordinateGeo>& _xLinePnts/*= QVector<XCoordinateGeo>()*/,const QString& _strName /*= QString::null*/)
		: XShape(_strName,XShape::eType_Polygon)
		, m_ePolygonType(_eType)
		, m_unMinPnt(0)
		, m_unMaxPnt(0)
		, m_qFillClr(Qt::green)
		, m_dFillAlpha(0.3)
		, m_eFillStyle(Qt::SolidPattern)
		, m_strFillImgPath(QString::null)
	{
		switch(_eType)
		{
		case ePolygon_Polygon:
			{
				m_unMinPnt = 3;
				m_unMaxPnt = 2000;
			}break;
		case ePolygon_Rect:
		case ePolygon_Circle:
		case ePolygon_Ellipses:
			{
				m_unMinPnt = 2;
				m_unMaxPnt = 2;
			}break;
		default:
			Q_ASSERT(false);
			break;
		}

		if(false == _xLinePnts.isEmpty())
			AddPnts(_xLinePnts);
	}

	XPolygonShape::~XPolygonShape()
	{
		m_xLinePnts.clear();
	}

	const QVector<XCoordinateGeo>& XPolygonShape::GetLinePnts() const
	{
		return m_xLinePnts;
	}

	const quint32 XPolygonShape::GetPntCount() const
	{
		return m_xLinePnts.count();
	}

	const bool XPolygonShape::AddPnts(const QVector<XCoordinateGeo>& _xLinePnts)
	{
		if(true == _xLinePnts.isEmpty())
			return false;
		if(m_xLinePnts.count() + _xLinePnts.count() > m_unMaxPnt)
			return false;

		int nCount = _xLinePnts.count();
		for (int nIdx =0; nIdx < nCount; nIdx ++)
		{
			if(false == _xLinePnts[nIdx].isValid())
			{
				nCount = 0;
				return false;
			}
		}

		m_xLinePnts << _xLinePnts;
		nCount = 0;
		return true;
	}

	const bool XPolygonShape::AddPnt(const XCoordinateGeo& _xPntGeo)
	{
		if(false == _xPntGeo.isValid())
			return false;

		if(m_xLinePnts.count()+1 > m_unMaxPnt)
			return false;

		m_xLinePnts.append(_xPntGeo);
		return true;
	}

	const bool XPolygonShape::ReplacePnt(const quint32 _unIdx, const XCoordinateGeo& _xNewGeo)
	{
		if(_unIdx >= m_xLinePnts.count())
			return false;

		m_xLinePnts[_unIdx] = _xNewGeo;
		return true;
	}

	const bool XPolygonShape::ReplaceLast(const XCoordinateGeo& _xNewGeo)
	{
		if(true == m_xLinePnts.isEmpty())
			return false;

		m_xLinePnts[m_xLinePnts.count()-1] = _xNewGeo;
		return true;
	}

	const bool XPolygonShape::IsValid() const
	{
		if(true == m_xLinePnts.isEmpty())
			return false;

		if(0 == m_unMinPnt || 0 == m_unMaxPnt)
			return false;

		if(m_xLinePnts.count() < m_unMinPnt)
			return false;
		if(m_xLinePnts.count() > m_unMaxPnt)
			return false;

		return true;
	}

	const quint32 XPolygonShape::GetMinPnts() const
	{
		return m_unMinPnt;
	}

	const quint32 XPolygonShape::GetMaxPnts() const
	{
		return m_unMaxPnt;
	}

	const XPolygonShape::EPolygonType XPolygonShape::GetPolygonType() const
	{
		return m_ePolygonType;
	}

	void XPolygonShape::SetFillClr(QColor _qFillClr)
	{
		m_qFillClr = _qFillClr;
	}

	void XPolygonShape::SetFillAlpha(double _dFillAlpha)
	{
		if(_dFillAlpha > 1.0 )
			_dFillAlpha = 1.0;
		else if(_dFillAlpha < 0.0)
			_dFillAlpha = 0.0;
		m_dFillAlpha = _dFillAlpha;
	}

	void XPolygonShape::SetFillStyle(Qt::BrushStyle _eFillStyle)
	{
		m_eFillStyle = _eFillStyle;
		if(m_eFillStyle != Qt::TexturePattern)
			m_strFillImgPath.clear();
	}

	void XPolygonShape::SetFillImg(const QString& _strImgPath)
	{
		if(Qt::TexturePattern != m_eFillStyle)
			return ;
		if(false == QFile::exists(_strImgPath))
			return ;
		m_qFillImg = QImage(_strImgPath);
		if(true == m_qFillImg.isNull())
			return ;

		m_strFillImgPath = _strImgPath;
	}

	const QColor XPolygonShape::GetFillClr() const
	{
		QColor retClr = m_qFillClr;
		retClr.setAlphaF(m_dFillAlpha);
		return retClr;
	}

	const double XPolygonShape::GetFillAlpha() const
	{
		return m_dFillAlpha;
	}

	Qt::BrushStyle XPolygonShape::GetFillStyle() const
	{
		return m_eFillStyle;
	}

	const QImage& XPolygonShape::GetFillImg() const
	{
		return m_qFillImg;
	}

	void XPolygonShape::Serialize(XArchive& _xArchive)
	{
		if (!IsSerialize())
			return;

		int nClassVersion = 0xFFFF;
		if (_xArchive.IsStoring())
		{
			nClassVersion = GetRuntimeClass()->GetClassVersion();
			if (nClassVersion >= XVER_1_0)
			{
				// 写入
				XShape::Serialize(_xArchive);
				_xArchive<<m_ePolygonType;
				_xArchive<<m_unMinPnt;
				_xArchive<<m_unMaxPnt;
				_xArchive<<m_qFillClr;
				_xArchive<<m_dFillAlpha;
				_xArchive<<m_eFillStyle;
				_xArchive<<m_strFillImgPath;
				_xArchive<<m_qFillImg;
				_xArchive<<m_xLinePnts.count();
				for(int nIdx=0;nIdx<m_xLinePnts.count();++nIdx)
				{
					_xArchive.WriteObject(m_xLinePnts[nIdx]);
				}
			}
		}
		else if (_xArchive.IsLoading())
		{
			nClassVersion = _xArchive.GetObjectVersion();
			if (nClassVersion >= XVER_1_0)
			{
				// 读取
				XShape::Serialize(_xArchive);
				int nEnum = 0;
				_xArchive>>nEnum;
				m_ePolygonType = EPolygonType(nEnum);
				_xArchive>>m_unMinPnt;
				_xArchive>>m_unMaxPnt;
				_xArchive>>m_qFillClr;
				_xArchive>>m_dFillAlpha;
				_xArchive>>nEnum;
				m_eFillStyle = Qt::BrushStyle(nEnum);
				_xArchive>>m_strFillImgPath;
				_xArchive>>m_qFillImg;
				int nPntCount = 0;
				_xArchive>>nPntCount;
				m_xLinePnts.resize(nPntCount);
				for (int nIdx=0;nIdx<nPntCount;++nIdx)
				{
					_xArchive.ReadObject(m_xLinePnts[nIdx]);
				}
			}
		}
	}

	XPolygonShape::XPolygonShape()
		:XShape("",XShape::eType_Polygon)
		, m_ePolygonType(XPolygonShape::ePolygon_Unknown)
		, m_unMinPnt(0)
		, m_unMaxPnt(0)
		, m_qFillClr(Qt::green)
		, m_dFillAlpha(0.0)
		, m_eFillStyle(Qt::SolidPattern)
		, m_strFillImgPath(QString::null)
	{
		m_xLinePnts.clear();
	}

	const bool XPolygonShape::SetPnts(const QVector<XCoordinateGeo>& _xLinePnts)
	{
		if(true == _xLinePnts.isEmpty())
			return false;
		if(_xLinePnts.count() > m_unMaxPnt)
			return false;

		int nCount = _xLinePnts.count();
		for (int nIdx =0; nIdx < nCount; nIdx ++)
		{
			if(false == _xLinePnts[nIdx].isValid())
			{
				nCount = 0;
				return false;
			}
		}

		m_xLinePnts.clear();
		m_xLinePnts << _xLinePnts;
		nCount = 0;
		return true;
	}

#pragma endregion
}