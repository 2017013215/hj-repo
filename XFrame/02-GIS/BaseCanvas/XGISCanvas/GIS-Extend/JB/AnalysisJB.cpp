#include <QString>

#include "JBType.h"
#include "AnalysisJB.h"
#include "CalcLib.h"
#include "LineJB.h"

extern int bzjd;

void CAnalysisJB::DrawPoint(JB_UNIT_HEAD* rJB)
{
    int nCode = 1;
    if(rJB->sColorGrpNo > 9)
    {
        rJB->sColorGrpNo = 0;
    }

    nCode <<= rJB->sColorGrpNo;


    /// 绘制图元的信息
    LINE_ATTR*  pLineAtter = 0;
    TEXT_ATTR* pTextAtter = 0;
    BMP_ATTR*   pBmpAtter = 0;

    switch(rJB->sType)
    {
    case U_BLINE:
        pLineAtter=(LINE_ATTR *)(rJB+1);
        CallBLine((int*)(pLineAtter+1),true,pLineAtter->sClose);
        break;
    case U_BBLINE:
        pLineAtter=(LINE_ATTR *)(rJB+1);
        CallBBLine((int*)(pLineAtter+1),true,pLineAtter->sClose);
        break;
    case U_PLINE:
    {
        pLineAtter=(LINE_ATTR *)(rJB+1);
        int* pDotNum = (int*)(pLineAtter+1);
        CallPLine((POINT*)(pDotNum+1),*pDotNum);
    }
        break;
    case U_LINE: /// 直线
        pLineAtter=(LINE_ATTR *)(rJB+1);
        CallPLine((POINT*)(pLineAtter+1),2);
        break;
    case U_RECT: /// 矩形
        pLineAtter=(LINE_ATTR *)(rJB+1);
        CallRect((POINT*)(pLineAtter+1));
        break;
    case U_ELLI: /// 椭圆
        pLineAtter=(LINE_ATTR *)(rJB+1);
        CallEllipse((int*)(pLineAtter+1));
        break;
    case U_PIE: /// 饼
        pLineAtter=(LINE_ATTR *)(rJB+1);
        CallPie((int*)(pLineAtter+1),1);
        break;
    case U_ARC: /// 圆弧
        pLineAtter=(LINE_ATTR *)(rJB+1);
//        CallPie((int*)(pLineAtter+1),0);
        break;
    case U_PGON:
        pLineAtter=(LINE_ATTR *)(rJB+1);
        CallPgon((int*)(pLineAtter+1));
        break;
    case U_TEXT:
        pTextAtter = (TEXT_ATTR*)(rJB+1);
        break;
    case U_TEXTBORDER:
        pTextAtter = (TEXT_ATTR*)(rJB+1);
        break;
    case U_BITMAP:
        pBmpAtter = (BMP_ATTR*)(rJB+1);
        break;
    }

    /// 设置线属性
    if(0 != pLineAtter)
    {
        SetLineAtter(pLineAtter);
    }

    if(0 != pTextAtter)
    {
    }

    if(0 != pBmpAtter)
    {
    }
}

/// 设置线属性
void CAnalysisJB::SetLineAtter(LINE_ATTR *pLineAttr)
{
}

/// 设置字符
void CAnalysisJB::CallText(POINT *pText)
{
}

/// 多点填充：中心(POINT),图形数(int) ,图形1点数(int),图形2点数(int),... POINT 1,POINT 2, ...
void CAnalysisJB::CallPgon(int *pData)
{
    POINT* pCenter = (POINT*)(pData);

    int* pNumPgon = (int*)(pCenter+1);
    int nNumCount = *pNumPgon;

    POINT* pPoint = (POINT*)(pNumPgon+nNumCount + 1);

    ++pNumPgon;
    ///
    for(int i=0; i<nNumCount; ++i)
    {
        /// 点数
        *pNumPgon;

        /// 移动指针位置
        pPoint += *pNumPgon;
    }
}

/// 四个点分别是 left_top right_bottom x1_y1 x2_y2
/// 逆时针为正
void CAnalysisJB::CallPie(int *pData,int nFlag)
{
    /// 4*7 + 1 = 29个字节
    int intnum = (1+ 2*3)*4 + 1;
    int *d=new int[intnum];

    PieDataToBLine(pData,d,nFlag);

    delete []d;
}

/// 抽取矩形数据
void CAnalysisJB::CallRect(POINT *pData)
{
    POINT pt0[5];

    /// 只有 左上和右下 俩点
    /// 组合成 矩形
    pt0[0].x=pData[0].x;  pt0[0].y=pData[0].y;
    pt0[1].x=pData[1].x;  pt0[1].y=pData[0].y;
    pt0[2].x=pData[1].x;  pt0[2].y=pData[1].y;
    pt0[3].x=pData[0].x;  pt0[3].y=pData[1].y;
    pt0[4].x=pData[0].x;  pt0[4].y=pData[0].y;
}

void CAnalysisJB::CallEllipse(int *pData)
{
    int intnum = (1+ 2*3)*4 + 1;
          int *d=new int[intnum];
    EllipseDataToBLine(pData,d);
    delete[] d;
}

/*================== Sutherland_Hodgman 面裁剪算法 ===================*/
/**********************************************************************/
/*                                                                    */
/*        功    能: Sutherland_Hodgman 裁剪算法(1) --- 判断顶点内外   */
/*        模 块 名: DLG_MANA2.C                                       */
/*        调 用 名: int inside(p,clip)                                */
/*        入口参数: int *p              顶点座标                      */
/*                  int *clip           裁剪边点列座标                */
/*        返 回 值: = 0                 顶点在裁剪边之外              */
/*                  = 1                 顶点在裁剪边之内              */
/*                                                                    */
/**********************************************************************/
int CAnalysisJB::inside(POINT p,POINT *c)
{
double long vw,vc,vp,wc,wp;

    vc=c[1].x-c[0].x;
    wp=p.y-c[0].y;
    vp=p.x-c[0].x;
    wc=c[1].y-c[0].y;
    vw=vc*wp-vp*wc;

    if(vw<=0L) return 1;
    else return 0;
}
#define ROUND(x)  int((x>=0)?x+0.5 : x-0.5)

/**********************************************************************/
/*                                                                    */
/*        功    能: Sutherland_Hodgman 裁剪算法(2) --- 计算交点Ｘ座标 */
/*        模 块 名: DLG_MANA2.C                                       */
/*        调 用 名: int intersect_x(a,b,c)                           */
/*        入口参数: POINT a              多边形起点座标                */
/*                  POINT b              多边形终点座标                */
/*                  POINT *c              裁剪边点列座标                */
/*        返 回 值:                     交点的Ｘ座标                  */
/*                                                                    */
/**********************************************************************/
int CAnalysisJB::intersect_x(POINT a,POINT b,POINT *c)
{
    double x;

    if(c[1].x==c[0].x)
    {
        return(c[0].x);
    }
    else
    {
        if(a.y!=b.y)
        {
            x = c[0].y-a.y;
            x = x*(b.x-a.x);
            x = x/(b.y-a.y);
            x = x+a.x;
        }
        else return(0x7fffffff);
    }
    return(ROUND(x));
}

/**********************************************************************/
/*                                                                    */
/*        功    能: Sutherland_Hodgman 裁剪算法(3) --- 计算交点Ｙ座标 */
/*        模 块 名: DLG_MANA2.C                                       */
/*        调 用 名: int intersect_y(a,b,c)                           */
/*        入口参数: POINT a              多边形起点座标                */
/*                  POINT b              多边形终点座标                */
/*                  POINT *c              裁剪边点列座标                */
/*        返 回 值:                     交点的Ｙ座标                  */
/*                                                                    */
/**********************************************************************/
int CAnalysisJB::intersect_y(POINT a,POINT b,POINT *c)
{
double y;

   if(c[1].y==c[0].y) return(c[0].y);
    if(a.x!=b.x) {
        y = c[0].x-b.x;
        y = y*(a.y-b.y);
        y = y/(a.x-b.x);
        y = y+b.y;
   }
   else return(0x7fffffff);

   return(ROUND(y));
}
/**********************************************************************/
/*                                                                    */
/*        功    能: Sutherland_Hodgman 裁剪算法(0)                    */
/*        模 块 名: DLG_MANA2.C                                       */
/*        调 用 名: int S_H_CLIP(out_v,in_length,in_v,clip)           */
/*        入口参数: int in_length       多边形节点数                  */
/*                  POINT *in_v           多边形点列座标                */
/*                  POINT *clip           裁剪边点列座标                */
/*        出口参数: POINT *out_v          多边形点列座标                */
/*        返 回 值: int out_length      多边形节点数                  */
/*                                                                    */
/**********************************************************************/
int CAnalysisJB::S_H_CLIP(POINT *in_v,POINT * out_v,int in_length,POINT *clip)
{
    int out_length,j;
    POINT a,b,p;

    out_length = 0;

//    if(CloseShape)
    {
        a.x	= in_v[in_length-1].x;  // 闭合多边形????????????? 2000.6.28日改
        a.y	= in_v[in_length-1].y;
    }
//    else
    {
        a.x	= in_v[0].x;
        a.y	= in_v[0].y;
    }

    for(j=0;j < in_length;j++)
    {
        b.x	= in_v[j].x;
        b.y	= in_v[j].y;
        if(inside(b,clip))
        {				/* Ｂ在内 */
            if(inside(a,clip))
            {				/* Ａ在内 */
                if(out_length==0 || out_v[out_length-1].x!=b.x || out_v[out_length-1].y!=b.y)
                {
                    out_v[out_length].x	= b.x;
                    out_v[out_length].y	= b.y;
                    out_length++;
                }
            }
            else
            {					/* Ａ在外 */
                p.x=intersect_x(a,b,clip);
                p.y=intersect_y(a,b,clip);
                if(out_length==0 || out_v[out_length-1].x!=p.x || out_v[out_length-1].y!=p.y)
                {
                    out_v[out_length].x	= p.x;
                    out_v[out_length].y	= p.y;
                    out_length++;
                }

                if(out_length==0 || out_v[out_length-1].x!=b.x || out_v[out_length-1].y!=b.y)
                {
                    out_v[out_length].x	= b.x;
                    out_v[out_length].y	= b.y;
                    out_length++;
                }
            }
        }
        else
        {						/* Ｂ在外 */
            if(inside(a,clip))
            {            		/* Ａ在内 */
                p.x=intersect_x(a,b,clip);
                p.y=intersect_y(a,b,clip);
                if(out_length==0 || out_v[out_length-1].x!=p.x || out_v[out_length-1].y!=p.y)
                {
                    out_v[out_length].x	= p.x;
                    out_v[out_length].y	= p.y;
                    out_length++;
                }
            }
        }
        a.x = b.x;
        a.y = b.y;
    }
    return(out_length);

}

/*============================ CLIP1 =================================*/
/**********************************************************************/
/*        功    能: 面要素裁切                                        */
/*        模 块 名: DLG_MANA4.C                                       */
/*	  调 用 名: int clip_area()
/*	  入口参数:      RECT r					裁剪矩形
/*                  int  count         多边形点数                    */
/*                  POINT * in          多边形点列座标                */
/*   出口参数: 	  POINT *out          多边形点列座标                */
/*        返 回 值: count              多边形点数                    */
/**********************************************************************/
int CAnalysisJB::ClipArea(POINT * in,POINT * out,int count,RECT r)
{
    POINT * out1;
    POINT win[5];
    int flag=0;

    win[0].x = win[1].x = win[4].x = r.left;
    win[0].y = win[3].y = win[4].y = r.top;
    win[2].x = win[3].x = r.right ;
    win[1].y = win[2].y = r.bottom;

    out1=new POINT[count*2];
    count = S_H_CLIP(in,out1,count,&win[0]);
    if(count>=2)
    {
        count = S_H_CLIP(out1,out,count,&win[1]);
        flag = 1;
    }

    if(count>=2)
    {
        count = S_H_CLIP(out,out1,count,&win[2]);
        flag =0;
    }

    if(count>=2)
    {
        count = S_H_CLIP(out1,out,count,&win[3]);
        flag =1;
    }


    if(flag==0) memcpy(out,out1,count * sizeof(POINT));

    delete out1;

    return(count);
}

/// 计算PLine
void CAnalysisJB::CallPLine(POINT *pData, int nNum)
{
    static DLK stDlk[] ={{1,2},{2,2},{3,2},{4,2},{8,3},{16,4},{3,3},{1,4},{3,4},{4,4},{8,8}};

    /// 都不用Round
    int JL,JK,JD,JW;
    int LW;
    POINT * outd;

    int LineType=3;

    if(LineType==1)
    {
        return;
    }

    //    LW=(ChgLineW)?LineWidth * ScaleX : LineWidth;
    if(LW<=0)
    {
        LW=1;
    }

    if(LineType>=3)
    {
        //        BeginPath(hdc);
        //        if(MapMode==DEVICE)
        {
            outd=pData;
            goto dodraw;
        }

        outd=new POINT[nNum*2];
        //        LPtoDP(D,nNum,outd);
//        nNum=ClipArea(outd,outd,nNum,ClipRect);
    }

dodraw:

    switch(LineType)
    {
    case 0:                    	//实线
    case 2:
//        Polyline1(pData,nNum);
        return;
    case 3:                    	//虚线
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
        if (LW>=1)
        {
            JL = stDlk[LineType-3].nDL*LW*0.6;
            JK = stDlk[LineType-3].nDK*LW*0.6;
        }
//        Draw_XuXian1(JL,JK,outd,nNum);
        break;
    case 14:                   	//点虚线
    case 15:
    case 16:
    case 17:
        JL = 20;
        JK = 5;
        JD = 5;

        switch(LineType)
        {
        case 15:
            JL = 30;
            break;
        case 16:
            JL = 27;
            JD = 10;
            break;
        case 17:
            JL=55;
            JK=12;
            JD=12;
            break;
        default:
            break;
        }

        if (LW>1)
        {
            JL = JL*LW*0.6;
            JK = JK*LW*0.6;
            JD = JD*LW*0.6;
        }
//        Draw_XuXian2(JL,JK,JD,outd,nNum);
        break;
    case 26:                   	//铁路线
    case 27:
    case 28:
    case 38:
    case 48:
    case 49:
    case 50:
    case 51:
        JL = 15;
        JW = 3;
        switch (LineType)
        {
        case 27:
            JL = 22;
            break;
        case 28:
        case 38:
            JL = 30;
            break;
        case 48:
        case 49:
            JL = 5;
            break;
        case 50:
        case 51:
            JL = 15;
            break;
        default:
            break;
        }

        if (LW>1)
        {
            JL = JL*LW*0.6;
            JW = JW*LW*0.6;
        }
//        Draw_XuXian3(JL,JW,outd,nNum,LineType);
        break;
    case 36:                      //国界线
    case 37:
        JL = 18;
        JK = 5;
        JW = 3;     					//横线长
        if (LineType==37)	{ JL=25; JK=8; }
        if (LW>1) {
            JL = JL*LW*0.6;
            JK = JK*LW*0.6;
            JW = JW*LW*0.6;
        }
//        Draw_XuXian4(JL,JK,JW,outd,nNum);
        break;
    case 39:
    case 40:
        JL = 20;
        JK = 5;
        JD = 3;							//X半径
        if (LW>1) {
            JL = JL*LW*0.6;
            JK = JK*LW*0.6;
            JD = JD*LW*0.6;
        }
        CLineJB::Draw_XuXian5(JL,JK,JD,outd,nNum,LineType);
        break;
    default:
        break;
    }

    if(outd!=pData) delete outd;
}

/// 绘制 图元
void CAnalysisJB::DrawGroup(const QT_GROUP &rGroup)
{
    int nCount = rGroup.byteInfo.size(),uLen;

    JB_UNIT_HEAD* pJBUH = (JB_UNIT_HEAD *)rGroup.byteInfo.data();

    /// 遍历所有基本绘制图元进行绘制
    for(;nCount>0;nCount -=  uLen)
    {
        uLen = pJBUH->unLen;

        /// 绘制基本图元
        DrawPoint(pJBUH);

        /// 移动数据指针
        pJBUH=(JB_UNIT_HEAD *)(((char *)pJBUH)+pJBUH->unLen);
    }
}

/// 计算BBLine
///   内含画线子程序   FillFlag=1 填充 CloseFlag=1 闭合
/// 形数(int)+长度1(int,Byte)+第一个BLine,+长度2+第二个... 长度含本身
void CAnalysisJB::CallBBLine(int* pData, bool bFill, int nClose)
{
    int i,*Data1[40];
    int nPolyNum;

    /// 线的数量
    nPolyNum=pData[0];
    if(nPolyNum>40)
    {
        nPolyNum=40;
    }

    ///
    Data1[0]=pData+2;
    for(i=0; i<nPolyNum; ++i)
    {
        if(0 != i)
        {
            Data1[i]=(int *)((BYTE *)Data1[i-1]+ *(Data1[i-1]-1));
        }

        /// 绘制线
        CallBLine(Data1[i],bFill,nClose);
    }
}

/// 计算BLine
///   画线子程序   FillFlag=1 填充 CloseFlag=1 闭合
///  意义      字节数
///---------------------点构成------------------
///  基本点数    4
///
///  标记           4   高字节  0位＝0直线＝1曲线  7位＝0不光滑，＝1光滑;
///                       低字节：控制点数
///  点1（x,y) (控制点1，控制点2)...
///---------------------------------------
void CAnalysisJB::CallBLine(int* pData, bool bFill, int nClose)
{
    /// 计算 点的 个数
    int nDataCount = BrowseLine(pData,nClose),*pBegin=0;

    POINT *xy0,* xy1,xy[4],dPt;
    xy0 =  xy1  = new POINT[nDataCount];

    /// 本段曲线的基本点数
    int nDotNum = *pData++,nCortlNum;
    pBegin  = pData;

    /// 遍历 曲线的点
    for(;nDotNum>0; --nDotNum)
    {
        /// 本段控制点个数
        nCortlNum = *pData & 0xFF;

        /// 判断是否有 控制点
        if((*pData & guanlian) /*&& HotSpotOff*/)
        {
            //            dPt=DPtoLP(xViewOrg-HotSpotX,yViewOrg-HotSpotY);
            //            dPt.x = dPt.x - pBegin[1];   dPt.y = dPt.y - pBegin[2];
        }
        else
        {
            dPt.x = dPt.y =0;
        }

        /// 如果是曲线
        if(*pData & quxian)
        {
            pData++;
            xy[0].x=*pData++ + dPt.x;
            xy[0].y=*pData++ + dPt.y;

            /// 一个控制点
            if(nCortlNum==1)
            {
                xy[1].x=*pData++ + dPt.x;
                xy[1].y=*pData++ + dPt.y;
            }
            else ///  两个控制点
            {
                /// 跳过第一个控制点
                pData++;
                pData++;
                xy[1].x=*pData++ + dPt.x;
                xy[1].y=*pData++ + dPt.y;
            }

            /// 最后一点且封闭
            if(nDotNum==1 && nClose)
            {
                pData=pBegin;
            }

            if((*pData & guanlian) /*&& HotSpotOff*/)
            {
                //                dPt=DPtoLP(xViewOrg-HotSpotX,yViewOrg-HotSpotY);
                //                dPt.x = dPt.x - pBegin[1];   dPt.y = dPt.y - pBegin[2];
            }
            else
            {
                dPt.x = dPt.y =0;
            }

            xy[3].x=pData[1] + dPt.x;
            xy[3].y=pData[2] + dPt.y;

            xy[2].x=pData[3] + dPt.x;
            xy[2].y=pData[4] + dPt.y;

            /// 计算贝塞尔曲线
            Bezier(xy,4,xy1);

            xy1+=bzjd;
        }
        else /// 直线
        {
            pData++;

            xy1->x=*pData++ + dPt.x;
            xy1->y=*pData++ + dPt.y;
            xy1++;

            ///  一个控制点
            if(nCortlNum==1)
            {
                pData++;
                pData++;
            }

            /// 最后一点且封闭
            if(nDotNum==1 && 1 == nClose)
            {
                pData=pBegin;
            }

            /// 判断是否
            if((*pData & guanlian)/* && HotSpotOff*/)
            {
                //                dPt=DPtoLP(xViewOrg-HotSpotX,yViewOrg-HotSpotY);
                //                dPt.x = dPt.x - pBegin[1];   dPt.y = dPt.y - pBegin[2];
            }
            else
            {
                dPt.x = dPt.y =0;
            }

            xy1->x=pData[1] + dPt.x;
            xy1->y=pData[2] + dPt.y;
        }

        /// 如果
        if(nDotNum==2 && nClose==0)
        {
            break;
        }
    }

    //    if(bFill)
    //    {
    //        DrawPolygon();
    //    }
    //    else
    //    {
    //        DrawLine(xy1);
    //    }

    /// 删除
    delete []xy0;
}
