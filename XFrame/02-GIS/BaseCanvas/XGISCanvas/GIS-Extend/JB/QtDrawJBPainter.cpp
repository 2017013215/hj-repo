#include "QtDrawJBPainter.h"

#include <QDebug>

#include <QRect>
#include <QFont>
#include <QColor>
#include <qmath.h>
#include <math.h>

/// 曲线标志
static const int const_curve_flag = 0x100;

CQtDrawJBPainter::CQtDrawJBPainter(QPaintDevice *pDevice)
    : QPainter(pDevice),
    m_bUseUserColor(false),
	m_userColor(Qt::red)
{
	init();
}

void CQtDrawJBPainter::init()
{
	m_sizeJB			= QSize(160, 160);
    m_dScale			= 1.0f;
    m_dJBFactor			= 0.72f;
    m_dTextSizeFactor	= 0.75f;
	m_nBezierPrecision	= 15;
}

void CQtDrawJBPainter::SetJBSize(QSize rSize)
{
    m_sizeJB = rSize;
}

void CQtDrawJBPainter::SetJBSize(int nWidth, int nHeight)
{
    m_sizeJB.setWidth(nWidth);
    m_sizeJB.setHeight(nHeight);
}

void CQtDrawJBPainter::SetUserColor(const QColor& rColor)
{
	if(m_userColor != rColor)
	{
		m_userColor = rColor;
    }
}

void CQtDrawJBPainter::setUseUserColor(bool b)
{
    m_bUseUserColor = b;
}
/// 设置贝塞尔精度
void CQtDrawJBPainter::SetBezierPrecision(int nPrecision)
{
    m_nBezierPrecision = nPrecision;
}

void CQtDrawJBPainter::DrawGroup(const QT_GROUP &rGroup)
{
    //qDebug() << "________________________________________________";
    //qDebug() << "Start Painter Group!";

	if(rGroup.unID == 10)
	{
		int md = 0;
	}

    save();

    // 更新包围盒
	m_rectBox.setTopLeft(QPoint(rGroup.rectBound.nLeft, rGroup.rectBound.nTop));
	m_rectBox.setBottomRight(QPoint(rGroup.rectBound.nRight, rGroup.rectBound.nBottom));

    // 计算缩放比列
	float dScaleX = (float)m_sizeJB.width() / m_rectBox.width() * m_dJBFactor;
	float dScaleY = (float)m_sizeJB.height() / m_rectBox.height() * m_dJBFactor;
	m_dScale = min(dScaleX, dScaleY);

    // 移动到包围盒中心,并进行缩放
    translate(qreal(-m_rectBox.center().x() * m_dScale), qreal(m_rectBox.center().y() * m_dScale));
    scale(m_dScale, -m_dScale);

    // 绘制包围盒（测试）
	/*QPen boxPen;
	boxPen.setColor(Qt::green);
	boxPen.setWidthF(1.0 / m_dScale);
	setPen(boxPen);
	QRectF selRt(0, 0, m_sizeJB.width()*boxPen.widthF(), m_sizeJB.height()*boxPen.widthF());
	drawRect(selRt);*/

#if 1
    //
    unsigned int uLen;                              ///< 单条图元的长度
    unsigned int uCount = rGroup.byteInfo.size();   ///< 总长度.

    // 获取头
    JB_UNIT_HEAD *pJBUH = (JB_UNIT_HEAD *)rGroup.byteInfo.data();


    // 遍历所有基本绘制图元进行绘制
    for(;uCount > 0;uCount -= uLen)
    {
        uLen = pJBUH->unLen;

        // 绘制基本图元
        DrawPoint(pJBUH);

        // 移动数据指针
        pJBUH = (JB_UNIT_HEAD *)(((char *)pJBUH) + uLen);
	}
#else



#endif

    restore();

    //qDebug() << "End Painter Group!";
    //qDebug() << "________________________________________________";

}

void CQtDrawJBPainter::DrawPoint(JB_UNIT_HEAD *rJB)
{
    int nCode = 1;
    if(rJB->sColorGrpNo > 9)
    {
        rJB->sColorGrpNo = 0;
    }

    nCode <<= rJB->sColorGrpNo;

    switch(rJB->sType)
    {
    case U_BLINE:   /// 曲/直线
        DrawBLine(rJB);
        //qDebug() << "U_BLINE";
        break;
    case U_BBLINE:  /// 内含曲/直线
        DrawBBline(rJB);
        //qDebug() << "U_BBLINE";
        break;
    case U_PLINE:
        DrawPLine(rJB);
        //qDebug() << "U_PLINE";
        break;
    case U_LINE: /// 直线
        DrawPLine(rJB);
        //qDebug() << "U_LINE";
        break;
    case U_RECT: /// 矩形
        DrawRect(rJB);
        //qDebug() << "U_RECT";
        break;
    case U_ELLI: /// 椭圆
        DrawEllipse(rJB);
        //qDebug() << "U_ELLI";
        break;
    case U_PIE: /// 饼
        DrawPie(rJB);
        //qDebug() << "U_PIE";
        break;
    case U_ARC: /// 圆弧
        DrawArc(rJB);
        //qDebug() << "U_ARC";
        break;
    case U_PGON:
        DrawPolygon(rJB);
        //qDebug() << "U_PGON";
        break;
    case U_TEXT:
        DrawText(rJB);
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
}

void CQtDrawJBPainter::DrawBLine(JB_UNIT_HEAD *rJB)
{
    LINE_ATTR *LA = (LINE_ATTR *)(rJB + 1);

    if (LA->sOutLineWidth)
        return;

    SetLineAttr(LA);

    POINT xy[4];
    int * d,*d0,nDotNum;
//    int CtrlDot,j;
    int CtrlDot;
//    float l1;

//    if(Dd->Path!=NULL) return;
//    Dd->Path = new Gdiplus::GraphicsPath;
//    Gdiplus::GraphicsPath * path = Dd->Path;

    bool bClose = LA->sClose;

    QPainterPath path;

    d=(int*)(LA + 1);

    int TotalDot = BrowseLine(d, bClose);
    POINT *xy0,*xy1;
    xy1 = xy0 = new POINT[TotalDot];

    nDotNum = *d++;								// 本曲线基本点数
    d0 = d;

    for(;nDotNum>0; nDotNum--)
    {
        CtrlDot=*d & 0xFF;						// 本段控制点数
        if(*d & const_curve_flag)				// 如果是曲线
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
            Bezier(xy, 4, xy1);

            for (int i=0;i<m_nBezierPrecision;++i) 
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

    if (LA->sFillType > 1)
    {
        //setBrush(Qt::red);

		xy1->x = xy0->x;
		xy1->y = xy0->y;

#if 0
		QPolygonF polygon;

		for (int i=0;i<TotalDot;++i) 
		{
			polygon.append(QPointF(xy0->x, xy0->y));
			++xy0;
		}

		drawPolygon(polygon);
#else
        QPointF *points = new QPointF[TotalDot];
        for (int i=0;i<TotalDot;++i)
        {
            points[i] = QPointF(xy0->x, xy0->y);
            ++xy0;
        }

        drawPolygon(points, TotalDot,Qt::WindingFill);
        delete [] points;
#endif
    }

    drawPath(path);
}

void CQtDrawJBPainter::DrawBBline(JB_UNIT_HEAD *rJB)
{
    LINE_ATTR *LA = (LINE_ATTR *)(rJB + 1);

    if (LA->sOutLineWidth)
        return;

    SetLineAttr(LA);

    bool bClose = LA->sClose;

    POINT *xy0,*xy1,xy[4];
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

        vNum[i] = BrowseLine(Data1[i], bClose);
        nTotalDot += vNum[i];
    }

    xy1 = xy0 = new POINT[nTotalDot];

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
            if(*d & const_curve_flag)				// 如果是曲线
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
                Bezier(xy, 4, xy1);

                for (int nBezier=0;nBezier < m_nBezierPrecision;++nBezier)
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

        POINT *xy2 = xy0+nDotCnt;
        if (LA->sFillType > 1)
        {
            xy1->x = xy2->x;
            xy1->y = xy2->y;
            QPointF *points = new QPointF[vNum[i]];
            for (int k=0;k<vNum[i];++k)
            {
                points[k] = QPointF(xy2->x, xy2->y);
                ++xy2;
            }

            drawPolygon(points, vNum[i],Qt::OddEvenFill);
            delete [] points;
        }

        /*
        POINT *xy2 = xy0+DotCnt;
        if (LA->sFillType > 1)
        {
            //setBrush(Qt::red);

//            xy1->x = xy2->x;
//            xy1->y = xy2->y;

            QPointF *points = new QPointF[vNum[i]];
            for (int i=0;i<vNum[i];++i)
            {
                points[i] = QPointF(xy2->x, xy2->y);
                ++xy2;
            }

            drawPolygon(points, vNum[i],Qt::OddEvenFill);
            delete [] points;
        }
        */

        setBrush(Qt::NoBrush);
        nDotCnt+=vNum[i];
        xy1=xy0+nDotCnt;
    }

    drawPath(path);

   // delete [] xy0;
}

void CQtDrawJBPainter::DrawPLine(JB_UNIT_HEAD *rJB)
{
    LINE_ATTR *LA = (LINE_ATTR *)(rJB + 1);

    if (LA->sOutLineWidth)
        return;

    int * d=(int *)(LA + 1);
    int DotNum=*d++;

    POINT *pt=(POINT*)d;

    //QBrush lb;
    //lb.setStyle(Qt::SolidPattern);
    //lb.setColor(MakeColor(LA->));
    //setBrush(lb);
    //QPen hpen1;
    //hpen1.setStyle(Qt::SolidLine);
    //hpen1.setCapStyle(Qt::FlatCap);
    //hpen1.setWidth(LA->sLineWidth);
    //setPen(hpen1);
    SetLineAttr(LA);
    setCompositionMode(QPainter::CompositionMode_SourceOver);

    QPoint * QD = new QPoint[DotNum];
    for(int i=0; i<DotNum; i++) 
	{
        QD[i].setX(pt[i].x);
        QD[i].setY(pt[i].y);
    }
    drawPolyline(QD, DotNum);
   // setPen(Qt::black);

    delete [] QD;
}

void CQtDrawJBPainter::DrawRect(JB_UNIT_HEAD *rJB)
{
    LINE_ATTR *LA = (LINE_ATTR *)(rJB + 1);

    if (LA->sOutLineWidth)
        return;

    SetLineAttr(LA);

    POINT *pt = (POINT*)(LA + 1);

    drawRect(QRect(pt[0].x, pt[0].y, pt[1].x - pt[0].x, pt[1].y - pt[0].y));
}

void CQtDrawJBPainter::DrawEllipse(JB_UNIT_HEAD *rJB)
{
    //setRenderHint(QPainter::Antialiasing);

    LINE_ATTR *LA = (LINE_ATTR *)(rJB + 1);

    if (LA->sOutLineWidth)
        return;

    SetLineAttr(LA);

    //setPen(MakeColor(LA->sLineC));
   // if (LA->sFillType > 1)
   // {
   //      QBrush brush;
   //      brush.setStyle(Qt::SolidPattern);
   //      brush.setColor(MakeColor(LA->nFillC));
   //      setBrush(brush);
   //     //setBrush(Qt::red);
   // }
   // else
   // {
   //     setBrush(Qt::NoBrush);
   // }
   // ////qDebug() << LA->sFillType;

    POINT *pt = (POINT*)(LA + 1);

    drawEllipse(QRect(pt[0].x, pt[0].y, pt[1].x - pt[0].x, pt[1].y - pt[0].y));
}

void CQtDrawJBPainter::DrawArc(JB_UNIT_HEAD *rJB)
{
    LINE_ATTR *LA = (LINE_ATTR *)(rJB + 1);

    SetLineAttr(LA);
	
    POINT *pt = (POINT*)(LA + 1);

    /// 外边框
    QRect rect(pt[0].x, pt[0].y, pt[1].x - pt[0].x, pt[1].y - pt[0].y);
// 	setPen(Qt::red);
// 	drawRect(rect);
// 	setPen(Qt::green);
    //qDebug()<< "Rect-Center:"<<rect.center();
    //qDebug() << "Point1:" << pt[0].x << " " << pt[0].y;
    //qDebug() << "Point2:" << pt[1].x << " " << pt[1].y;
    //qDebug() << "Point3:" << pt[2].x << " " << pt[2].y;
    //qDebug() << "Point4:" << pt[3].x << " " << pt[3].y;

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
    int nQuadrantP1 = Quadrant(p1.x(),p1.y());    ///< 象限
    //int nQuadrantP2 = Quadrant(p2.x(),p2.y());
	if (nQuadrantP1 == 1 || nQuadrantP1 == 2)
		dStart = -dStart;

    float dCosSpan = (p1.x() * p2.x() + p1.y() * p2.y())/
            (sqrt(float(p1.x() * p1.x() + p1.y() * p1.y()))*
             sqrt(float(p2.x() * p2.x() + p2.y() * p2.y())));

    dSpan = acos(dCosSpan)*180.0f/M_PI;

//     if (abs(nQuadrantP1 - nQuadrantP2)==2)
//     {
//         dSpan = 360.0 - dSpan;
//     }

    /// 为正时，p0->p1->p2 逆时针
    /// 为负时，           顺
    /// 为零时，           方向不变,
    int nCross = (p1.x() - p0.x())*(p2.y()-p1.y())-(p1.y()-p0.y())*(p2.x()-p1.x());
    if (nCross > 0)
    {
        dSpan = 360.0 - dSpan;
    }
    else if (nCross == 0)
    {
        dSpan = 0;
    }

	scale(1.0, -1.0);
	drawArc(QRect(rect.left(), -rect.bottom(), rect.width(), rect.height()), dStart * 16, dSpan * 16);
	scale(1.0, -1.0);

//    float dSpanAngle = acos((p1.x() * p2.x()+ p1.y() * p2.y())/(sqrt(float(p1.x()*p1.x() +p1.y()*p1.y()))*sqrt(float(p2.x()*p2.x()+p2.y()*p2.y()))));
//    dSpanAngle = dSpanAngle * 180 / 3.1415926;
//    //qDebug () << "Angle:" << spanAngle;
//    if (dSpanAngle < 0) /// 顺时针逆时针
//    {
//        dSpanAngle += 360;
//    }

//    float dStartAngle = acos((p3.x() * p2.x()+ p3.y() * p2.y())/(sqrt(float(p3.x()*p3.x() + p3.y()*p3.y()))*sqrt(float(p2.x()*p2.x()+p2.y()*p2.y()))));
//    //qDebug () << "startAngle1:" << dStartAngle;
//    dStartAngle = ((dStartAngle * 180) / 3.1415926);
//    //qDebug () << "startAngle2:" << startAngle;
//    if (dStartAngle < 0) /// 顺时针逆时针
//    {
//        dStartAngle += 360;
//    }

//    drawArc(rect, dStartAngle * 16, dSpanAngle * 16);

}

void CQtDrawJBPainter::DrawPie(JB_UNIT_HEAD *rJB)
{
    LINE_ATTR *LA = (LINE_ATTR *)(rJB + 1);

	if (LA->sOutLineWidth)
		return;

    SetLineAttr(LA);

    POINT *pt = (POINT*)(LA + 1);

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
    int nQuadrantP1 = Quadrant(p1.x(),p1.y());    ///< 象限
    //int nQuadrantP2 = Quadrant(p2.x(),p2.y());
    if (nQuadrantP1 == 1 || nQuadrantP1 == 2)
        dStart = -dStart;

    float dCosSpan = (p1.x() * p2.x() + p1.y() * p2.y())/
            (sqrt(float(p1.x() * p1.x() + p1.y() * p1.y()))*
             sqrt(float(p2.x() * p2.x() + p2.y() * p2.y())));

    dSpan = acos(dCosSpan)*180.0f/M_PI;

//     if (abs(nQuadrantP1 - nQuadrantP2)==2)
//     {
//         dSpan = 360.0 - dSpan;
//     }

    /// 为正时，p0->p1->p2 逆时针
    /// 为负时，           顺
    /// 为零时，           方向不变,
    int nCross = (p1.x() - p0.x())*(p2.y()-p1.y())-(p1.y()-p0.y())*(p2.x()-p1.x());
    if (nCross > 0)
    {
        dSpan = 360.0 - dSpan;
    }

	if (LA->sFillType > 1)
	{
		if(!m_bUseUserColor)
		{
			setBrush(MakeColor(LA->sLineC));
		}
		else
		{
			setBrush(m_userColor);
		}
	}
	scale(1.0, -1.0);
    drawPie(QRect(rect.left(), -rect.bottom(), rect.width(), rect.height()), dStart * 16, dSpan * 16);
    scale(1.0, -1.0);

//    QPoint p1 = QPoint((pt[2].x - rect.center().x()), (pt[2].y - rect.center().y()));
//    QPoint p2 = QPoint((pt[3].x - rect.center().x()), (pt[3].y - rect.center().y()));

//    float dSpanAngle = acos((p1.x() * p2.x()+ p1.y() * p2.y())/(sqrt(float(p1.x()*p1.x() +p1.y()*p1.y()))*sqrt(float(p2.x()*p2.x()+p2.y()*p2.y()))));
//    dSpanAngle = dSpanAngle * 180 / 3.1415926;
//    //qDebug () << "Angle:" << spanAngle;
//    if (dSpanAngle < 0) /// 顺时针逆时针
//    {
//        dSpanAngle += 360;
//    }

//    QPoint p3 = QPoint((rect.width() - rect.center().x()), rect.center().y());
//    float dStartAngle = acos((p3.x() * p2.x()+ p3.y() * p2.y())/(sqrt(float(p3.x()*p3.x() + p3.y()*p3.y()))*sqrt(float(p2.x()*p2.x()+p2.y()*p2.y()))));
//    //qDebug () << "startAngle1:" << startAngle;
//    dStartAngle = ((dStartAngle * 180) / 3.1415926);
//    //qDebug () << "startAngle2:" << startAngle;
//    if (dStartAngle < 0) /// 顺时针逆时针
//    {
//        dStartAngle += 360;
//    }

//    drawPie(rect, dStartAngle * 16, dSpanAngle * 16);
}

void CQtDrawJBPainter::DrawText(JB_UNIT_HEAD *rJB)
{
    TEXT_ATTR *TA = (TEXT_ATTR *)(rJB + 1);

	if(!m_bUseUserColor)
	{
        QPen pen = this->pen();
        pen.setColor(MakeColor(TA->sTextColor));
        setPen(pen);
	}
	else
	{
		QPen pen = this->pen();
        pen.setColor(m_userColor);
		setPen(pen);
	}

    POINT *pt = (POINT *)(TA+1);

    QFont font;
    font.setFamily(QString::fromLocal8Bit(TA->csFontName));
    font.setPointSize(TA->sFontHight * m_dTextSizeFactor);
    setFont(font);
    //int nOffsetX = abs(pt->x - m_rectBox.left());
    //int nOffsetY = abs(pt->y - m_rectBox.top());

	scale(1.0, -1.0);

	double dDistance = abs(m_rectBox.bottom() - pt->y);
	double dScaleOffset = m_rectBox.top() + dDistance + fontMetrics().height();

	translate(0, -m_rectBox.center().y()*2);

 	//setPen(Qt::red);
 	//drawRect(QRect(pt->x ,dScaleOffset, 30, 30));

	drawText(QPointF(pt->x ,dScaleOffset),QString::fromLocal8Bit((char*)(pt+1)));
	
    scale(1.0, -1.0);
}

void CQtDrawJBPainter::DrawPolygon(JB_UNIT_HEAD *rJB)
{
    LINE_ATTR *LA = (LINE_ATTR *)(rJB + 1);

    if (LA->sOutLineWidth)
        return;

    int *DATA = (int *)(LA + 1);
    int nDotNum = *DATA++;

    POINT *pt = (POINT*)(DATA);

    setRenderHint(QPainter::Antialiasing);

    if(LA->sFillType>2)
	{
        QPoint *qoutd = new QPoint[nDotNum];
        for(int i=0; i<nDotNum; i++)
        {
            qoutd[i] = QPoint(pt[i].x, pt[i].y);
        }

        QBrush brush;
        brush.setStyle((Qt::BrushStyle)(LA->sFillType-3));
		if(!m_bUseUserColor)
		{
			brush.setColor(MakeColor(LA->nFillC));
		}
		else
		{
			setBrush(m_userColor);
		}
        setBrush(brush);
        //setBrush(Qt::red);
        drawPolygon(qoutd, nDotNum);
        delete [] qoutd;
    }
    else 
	{
		QBrush brush;
		brush.setStyle(Qt::SolidPattern);
		if(!m_bUseUserColor)
		{
			brush.setColor(MakeColor(LA->nFillC));
		}
		else
		{
			setBrush(m_userColor);
		}

		setBrush(brush);

        QPoint *pQoutd = new QPoint[nDotNum];
        for(int i=0; i<nDotNum; i++)
        {
            pQoutd[i] = QPoint(pt[i].x, pt[i].y);
        }
        drawPolygon(pQoutd, nDotNum);
        delete [] pQoutd;
   }
}

void CQtDrawJBPainter::SetLineAttr(LINE_ATTR *LA)
{
    /*
    QPen pen;
    //pen.setWidth(LA->sLineWidth);
    pen.setStyle((Qt::PenStyle)(LA->sLineType-1));
    setPen(pen);
    */

	if(!m_bUseUserColor)
	{
        QPen pen = this->pen();
        pen.setColor(MakeColor(LA->sLineC));
		pen.setWidth(1.0 / m_dScale);//LXQ Add
        setPen(pen);

		if (LA->sFillType > 1)
			setBrush(MakeColor(LA->nFillC));
		else
			setBrush(Qt::NoBrush);
	}
	else
	{
		QPen pen = this->pen();
		pen.setColor(m_userColor);
		pen.setWidth(1.0 / m_dScale);//LXQ Add
		setPen(pen);
		if (LA->sFillType > 1)
		{
			setBrush(m_userColor);
		}
		else
			setBrush(Qt::NoBrush);
	}
}

int CQtDrawJBPainter::BrowseLine(int *Data, int flag)
{
    int *d = Data, DotNum;
    int TotalDot = 1;

    DotNum = *d++;  // 本曲线基本点数

    for (;DotNum > 0; DotNum--) 
	{
        if (*d & const_curve_flag) 
		{
            TotalDot+=m_nBezierPrecision + 1;
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

int CQtDrawJBPainter::Quadrant(int x, int y)
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

void CQtDrawJBPainter::Bezier(POINT *pt, int n, POINT *pxy0)
{
    int j,Seg;
    POINT * pxy;
    float t1,t2,t3,a1,a2,a3;

    Seg=(n-1)/3;  n=Seg*3+1;    // 曲线段数

    pxy=pxy0;

    do 
	{
        for (j=0; j<=m_nBezierPrecision;j++,pxy++)
		{
            t1=(float)j/m_nBezierPrecision;
            t2=t1*t1;
            t3=t2*t1;
            a1=-t3+3.0*(t2-t1)+1;
            a2=3.0*(t3-2.0*t2+t1);
            a3=3.0*(-t3+t2);  // a4=t3;
            pxy->x=a1*pt->x+a2*(pt+1)->x+a3*(pt+2)->x+t3*(pt+3)->x;
            pxy->y=a1*pt->y+a2*(pt+1)->y+a3*(pt+2)->y+t3*(pt+3)->y;
        }
        n-=3;  
		pt+=3;
    } 
	while(n>1);
}
