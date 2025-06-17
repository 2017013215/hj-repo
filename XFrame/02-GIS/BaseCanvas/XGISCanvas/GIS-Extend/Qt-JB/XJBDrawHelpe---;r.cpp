#include "XJBDrawHelper.h"
#include "JBDef.h"
#include "XJBItem.h"
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
	bool XJBDrawHelper::ms_bIsUseUserClr = true;
	QColor XJBDrawHelper::ms_cUserClr = Qt::red;
	quint16 XJBDrawHelper::ms_unBezierPrecision = 15;
	const int XJBDrawHelper::ms_nConstCurveFlag = 0x100;
	quint32 XJBDrawHelper::ms_unPointCount = 2048;
	QJB_POINT* XJBDrawHelper::ms_cPoints = new QJB_POINT[ms_unPointCount];
	QPointF* XJBDrawHelper::ms_cQPoints = new QPointF[ms_unPointCount];

	XJBDrawHelper::XJBDrawHelper()
	{

	}

	XJBDrawHelper::~XJBDrawHelper()
	{

	}

	const bool XJBDrawHelper::DrawPointJB(QPainter* _p, const Q_JBItem* _pJBItemInfo, QList<QPointF>& _dataPoints, QColor _cUserClr /*= Qt::red*/, bool _bIsUserClr /*= true*/,QSize& _cJBSize/* = QSize(30,30)*/)
	{
		if(nullptr == _pJBItemInfo || nullptr == _p)
		{
			return false;
		}

		ms_bIsUseUserClr = _bIsUserClr;
		ms_cUserClr = _cUserClr;
		ms_pPainter = _p;
		ms_pPainter->save();

		// 点状JB
			ms_pPainter->setCompositionMode(QPainter::CompositionMode_Source);
			foreach(QPointF pt,_dataPoints)
			{
				ms_pPainter->save();
				ms_pPainter->translate(pt.x(),pt.y());
				// 计算包围盒
				ms_cBoxRct = _pJBItemInfo->rectBound;
				float dScaleX = (float)_cJBSize.width() / ms_cBoxRct.width() * ms_fJBFactor;
				float dScaleY = (float)_cJBSize.height() / ms_cBoxRct.height() * ms_fJBFactor;
				ms_dScale = std::min(dScaleX, dScaleY);
				ms_pPainter->translate(-ms_cBoxRct.center().x()*ms_dScale,ms_cBoxRct.center().y()*ms_dScale);
				ms_pPainter->scale(ms_dScale, -ms_dScale);

				{
					unsigned int uLen;                              //< 单条图元的长度
					unsigned int uCount = _pJBItemInfo->byteInfo.size();   //< 总长度.

					// 获取头
					QJB_ITEM_UNIT_HEAD *pJBUH = (QJB_ITEM_UNIT_HEAD *)_pJBItemInfo->byteInfo.data();

					// 遍历所有基本绘制图元进行绘制
					int kk = 0;
					bool bRes = false;
					for(;uCount > 0;uCount -= uLen)
					{
						uLen = pJBUH->unLen;

						// 绘制基本图元
						bRes = drawUnit(pJBUH);
						Q_ASSERT(bRes);

						// 移动数据指针
						pJBUH = (QJB_ITEM_UNIT_HEAD *)(((char *)pJBUH) + uLen);
					}
				}
				ms_pPainter->restore();
			}
		ms_pPainter->restore();
		ms_pPainter = nullptr;

		return true;
	}

	const bool XJBDrawHelper::DrawPointJB(QPainter* _p, const XShapePointJB* _pShapePtr, QPointF& _qScreenPnt)
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
					dwBLine(unitInfo);
				}
				break;
			case U_BBLINE:
				{
					dwBBLine(unitInfo);
				}break;
			case U_PLINE:
			case U_LINE:
				{
					dwPLine(unitInfo);
				}break;
			case U_RECT:
				{
					dwRect(unitInfo);
				}break;
			case U_ELLI:
				{
					dwEllipse(unitInfo);
				}break;
			case U_PIE:
				{
					dwPie(unitInfo);
				}break;
			case U_ARC:
				{
					dwArc(unitInfo);
				}break;
			case U_PGON:
				{
					dwPolygon(unitInfo);
				}break;
			case U_TEXT:
				{
					dwText(unitInfo);
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

	const bool XJBDrawHelper::DrawLineJB(QPainter* _p, const Q_JBItem* _pJBItemInfo, QList<QPointF>& _dataPnts, bool _bIsDone /*= false*/, QColor _cUserClr /*= Qt::red*/, bool _bIsUserClr /*= true*/)
	{
		if(nullptr == _pJBItemInfo || nullptr == _p || _dataPnts.size() < 2)
		{
			return false;
		}

		ms_bIsUseUserClr = _bIsUserClr;
		ms_cUserClr = _cUserClr;
		ms_pPainter = _p;
		ms_pPainter->save();

		ms_pPainter->setPen(Qt::magenta);

		//ms_pPainter->drawLine(_dataPnts.first(),_dataPnts.last());

		{
			unsigned int uLen;                              //< 单条图元的长度
			unsigned int uCount = _pJBItemInfo->byteInfo.size();   //< 总长度.

			// 获取头
			QJB_ITEM_UNIT_HEAD *pJBUH = (QJB_ITEM_UNIT_HEAD *)_pJBItemInfo->byteInfo.data();

			// 遍历所有基本绘制图元进行绘制
			int kk = 0;
			bool bRes = false;
			for(;uCount > 0;uCount -= uLen)
			{
				uLen = pJBUH->unLen;

				// 绘制基本图元
				bRes = drawUnit(pJBUH,_dataPnts);
				Q_ASSERT(bRes);
				break;
				// 移动数据指针
				pJBUH = (QJB_ITEM_UNIT_HEAD *)(((char *)pJBUH) + uLen);
				break;
			}
		}

		ms_pPainter->restore();
		return true;
	}

	const bool XJBDrawHelper::drawUnit(QJB_ITEM_UNIT_HEAD* _pUnitInfo,QList<QPointF>& _dataPnts)
	{
		if(nullptr == ms_pPainter || nullptr == _pUnitInfo)
		{
			return false;
		}

		switch(_pUnitInfo->snType)
		{
		case U_BLINE:   /// 曲/直线
			drawBLine(_pUnitInfo,_dataPnts);
			//qDebug() << "U_BLINE";
			break;
		case U_BBLINE:  /// 内含曲/直线
			drawBBLine(_pUnitInfo);
			//qDebug() << "U_BBLINE";
			break;
		case U_PLINE:
			drawPLine(_pUnitInfo);
			//qDebug() << "U_PLINE";
			break;
		case U_LINE: /// 直线
			drawPLine(_pUnitInfo);
			//qDebug() << "U_LINE";
			break;
		case U_RECT: /// 矩形
			drawRect(_pUnitInfo);
			//qDebug() << "U_RECT";
			break;
		case U_ELLI: /// 椭圆
			drawEllipse(_pUnitInfo);
			//qDebug() << "U_ELLI";
			break;
		case U_PIE: /// 饼
			drawPie(_pUnitInfo);
			//qDebug() << "U_PIE";
			break;
		case U_ARC: /// 圆弧
			drawArc(_pUnitInfo);
			//qDebug() << "U_ARC";
			break;
		case U_PGON:
			drawPolygon(_pUnitInfo);
			//qDebug() << "U_PGON";
			break;
		case U_TEXT:
			drawText(_pUnitInfo);
			//qDebug() << "U_TEXT";
			break;
		case U_TEXTBORDER:
			//qDebug() << "U_TEXTBORDER";
			break;
		case U_BITMAP:
			//qDebug() << "U_BITMAP";
			break;
		default:
			break;
		}

		return true;
	}

	void XJBDrawHelper::drawBLine(QJB_ITEM_UNIT_HEAD* _pUnitInfo,QList<QPointF>& _dataPnts)
	{
		const LINE_ATTR *pLAtt = (LINE_ATTR *)(_pUnitInfo + 1);

		if (0 < pLAtt->snOutLineWidth)
		{	
			return ;
		}

		int nDataCount = _dataPnts.count();

		setLineAttr(pLAtt);

		QJB_POINT xy[4];
		int * d,*d0,nDotNum;
		//    int CtrlDot,j;
		int CtrlDot;
		bool bClose = pLAtt->snClose;
		
		QPainterPath path;

		d=(int*)(pLAtt + 1);

		int TotalDot = browseLine(d, bClose);
		checkPointCache(TotalDot);
		QJB_POINT *xy0,*xy1;
		xy1 = xy0 = ms_cPoints;

		nDotNum = *d++;								// 本曲线基本点数
		d0 = d;

		for(;nDotNum>0; nDotNum--)
		{
			CtrlDot=*d & 0xFF;						// 本段控制点数
			if(*d & ms_nConstCurveFlag)				// 如果是曲线
			{
				d++;
				xy[0].x=*d++; 
				xy[0].y=*d++;

				if(CtrlDot==1)						// 一个控制点
				{ 
					xy[1].x=*d++; 
					xy[1].y=*d++;
				}  
				else								// 两个控制点
				{ 
					d++; 
					d++; 
					xy[1].x=*d++; 
					xy[1].y=*d++;
				} 

				if(nDotNum==1 && bClose)				// 最后一点且封闭
				{
					xy[3].x=d0[1]; xy[3].y=d0[2];
					xy[2].x=d0[3]; xy[2].y=d0[4];
				}
				else
				{
					xy[3].x=d[1]; xy[3].y=d[2];
					xy[2].x=d[3]; xy[2].y=d[4];
				}

				path.moveTo((int)xy[0].x,xy[0].y);
				// path.cubicTo(xy[1].x,xy[1].y,xy[2].x,xy[2].y,xy[3].x,xy[3].y);
				bezier(xy, 4, xy1);

				for (int i=0;i<ms_unBezierPrecision;++i) 
				{
					++xy1;
					path.lineTo(xy1->x, xy1->y);
				}
				++xy1;
			}
			else // 直线
			{
				d++;

// 				if(nDataCount < 1)
// 				{
					xy[0].x=*d++; 
					xy[0].y=*d++;
// 				}else
// 				{
// 					xy[0].x=_dataPnts.first().x();
// 					xy[0].y =_dataPnts.first().y();
// 				}


				if(CtrlDot==1)
				{
					d++; 
					d++;
				}

				if(nDotNum==1 && bClose)			// 最后一点且封闭
				{
					xy[1].x = d0[1];   
					xy[1].y = d0[2];
				}
				else
				{
					xy[1].x = d[1]; 
					xy[1].y = d[2];
				}

				path.moveTo((int)xy[0].x,xy[0].y);
				path.lineTo(xy[1].x,xy[1].y);

				xy1->x = xy[1].x; 
				xy1->y = xy[1].y;
				++xy1;
			}

			if(nDotNum==2 && bClose==0)
			{
				break;
			}
		}

		if (pLAtt->snFillType > 1)
		{
			//setBrush(Qt::red);

			xy1->x = xy0->x;
			xy1->y = xy0->y;

			QPointF *points = ms_cQPoints;
			for (int i=0;i<TotalDot;++i)
			{
				points[i] = QPointF(xy0->x, xy0->y);
				++xy0;
			}

			ms_pPainter->drawPolygon(points, TotalDot,Qt::WindingFill);
			points = nullptr;
		}

		xy0 = xy1 = nullptr;
		d = d0 = nullptr;
		pLAtt = nullptr;
		ms_pPainter->drawPath(path);

		pLAtt = nullptr;
	}

	void XJBDrawHelper::setLineAttr(const LINE_ATTR* _pLineAttr)
	{
		if(nullptr == _pLineAttr || nullptr == ms_pPainter)
		{
			return ;
		}

		if(ms_bIsUseUserClr)
		{
			QPen pen = ms_pPainter->pen();
			if(_pLineAttr->nLineC > 0 && _pLineAttr->nLineC != _pLineAttr->unFillC && _pLineAttr->snFillType > 1)
			{
				pen.setColor(MakeColor(_pLineAttr->nLineC));
			}else
				pen.setColor(ms_cUserClr);
			pen.setWidth(1.0 / ms_dScale);//LXQ Add
			ms_pPainter->setPen(pen);
			if (_pLineAttr->snFillType > 1)
			{
				ms_pPainter->setBrush(ms_cUserClr);
			}
			else
				ms_pPainter->setBrush(Qt::NoBrush);
		}else
		{
			QPen pen = ms_pPainter->pen();
			pen.setColor(MakeColor(_pLineAttr->nLineC));
			pen.setWidth(1.0 / ms_dScale);//LXQ Add
			ms_pPainter->setPen(pen);

			if (_pLineAttr->snFillType > 1)
				ms_pPainter->setBrush(MakeColor(_pLineAttr->unFillC));
			else
				ms_pPainter->setBrush(Qt::NoBrush);
		}
	}

	int XJBDrawHelper::browseLine(int *_pData, int flag)
	{
		int *d = _pData, DotNum;
		int TotalDot = 1;

		DotNum = *d++;  // 本曲线基本点数

		for (;DotNum > 0; DotNum--) 
		{
			if (*d & ms_nConstCurveFlag) 
			{
				TotalDot+=ms_unBezierPrecision + 1;
			}
			else 
			{
				TotalDot++;
			}
			d += (*d & 0xFF)*2 + 3;
			if (DotNum == 2 && flag == 0)
				break;
		}
		return TotalDot;
	}

	void XJBDrawHelper::bezier(QJB_POINT *_pts, int _n, QJB_POINT *_pOutResPtr)
	{
		int j,Seg;
		QJB_POINT * pxy;
		float t1,t2,t3,a1,a2,a3;

		Seg=(_n-1)/3;  _n=Seg*3+1;    // 曲线段数

		pxy=_pOutResPtr;

		do 
		{
			for (j=0; j<=ms_unBezierPrecision;j++,pxy++)
			{
				t1=(float)j/ms_unBezierPrecision;
				t2=t1*t1;
				t3=t2*t1;
				a1=-t3+3.0*(t2-t1)+1;
				a2=3.0*(t3-2.0*t2+t1);
				a3=3.0*(-t3+t2);  // a4=t3;
				pxy->x=a1*_pts->x+a2*(_pts+1)->x+a3*(_pts+2)->x+t3*(_pts+3)->x;
				pxy->y=a1*_pts->y+a2*(_pts+1)->y+a3*(_pts+2)->y+t3*(_pts+3)->y;
			}
			_n-=3;  
			_pts+=3;
		} 
		while(_n>1);
	}

	void XJBDrawHelper::drawBBLine( QJB_ITEM_UNIT_HEAD* _pUnitInfo)
	{
		LINE_ATTR *LA = (LINE_ATTR *)(_pUnitInfo + 1);

		if (LA->snOutLineWidth)
			return;

		setLineAttr(LA);

		bool bClose = LA->snClose;

		QJB_POINT *xy0,*xy1,xy[4];
		int *d,*d0,nDotNum,nTotalDot;
		int CtrlDot,nDotCnt;
		int *DATA = (int*)(LA + 1);

		int nPolyNum = DATA[0];
		if (nPolyNum > 40) {
			nPolyNum = 40;
		}

		int *Data1[40], vNum[40];
		Data1[0] = DATA + 2;

		nTotalDot = 0;
		int i = 0;
		for (;i < nPolyNum;i++)
		{
			if (i) Data1[i] = (int*)((unsigned char*)Data1[i-1] + *(Data1[i-1]-1));

			vNum[i] = browseLine(Data1[i], bClose);
			nTotalDot += vNum[i];
		}

		checkPointCache(nTotalDot);
		xy1 = xy0 = ms_cPoints;

		QPainterPath path;

		i=0;
		nDotCnt=0;
		for (;i<nPolyNum;i++)
		{
			d=(int *)Data1[i];
			nDotNum=*d++;
			d0=d;

			for(;nDotNum>0; nDotNum--)
			{
				CtrlDot=*d & 0xFF;						// 本段控制点数
				if(*d & ms_nConstCurveFlag)				// 如果是曲线
				{    
					d++;
					xy[0].x = *d++;
					xy[0].y = *d++;

					if(CtrlDot==1)
					{									// 一个控制点
						xy[1].x=*d++; 
						xy[1].y=*d++;
					}  
					else								// 两个控制点
					{ 
						d++; 
						d++; 
						xy[1].x=*d++; 
						xy[1].y=*d++;
					} 

					if(nDotNum==1 && bClose)				// 最后一点且封闭
					{
						xy[3].x=d0[1]; xy[3].y=d0[2];
						xy[2].x=d0[3]; xy[2].y=d0[4];
					}
					else
					{
						xy[3].x=d[1]; xy[3].y=d[2];
						xy[2].x=d[3]; xy[2].y=d[4];
					}

					path.moveTo((int)xy[0].x,xy[0].y);
					//path.cubicTo(xy[1].x,xy[1].y,xy[2].x,xy[2].y,xy[3].x,xy[3].y);
					bezier(xy, 4, xy1);

					for (int nBezier=0;nBezier < ms_unBezierPrecision;++nBezier)
					{
						++xy1;
						path.lineTo(xy1->x, xy1->y);
					}
					++xy1;
				}
				else // 直线
				{
					d++;
					xy[0].x=*d++; 
					xy[0].y=*d++;
					if(CtrlDot==1)
					{
						d++; 
						d++;
					}

					if(nDotNum==1 && bClose)// 最后一点且封闭
					{
						xy[1].x = d0[1];   
						xy[1].y = d0[2];
					}
					else
					{
						xy[1].x = d[1];
						xy[1].y = d[2];
					}

					path.moveTo((int)xy[0].x,xy[0].y);
					path.lineTo(xy[1].x,xy[1].y);

					xy1->x = xy[1].x;
					xy1->y = xy[1].y;
					++xy1;
				}

				if(nDotNum==2 && bClose==0)
					break;
			}

			QJB_POINT *xy2 = xy0+nDotCnt;
			if (LA->snFillType > 1)
			{
				xy1->x = xy2->x;
				xy1->y = xy2->y;
				QPointF *points = ms_cQPoints;
				for (int k=0;k<vNum[i];++k)
				{
					points[k] = QPointF(xy2->x, xy2->y);
					++xy2;
				}

				ms_pPainter->drawPolygon(points, vNum[i],Qt::OddEvenFill);
				points = nullptr;
			}

			ms_pPainter->setBrush(Qt::NoBrush);
			nDotCnt+=vNum[i];
			xy1=xy0+nDotCnt;
		}

		ms_pPainter->drawPath(path);

		d = d0 = DATA = nullptr;
		LA = nullptr;
	}

	void XJBDrawHelper::drawPLine(QJB_ITEM_UNIT_HEAD* _pUnitInfo)
	{
		LINE_ATTR *LA = (LINE_ATTR *)(_pUnitInfo + 1);

		if (LA->snOutLineWidth)
			return;

		int * d=(int *)(LA + 1);
		int DotNum=*d++;

		QJB_POINT *pt=(QJB_POINT*)d;

		setLineAttr(LA);
		ms_pPainter->setCompositionMode(QPainter::CompositionMode_SourceOver);

		checkPointCache(DotNum);
		QPointF * QD = ms_cQPoints;
		for(int i=0; i<DotNum; i++) 
		{
			QD[i].setX(pt[i].x);
			QD[i].setY(pt[i].y);
		}
		ms_pPainter->drawPolyline(QD, DotNum);
		ms_pPainter->setCompositionMode(QPainter::CompositionMode_Source);
		pt = nullptr;
		QD = nullptr;
		LA = nullptr;
		d = nullptr;
	}

	void XJBDrawHelper::clearDataMem()
	{
		memset(ms_cPoints,0,sizeof(QJB_POINT)*ms_unPointCount);
		memset(ms_cQPoints,0,sizeof(QPointF)*ms_unPointCount);
		delete[]ms_cPoints;
		delete[] ms_cQPoints;
		ms_unPointCount = 0;
	}

	void XJBDrawHelper::checkPointCache(quint32 _unNewCount)
	{
		if(_unNewCount > ms_unPointCount)
		{
			memset(ms_cPoints,0,sizeof(QJB_POINT)*ms_unPointCount);
			memset(ms_cQPoints,0,sizeof(QPointF)*ms_unPointCount);
			delete[]ms_cPoints;
			delete[] ms_cQPoints;
			ms_cPoints = new QJB_POINT[_unNewCount];
			ms_cQPoints = new QPointF[_unNewCount];
			ms_unPointCount = _unNewCount;
		}
	}

	void XJBDrawHelper::drawRect(QJB_ITEM_UNIT_HEAD* _pUnitInfo)
	{
		LINE_ATTR *LA = (LINE_ATTR *)(_pUnitInfo + 1);

		if (LA->snOutLineWidth)
			return;

		setLineAttr(LA);

		QJB_POINT *pt = (QJB_POINT*)(LA + 1);

		ms_pPainter->drawRect(QRect(pt[0].x, pt[0].y, pt[1].x - pt[0].x, pt[1].y - pt[0].y));
		pt = nullptr;
		LA = nullptr;
	}

	void XJBDrawHelper::drawEllipse(QJB_ITEM_UNIT_HEAD* _pUnitInfo)
	{
		LINE_ATTR *LA = (LINE_ATTR *)(_pUnitInfo + 1);

		if (LA->snOutLineWidth)
			return;

		setLineAttr(LA);

		QJB_POINT *pt = (QJB_POINT*)(LA + 1);

		ms_pPainter->drawEllipse(QRect(pt[0].x, pt[0].y, pt[1].x - pt[0].x, pt[1].y - pt[0].y));
		pt = nullptr;
		LA = nullptr;
	}

	void XJBDrawHelper::drawPie( QJB_ITEM_UNIT_HEAD* _pUnitInfo)
	{
		LINE_ATTR *LA = (LINE_ATTR *)(_pUnitInfo + 1);

		if (LA->snOutLineWidth)
			return;

		setLineAttr(LA);

		QJB_POINT *pt = (QJB_POINT*)(LA + 1);

		QRect rect(pt[0].x, pt[0].y, pt[1].x - pt[0].x, pt[1].y - pt[0].y);

		/// p1是起点，p2是终点，p0是计算起点  3个点都相对于矩形的中心(相对坐标）
		/// 1、首先计算 p0->p1->p2 顺时针还是逆时针
		QPoint p0 = QPoint((rect.center().x()+rect.width()/2) - rect.center().x(), 0);      ///< 计算的起点
		QPoint p1 = QPoint((pt[2].x - rect.center().x()), -(pt[2].y - rect.center().y()));   ///< 起始点向量
		QPoint p2 = QPoint((pt[3].x - rect.center().x()), -(pt[3].y - rect.center().y()));   ///< 终点向量


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

		if (LA->snFillType > 1)
		{
			if(!ms_bIsUseUserClr)
			{
				ms_pPainter->setBrush(MakeColor(LA->nLineC));
			}
			else
			{
				ms_pPainter->setBrush(ms_cUserClr);
			}
		}
		ms_pPainter->scale(1.0, -1.0);
		ms_pPainter->drawPie(QRect(rect.left(), -rect.bottom(), rect.width(), rect.height()), dStart * 16, dSpan * 16);
		ms_pPainter->scale(1.0, -1.0);

		pt = nullptr;

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

	void XJBDrawHelper::drawArc(QJB_ITEM_UNIT_HEAD* _pUnitInfo)
	{
		LINE_ATTR *LA = (LINE_ATTR *)(_pUnitInfo + 1);

		setLineAttr(LA);

		QJB_POINT *pt = (QJB_POINT*)(LA + 1);

		/// 外边框
		QRect rect(pt[0].x, pt[0].y, pt[1].x - pt[0].x, pt[1].y - pt[0].y);

		/// p1是起点，p2是终点，p0是计算起点  3个点都相对于矩形的中心(相对坐标）
		/// 1、首先计算 p0->p1->p2 顺时针还是逆时针
		QPoint p0 = QPoint((rect.center().x()+rect.width()/2) - rect.center().x(), 0);      ///< 计算的起点
		QPoint p1 = QPoint((pt[2].x - rect.center().x()), -(pt[2].y - rect.center().y()));   ///< 起始点向量
		QPoint p2 = QPoint((pt[3].x - rect.center().x()), -(pt[3].y - rect.center().y()));   ///< 终点向量


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

		pt = nullptr;
		LA = nullptr;
	}

	void XJBDrawHelper::drawPolygon(QJB_ITEM_UNIT_HEAD* _pUnitInfo)
	{
		LINE_ATTR *LA = (LINE_ATTR *)(_pUnitInfo + 1);

		if (LA->snOutLineWidth)
			return;

		int *DATA = (int *)(LA + 1);
		int nDotNum = *DATA++;

		QJB_POINT *pt = (QJB_POINT*)(DATA);

		checkPointCache(nDotNum);

		if(LA->snFillType>2)
		{
			QPointF *qoutd = ms_cQPoints;
			for(int i=0; i<nDotNum; i++)
			{
				qoutd[i] = QPointF(pt[i].x, pt[i].y);
			}

			QBrush brush;
			brush.setStyle((Qt::BrushStyle)(LA->snFillType-3));
			if(!ms_bIsUseUserClr)
			{
				brush.setColor(MakeColor(LA->unFillC));
			}
			else
			{
				//setBrush(m_userColor);
				brush.setColor(ms_cUserClr);
			}
			ms_pPainter->setBrush(brush);
			//setBrush(Qt::red);
			ms_pPainter->drawPolygon(qoutd, nDotNum);
			qoutd = nullptr;
		}
		else 
		{
			QBrush brush;
			brush.setStyle(Qt::SolidPattern);
			if(!ms_bIsUseUserClr)
			{
				brush.setColor(MakeColor(LA->unFillC));
			}
			else
			{
				//setBrush(m_userColor);
				brush.setColor(ms_cUserClr);
			}

			ms_pPainter->setBrush(brush);

			QPointF *pQoutd = ms_cQPoints;
			for(int i=0; i<nDotNum; i++)
			{
				pQoutd[i] = QPointF(pt[i].x, pt[i].y);
			}
			ms_pPainter->drawPolygon(pQoutd, nDotNum);
			pQoutd = nullptr;
		}

		DATA = nullptr;
		pt = nullptr;
	}

	void XJBDrawHelper::drawText(QJB_ITEM_UNIT_HEAD* _pUnitInfo)
	{
		TEXT_ATTR *TA = (TEXT_ATTR *)(_pUnitInfo + 1);

		//if(!ms_bIsUseUserClr)
		{
			QPen pen = ms_pPainter->pen();
			pen.setColor(MakeColor(TA->unTextColor));
			ms_pPainter->setPen(pen);
		}
// 		else
// 		{
// 			QPen pen = ms_pPainter->pen();
// 			pen.setColor(ms_cUserClr);
// 			ms_pPainter->setPen(pen);
// 		}

		QJB_POINT *pt = (QJB_POINT *)(TA+1);

		QFont font;
		font.setFamily(QString::fromLocal8Bit(TA->cFontName));
		font.setPointSize(TA->snFontHight * ms_fTextFactor);
		ms_pPainter->setFont(font);

		ms_pPainter->scale(1.0, -1.0);

		double dDistance = abs(ms_cBoxRct.bottom() - pt->y);
		double dScaleOffset = ms_cBoxRct.top() + dDistance + ms_pPainter->fontMetrics().height();

		ms_pPainter->translate(0, -ms_cBoxRct.center().y()*2);

		ms_pPainter->drawText(QPointF(pt->x ,dScaleOffset),QString::fromLocal8Bit((char*)(pt+1)));

		ms_pPainter->scale(1.0, -1.0);

		pt = nullptr;
	}

	const bool XJBDrawHelper::DrawJB(QPainter* _p, const XJBItem* _pJBItem, QVector<QPointF>& _dataPnts, bool _bIsDrawDynamicLine /*= false*/, bool _bIsDetail /*= false*/)
	{
		if(nullptr == _p || nullptr == _pJBItem || nullptr == _pJBItem->m_pJBDrawInfo)
		{
			return false;
		}

		Q_JBItem* pDrawItem = _pJBItem->m_pJBDrawInfo;
		ms_pPainter = _p;
		ms_pPainter->save();


		// 判断是否绘制动态线--如果绘制静态线，将只使用设定为中心点，其他动态点将不被使用
		if(false == _bIsDrawDynamicLine)
		{
			QPointF centerPt;
			if(_pJBItem->m_cCenterPnt.x() != COOR_FAILD_VAL && _pJBItem->m_cCenterPnt.y() != COOR_FAILD_VAL)
			{
				centerPt = _pJBItem->m_cCenterPnt;
			}else
			{
				if(_dataPnts.isEmpty())
				{
					return false;
				}
				centerPt = _dataPnts.first();
			}

			ms_pPainter->translate(centerPt);
			// 旋转
			ms_pPainter->rotate(_pJBItem->m_dAngle);

			// 计算包围盒
			ms_cBoxRct = pDrawItem->rectBound;
			ms_dScaleX = (float)_pJBItem->m_cSize.width() / ms_cBoxRct.width() * ms_fJBFactor;
			ms_dScaleY = (float)_pJBItem->m_cSize.height() / ms_cBoxRct.height() * ms_fJBFactor;
			ms_dScale = std::min(ms_dScaleX,ms_dScaleY);
			ms_pPainter->translate(-ms_cBoxRct.center().x()*ms_dScale,ms_cBoxRct.center().y()*ms_dScale);
			ms_pPainter->scale(ms_dScale, -ms_dScale);
			ms_pPainter->setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);
		
		}
		
		foreach(Q_JBItemUnitInfo unitInfo,pDrawItem->cDrawUnitInfo)
		{
			switch(unitInfo.unType)
			{
			case U_BLINE:	// 曲线&直线
				{
					dwBLine(unitInfo,_pJBItem,_dataPnts,_bIsDetail);
				}
				break;
			case U_BBLINE:
				{
					dwBBLine(unitInfo,_pJBItem,_dataPnts,_bIsDetail);
				}break;
			case U_PLINE:
			case U_LINE:
				{
					dwPLine(unitInfo,_pJBItem,_dataPnts,_bIsDetail);
				}break;
			case U_RECT:
				{
					dwRect(unitInfo,_pJBItem,_dataPnts,_bIsDetail);
				}break;
			case U_ELLI:
				{
					dwEllipse(unitInfo,_pJBItem,_dataPnts,_bIsDetail);
				}break;
			case U_PIE:
				{
					dwPie(unitInfo,_pJBItem,_dataPnts,_bIsDetail);
				}break;
			case U_ARC:
				{
					dwArc(unitInfo,_pJBItem,_dataPnts,_bIsDetail);
				}break;
			case U_PGON:
				{
					dwPolygon(unitInfo,_pJBItem,_dataPnts,_bIsDetail);
				}break;
			case U_TEXT:
				{
					dwText(unitInfo,_pJBItem,_dataPnts,_bIsDetail);
				}break;
			case U_TEXTBORDER:
				{
					//dwTextBorder(unitInfo,_pJBItem,_dataPnts,_bIsDetail); // 绘制位置不符
				}break;
			default:
				break;
			}
			//break;
		}
		ms_pPainter->restore();

		return true;
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

	void XJBDrawHelper::dwBLine(Q_JBItemUnitInfo& _cRefUnit, const XJBItem* _pItem, QVector<QPointF>& _dataPnts /*= QList<QPointF>()*/, bool _bIsDetail /*= false*/)
	{
		if (0 < _cRefUnit.cLineAttr.snOutLineWidth)
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

	void XJBDrawHelper::dwBLine(Q_JBItemUnitInfo& _cRefUnit)
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

	void XJBDrawHelper::dwBBLine(Q_JBItemUnitInfo& _cRefUnit, const XJBItem* _pItem, QVector<QPointF>& _dataPnts, bool _bIsDetail /*= false*/)
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
		foreach(Q_JBUnitSegm parentSegm,_cRefUnit.cSegms)
		{
			QVector<QPointF> dwPnts;
			QVector<QPointF> dataPnt;
			QVector<QPointF> ctlPnt;
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
	}

	void XJBDrawHelper::dwBBLine(Q_JBItemUnitInfo& _cRefUnit)
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
		foreach(Q_JBUnitSegm parentSegm,_cRefUnit.cSegms)
		{
			QVector<QPointF> dwPnts;
			QVector<QPointF> dataPnt;
			QVector<QPointF> ctlPnt;
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
	}

	void XJBDrawHelper::dwPLine(Q_JBItemUnitInfo& _cRefUnit, const XJBItem* _pItem, QVector<QPointF>& _dataPnts, bool _bIsDetail /*= false*/)
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
	}

	void XJBDrawHelper::dwPLine(Q_JBItemUnitInfo& _cRefUnit)
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
	}

	void XJBDrawHelper::dwRect(Q_JBItemUnitInfo& _cRefUnit, const XJBItem* _pItem, QVector<QPointF>& _dataPnts, bool _bIsDetail /*= false*/)
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
	}

	void XJBDrawHelper::dwRect(Q_JBItemUnitInfo& _cRefUnit)
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
	}

	void XJBDrawHelper::dwEllipse(Q_JBItemUnitInfo& _cRefUnit, const XJBItem* _pItem, QVector<QPointF>& _dataPnts, bool _bIsDetail /*= false*/)
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
	}

	void XJBDrawHelper::dwEllipse(Q_JBItemUnitInfo& _cRefUnit)
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
	}

	void XJBDrawHelper::dwPie(Q_JBItemUnitInfo& _cRefUnit, const XJBItem* _pItem, QVector<QPointF>& _dataPnts, bool _bIsDetail /*= false*/)
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
	}

	void XJBDrawHelper::dwPie(Q_JBItemUnitInfo& _cRefUnit)
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
	}

	void XJBDrawHelper::dwArc(Q_JBItemUnitInfo& _cRefUnit, const XJBItem* _pItem, QVector<QPointF>& _dataPnts, bool _bIsDetail /*= false*/)
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
	}

	void XJBDrawHelper::dwArc(Q_JBItemUnitInfo& _cRefUnit)
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
	}

	void XJBDrawHelper::dwPolygon(Q_JBItemUnitInfo& _cRefUnit, const XJBItem* _pItem, QVector<QPointF>& _dataPnts, bool _bIsDetail /*= false*/)
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
	}

	void XJBDrawHelper::dwPolygon(Q_JBItemUnitInfo& _cRefUnit)
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
	}

	void XJBDrawHelper::dwText(Q_JBItemUnitInfo& _cRefUnit, const XJBItem* _pItem, QVector<QPointF>& _dataPnts, bool _bIsDetail /*= false*/)
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
	}

	void XJBDrawHelper::dwText(Q_JBItemUnitInfo& _cRefUnit)
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
	}

	void XJBDrawHelper::dwTextBorder(Q_JBItemUnitInfo& _cRefUnit, const XJBItem* _pItem, QVector<QPointF>& _dataPnts, bool _bIsDetail /*= false*/)
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

	void XJBDrawHelper::dwTextBorder(Q_JBItemUnitInfo& _cRefUnit)
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

	const bool XJBDrawHelper::DrawLineJB(QPainter* _p, const XShapeLineJB* _pLinePtr, QList<QPointF>& _qScrPnts)
	{
		if(nullptr == _p || nullptr == _pLinePtr || false == _pLinePtr->IsValid())
			return false;

		const ExceptiveInfo* pExInfo = _pLinePtr->GetExceptiveInfo();
		if(nullptr == pExInfo || _qScrPnts.count() > 1 )
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
			pen.setColor(_pLinePtr->GetLineClr());
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
}

