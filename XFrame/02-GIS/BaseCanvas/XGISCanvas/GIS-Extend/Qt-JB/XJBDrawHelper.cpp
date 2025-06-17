#include "XJBDrawHelper.h"
#include "JBDef.h"
#include "QRect"
#include "GIS-Extend\XShapeDef.h"
#include "XJBInfoManager.h"
#include "plotUtils.h"


#define GetAValue(rgba)  ((unsigned char)((rgba) >> 24))
#define GetRValue(rgb)   ((unsigned char) (rgb))
#define GetGValue(rgb)   ((unsigned char) (((unsigned short) (rgb)) >> 8))
#define GetBValue(rgb)   ((unsigned char) ((rgb) >> 16))
#define MakeColor(rgba)   QColor(GetRValue(rgba),GetGValue(rgba),GetBValue(rgba),255-GetAValue(rgba))

#define DEFAULT_POINTS_COUNT 4096

namespace XGIS
{
	QPainter* XJBDrawHelper::ms_pPainter = nullptr;
	float XJBDrawHelper::ms_fJBFactor = 0.70f;
	float XJBDrawHelper::ms_fTextFactor = 0.75f;
	QRect XJBDrawHelper::ms_cBoxRct = QRect(0,0,0,0);
	double XJBDrawHelper::ms_dScale = 1;
	double XJBDrawHelper::ms_dScaleX = 1;
	double XJBDrawHelper::ms_dScaleY = 1;
	bool XJBDrawHelper::ms_bIsUseUserClr = false;
	QColor XJBDrawHelper::ms_cUserClr = Qt::red;
	quint16 XJBDrawHelper::ms_unBezierPrecision = 15;
	const int XJBDrawHelper::ms_nConstCurveFlag = 0x100;

	XJBDrawHelper::XJBDrawHelper()
	{

	}

	XJBDrawHelper::~XJBDrawHelper()
	{

	}

	const bool XJBDrawHelper::DrawPointJB(QPainter* _p, const XShapePointJB* _pShapePtr, QPointF& _qScreenPnt, bool _bIsDetail /*= false*/, bool _bIsUserAngle /*= true*/)
	{
		if(nullptr == _pShapePtr || nullptr == _p)
		{
			return false;
		}

		if(false == _pShapePtr->IsValid())
		{
			return false;
		}

		const Q_JBItem* pItem = XJBInfoManager::GetInstance()->FindJBItem(_pShapePtr->GetMarkLibID(),_pShapePtr->GetMarkItemID());
		if(nullptr == pItem)
			return false;

		ms_pPainter = _p;

		// 军标位置计算
		ms_cBoxRct = pItem->rectBound;
		QSize qJBSize = _pShapePtr->GetSize();
		ms_dScaleX = (float)qJBSize.width() / ms_cBoxRct.width() * ms_fJBFactor;
		ms_dScaleY = (float)qJBSize.height() / ms_cBoxRct.height() * ms_fJBFactor;
		ms_dScale = std::min(ms_dScaleX,ms_dScaleY);

		// 绘制名称
		QString strName = _pShapePtr->GetName();
		if(false == strName.isEmpty())
		{
			QColor qNameClr = _pShapePtr->GetNameColor();
			QFont qNameFont = _pShapePtr->GetNameFont();

			ms_pPainter->setPen(QPen(qNameClr));
			ms_pPainter->setFont(qNameFont);

			QPointF textPos = _qScreenPnt;
			textPos.setY(textPos.y()+30);

			ms_pPainter->drawText(textPos,strName);

		}

		ms_pPainter->save();

		// 判断是否绘制动态线--如果绘制静态线，将只使用设定为中心点，其他动态点将不被使用
		ms_pPainter->translate(_qScreenPnt);
		// 旋转
		double dHeading = _pShapePtr->GetHeading();
		//角度修正
		if(true == _bIsUserAngle)
		{
			// 计算两点连线角度，正北为0
			double dOrAng = sfnGetPntAngle(QPointF(0,0),QPointF(pItem->cHotSpot.x(),-pItem->cHotSpot.y()));
			dHeading += 360.0-dOrAng;
		}
		ms_pPainter->rotate(dHeading);

		QColor qUserClr = _pShapePtr->GetUserClr();
		ms_cUserClr = qUserClr;
		if(ms_cUserClr.isValid())
		{
			ms_bIsUseUserClr = true;
			
		}else
		{
			ms_bIsUseUserClr = false;
		}

		// 计算包围盒
		ms_pPainter->translate(-ms_cBoxRct.center().x()*ms_dScale,ms_cBoxRct.center().y()*ms_dScale);
		ms_pPainter->scale(ms_dScale, -ms_dScale);
		ms_pPainter->setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);

		// 绘制军标
		foreach(Q_JBItemUnitInfo unitInfo,pItem->cDrawUnitInfo)
		{
			switch(unitInfo.unType)
			{
			case U_BLINE:	// 曲线&直线
				{
					dwBLine(unitInfo,_bIsDetail);
				}
				break;
			case U_BBLINE:
				{
					dwBBLine(unitInfo,_bIsDetail);
				}break;
			case U_PLINE:
			case U_LINE:
				{
					dwPLine(unitInfo,_bIsDetail);
				}break;
			case U_RECT:
				{
					dwRect(unitInfo,_bIsDetail);
				}break;
			case U_ELLI:
				{
					dwEllipse(unitInfo,_bIsDetail);
				}break;
			case U_PIE:
				{
					dwPie(unitInfo,_bIsDetail);
				}break;
			case U_ARC:
				{
					dwArc(unitInfo,_bIsDetail);
				}break;
			case U_PGON:
				{
					dwPolygon(unitInfo,_bIsDetail);
				}break;
			case U_TEXT:
				{
					dwText(unitInfo,_bIsDetail);
				}break;
			case U_TEXTBORDER:
				{
					//dwTextBorder(unitInfo); // 绘制位置不符
				}break;
			default:
				break;
			}
			//break;
		}

		ms_pPainter->restore();

		return true;
	}

	const bool XJBDrawHelper::DrawLineJB(QPainter* _p, const XShapeLineJB* _pLinePtr, QList<QPointF>& _qScrPnts)
	{
		if(nullptr == _p || nullptr == _pLinePtr || false == _pLinePtr->IsValid())
			return false;

		const ExceptiveInfo* pExInfo = _pLinePtr->GetExceptiveInfo();
		if(nullptr == pExInfo || _qScrPnts.count() < 2 )
			return false;

		ByyVec3dArray datas;
		foreach(QPointF pt,_qScrPnts)
		{
			datas.append(QVector3D(pt.x(),pt.y(),0));
		}
		ByyVec3dArray res;
		if(_qScrPnts.count() >= pExInfo->unMinPoint && _qScrPnts.count() <= pExInfo->unMaxPoint)
		{
			// 根据特殊绘制规则，调用不同的绘制函数
			switch(pExInfo->eDwType)
			{
			case ExceptiveInfo::edwType_DoubleArrow:
				{
					res = ByyPlotUtils::generateDoubleArrow(datas);
				}break;
			case ExceptiveInfo::edwType_FineArrow:
				{
					res = ByyPlotUtils::generateFineArrow(datas);
				}break;
			case ExceptiveInfo::edwType_GatheringPlace:
				{
					res = ByyPlotUtils::generateGatheringPlace(datas);
				}break;
			case ExceptiveInfo::edwType_AttackArrow:
				{
					res = ByyPlotUtils::generateAttackArrow(datas);
				}break;
			case  ExceptiveInfo::edwType_StraightArrow:
				{
					res = ByyPlotUtils::generateStraightArrow(datas);
				}break;
			default:
				break;
			}
		}else
		{
			res = datas;
		}

		if(false == res.isEmpty())
		{
			// 绘制连线
			_p->save();

			QPen pen;
			pen.setColor(_pLinePtr->GetPenClr());
			pen.setStyle(_pLinePtr->GetPenStyle());
			pen.setCapStyle(_pLinePtr->GetPenCapStyle());
			pen.setJoinStyle(_pLinePtr->GetPenJoinStyle());
			_p->setPen(pen);

			QPainterPath path;
			path.moveTo(res.first().toPointF());
			for (int nIdx=1;nIdx<res.count();++nIdx)
			{
				path.lineTo(res[nIdx].toPointF());
			}
			_p->drawPath(path);
			_p->restore();
		}

		pExInfo = nullptr;
		return true;
	}

	int XJBDrawHelper::quadrant(int x, int y)
	{
		int nResult = 0;
		if (x > 0)
		{
			if (y > 0)
				nResult = 1;
			else
				nResult = 4;
		}
		else
		{
			if (y > 0)
				nResult = 2;
			else
				nResult = 3;
		}
		return nResult;
	}

	void XJBDrawHelper::setLineAttr(const LINE_ATTR& _pLineAttr)
	{
		if(nullptr == ms_pPainter)
		{
			return ;
		}

		QPen pen = ms_pPainter->pen();
		if(ms_bIsUseUserClr)
			pen.setColor(ms_cUserClr);
		else
			pen.setColor(MakeColor(_pLineAttr.nLineC));
		pen.setWidthF(_pLineAttr.snLineWidth /** ms_dScale*/);//LXQ Add
		pen.setJoinStyle(Qt::MiterJoin);
		ms_pPainter->setPen(pen);

		if (_pLineAttr.snFillType > 1)
		{
			if(ms_bIsUseUserClr)
				ms_pPainter->setBrush(ms_cUserClr);
			else
				ms_pPainter->setBrush(MakeColor(_pLineAttr.unFillC));
		}
		else
			ms_pPainter->setBrush(Qt::NoBrush);
	}

	int XJBDrawHelper::calcLinePts(Q_JBItemUnitInfo& _cRefUnit, int flag)
	{
		int nSegmCount = _cRefUnit.cSegms.count();
		int nTotalDot = 1;
		for (int i=0;i<nSegmCount;++i)
		{
			if(_cRefUnit.cSegms[i].bIsCurve)
			{
				nTotalDot += ms_unBezierPrecision + 1;
			}else
				nTotalDot ++;
		}
		return nTotalDot;
	}

	void XJBDrawHelper::calcBezier(QVector<QPointF>& _cPnts, int _nPntCount, QVector<QPointF>& _cRefResultPnts)
	{
		int j,Seg;
		float t1,t2,t3,a1,a2,a3;

		Seg=(_nPntCount-1)/3;  
		int nCount = Seg*3+1;    // 曲线段数
		if(nCount != _cPnts.size())
		{
			Q_ASSERT(false);
			return ;
		}

		double x,y;
		x=y=0;
		int nPtIdx = 0;
		do 
		{
			for (j=0; j<=ms_unBezierPrecision;j++)
			{
				t1=(float)j/ms_unBezierPrecision;
				t2=t1*t1;
				t3=t2*t1;
				a1=-t3+3.0*(t2-t1)+1;
				a2=3.0*(t3-2.0*t2+t1);
				a3=3.0*(-t3+t2);  // a4=t3;
				x=a1*_cPnts[nPtIdx].x()+a2*_cPnts[nPtIdx+1].x()+a3*_cPnts[nPtIdx+2].x()+t3*_cPnts[nPtIdx+3].x();
				y=a1*_cPnts[nPtIdx].y()+a2*_cPnts[nPtIdx+1].y()+a3*_cPnts[nPtIdx+2].y()+t3*_cPnts[nPtIdx+3].y();
				_cRefResultPnts.append(QPointF(x,y));
			}
			nCount-=3;
			nPtIdx+=3;
		} 
		while(nCount>1);
	}

	void XJBDrawHelper::dwBLine(Q_JBItemUnitInfo& _cRefUnit, bool _bIsDetail /*= false*/)
	{
		if (_cRefUnit.cLineAttr.snOutLineWidth > 0)
		{	
			return ;
		}
		setLineAttr(_cRefUnit.cLineAttr);

		bool bClose = _cRefUnit.cLineAttr.snClose;

		QPainterPath path;
		QVector<QPointF> dwPnts;
		QVector<QPointF> dataPnt;
		QVector<QPointF> ctlPnt;
		foreach(Q_JBUnitSegm segm,_cRefUnit.cSegms)
		{
			dataPnt << segm.cPoints;
			ctlPnt << segm.cControlPnts;
			if(segm.bIsCurve)
			{
				if(segm.cPoints.size()!=4)
				{
					Q_ASSERT(false);
				}
				if(segm.cPoints.first() == segm.cPoints.last())
				{
					continue;
				}
				QVector<QPointF> curvePnts;
				calcBezier(segm.cPoints,4,curvePnts);

				int nFirstIdx = 0;
				if(false != dwPnts.isEmpty() || dwPnts.last() != curvePnts.first())
				{
					path.moveTo(segm.cPoints.first());
					dataPnt.append(segm.cPoints.first());
				}else
				{
					nFirstIdx = 1;
					dataPnt.append(segm.cPoints[1]);
					dataPnt.append(segm.cPoints[2]);
					//dataPnt.append(segm.cPoints[3]);
				}

				for (int i=nFirstIdx;i<curvePnts.count();i++)
				{
					dwPnts.append(curvePnts[i]);
					path.lineTo(curvePnts[i]);
				}
				curvePnts.clear();
			}else
			{
				int nFirstIdx = 0;
				if(false != dwPnts.isEmpty() || dwPnts.last() != segm.cPoints.first())
				{
					path.moveTo(segm.cPoints.first());
				}else
				{
					nFirstIdx = 1;
				}

				for (int i=nFirstIdx;i<segm.cPoints.count();i++)
				{
					dwPnts.append(segm.cPoints[i]);
					path.lineTo(segm.cPoints[i]);
				}
			}
		}

		if (_cRefUnit.cLineAttr.snFillType > 1)
		{
			dwPnts.append(dwPnts.first());
			ms_pPainter->drawPolygon(QPolygonF(dwPnts),Qt::WindingFill);
		}

		if(bClose)
			path.lineTo(dwPnts.first());
		ms_pPainter->drawPath(path);

		// 绘制详细信息
		if(true == _bIsDetail)
		{
			// 绘制数据点
			ms_pPainter->save();
			ms_pPainter->setPen(QPen(Qt::green,5));
			ms_pPainter->drawPoints(dataPnt.data(),dataPnt.size());
			ms_pPainter->setPen(QPen(Qt::yellow,5));
			ms_pPainter->drawPoints(ctlPnt.data(),ctlPnt.size());
			ms_pPainter->restore();
		}
	}

	void XJBDrawHelper::dwBBLine(Q_JBItemUnitInfo& _cRefUnit, bool _bIsDetail /*= false*/)
	{
		if (_cRefUnit.cLineAttr.snOutLineWidth)
			return;

		setLineAttr(_cRefUnit.cLineAttr);

		bool bClose = _cRefUnit.cLineAttr.snClose;

		int nDotNum,nTotalDot;
		int CtrlDot,nDotCnt;

		int nPolyNum = _cRefUnit.cSegms.count();

		nTotalDot = 0;
		int i = 0;

		QPainterPath path;

		i=0;
		nDotCnt=0;
		QVector<QPointF> dataPnt;
		QVector<QPointF> ctlPnt;
		foreach(Q_JBUnitSegm parentSegm,_cRefUnit.cSegms)
		{
			QVector<QPointF> dwPnts;
			foreach(Q_JBUnitSegm segm,parentSegm.cSubSegms)
			{
				dataPnt << segm.cPoints;
				ctlPnt << segm.cControlPnts;
				if(segm.bIsCurve)
				{
					if(segm.cPoints.size()!=4)
					{
						Q_ASSERT(false);
					}
					if(segm.cPoints.first() == segm.cPoints.last())
					{
						continue;
					}
					QVector<QPointF> curvePnts;
					calcBezier(segm.cPoints,4,curvePnts);

					int nFirstIdx = 0;
					if(false != dwPnts.isEmpty() || dwPnts.last() != curvePnts.first())
					{
						path.moveTo(segm.cPoints.first());
						dataPnt.append(segm.cPoints.first());
					}else
					{
						nFirstIdx = 1;
						dataPnt.append(segm.cPoints[1]);
						dataPnt.append(segm.cPoints[2]);
						//dataPnt.append(segm.cPoints[3]);
					}

					for (int i=nFirstIdx;i<curvePnts.count();i++)
					{
						dwPnts.append(curvePnts[i]);
						path.lineTo(curvePnts[i]);
					}
					curvePnts.clear();
				}else
				{
					int nFirstIdx = 0;
					if(false != dwPnts.isEmpty() || dwPnts.last() != segm.cPoints.first())
					{
						path.moveTo(segm.cPoints.first());
					}else
					{
						nFirstIdx = 1;
					}

					for (int i=nFirstIdx;i<segm.cPoints.count();i++)
					{
						dwPnts.append(segm.cPoints[i]);
						path.lineTo(segm.cPoints[i]);
					}
				}
			}

			if (_cRefUnit.cLineAttr.snFillType > 1)
			{
				dwPnts.append(dwPnts.first());
				ms_pPainter->drawPolygon(QPolygonF(dwPnts),Qt::OddEvenFill);
			}
			ms_pPainter->setBrush(Qt::NoBrush);
		}

		ms_pPainter->drawPath(path);

		// 绘制详细信息
		if(true == _bIsDetail)
		{
			// 绘制数据点
			ms_pPainter->save();
			ms_pPainter->setPen(QPen(Qt::green,5));
			ms_pPainter->drawPoints(dataPnt.data(),dataPnt.size());
			ms_pPainter->setPen(QPen(Qt::yellow,5));
			ms_pPainter->drawPoints(ctlPnt.data(),ctlPnt.size());
			ms_pPainter->restore();
		}
	}

	void XJBDrawHelper::dwPLine(Q_JBItemUnitInfo& _cRefUnit, bool _bIsDetail /*= false*/)
	{
		if (_cRefUnit.cLineAttr.snOutLineWidth)
			return;

		if(_cRefUnit.cSegms.count() != 1)
		{
			return;
		}

		setLineAttr(_cRefUnit.cLineAttr);
		ms_pPainter->setCompositionMode(QPainter::CompositionMode_SourceOver);
		ms_pPainter->drawPolyline(_cRefUnit.cSegms.first().cPoints.data(), _cRefUnit.cSegms.first().cPoints.count());
		ms_pPainter->setCompositionMode(QPainter::CompositionMode_Source);

		// 绘制详细信息
		if(true == _bIsDetail)
		{
			// 绘制数据点
			ms_pPainter->save();
			ms_pPainter->setPen(QPen(Qt::green,5));
			ms_pPainter->drawPoints(_cRefUnit.cSegms.first().cPoints.data(),_cRefUnit.cSegms.first().cPoints.count());
			// 			ms_pPainter->setPen(QPen(Qt::yellow,5));
			// 			ms_pPainter->drawPoints(ctlPnt.data(),ctlPnt.size());
			ms_pPainter->restore();
		}
	}

	void XJBDrawHelper::dwRect(Q_JBItemUnitInfo& _cRefUnit, bool _bIsDetail /*= false*/)
	{
		if (_cRefUnit.cLineAttr.snOutLineWidth)
			return;

		if(_cRefUnit.cSegms.count() != 1)
		{
			return;
		}
		if(_cRefUnit.cSegms.first().cPoints.count() < 2)
		{
			return;
		}

		setLineAttr(_cRefUnit.cLineAttr);
		ms_pPainter->drawRect(_cRefUnit.cSegms.first().cPoints[0].x(),_cRefUnit.cSegms.first().cPoints[0].y(),
			_cRefUnit.cSegms.first().cPoints[1].x() -_cRefUnit.cSegms.first().cPoints[0].x(),_cRefUnit.cSegms.first().cPoints[1].y() -_cRefUnit.cSegms.first().cPoints[0].y());

		// 绘制详细信息
		if(true == _bIsDetail)
		{
			// 绘制数据点
			ms_pPainter->save();
			ms_pPainter->setPen(QPen(Qt::green,5));
			ms_pPainter->drawPoints(_cRefUnit.cSegms.first().cPoints.data(),_cRefUnit.cSegms.first().cPoints.count());
			// 			ms_pPainter->setPen(QPen(Qt::yellow,5));
			// 			ms_pPainter->drawPoints(ctlPnt.data(),ctlPnt.size());
			ms_pPainter->restore();
		}
	}

	void XJBDrawHelper::dwEllipse(Q_JBItemUnitInfo& _cRefUnit, bool _bIsDetail /*= false*/)
	{
		if (_cRefUnit.cLineAttr.snOutLineWidth)
			return;

		if(_cRefUnit.cSegms.count() != 1)
		{
			return;
		}
		if(_cRefUnit.cSegms.first().cPoints.count() < 2)
		{
			return;
		}

		setLineAttr(_cRefUnit.cLineAttr);
		ms_pPainter->drawEllipse(_cRefUnit.cSegms.first().cPoints[0].x(),_cRefUnit.cSegms.first().cPoints[0].y(),
			_cRefUnit.cSegms.first().cPoints[1].x() -_cRefUnit.cSegms.first().cPoints[0].x(),_cRefUnit.cSegms.first().cPoints[1].y() -_cRefUnit.cSegms.first().cPoints[0].y());

		// 绘制详细信息
		if(true == _bIsDetail)
		{
			// 绘制数据点
			ms_pPainter->save();
			ms_pPainter->setPen(QPen(Qt::green,5));
			ms_pPainter->drawPoints(_cRefUnit.cSegms.first().cPoints.data(),_cRefUnit.cSegms.first().cPoints.count());
			//ms_pPainter->setPen(QPen(Qt::yellow,5));
			//ms_pPainter->drawPoints(ctlPnt.data(),ctlPnt.size());
			ms_pPainter->restore();
		}
	}

	void XJBDrawHelper::dwPie(Q_JBItemUnitInfo& _cRefUnit, bool _bIsDetail /*= false*/)
	{
		if (_cRefUnit.cLineAttr.snOutLineWidth)
			return;

		setLineAttr(_cRefUnit.cLineAttr);

		if(_cRefUnit.cSegms.count() != 1)
		{
			return;
		}

		QVector<QPointF> pt = _cRefUnit.cSegms.first().cPoints;
		if(pt.count() != 4)
		{
			return ;
		}

		QRect rect(pt[0].x(), pt[0].y(), pt[1].x() - pt[0].x(), pt[1].y() - pt[0].y());

		/// p1是起点，p2是终点，p0是计算起点  3个点都相对于矩形的中心(相对坐标）
		/// 1、首先计算 p0->p1->p2 顺时针还是逆时针
		QPoint p0 = QPoint((rect.center().x()+rect.width()/2) - rect.center().x(), 0);      ///< 计算的起点
		QPoint p1 = QPoint((pt[2].x() - rect.center().x()), -(pt[2].y() - rect.center().y()));   ///< 起始点向量
		QPoint p2 = QPoint((pt[3].x() - rect.center().x()), -(pt[3].y() - rect.center().y()));   ///< 终点向量


		float dStart = 0; ///< 起始角度
		if (p1.y() == 0)
		{
			if (p1.x()>=0)
				dStart = 0;
			else
				dStart = 180.0;
		}
		else
		{
			float dCosStart = (p0.x() * p1.x() + p0.y() * p1.y())/
				(sqrt(float(p0.x() * p0.x() + p0.y() * p0.y()))*
				sqrt(float(p1.x() * p1.x() + p1.y() * p1.y())));

			dStart = acos(dCosStart)*180.0f/M_PI;
		}

		float dSpan = 0; ///< 经过的角度 p1->p2
		int nQuadrantP1 = quadrant(p1.x(),p1.y());    ///< 象限
		//int nQuadrantP2 = Quadrant(p2.x(),p2.y());
		if (nQuadrantP1 == 1 || nQuadrantP1 == 2)
			dStart = -dStart;

		float dCosSpan = (p1.x() * p2.x() + p1.y() * p2.y())/
			(sqrt(float(p1.x() * p1.x() + p1.y() * p1.y()))*
			sqrt(float(p2.x() * p2.x() + p2.y() * p2.y())));

		dSpan = acos(dCosSpan)*180.0f/M_PI;

		// 为正时，p0->p1->p2 逆时针
		// 为负时，           顺
		// 为零时，           方向不变,
		int nCross = (p1.x() - p0.x())*(p2.y()-p1.y())-(p1.y()-p0.y())*(p2.x()-p1.x());
		if (nCross > 0)
		{
			dSpan = 360.0 - dSpan;
		}

		if (_cRefUnit.cLineAttr.snFillType > 1)
		{
			ms_pPainter->setBrush(MakeColor(_cRefUnit.cLineAttr.nLineC));
		}
		ms_pPainter->scale(1.0, -1.0);
		ms_pPainter->drawPie(QRect(rect.left(), -rect.bottom(), rect.width(), rect.height()), dStart * 16, dSpan * 16);
		ms_pPainter->scale(1.0, -1.0);

		pt.clear();

		// 绘制详细信息
		if(true == _bIsDetail)
		{
			// 绘制数据点
			ms_pPainter->save();
			ms_pPainter->setPen(QPen(Qt::green,5));
			ms_pPainter->drawPoints(_cRefUnit.cSegms.first().cPoints.data(),_cRefUnit.cSegms.first().cPoints.count());
			//ms_pPainter->setPen(QPen(Qt::yellow,5));
			//ms_pPainter->drawPoints(ctlPnt.data(),ctlPnt.size());
			ms_pPainter->restore();
		}
	}

	void XJBDrawHelper::dwArc(Q_JBItemUnitInfo& _cRefUnit, bool _bIsDetail /*= false*/)
	{
		if (_cRefUnit.cLineAttr.snOutLineWidth)
			return;

		setLineAttr(_cRefUnit.cLineAttr);

		if(_cRefUnit.cSegms.count() != 1)
		{
			return;
		}

		QVector<QPointF> pt = _cRefUnit.cSegms.first().cPoints;
		if(pt.count() != 4)
		{
			return ;
		}

		QRect rect(pt[0].x(), pt[0].y(), pt[1].x() - pt[0].x(), pt[1].y() - pt[0].y());

		/// p1是起点，p2是终点，p0是计算起点  3个点都相对于矩形的中心(相对坐标）
		/// 1、首先计算 p0->p1->p2 顺时针还是逆时针
		QPoint p0 = QPoint((rect.center().x()+rect.width()/2) - rect.center().x(), 0);      ///< 计算的起点
		QPoint p1 = QPoint((pt[2].x() - rect.center().x()), -(pt[2].y() - rect.center().y()));   ///< 起始点向量
		QPoint p2 = QPoint((pt[3].x() - rect.center().x()), -(pt[3].y() - rect.center().y()));   ///< 终点向量


		float dStart = 0; ///< 起始角度
		if (p1.y() == 0)
		{
			if (p1.x()>=0)
				dStart = 0;
			else
				dStart = 180.0;
		}
		else
		{
			float dCosStart = (p0.x() * p1.x() + p0.y() * p1.y())/
				(sqrt(float(p0.x() * p0.x() + p0.y() * p0.y()))*
				sqrt(float(p1.x() * p1.x() + p1.y() * p1.y())));

			dStart = acos(dCosStart)*180.0f/M_PI;
		}

		float dSpan = 0; ///< 经过的角度 p1->p2
		int nQuadrantP1 = quadrant(p1.x(),p1.y());    ///< 象限
		if (nQuadrantP1 == 1 || nQuadrantP1 == 2)
			dStart = -dStart;

		float dCosSpan = (p1.x() * p2.x() + p1.y() * p2.y())/
			(sqrt(float(p1.x() * p1.x() + p1.y() * p1.y()))*
			sqrt(float(p2.x() * p2.x() + p2.y() * p2.y())));

		dSpan = acos(dCosSpan)*180.0f/M_PI;

		// 为正时，p0->p1->p2 逆时针
		// 为负时，           顺
		// 为零时，           方向不变,
		int nCross = (p1.x() - p0.x())*(p2.y()-p1.y())-(p1.y()-p0.y())*(p2.x()-p1.x());
		if (nCross > 0)
		{
			dSpan = 360.0 - dSpan;
		}
		else if (nCross == 0)
		{
			dSpan = 0;
		}

		ms_pPainter->scale(1.0, -1.0);
		ms_pPainter->drawArc(QRect(rect.left(), -rect.bottom(), rect.width(), rect.height()), dStart * 16, dSpan * 16);
		ms_pPainter->scale(1.0, -1.0);
		pt.clear();

		// 绘制详细信息
		if(true == _bIsDetail)
		{
			// 绘制数据点
			ms_pPainter->save();
			ms_pPainter->setPen(QPen(Qt::green,5));
			ms_pPainter->drawPoints(_cRefUnit.cSegms.first().cPoints.data(),_cRefUnit.cSegms.first().cPoints.count());
			//ms_pPainter->setPen(QPen(Qt::yellow,5));
			//ms_pPainter->drawPoints(ctlPnt.data(),ctlPnt.size());
			ms_pPainter->restore();
		}

	}

	void XJBDrawHelper::dwPolygon(Q_JBItemUnitInfo& _cRefUnit, bool _bIsDetail /*= false*/)
	{
		if (_cRefUnit.cLineAttr.snOutLineWidth)
			return;

		setLineAttr(_cRefUnit.cLineAttr);

		if(_cRefUnit.cSegms.count() != 1)
		{
			return;
		}

		QVector<QPointF> pt = _cRefUnit.cSegms.first().cPoints;
		if(pt.isEmpty())
		{
			return ;
		}

		if(_cRefUnit.cLineAttr.snFillType>2)
		{
			QBrush brush;
			brush.setStyle((Qt::BrushStyle)(_cRefUnit.cLineAttr.snFillType-3));
			brush.setColor(MakeColor(_cRefUnit.cLineAttr.unFillC));
			ms_pPainter->setBrush(brush);
			ms_pPainter->drawPolygon(pt.data(), pt.count());
		}
		else 
		{
			QBrush brush;
			brush.setStyle(Qt::SolidPattern);
			brush.setColor(MakeColor(_cRefUnit.cLineAttr.unFillC));
			ms_pPainter->setBrush(brush);
			ms_pPainter->drawPolygon(pt.data(), pt.count());
		}
		pt.clear();

		// 绘制详细信息
		if(true == _bIsDetail)
		{
			// 绘制数据点
			ms_pPainter->save();
			ms_pPainter->setPen(QPen(Qt::green,5));
			ms_pPainter->drawPoints(_cRefUnit.cSegms.first().cPoints.data(),_cRefUnit.cSegms.first().cPoints.count());
			//ms_pPainter->setPen(QPen(Qt::yellow,5));
			//ms_pPainter->drawPoints(ctlPnt.data(),ctlPnt.size());
			ms_pPainter->restore();
		}
	}

	void XJBDrawHelper::dwText(Q_JBItemUnitInfo& _cRefUnit, bool _bIsDetail /*= false*/)
	{
		QPen pen = ms_pPainter->pen();
		pen.setColor(MakeColor(_cRefUnit.cTxtAttr.unTextColor));
		ms_pPainter->setPen(pen);

		if(_cRefUnit.cSegms.count() != 1 || _cRefUnit.cSegms.first().cPoints.count() < 1)
		{
			return;
		}
		QPointF pt = _cRefUnit.cSegms.first().cPoints.first();

		QFont font;
		font.setFamily(QString::fromLocal8Bit(_cRefUnit.cTxtAttr.cFontName));
		font.setPointSize(_cRefUnit.cTxtAttr.snFontHight * ms_fTextFactor);
		ms_pPainter->setFont(font);

		ms_pPainter->scale(1.0, -1.0);
		double dDistance = abs(ms_cBoxRct.bottom() - pt.y());
		double dScaleOffset = ms_cBoxRct.top() + dDistance + ms_pPainter->fontMetrics().height();

		ms_pPainter->translate(0, -ms_cBoxRct.center().y()*2);

		ms_pPainter->drawText(QPointF(pt.x() ,dScaleOffset),_cRefUnit.cSegms.first().strTxt);

		ms_pPainter->scale(1.0, -1.0);

		// 绘制详细信息
		if(true == _bIsDetail)
		{
			// 绘制数据点
			ms_pPainter->save();
			ms_pPainter->setPen(QPen(Qt::green,5));
			ms_pPainter->drawPoints(_cRefUnit.cSegms.first().cPoints.data(),_cRefUnit.cSegms.first().cPoints.count());
			//ms_pPainter->setPen(QPen(Qt::yellow,5));
			//ms_pPainter->drawPoints(ctlPnt.data(),ctlPnt.size());
			ms_pPainter->restore();
		}
	}

	void XJBDrawHelper::dwTextBorder(Q_JBItemUnitInfo& _cRefUnit, bool _bIsDetail /*= false*/)
	{
		QPen pen = ms_pPainter->pen();
		pen.setColor(MakeColor(_cRefUnit.cTxtAttr.unTextColor));
		ms_pPainter->setPen(pen);

		if(_cRefUnit.cSegms.count() != 1 || _cRefUnit.cSegms.first().cPoints.count() < 2)
		{
			return;
		}

		ms_pPainter->drawRect(_cRefUnit.cSegms.first().cPoints[0].x(),_cRefUnit.cSegms.first().cPoints[0].y(),
			_cRefUnit.cSegms.first().cPoints[1].x() -_cRefUnit.cSegms.first().cPoints[0].x(),_cRefUnit.cSegms.first().cPoints[1].y() -_cRefUnit.cSegms.first().cPoints[0].y());
	}

}

