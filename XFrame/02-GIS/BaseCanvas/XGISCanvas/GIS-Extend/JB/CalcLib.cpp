//****************************************
//             公用计算库
//****************************************
#include <math.h>
#include <QTextCodec>
#include "CalcLib.h"
#define min(a, b)  (((a) < (b)) ? (a) : (b))
#define max(a, b)  (((a) > (b)) ? (a) : (b))

/// YTY bzjd 关键点
int bzjd = 14;
//-----------------------------
//  浏览线元，决定点数
//-----------------------------
int BrowseLine(int * Data,int flag)
{
    int * d=Data,DotNum;
    int TotalDot=1;

    DotNum=*d++;          // 本曲线基本点数
    for(;DotNum>0; DotNum--)
    {
        if(*d & quxian)// 如果是曲线
        {
            TotalDot+=bzjd;
        }
        else
        {
            TotalDot++;
        }

        /// 偏移一定的位置
        d+=(*d & 0xFF)*2+3;

        if(DotNum==2 && flag==0)
        {
            break;
        }
    }
    return (TotalDot);
}

//---------------------------------
//    构造贝赛尔曲线
//---------------------------------
int Bezier(POINT * pt,int n,POINT *pxy0)
{
    int j, Seg, DotNum=0;
    POINT * pxy;
    float t1, t2, t3, a1, a2, a3;

    Seg = (n-1)/3;
    n=Seg*3+1;    // 曲线段数

    pxy = pxy0;
loop:
    for (j=0; j<=bzjd; j++)
    {
        t1=(float)j/bzjd;

        t2=t1*t1;
        t3=t2*t1;
        a1=-t3+3.0*(t2-t1)+1;
        a2=3.0*(t3-2.0*t2+t1);
        a3=3.0*(-t3+t2);  // a4=t3;
        pxy->x=a1*pt->x+a2*(pt+1)->x+a3*(pt+2)->x+t3*(pt+3)->x;
        pxy->y=a1*pt->y+a2*(pt+1)->y+a3*(pt+2)->y+t3*(pt+3)->y;
        if(j==0
                || j==bzjd
                || abs((pxy-1)->x - pxy->x)>=2
                || abs((pxy-1)->y - pxy->y)>=2)
        {
            pxy++;
            DotNum++;
        }
    }
    n -=3;
    pt +=3;

    if(n>1)
    {
        goto loop;
    }

    return DotNum;
}

//========== 求弧与直线的交点 ==========
void FindPtInCurve(POINT *pt,int x,int y)
{
    //xy为交点，pt入4点(点1,控制点1,控制点2,点2)，
    //             出7点(首尾点1,7不变,中间插入:控制点1,2,交点,控制点3,4)
    float tmin,tmax,t1,t2,t3;
    float a1,a2,a3;
    float x1,y1,x2 =-30000,y2=-30000;
    float l1,l2,x01,y01;

    //---查找线上的一点，找到 t---
    tmin=0; tmax=1;
    x01=pt->x; y01=pt->y;
    do
    {
        t1=(tmax+tmin)/2;
        t2=t1*t1;
        t3=t2*t1;
        a1=-t3+3.0*(t2-t1)+1;
        a2=3.0*(t3-2.0*t2+t1);
        a3=3.0*(-t3+t2);  		// a4=t3;
        x1=a1*pt->x+a2*(pt+1)->x+a3*(pt+2)->x+t3*(pt+3)->x;
        y1=a1*pt->y+a2*(pt+1)->y+a3*(pt+2)->y+t3*(pt+3)->y;
        if(fabs(x1-x)<1 && fabs(y1-y)<1) break;
        l1=(x01 - x)*(x01 - x)+(y01-y)*(y01-y);
        l2=(x1 - x01)*(x1 - x01)+(y1-y01)*(y1-y01);

        if(l1>l2)
        {
            x01=x1 ; y01=y1;
            tmin=t1;
        }
        else
        {
            tmax=t1;
        }

        if(fabs(x1-x2)<0.1 && fabs(y1-y2)<0.1)
        {
            break;
        }

        x2=x1;
        y2=y1;
    } while(1);

    //----------根据 t 将线分割-----------
    struct
    {
        float x;
        float y;
    } p0,p1,p2,p3,p4,p5;

    p0.x=(float)(pt[1].x-pt[0].x)*t1+pt[0].x;
    p0.y=(float)(pt[1].y-pt[0].y)*t1+pt[0].y;

    p1.x=(float)(pt[2].x-pt[1].x)*t1+pt[1].x;
    p1.y=(float)(pt[2].y-pt[1].y)*t1+pt[1].y;

    p2.x=(float)(pt[3].x-pt[2].x)*t1+pt[2].x;
    p2.y=(float)(pt[3].y-pt[2].y)*t1+pt[2].y;

    p3.x=(float)(p1.x-p0.x)*t1+p0.x;
    p3.y=(float)(p1.y-p0.y)*t1+p0.y;

    p4.x=(float)(p2.x-p1.x)*t1+p1.x;
    p4.y=(float)(p2.y-p1.y)*t1+p1.y;

    p5.x=(float)(p4.x-p3.x)*t1+p3.x;
    p5.y=(float)(p4.y-p3.y)*t1+p3.y;

    pt[6].x=pt[3].x+0.5; pt[6].y=pt[3].y+0.5;
    pt[1].x=p0.x+0.5;    pt[1].y=p0.y+0.5;
    pt[2].x=p3.x+0.5;    pt[2].y=p3.y+0.5;
    pt[3].x=p5.x+0.5;    pt[3].y=p5.y+0.5;
    pt[4].x=p4.x+0.5;    pt[4].y=p4.y+0.5;
    pt[5].x=p2.x+0.5;    pt[5].y=p2.y+0.5;

    return;
}


//======== 求内定比分点,距p1内长度L2的点=======
void N_fd(float L2,int x1,int y1,int x2,int y2,int *x0,int *y0)
{
    double L1;

    L1 =sqrt(1.0*(x1-x2)*(x1-x2)+1.0*(y1-y2)*(y1-y2));
    if(L1==0)
    {
        *x0=x1;
        *y0=y1;
    }
    else
    {
        *x0=Round(x1+(x2-x1)*L2/L1);
        *y0=Round(y1+(y2-y1)*L2/L1);
    }
    return;
}

//======== 求外定比分点,距p1外长度L2的点 =======
void W_fd(float L2,int x1,int y1,int x2,int y2,int *x0,int *y0)
{
    float L1;
    L1 =sqrt(1.0*(x1-x2)*(x1-x2)+1.0*(y1-y2)*(y1-y2));
    if(L1==0)
    {
        *x0=x1;
        *y0=y1;
    }
    else
    {
        *x0=Round(x1+(x1-x2)*L2/L1);
        *y0=Round(y1+(y1-y2)*L2/L1);
    }

    return;
}


//========= 求三角形(地域线)某边对面顶点向外延伸的点 ==========
void BT_YC_Point(float Rate,int con,int p3x,int p3y,int p1x,int p1y,int p2x,int p2y,int *x1,int *y1)
{
    //p3-p1为边,p2对面点(待求控制点)，
    //con为是否控制两控制点的方向(分点比例),=0否,=1左控,=2右控
    //Rate 为三边总长度的分量值
    int x0,y0;
    float L1,L2,L3,L4;

    /// 求边p1-p2的定比分点
    L1=sqrt((double)(p2x-p3x)*(p2x-p3x) + (p2y-p3y)*(p2y-p3y));
    L2=sqrt((double)(p1x-p2x)*(p1x-p2x) + (p1y-p2y)*(p1y-p2y));
    L3=sqrt((double)(p1x-p3x)*(p1x-p3x) + (p1y-p3y)*(p1y-p3y));
    /// 外展长度
    L4=(L1+L2+L3)*Rate;

    /// 三点一线
    if(fabs(L1+L2-L3)<0.05)
    {
        if(L1>L2)
        {
            BT_3d(L4,p3x,p3y,p2x,p2y,x1,y1,x1,y1);
        }
        else
        {
            BT_3d(L4,p1x,p1y,p2x,p2y,x1,y1,x1,y1);
        }
        return;
    }

    if (con==1)
    {
        L1=L1*2.0;
    }

    if (con==2)
    {
        L2=L2*2.0;
    }

    /// (x0-p1x)/(p2x-x0)=L1/L2 定比分点 移中心点x1,y1
    x0=Round((L1*p1x+L2*p3x)/(L1+L2));
    /// (y0-p1y)/(p2y-y0)=L1/L2
    y0=Round((L1*p1y+L2*p3y)/(L1+L2));

    W_fd(L4,p2x,p2y,x0,y0,x1,y1);
}

//========= 求三角形(地域线)某边对面顶点的两个控制点 ==========
void BT_2_Point(float Rate,int con,int p3x,int p3y,int p1x,int p1y,int p2x,int p2y,int *x1,int *y1,int *x2,int *y2)
{
    //p3-p1为边,p2对面点(待求控制点)，
    //con为是否控制两控制点的方向(分点比例),=0否,=1左控,=2右控
    int x0,y0,x,y;
    float L1,L2;

    L1=sqrt((double)(p2x-p3x)*(p2x-p3x)+(p2y-p3y)*(p2y-p3y)); 	//求边p1-p2的定比分点
    L2=sqrt((double)(p1x-p2x)*(p1x-p2x)+(p1y-p2y)*(p1y-p2y));
    if (con==1)
    {
        L1=L1*2.0;
    }

    if (con==2)
    {
        L2=L2*2.0;
    }

    x0=Round((L1*p1x+L2*p3x)/(L1+L2)); //(x0-p1x)/(p2x-x0)=L1/L2 定比分点 移中心点x1,y1
    y0=Round((L1*p1y+L2*p3y)/(L1+L2)); //(y0-p1y)/(p2y-y0)=L1/L2

    //	L3=sqrt(1.0*(p1x-x0)*(p1x-x0)+1.0*(p1y-y0)*(p1y-y0));	//若三点一线则 L3=0
    //	if (L3<=1.0) {	}													//三点一线时

    BT_1_Point(Rate,p3x,p3y,x0,y0,p2x,p2y,&x,&y);

    *x1=x;
    *y1=y;

    BT_1_Point(Rate,p1x,p1y,x0,y0,p2x,p2y,&x,&y);

    *x2=x;
    *y2=y;

    return;
}

//========= 求三角形(地域线)某边一侧对面点的(一个)控制点 ==========
void BT_1_Point(float Rate,int x3,int y3,int x1,int y1,int x2,int y2,int *x30,int *y30)
{
    //已知：三点1,2,3, 求垂直与1-2长度为2-3 / Rate ,在2-3 一侧的点
    //已知：P3边点(求此点这边的P2的控制点),P1中点(已定比分点),P2对面点(待求控制点)
    //Rate为控制点线长与边长的比率
    float L1,L2;
    int p1x,p1y,p2x,p2y;

    L1=sqrt(1.0*(x2-x1)*(x2-x1)+1.0*(y2-y1)*(y2-y1));

    /// 三点一线时
    if (L1<=1.0)
    {
        *x30=x2-1.0*(x2-x3)/Rate;
        *y30=y2-1.0*(y2-y3)/Rate;
        return;
    }

    L2=sqrt(1.0*(x3-x2)*(x3-x2)+1.0*(y3-y2)*(y3-y2))/Rate;
    BT_3d(L2,x1,y1,x2,y2,&p2x,&p2y,&p1x,&p1y);

    L1=1.0*(x3-p1x)*(x3-p1x)+1.0*(y3-p1y)*(y3-p1y);
    L2=1.0*(x3-p2x)*(x3-p2x)+1.0*(y3-p2y)*(y3-p2y);

    if (L1>=L2)
    {
        *x30=p2x;	*y30=p2y;
    }
    else
    {
        *x30=p1x;	*y30=p1y;
    }

    return;
}

//=========== 求点在线的左右 ============
//已知p1-p2线,求点p3在线的:左侧=0;右侧=1;(p1-p2为方向)
int BT_lr(int x1,int y1,int x2,int y2,int x3,int y3)
{
    float L1,L2;
    int xx1,yy1,xx2,yy2;

    x2-=x1; y2-=y1;
    x3-=x1; y3-=y1;

    xx1= + y2;				//顺时针旋转,平移回x2
    yy1= - x2;
    xx2= - y2;				//逆时针旋转,平移回x2
    yy2= + x2;

    L1=1.0*(x3-xx1)*(x3-xx1)+1.0*(y3-yy1)*(y3-yy1);
    L2=1.0*(x3-xx2)*(x3-xx2)+1.0*(y3-yy2)*(y3-yy2);

    if(L1>L2)
    {
        return 1;
    }

    else   return 0;
}

//======== 已知两点线(p1-p2)，第三点到一端点(p2)的距离(L2)，求第三点(p2端两侧)======
void BT_3d(float L2,int x1,int y1,int x2,int y2,int *x3,int *y3,int *x4,int *y4)
{
    double L1;
    int x,y;

    L1=sqrt(1.0*(x1-x2)*(x1-x2)+1.0*(y1-y2)*(y1-y2));
    if (L1==0)
    {		  		//防同点错,此处不会发生
        y=0;
        x=L2;
    }
    else
    {
        x=Round(L2*(x1-x2)/L1);	//定比并平移x0
        y=Round(L2*(y1-y2)/L1);
    }
    *x3= -y + x2;				//逆时针旋转,平移回x2,(左边点)
    *y3= +x + y2;
    *x4= +y + x2;				//顺时针旋转,平移回x2,(右边点)
    *y4= -x + y2;
    return;
}

//================================================
//    已知三点d1,d2,d3,求d1-d3平行和垂直于d1-d2的长度
//    返回:l1:平行,l2垂直
//================================================
void Fl(POINT d1,POINT d2,POINT d3,double * l1,double * l2)
{
    double A,B,C,COS,SIN;

    A=(double)(d3.x-d2.x)*(d3.x-d2.x)+(double)(d3.y-d2.y)*(d3.y-d2.y);
    B=sqrt((double)(d1.x-d2.x)*(d1.x-d2.x)+(double)(d1.y-d2.y)*(d1.y-d2.y));
    C=sqrt((double)(d1.x-d3.x)*(d1.x-d3.x)+(double)(d1.y-d3.y)*(d1.y-d3.y));

    /// d1 d3 重叠
    if(C==0 || B==0)
    {
        *l1=*l2=0;
        return;
    }

    if(A==0)
    {
        *l1=B; *l2=0;
        return;
    }

    COS=(B*B+C*C-A)/2.0/B/C;  // cosA=(b*b+c*c-a*a)/(2*b*c)

    if(COS>=1)
    {
        COS=1;
        SIN=0;
    }
    else
    {
        SIN=sqrt(1-COS*COS);
    }

    *l1=C*COS;
    *l2=C*SIN;
    return;
}

//================================================
//    已知一线(d1-d2) 和一点d3,求对称于线d1-d2的点
//================================================
POINT Dcd(POINT d1,POINT d2,POINT d3)
{
    double k,b1,b2,xx,yy;
    POINT pt;

    // 求第一条线方程 y=kx+b1
    if(d2.x==d1.x)
    { // 垂线
        xx=d1.x;
        yy=d3.y;
    }
    else
    {

        k=(double)(d2.y-d1.y)/(d2.x-d1.x);
        if(k==0)
        {   //横线
            xx=d3.x;  yy=d1.y;
        }
        else
        {

            b1=d1.y-k*d1.x;
            // 求第与之垂直的线方程 y=-kx+b2
            b2=d3.y+d3.x/k;
            // 求交点
            xx=(b2-b1)/(k+1.0/k);
            yy=k*xx+b1;
        }
    }
    // 求出对称点
    pt.x=Round(2*xx-d3.x);
    pt.y=Round(2*yy-d3.y);

    return pt;
}

//======== 已知三角形，求过顶点(p2)且平行于底边(p1-p3)的平行线(p4-p5)的两端点=======
void BT_pxx(float Rate0, float Rate,int x1,int y1,int x2,int y2,int x3,int y3,int *x4,int *y4,int *x5,int *y5)
{
    //Rate为底边长与平行线长的比例(取3.0), p1-p3为边，p2为顶点
    //Rate0为修正系数 0<Rate0<10000   =0为不修正
    float L1,L2,L3,x,y,x0,y0,k,b1,b2,Rate1;

    x=x2+(x3-x1);				//平移矢量顶
    y=y2+(y3-y1);

    if(x1==x3)
    {
        x0=x1;
        y0=y;
    }
    else if(y1==y3)
    {
        x0=x;
        y0=y1;
    }
    else
    {
        k=1.0*(y3-y1)/(x3-x1);
        b1=y1-x1*k;
        b2=y+x/k;
        x0=(b2-b1)/(k+1.0/k);
        y0=k*x0+b1;
    }

    L3 =sqrt((double)(x2- x)*(x2- x) + (y2- y)*(y2- y));	//矢量长
    L1 =sqrt((double)(x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)); //左边长
    L2 =sqrt((double)(x2-x3)*(x2-x3) +  (y2-y3)*(y2-y3)); //右边长

    if(L1+L2)
    {
        Rate1=1.0+(L2-L1)*Rate0/(L2+L1);
    }
    else
    {
        Rate1=1;
    }

    x=x0+(x-x0)*Rate1;
    y=y0+(y-y0)*Rate1;
    if(L3==0)
    {
        L3=1;
    }

    *x4 =Round(x2+(x2-x)*L1/(Rate*L3));
    *y4 =Round(y2+(y2-y)*L1/(Rate*L3));
    *x5 =Round(x2+(x-x2)*L2/(Rate*L3));
    *y5 =Round(y2+(y-y2)*L2/(Rate*L3));
    return;
}

//===============================================================
//    (垂线交点）已知一线(d1-d2) 和一点d3,求过d3垂直于d1-d2的线在d1-d2的交点
//===============================================================
POINT Cxjd(POINT d1,POINT d2,POINT d3)
{
    double k,b1,b2;
    POINT pt;

    // 求第一条线方程 y=kx+b1
    if(d2.x==d1.x)
    { // 垂线
        pt.x=d1.x;
        pt.y=d3.y;
    }
    else
    {

        k=(double)(d2.y-d1.y)/(d2.x-d1.x);
        if(k==0)
        {   //横线
            pt.x=d3.x;
            pt.y=d1.y;
        }
        else
        {

            b1=d1.y-k*d1.x;
            // 求第与之垂直的线方程 y=-kx+b2
            b2=d3.y+d3.x/k;
            // 求交点
            b2 = (b2-b1)/(k+1.0/k);
            pt.x=Round(b2);
            pt.y=Round(k*b2+b1);
        }
    }

    return pt;
}

//======== (梯形)已知两点直线，一控制点，求另一对应控制点========
void BT_dyd(float Rate,int x1,int y1,int x2,int y2,int x3,int y3,int *x,int *y)
{
    //p1-p2为边，p3为p2点的控制点
    //Rate: 求出的控制点与直线的距离为另一控制点与直线距离的倍数 (0<=Date<=1.0)(1.0为不向下修正)
    float k,b1,b2,L1,L2,xx,yy,x0,y0;

    if (abs(y1-y2)==0)
    {
        xx=x1+x2-x3;
        yy=y3;
    }
    else if (abs(x1-x2)==0)
    {
        xx=x3;
        yy=y1+y2-y3;
    }
    else
    {
        k=1.0*(y1-y2)/(x1-x2);
        b1=(y2+y1)/2.0 + (x1+x2)/(2.0*k);
        b2=y3-k*x3;
        xx=(b1-b2)/(k+1.0/k);
        yy = k*xx + b2;
        xx = 2.0*xx-x3;
        yy = 2.0*yy-y3;
    }

    L1 =sqrt(1.0*(x1-x2)*(x1-x2)+1.0*(y1-y2)*(y1-y2));
    L2 =sqrt(1.0*(x1-xx)*(x1-xx)+1.0*(y1-yy)*(y1-yy));

    if (L1>0)
    {
        x0=x1+(x2-x1)*L2/L1;
        y0=y1+(y2-y1)*L2/L1;
    }
    else
    {
        x0=x1;
        y0=y1;
    }

    *x=Round(x0+(xx-x0)*Rate);
    *y=Round(y0+(yy-y0)*Rate);
    return;
}

//====== 求分角线，已知三点p1p2p3和距离L，求p2处沿平分线距离为L的点,左xl,右xr ======
void FenJiao1(double L,int x1,int y1,int x2,int y2,int x3,int y3,int *xl,int *yl,int *xr,int *yr)
{
    int x0,y0,x4,y4,x5,y5;
    double L3;        //拐角距离为L0(平行线到中心线距离为L)
    double JJ=500;

    //构造两点p0,p4,距p2点直线距离为JJ
    N_fd(JJ,x2,y2,x1,y1,&x0,&y0);	 			//求距P2点向内JJ处的点(由p2向p1方向)
    N_fd(JJ,x2,y2,x3,y3,&x4,&y4);	 			//求距P2点向内JJ处的点(由p2向p3方向)

    x5 = (x4+x0)/2.0;								//求p0-p4连线的中心点p5
    y5 = (y4+y0)/2.0;

    x0-=(x5-x2);    y0-=(y5-y2);        // p0-p2为过p2点的垂直于平分线的的线
    BT_3d(L,x0,y0,x2,y2,xl,yl,xr,yr);
    return;
}

//-------------------------------------------
//    插入直线点
//-------------------------------------------
int FindLineDot(POINT * pt,POINT *pt0,int wc)
{
    RECT r;
    double k,b1,b2, x0,y0;

    // 求给定点与直线的垂直交线
    // 求第一条线方程 y=kx+b1
    if(pt[1].x==pt[0].x)
    { // 垂线
        x0=pt[0].x;
        y0=pt0->y;
    }
    else
    {

        k=(double)(pt[1].y-pt[0].y)/(pt[1].x-pt[0].x);
        if(k==0)
        {   //横线
            x0=pt0->x;
            y0=pt[0].y;
        }else
        {

            b1=pt[0].y-k*pt[0].x;
            // 求第与之垂直的线方程 y=-kx+b2
            b2=pt0->y+pt0->x/k;
            // 求交点
            x0=(b2-b1)/(k+1.0/k);
            y0=k*x0+b1;
        }
    }

    r.left  = min(pt[0].x,pt[1].x);
    r.right = max(pt[0].x,pt[1].x);
    r.top   = min(pt[0].y,pt[1].y);
    r.bottom= max(pt[0].y,pt[1].y);
    if(x0<r.left || x0>r.right || y0<r.top || y0>r.bottom)
    {
        return -1;
    }

    if(fabs(pt0->x-x0)>wc || fabs(pt0->y-y0)>wc)
    {
        return -1;
    }

    pt0->x=Round(x0);
    pt0->y=Round(y0);
    return 1;
}
//-------------------------------------------
//    计算B曲线长度
//-------------------------------------------
float GetBezierLen(POINT * Bzpt,float t,int jd)
{
    return GetBezierLen1(Bzpt,0,t,jd);
}

double ft(double x1,double x2,double x3,double x4,double t)
{
    double t2,a1,a2,a3,a4;

    t2=t*t;
    a1=-3.0*t2+6.0*t-3.0;
    a2=9.0*t2-12.0*t+3.0;
    a3=-9.0*t2+6.0*t;
    a4=3*t2;
    return x1*a1+x2*a2+x3*a3+x4*a4;
}

//-------------------------------------------
//    计算分段B曲线长度
//-------------------------------------------
float GetBezierLen1(POINT * Bzpt,float t0,float t1,int jd)
{
    int i,j;
    double t2,t3,t4,ax,ay,a1,Len,Len1;
    double dt;
    double ck[]={751,3577,1323,2989,2989,1323,3577,751};


    if(t0<0)
    {
        t0=0;
    }

    if(t1>1)
    {
        t1=1;
    }

    /// 分段积分长
    t3=(t1-t0)/jd;

    /// 每段分7份
    dt=t3/7.0;
    for(Len1=0,t4=0,j=0; j<jd; j++,t4+=t3)
    {
        for(Len=0,t2=t4,i=0; i<8; t2+=dt,i++)
        {
            ax=ft(Bzpt->x,(Bzpt+1)->x,(Bzpt+2)->x,(Bzpt+3)->x,t2);
            ay=ft(Bzpt->y,(Bzpt+1)->y,(Bzpt+2)->y,(Bzpt+3)->y,t2);
            Len+=ck[i]*sqrt(ax*ax+ay*ay);
        }
        Len = t3*Len/17280.0;
        Len1+=Len;
    }
    return Len1;
}
//================================================
//    返回曲线长度或指定长度上的切线
//================================================
float Q_Bezier(POINT * pt, long length,float jd)
{
    //返回参数t1,是整线长度
    //如果返回t1<=0, 则数组放切线的两个点，第一个点在线上，第二个点在切线方向上
    int j;
    float t1,t2,t3,a1,a2,a3,a4,x1=0,y1=0,x2,y2;
    float len1=0,tmin,tmax,tt;

    if(length<=0)
    {
        t1=0;
        goto d1;
    }

    tmin=0; tmax=1;
    tt=0;
    len1=GetBezierLen1(pt,0,1,2);
    if(length>len1)
    {
        return len1;
    }
    else if(length==len1)
    {
        t1=1;
        goto d1;
    }

    do
    {
        t1=(tmax+tmin)/2;
        t3=t1/10.0;

        len1=GetBezierLen1(pt,0,t1,1);
        if(len1>length)
        {
            tmax=t1;
        }
        else
        {
            if(t1==1) return len1;
            tmin=t1;
        }
        //		if(fabs(tt-t1)<0.001) break;
        if(fabs(len1-length)<jd)
        {
            break;
        }
        tt=t1;
    }while(1);

d1:
    // 求点
    t2=t1*t1;
    t3=t2*t1;
    a1=-t3+3.0*(t2-t1)+1;
    a2=3.0*(t3-2.0*t2+t1);
    a3=3.0*(-t3+t2);  // a4=t3;
    x2=a1*pt->x+a2*(pt+1)->x+a3*(pt+2)->x+t3*(pt+3)->x;
    y2=a1*pt->y+a2*(pt+1)->y+a3*(pt+2)->y+t3*(pt+3)->y;
    // 求切线
    a1=-3.0*(t2-2*t1+1);
    a2=3-12*t1+9*t2;
    a3=6*t1-9*t2;
    a4=3*t2;
    pt[1].x=a1*pt->x+a2*(pt+1)->x+a3*(pt+2)->x+a4*(pt+3)->x + x2;
    pt[1].y=a1*pt->y+a2*(pt+1)->y+a3*(pt+2)->y+a4*(pt+3)->y + y2;
    pt[0].x=x2;
    pt[0].y=y2;
    return -t1;
}
//-------------------------------------------
//    B曲线上距离pt1最近点 wc:误差范围
//-------------------------------------------
float FindBezierDot(POINT * Bzpt,POINT * pt1,int wc)
{
    float tmin,tmax,t1,t2,t3,tt;
    float a1,a2,a3;
    float x1,y1;
    float l1,l2;
    float x01,y01;

    // 查找线上的一点，找到 t
    tmin=0;
    tmax=1;

    x01=Bzpt->x;
    y01=Bzpt->y;

    tt=0;
    do
    {
        t1=(tmax+tmin)/2;
        t2=t1*t1;
        t3=t2*t1;
        a1=-t3+3.0*(t2-t1)+1;
        a2=3.0*(t3-2.0*t2+t1);
        a3=3.0*(-t3+t2);  // a4=t3;
        x1=a1*Bzpt->x+a2*(Bzpt+1)->x+a3*(Bzpt+2)->x+t3*(Bzpt+3)->x;
        y1=a1*Bzpt->y+a2*(Bzpt+1)->y+a3*(Bzpt+2)->y+t3*(Bzpt+3)->y;

        if(fabs(x1-pt1->x)<1 && fabs(y1-pt1->y)<1)
        {
            break;
        }

        l1=(x01 - pt1->x)*(x01 - pt1->x)+(y01-pt1->y)*(y01-pt1->y);
        l2=(x1 - x01)*(x1 - x01)+(y1-y01)*(y1-y01);

        if(l1>l2)
        {
            x01=x1 ; y01=y1;
            tmin=t1;
        }
        else
        {
            tmax=t1;
        }
        if(fabs(tt-t1)<0.003)
        {
            break;
        }

        tt=t1;
    }while(1);

    if(fabs(x1-pt1->x)>wc || fabs(y1-pt1->y)>wc)
    {
        return -1.0;
    }

    pt1->x=x1;  pt1->y=y1;
    return t1;
}

//-----------------------------------------------
//    B曲线加点 （入4点 出7点） t1:0-1
//-----------------------------------------------
void AddDotOnBezier(POINT * pt,float t1)
{
    //----------根据t 将线分割-----------
    struct {
        float x;
        float y;
    }p0,p1,p2,p3,p4,p5;

    p0.x=(float)(pt[1].x-pt[0].x)*t1+pt[0].x;
    p0.y=(float)(pt[1].y-pt[0].y)*t1+pt[0].y;

    p1.x=(float)(pt[2].x-pt[1].x)*t1+pt[1].x;
    p1.y=(float)(pt[2].y-pt[1].y)*t1+pt[1].y;

    p2.x=(float)(pt[3].x-pt[2].x)*t1+pt[2].x;
    p2.y=(float)(pt[3].y-pt[2].y)*t1+pt[2].y;

    p3.x=(float)(p1.x-p0.x)*t1+p0.x;
    p3.y=(float)(p1.y-p0.y)*t1+p0.y;

    p4.x=(float)(p2.x-p1.x)*t1+p1.x;
    p4.y=(float)(p2.y-p1.y)*t1+p1.y;

    p5.x=(float)(p4.x-p3.x)*t1+p3.x;
    p5.y=(float)(p4.y-p3.y)*t1+p3.y;


    pt[6].x=Round(pt[3].x); pt[6].y=Round(pt[3].y);
    pt[1].x=Round(p0.x);    pt[1].y=Round(p0.y);
    pt[2].x=Round(p3.x);    pt[2].y=Round(p3.y);
    pt[3].x=Round(p5.x);    pt[3].y=Round(p5.y);
    pt[4].x=Round(p4.x);    pt[4].y=Round(p4.y);
    pt[5].x=Round(p2.x);    pt[5].y=Round(p2.y);
}


//==========================================
//------------计算线元边界-----------------
//==========================================
RECT CalcLineUnit(int * Data,int CloseFlag)
{
    POINT *xy0,* xy1,xy[4];
    int * d,*d0,DotNum,TotalDot;
    int CtrlDot;
    RECT r;

    TotalDot=BrowseLine(Data,CloseFlag);  // 共需端点数
    xy1=xy0=new POINT[TotalDot];
    d=(int *)Data;
    DotNum=*d++;          // 本曲线基本点数
    d0=d;
    for(;DotNum>0; DotNum--)
    {
        CtrlDot=*d & 0xFF;  // 本段控制点数
        if(*d & quxian)
        {    // 如果是曲线
            d++;
            xy[0].x=*d++; xy[0].y=*d++;
            if(CtrlDot==1)
            {
                xy[1].x=*d++;
                xy[1].y=*d++;
            }  // 一个控制点
            else
            {
                d++;
                d++ ;
                xy[1].x=*d++;
                xy[1].y=*d++;
            } // 两个控制点

            if(DotNum==1 && CloseFlag)
            { // 最后一点且封闭
                xy[3].x=d0[1]; xy[3].y=d0[2];
                xy[2].x=d0[3]; xy[2].y=d0[4];
            }
            else
            {
                xy[3].x=d[1]; xy[3].y=d[2];
                xy[2].x=d[3]; xy[2].y=d[4];
            }
            Bezier(xy,4,xy1);
            xy1+=bzjd;
        }
        else
        {				// 直线
            d++;
            xy1->x=*d++; xy1->y=*d++;  xy1++;
            if(CtrlDot==1)
            {
                d++;
                d++;
            }  // 一个控制点

            if(DotNum==1 && CloseFlag)
            { // 最后一点且封闭
                xy1->x=d0[1];
                xy1->y=d0[2];
            }
            else
            {
                xy1->x=d[1];
                xy1->y=d[2];
            }
        }
        if(DotNum==2 && CloseFlag==0) break;
    }

    xy1=xy0;
    SetRect(&r,xy1->x,xy1->y,xy1->x,xy1->y);
    xy1++;
    for(;TotalDot>1;TotalDot--,xy1++)
    {
        if(xy1->x > r.right)
        {
            r.right=xy1->x;
        }
        else if(xy1->x < r.left)
        {
            r.left=xy1->x;
        }
        if(xy1->y > r.bottom)
        {
            r.bottom=xy1->y;
        }
        else if(xy1->y < r.top)
        {
            r.top=xy1->y;
        }
    }
    delete xy0;
    return r;
}
//========================================
//-----------计算多点连线外接矩形（包含线宽）-----------------
//========================================
/// YTY add
LINE_ATTR DrawAttr;
RECT CalcPLineB(POINT * Data,int n)
{
    POINT *xy0=Data;
    RECT r;
    int w=DrawAttr.sLineWidth/2;

    if(w==0) w=1;

    SetRect(&r,xy0->x-w,xy0->y-w,xy0->x+w,xy0->y+w);
    xy0++;
    for(;n>1;n--,xy0++)
    {
        if(xy0->x > r.right)
        {
            r.right=xy0->x+w;
        }
        else if(xy0->x < r.left)
        {
            r.left=xy0->x-w;
        }

        if(xy0->y > r.bottom)
        {
            r.bottom=xy0->y+w;
        }
        else if(xy0->y < r.top)
        {
            r.top=xy0->y-w;
        }
    }
    return r;
}

//-----------------计算多点连线总长-----------------
double CalcPLineL(POINT * Data,int n)
{
    double L=0;
    int i;
    for(i=1; i<n; i++)
    {
        L+=sqrt((double)(Data[i].x-Data[i-1].x)*(Data[i].x-Data[i-1].x)+(double)(Data[i].y-Data[i-1].y)*(Data[i].y-Data[i-1].y));
    }

    return L;
}
//-----------------从端点找到长度为L的点-----------------
//    增加的点返回在n
//-------------------------------------------------------
int FindPLineD(POINT * Data,int * n,double Len)
{
    double L=0,last_L;
    int i,x0,y0;

    for(i=1; i<*n; i++)
    {
        last_L=L;
        L+=sqrt((double)(Data[i].x-Data[i-1].x)*(Data[i].x-Data[i-1].x)+(double)(Data[i].y-Data[i-1].y)*(Data[i].y-Data[i-1].y));
        if(fabs(L-Len)<1)
        {
            return i;
        }
        else if(L>Len)
        {
            memmove(Data+i+1,Data+i,sizeof(POINT)*(*n-i));
            N_fd(Len-last_L,Data[i-1].x,Data[i-1].y,Data[i].x,Data[i].y,&x0,&y0);
            Data[i].x=x0;
            Data[i].y=y0;
            *n=*n+1;
            return i;
        }
    }
    return *n-1;
}

//======== 已知两直线(d1-d2,d3-d4)（两点式方程），求两线交点 =======
POINT JiaoD(POINT d1,POINT d2,POINT d3,POINT d4)
{
    double k1,k2,b1,b2,x;
    POINT pt;

    /// 第一条线垂直的情况
    if(d2.x==d1.x)
    {
        pt.x=d2.x;
        if(d3.x==d4.x)
        {
            pt.y=d2.y;
            return pt;
        }
        else
        {
            k2=(double)(d4.y-d3.y)/(d4.x-d3.x);	//求第二条直线方程 y=k2*x+b2
            b2=d3.y-k2*d3.x;
            pt.y=Round(k2*pt.x+b2);
            return pt;
        }
    }
    if(d3.x==d4.x)
    {	//第二条线垂直的情况
        pt.x=d3.x;
        k1=(double)(d2.y-d1.y)/(d2.x-d1.x);		//求第一条直线方程 y=k1*x+b1
        b1=d1.y-k1*d1.x;
        pt.y=Round(k1*pt.x+b1);
        return pt;
    }

    k1=(double)(d2.y-d1.y)/(d2.x-d1.x);
    k2=(double)(d4.y-d3.y)/(d4.x-d3.x);
    if (k1==k2)
    {
        pt.x=d2.x;
        pt.y=d2.y;
        return pt;
    }
    b1=d1.y-k1*d1.x;		//求b1:  y=k1*x+b1
    b2=d3.y-k2*d3.x;		//求b2:  y=k2*x+b2
    x=(b2-b1)/(k1-k2);   //求交点
    pt.x=Round(x);
    pt.y=Round(k2*x+b2);
    return pt;
}
//===================================================
//
//     计算拟合曲线长度
//===================================================
float CalcCLineLen(FPOINT * s,int n)
{
    POINT * pp;
    float len;
    int i;

    pp = new POINT[n];
    for(i=0; i<n ; i++,s++)
    {
        pp[i].x=s->x*100;
        pp[i].y=s->y*100;
    }
    len=CalcCLineLen(pp,n);
    delete pp;
    return len;
}

float CalcCLineLen(POINT * s,int N)
{
    POINT * sr, *sl,s4[7];
    float LineLen;
    int i;

    sr=new POINT[N];
    sl=new POINT[N];

    if(N==2)
    {   // 两点，画直线
        sr[0].x=sl[1].x=(s[0].x+s[1].x)/2;
        sr[0].y=sl[1].y=(s[0].y+s[1].y)/2;
    }
    else
    {
        //----------求各点的两控制点---------------
        for (i=1; i<=N-2; i++)
        {
            //---------sl,sr为s[i-1]-s[i+1]的平行线，即为控制点-----------
            BT_pxx(8.0,3.0,s[i-1].x,s[i-1].y,s[i].x,s[i].y,s[i+1].x,s[i+1].y,(int *)(sl[i].x),(int *)(sl[i].y),(int *)(sr[i].x), (int *)(sr[i].y));
            if (i==1)
            {
                //---------当在起点时，求对应控制点----------
                BT_dyd(0.6,s[0].x,s[0].y,s[1].x,s[1].y,sl[1].x,sl[1].y,(int *)(sr[0].x), (int *)(sr[0].y));
            }
            if (i==N-2)
            {
                //---------当终点时，求对应控制点----------
                //   			BT_dyd(0.6,s[i].x,s[i].y,s[i-1].x,s[i-1].y,sr[i-1].x,sr[i-1].y,&(int)sl[i].x,&(int)sl[i].y);
                BT_dyd(0.6,s[i+1].x,s[i+1].y,s[i].x,s[i].y,sr[i].x,sr[i].y,(int*)(sl[i+1].x),(int*)(sl[i+1].y));
            }
        }
    }

    //----------累计曲线的总长度---------
    for(LineLen=0,i=0; i<N-1; i++)
    {
        s4[0].x=s[i].x;
        s4[0].y=s[i].y;

        s4[1].x=sr[i].x;
        s4[1].y=sr[i].y;

        s4[2].x=sl[i+1].x;
        s4[2].y=sl[i+1].y;

        s4[3].x=s[i+1].x;
        s4[3].y=s[i+1].y;

        LineLen+=GetBezierLen(s4,1,2);
    }

    delete sr;
    delete sl;

    return LineLen;
}

//===================================================
//     椭圆数据到B曲线
//
//===================================================
void EllipseDataToBLine(int * d0,int *d1)
{
    float bl=0.226;
    int kuan,gao;

    kuan=d0[2]-d0[0];   gao=d0[3]-d0[1];
    *d1++=4;

    *d1++=0x8102;
    *d1++=Round(d0[0]+kuan/2.0);
    *d1++=d0[1];
    *d1++=Round(d0[0]+kuan*bl);
    *d1++=d0[1];
    *d1++=Round(d0[2]-kuan*bl);
    *d1++=d0[1];

    *d1++=0x8102;
    *d1++=d0[2];
    *d1++=Round(d0[1]+gao/2.0);
    *d1++=d0[2];
    *d1++=Round(d0[1]+gao*bl);
    *d1++=d0[2];
    *d1++=Round(d0[3]-gao*bl);

    *d1++=0x8102;
    *d1++=Round(d0[0]+kuan/2.0);
    *d1++=d0[3];
    *d1++=Round(d0[2]-kuan*bl);
    *d1++=d0[3];
    *d1++=Round(d0[0]+kuan*bl);
    *d1++=d0[3];

    *d1++=0x8102;
    *d1++=d0[0];
    *d1++=Round(d0[1]+gao/2.0);
    *d1++=d0[0];
    *d1++=Round(d0[3]-gao*bl);
    *d1++=d0[0];

    *d1=Round(d0[1]+gao*bl);
}
//===================================================
//
//			判断点落在园那个象限
//  4 | 1
//  - + -
//  3 | 2
//===================================================
int PtInEllipse(int * d0,int x,int y)
{
    if(x>=(d0[0]+d0[2])/2)
    {  // 右边
        if(y<(d0[1]+d0[3])/2)
        {
            return 1;
        }
        else
        {
            return 2;
        }

    }
    else
    {
        if(y<(d0[1]+d0[3])/2)
        {
            return 4;
        }
        else
        {
            return 3;
        }
    }
}

#define EL(hdc,x,y)  Ellipse(hdc,x-3,y-3,x+4,y+4);
void PieDataToBLine(int * d0,int *d1,int flag)
{
    int Data0[40];
    struct D1
    {
        int DotF;
        int x,y;
        int cx1,cy1;
        int cx2,cy2;
    } *Data;
    int i,i1,i2,*P,N;
    POINT pt1[7],pt2[7];

    EllipseDataToBLine(d0,(int *)Data0);  // 先转化为椭圆
    Data=(struct D1 *)(Data0+1);
    i2=PtInEllipse(d0,d0[6],d0[7]);  // 弧线上取第二点
    switch(i2){
    case 1:
    case 2:
    case 3:
        pt2[0].x=Data[i2-1].x;  pt2[0].y=Data[i2-1].y;
        pt2[1].x=Data[i2-1].cx2;  pt2[1].y=Data[i2-1].cy2;
        pt2[2].x=Data[i2].cx1;  pt2[2].y=Data[i2].cy1;
        pt2[3].x=Data[i2].x;  pt2[3].y=Data[i2].y;
        break;
    case 4:
        pt2[0].x=Data[3].x;  pt2[0].y=Data[3].y;
        pt2[1].x=Data[3].cx2;  pt2[1].y=Data[3].cy2;
        pt2[2].x=Data[0].cx1;  pt2[2].y=Data[0].cy1;
        pt2[3].x=Data[0].x;  pt2[3].y=Data[0].y;
        break;
    }


    FindPtInCurve(pt2,d0[6],d0[7]);

    i1=PtInEllipse(d0,d0[4],d0[5]);  // 弧线上取第一点

    N=abs(i1-i2);  // 相差象限数
    P=d1+1;
    if(N==0){  // 同一象限且为劣弧,从1点到2点为顺时针
        switch(i1){
        case 1:
            if(d0[4]<d0[6] || d0[5]<d0[7]){
goback:
                pt1[0].x=pt2[0].x;  pt1[0].y=pt2[0].y;
                pt1[1].x=pt2[1].x;  pt1[1].y=pt2[1].y;
                pt1[2].x=pt2[2].x;  pt1[2].y=pt2[2].y;
                pt1[3].x=pt2[3].x;  pt1[3].y=pt2[3].y;
                FindPtInCurve(pt1,d0[4],d0[5]);    // 弧线上取第一点

                d1[0]=2;
                if(flag){  // 扇形
                    d1[0]+=2;
                    *P++=0;
                    *P++=(d0[0]+d0[2])/2; *P++=(d0[1]+d0[3])/2;
                }
                *P++=0x8101;
                *P++=pt1[3].x; *P++=pt1[3].y;
                *P++=pt1[4].x; *P++=pt1[4].y;
                *P++=0x8001;
                *P++=pt1[6].x; *P++=pt1[6].y;
                *P++=pt1[5].x; *P++=pt1[5].y;
                if(flag){
                    *P++=0;
                    *P++=(d0[0]+d0[2])/2; *P++=(d0[1]+d0[3])/2;
                }
                /*               POINT * pp;
               int i;
               pp=CLine(d1,&i,0);
               HDC hdc=GetDC(0);
               Draw_PLine(hdc,pp,i);
               delete pp;
               ReleaseDC(0,hdc);*/
                return;
            }
            break;
        case 2:
            if(d0[4]>d0[6] || d0[5]<d0[7]) goto goback;
            break;
        case 3:
            if(d0[4]>d0[6] || d0[5]>d0[7]) goto goback;
            break;
        case 4:
            if(d0[4]<d0[6] || d0[5]>d0[7]) goto goback;
            break;
        }
        pt1[0].x=pt2[3].x;  pt1[0].y=pt2[3].y;
        pt1[1].x=pt2[4].x;  pt1[1].y=pt2[4].y;
        pt1[2].x=pt2[5].x;  pt1[2].y=pt2[5].y;
        pt1[3].x=pt2[6].x;  pt1[3].y=pt2[6].y;
        FindPtInCurve(pt1,d0[4],d0[5]);           // 弧线上取第一点
    }
    else
    {   // 不同一象限,从1点到2点为顺时针
        switch(i1)
        {
        case 1:
        case 2:
        case 3:
            pt1[0].x=Data[i1-1].x;  pt1[0].y=Data[i1-1].y;
            pt1[1].x=Data[i1-1].cx2;  pt1[1].y=Data[i1-1].cy2;
            pt1[2].x=Data[i1].cx1;  pt1[2].y=Data[i1].cy1;
            pt1[3].x=Data[i1].x;  pt1[3].y=Data[i1].y;
            break;
        case 4:
            pt1[0].x=Data[3].x;  pt1[0].y=Data[3].y;
            pt1[1].x=Data[3].cx2;  pt1[1].y=Data[3].cy2;
            pt1[2].x=Data[0].cx1;  pt1[2].y=Data[0].cy1;
            pt1[3].x=Data[0].x;  pt1[3].y=Data[0].y;
            break;
        }
        FindPtInCurve(pt1,d0[4],d0[5]);
    }

    //   HDC hdc=GetDC(0);
    d1[0]=0;
    if(flag)
    {  // 扇形
        d1[0]+=2;
        *P++=0;
        *P++=(d0[0]+d0[2])/2; *P++=(d0[1]+d0[3])/2;
    }

    d1[0]+=3;
    *P++=0x8101;
    *P++=pt1[3].x; *P++=pt1[3].y;
    *P++=pt1[4].x; *P++=pt1[4].y;
    *P++=0x8102;
    *P++=pt1[6].x; *P++=pt1[6].y;
    *P++=pt1[5].x; *P++=pt1[5].y;

    if(i1>=i2) i2+=4;
    for(;i1<i2-1; i1++)
    {
        d1[0]++;
        i=i1;  if(i>=4) i-=4;
        *P++=Data[i].cx2;
        *P++=Data[i].cy2;

        *P++=0x8102;
        i=i1+1;  if(i>=4) i-=4;
        *P++=Data[i].x;  *P++=Data[i].y;
        *P++=Data[i].cx1;  *P++=Data[i].cy1;
    }

    *P++=pt2[1].x; *P++=pt2[1].y;
    *P++=0x8001;
    *P++=pt2[3].x; *P++=pt2[3].y;
    *P++=pt2[2].x; *P++=pt2[2].y;

    if(flag)
    {
        *P++=0;
        *P++=(d0[0]+d0[2])/2; *P++=(d0[1]+d0[3])/2;
    }

    return;

}
//==================================================
//    求园上的切点
// 园心x0,y0, 半径r, 外一点 x1,y1,
// D==0 失败, D>0成功
//==================================================
int GetCutDot(float x0,float y0,float r,float x1,float y1,float * x2,float * y2,float *x3, float *y3)
{
    float len,D,D2;

    len=GETLEN(x0,y0,x1,y1);

    // 求园上切点
    D=len*len-r*r;
    if(D>0) D=r/sqrt(D);
    else D=0;
    D2=D*D;
    *x2=(x0+D*y1-D*y0+D2*x1)/(1+D2);
    *y2=y0-D*x1+D*(*x2);

    *x3=(x0-D*y1+D*y0+D2*x1)/(1+D2);
    *y3=y0+D*x1-D*(*x3);
    return D;
}

//==================================================
//    旋转直线一端点 (逆时针)
// 园心x0,y0, 另一点 x1,y1, 角度jiao(弧度)
//==================================================
void RotateLine(float x0,float y0,float x1,float y1,float jiao,float * x2,float * y2)
{
    float COS,SIN;
    float x3,y3;

    x3=x1-x0; y3=y1-y0;
    COS=cos(jiao);  SIN=sin(jiao);
    *x2=x3*COS+y3*SIN+x0;
    *y2=-x3*SIN+y3*COS+y0;
}

//=============================================
//    根据两线返回角度
//  x1,y1, x2,y2:两线(度)
//=============================================
float CalcAngle(float x1,float y1,float x2,float y2)
{
    float wb1,jiao1,jiao2,jiao3;
    double a;

    wb1=GETLEN(x1,y1,x2,y2);
    if(wb1==0) wb1=1;
    a=(double)(-x1 + x2) / wb1;
    if(a>1) a=1; else if(a<-1) a=-1;
    jiao1 = asin(a);
    a=(double)(y1 - y2) / wb1;
    if(a>1) a=1; else if(a<-1) a=-1;
    jiao2 = acos(a);
    if(jiao1>=0) jiao3 = Round((jiao2 *180.0)/3.1415926);
    else if(jiao2<=3.1415926/2) jiao3 = Round(360+(jiao1 *180.0)/3.1415926);
    else jiao3 = Round(180-(jiao1 *180.0)/3.1415926);
    return jiao3;
}
//======================================================
//    判断折线是否顺时针顺序
//======================================================
bool IsClockwisePoly(FPOINT * fpt,int n)
{
    int jiao;

    jiao = SumPolyInterAngle(fpt,n);
    if(fabs(jiao-3.1415926*(n-2))<=3.1415926/5.0) return true;
    else return false;
}
//======================================================
//    计算多边形内角和(顺时针顺序) 弧度 3.1415926 * (n-2)
//======================================================
float SumPolyInterAngle(FPOINT * fpt,int n)
{
    float totalj=0,jiao;
    int i,zr;
    int x1,y1,x2,y2,x3,y3;

    if(n<=2) return 0;
    for(i=0; i<n; i++){
        if(i==0){
            x1=fpt[n-2].x;   y1=fpt[n-2].y;
            x2=fpt[n-1].x;   y2=fpt[n-1].y;
            x3=fpt[0].x;   y3=fpt[0].y;
        }else if(i==1){
            x1=fpt[n-1].x;   y1=fpt[n-1].y;
            x2=fpt[0].x;   y2=fpt[0].y;
            x3=fpt[1].x;   y3=fpt[1].y;
        }else{
            x1=fpt[i-2].x;   y1=fpt[i-2].y;
            x2=fpt[i-1].x;   y2=fpt[i-1].y;
            x3=fpt[i].x;   y3=fpt[i].y;
        }
        jiao = GetLineAngle(x1,y1,x2,y2,x3,y3);
        zr = BT_lr(x1,y1,x2,y2,x3,y3);
        if(zr==0) jiao = 3.1415926 * 2 - jiao; // 逆时针取对角
        totalj += jiao;
    }
    return totalj;
}
//======================================================
//    求夹角(1-2 与 2-3) 弧度
//======================================================
float GetLineAngle(int x1,int y1,int x2,int y2,int x3,int y3)
{
    float aa,bb,cc;
    float a,b,c,jiao;

    a = sqrt((float)(x1-x2)*(x1-x2)+(float)(y1-y2)*(y1-y2));
    b = sqrt((float)(x3-x2)*(x3-x2)+(float)(y3-y2)*(y3-y2));
    c = sqrt((float)(x3-x1)*(x3-x1)+(float)(y3-y1)*(y3-y1));
    if(a==0 || b==0 || c==0) return  0;

    aa = a*a; bb=b*b; cc=c*c;
    jiao = acos((aa+bb-cc)/2/a/b);
    return jiao;
}

//解3阶行列式
// sz[0][1]为0行1列
float HLS_3(float sz[3][3])
{
    float a1,a2,a3,a4,a5,a6;
    a1=sz[0][0]*sz[1][1]*sz[2][2];
    a2=sz[1][0]*sz[2][1]*sz[0][2];
    a3=sz[2][0]*sz[0][1]*sz[1][2];

    a4=sz[0][2]*sz[1][1]*sz[2][0];
    a5=sz[0][1]*sz[1][0]*sz[2][2];
    a6=sz[0][0]*sz[1][2]*sz[2][1];
    return a1+a2+a3-a4-a5-a6;

}

//解4阶行列式
// sz[0][1]为0行1列
float HLS_4(float sz[4][4])
{
    float a1,a2,a3,a4;
    float sz1[3][3];

    sz1[0][0]=sz[1][1]; sz1[0][1]=sz[1][2]; sz1[0][2]=sz[1][3];
    sz1[1][0]=sz[2][1]; sz1[1][1]=sz[2][2]; sz1[1][2]=sz[2][3];
    sz1[2][0]=sz[3][1]; sz1[2][1]=sz[3][2]; sz1[2][2]=sz[3][3];
    a1=HLS_3(sz1)*sz[0][0];

    sz1[0][0]=sz[1][0]; sz1[0][1]=sz[1][2]; sz1[0][2]=sz[1][3];
    sz1[1][0]=sz[2][0]; sz1[1][1]=sz[2][2]; sz1[1][2]=sz[2][3];
    sz1[2][0]=sz[3][0]; sz1[2][1]=sz[3][2]; sz1[2][2]=sz[3][3];
    a2=HLS_3(sz1)*sz[0][1];

    sz1[0][0]=sz[1][0]; sz1[0][1]=sz[1][1]; sz1[0][2]=sz[1][3];
    sz1[1][0]=sz[2][0]; sz1[1][1]=sz[2][1]; sz1[1][2]=sz[2][3];
    sz1[2][0]=sz[3][0]; sz1[2][1]=sz[3][1]; sz1[2][2]=sz[3][3];
    a3=HLS_3(sz1)*sz[0][2];

    sz1[0][0]=sz[1][0]; sz1[0][1]=sz[1][1]; sz1[0][2]=sz[1][2];
    sz1[1][0]=sz[2][0]; sz1[1][1]=sz[2][1]; sz1[1][2]=sz[2][2];
    sz1[2][0]=sz[3][0]; sz1[2][1]=sz[3][1]; sz1[2][2]=sz[3][2];
    a4=HLS_3(sz1)*sz[0][3];

    return a1-a2+a3-a4;
}


//==============================================
//    作者:都书波
//   计算通过两点a,b的直线与X轴的夹角(0--2*PAI)
//    返回值为弧度
//==============================================
float LD_JIAJIAO(FPOINT a,FPOINT b)
{
    //float angle,tmp;
    double pai=4*atan((double)1);
    double k;

    if(fabs(a.x-b.x)<2 && fabs(a.y-b.y)<2)
    {     return 0;  }
    else if(fabs(a.x-b.x)<2)
    {     if(a.y<b.y)  return (float)(pai/2.0);
        else         return (float)(pai*3.0/2);
    }
    else if(fabs(a.y-b.y)<2)
    {     if(a.x>b.x)  return (float)pai;
        else        return 0;
    }
    else
    {      k=(b.y-a.y)/(b.x-a.x);

        if(k>0)
        {    if(b.y>a.y) return (float)atan(k);
            else        return (float)(atan(k)+pai);
        }else
        {    if(b.y>a.y)  return (float)(pai+atan(k));
            else         return  (float)(atan(k)+2*pai);
        }
    }
}

//==================================
//       作者：都书波
//   判断一点是否在三角形的内部
//   1为是，-1为否
//==================================
int Dot_In_Triangle(FPOINT G,FPOINT A,FPOINT B,FPOINT C)
{
    double a,b,c,x,y,z;
    double cosa,cosb,cosc;
    double cosa1,cosa2,cosb1,cosb2,cosc1,cosc2;

    a=GETLEN(B.x,B.y,C.x,C.y);
    b=GETLEN(A.x,A.y,C.x,C.y);
    c=GETLEN(A.x,A.y,B.x,B.y);
    if(a<1) a=1;
    if(b<1) b=1;
    if(c<1) c=1;

    x=GETLEN(G.x,G.y,A.x,A.y);
    y=GETLEN(G.x,G.y,B.x,B.y);
    z=GETLEN(G.x,G.y,C.x,C.y);
    if(x<1) x=1;
    if(y<1) y=1;
    if(z<1) z=1;

    cosa=(b*b+c*c-a*a)/(2*b*c);
    cosa1=(c*c+x*x-y*y)/(2*c*x);
    if(cosa1<cosa)  return -1;

    cosa2=(b*b+x*x-z*z)/(2*b*x);
    if(cosa2<cosa)  return -1;

    cosb=(a*a+c*c-b*b)/(2*a*c);
    cosb1=(c*c+y*y-x*x)/(2*y*c);
    if(cosb1<cosb)  return -1;

    cosb2=(a*a+y*y-z*z)/(2*a*y);
    if(cosb2<cosb)  return -1;

    cosc=(a*a+b*b-c*c)/(2*a*b);
    cosc1=(a*a+z*z-y*y)/(2*a*z);
    if(cosc1<cosc)  return -1;

    cosc2=(b*b+z*z-x*x)/(2*b*z);
    if(cosc2<cosc)  return -1;

    return 1;
}

//===========================================
//   作者:都书波
//   已知三角形ABC,AB长为c,BC长为a,AC长为b,
//              G为BC边(线段内部)上的一点,且角GAB的大小为alpha;
//   求线段AG的长度
//===========================================
float Cal_AG_Length(FPOINT A,FPOINT B,FPOINT C,float alpha)
{
    double a,b,c,temp;
    double cosa,cosb,cosc,sinb,sinc,jiao_a,theta1,theta2;

    a=GETLEN(B.x,B.y,C.x,C.y);
    b=GETLEN(A.x,A.y,C.x,C.y);
    c=GETLEN(A.x,A.y,B.x,B.y);
    if(a<1) a=1;
    if(b<1) b=1;
    if(c<1) c=1;

    //   余弦定理
    cosa=(b*b+c*c-a*a)/(2*b*c);
    cosb=(a*a+c*c-b*b)/(2*a*c);
    cosc=(a*a+b*b-c*c)/(2*a*b);

    sinc=sqrt(1-cosc*cosc);
    sinb=sqrt(1-cosb*cosb);

    jiao_a=acos(cosa);
    theta1=alpha;
    theta2=jiao_a-alpha;
    //   对三角形GAC和三角形GAB分别利用正弦定理求出
    temp=sin(theta2)/sinc+sin(theta1)/sinb;
    return (float)a/(float)temp;
}


//==================================================
//    旋转多边形
//    园心x0,y0
//==================================================
void RotatePoly(POINT * pp,int N,float x0,float y0,float jiao)
{
    float COS,SIN;
    float x3,y3;

    COS=cos(jiao);  SIN=sin(jiao);
    for(;N>0; N--,pp++){
        x3=(pp->x-x0)*COS+(pp->y-y0)*SIN+x0;
        y3=-(pp->x-x0)*SIN+(pp->y-y0)*COS+y0;
        pp->x=x3; pp->y=y3;
    }
}

//========================================
//          获取多个点的外接矩形
//========================================
RECT GetRect(POINT * pt)
{
    RECT r;
    long ptx [] = {pt[0].x,pt[1].x,pt[2].x,pt[3].x};
    long pty [] ={pt[0].y,pt[1].y,pt[2].y,pt[3].y};
    r.left=_Min(4, ptx);
    r.top=_Min(4, pty);
    r.right=_Max(4, ptx);
    r.bottom=_Max(4, pty);
    return r;
}

//========================================
//    旋转、缩放和平移
//========================================
void TransformUnit(int * Array,int * TranE,int num,int x,int y,float Scale,float Sin,float Cos,int TranFlag)
{
    int i,x1,y1;

    for(i=0; (TranFlag & 4) && i<num; ){
        x1=Array[TranE[i]]; y1=Array[TranE[i+1]];
        Array[TranE[i++]]=Round(x1*Cos+y1*Sin);
        Array[TranE[i++]]=Round(x1*Sin-y1*Cos);
    }

    for(i=0; (TranFlag & 2) && i<num; i++){
        Array[TranE[i]]*=Scale;
    }

    for(i=0; (TranFlag & 1) && i<num; ){
        Array[TranE[i++]]+=x;
        Array[TranE[i++]]+=y;
    }

}
