#include <cmath>
#include "LineJB.h"

#define SearchXY  3
#define GETLEN(x1,y1,x2,y2) sqrt((double)((x1)-(x2))*((x1)-(x2))+(double)((y1)-(y2))*((y1)-(y2)))

double Yu=0;			//线段内的虚线段余数
static int x_end=0,y_end=0;
static int LineOut;
int Anti_Aliase=0;
int LWidth;
int LColor;

CLineJB::CLineJB()
{

}

//======================= 求两线交点 =======================
//
//  两线有交点返回true，(x, y)为交点坐标；没有交点返回false。
//
//========================================================
bool CLineJB::Line2Dot(double x11,double y11,double x12,double y12,double x21,double y21,double x22,double y22,double & x,double & y)
{
double dx1,dy1,dx2,dy2;
double c,c1,c2;

   dx1 = x12 - x11;
   dy1 = y12 - y11;
   dx2 = x22 - x21;
   dy2 = y22 - y21;
   c = -dy1 * dx2 + dy2 * dx1;
   if(c==0)
       return false; // 两线重合 或平行
   c1 = x11*y12 - x12*y11;
   c2 = x21*y22 - x22*y21;
   x = (-c1*dx2 + c2*dx1)/c;
   y = (c2*dy1 - c1*dy2)/c;
   return true;

}

//==================================================
//
// 求点在线的左右，已知p1-p2线(p1-p2为方向),求点p3在线的:左侧=0;右侧=1;
//
//==================================================
int CLineJB::BT_lr(double x1,double y1,double x2,double y2,double x3,double y3)
{
    double xx1,yy1,xx2,yy2,L1,L2;

    x2 -= x1; y2 -= y1;
    x3 -= x1; y3 -= y1;

    xx1 = + y2;				//顺时针旋转,平移回x2
    yy1 = - x2;
    xx2 = - y2;				//逆时针旋转,平移回x2
    yy2 = + x2;

    L1=1.0*(x3-xx1)*(x3-xx1)+1.0*(y3-yy1)*(y3-yy1);
    L2=1.0*(x3-xx2)*(x3-xx2)+1.0*(y3-yy2)*(y3-yy2);
    if(L1>L2)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//====================================================
//
//	   		子程序
//
//====================================================
//==== 已知两点线(p1-p2)，第三点到(p2)的距离(L2)，求第三点(p2端两侧或一侧画线)====
void CLineJB::BT_2d(double L2,double x1,double y1,double x2,double y2,int lr)
{                                                  	//lr=0从左往右,=1从右往左
    double L,x,y,xl,yl,xr,yr;

    L=sqrt(1.0*(x1-x2)*(x1-x2)+1.0*(y1-y2)*(y1-y2));
    if (L==0) {     //防同点错,此处不会发生
        x=0;
        y=0;
    } else {
        x=L2*(x1-x2)/L;     //定比并平移x0
        y=L2*(y1-y2)/L;
    }
    xl= -y + x2;        //逆时针旋转,平移回x2,(左边点)
    yl= +x + y2;
    xr= +y + x2;        //顺时针旋转,平移回x2,(右边点)
    yr= -x + y2;
}

///======== 已知方向(两点线p1-p2)，点到p2的距离L2，求第3,4点(p2端两侧)======
void CLineJB::BT_3d(double L2,double x1,double y1,double x2,double y2,double *xl,double *yl,double *xr,double *yr)
{
    double L,x,y;

    x = x1-x2;
    y = y1 - y2;

    /// 计算距离
    L=sqrt(x*x + y*y);

    /// 防同点错,此处不会发生
    if (L==0)
    {
        x=0;
        y=0;
    }
    else /// 定比并平移x0
    {
        x=L2*(x1-x2)/L;
        y=L2*(y1-y2)/L;
    }

    /// 逆时针旋转,平移回x2,(左边点)
    *xl= -y + x2;
    *yl= +x + y2;

    /// 顺时针旋转,平移回x2,(右边点)
    *xr= +y + x2;
    *yr= -x + y2;
}

/// ======== 求内定比分点,方向p1-p2,距p1长度L2的点(已知p1p2距离L)=======
void CLineJB::M_fd(double L2,double x1,double y1,double x2,double y2,double L,double *x0,double *y0)
{
    if(fabs(L)<=0.001)
    {
        *x0=x1;
        *y0=y1;
        return;
    }
    *x0=x1+(x2-x1)*L2/L;
    *y0=y1+(y2-y1)*L2/L;
    return;
}

/// ======== 求内定比分点,方向p1-p2,距p1内长度L2的点=======
void CLineJB::N_fd(double L2,double x1,double y1,double x2,double y2,double *x0,double *y0)
{
    double L,x,y;
    x = x1-x2;
    y = y1-y2;

    L =sqrt(x*x+y*y);

    M_fd(L2,x1,y1,x2,y2,L,x0,y0);
}

/// ======== 求外定比分点,方向p2-p1,距p1外延长度L2的点 =======
void CLineJB::W_fd(double L2,double x1,double y1,double x2,double y2,double *x0,double *y0)
{
    double L,dX,dY;
    dX = x1 - x2;
    dY = y1 - y2;

    L =sqrt(dX*dX+dY*dY);

    if(fabs(L)<=0.001)
    {
        *x0=x1; *y0=y1;
        return;
    }
    *x0=x1+(x1-x2)*L2/L;
    *y0=y1+(y1-y2)*L2/L;
    return;
}

/// ======= 画叉(十字) -- 顺p1,p2线的方向,在p2点处,垂直距离L,画叉(十字) =======
void CLineJB::Drw_X(double L2,double x1,double y1,double x2,double y2,int lr,int LineType)
{                                                   	//lr=0从左往右,=1从右往左
    double x3,y3,x4,y4;
    double xl_1,yl_1,xr_1,yr_1,xl_2,yl_2,xr_2,yr_2;


    if (x1==x2 && y1==y2) return;
    N_fd(L2,x2,y2,x1,y1,&x3,&y3);			//求距P2点向内L2处的点(由p2向p1方向)
    W_fd(L2,x2,y2,x1,y1,&x4,&y4);	 	    	//求距P2点向外L2处的点(由p1向p2方向延伸)

    if (LineType==38 ||LineType==39) //画叉
    {
        BT_3d(L2,x1,y1,x3,y3,&xl_1,&yl_1,&xr_1,&yr_1);//求P3处左右2点
        BT_3d(L2,x1,y1,x4,y4,&xr_2,&yr_2,&xl_2,&yl_2);//求P4处左右2点

        if (lr==0)
        {//从左至右画

        }
        else
        {//从右至左画

        }
    }
    else//LineType=40 画十字
    {
        BT_3d(L2,x1,y1,x2,y2,&xl_1,&yl_1,&xr_1,&yr_1);//求P2处左右2点

        //画直线

        if (lr==0)
        {//从左至右画
        }
        else
        {//从右至左画
        }
    }
    return;
}

//==============================
//
//          线型：3-13 虚线
//
//==============================
void CLineJB::Draw_XuXian1(int DL,int DK,POINT * D,int num)
{
    double Qx,Qy,Zx,Zy;			//每个虚线段的起点、实部终点
    double Xdx,Xdy;  				//每个虚线段的增量(含空),余数点的增量
    double Sdx,Sdy;				//每个虚线段内实部的增量
    double Dn,Xu_L,L1,L2,L3;
    double Yx,Yy;          		//计入余数后,画线的新起点
    double x,y;
    int x1,y1,x2,y2;				//画线线段两端点
    int i,j,N,Pnum=0;
    POINT pt[2000];


    Xu_L = DL+DK;
    x1 = D->x;           				//整个线的起始点
    y1 = D->y;
    D++;

    if (abs(x1-x_end)>SearchXY || abs(y1-y_end)>SearchXY)
        Yu=0;
    for (i=1; i<num; i++,D++)
    {
        x2 = x_end = D->x;     				//线段的下一拐点
        y2 = y_end = D->y;
        L1 = sqrt(1.0*(x1-x2)*(x1-x2)+1.0*(y1-y2)*(y1-y2));//线段长度

        //--------处理前余数, 新起点YxYy之前的画线---------
        if (Yu>0)
        {								//上笔(次)画线完保留Yu,重新计算L2
            L2 = Xu_L - Yu; 					//起点x1y1点到新起点的长度
            if (L1>=L2)
            {      				//线段L1够画完余数所剩虚线段
                M_fd(L2,x1,y1,x2,y2,L1,&Yx,&Yy);	//YxYy新起点
                if (L2>DK) //有剩余实部
                {
                    if (i==1)//首笔落笔,其它均不落笔
                    {
                        if(Pnum)
                        {
                            LineOut=0;

                            Pnum=0;
                            pt[Pnum].x = x1;  pt[Pnum++].y = y1;
                        }
                        Xdx = (L2-DK)*(Yx-x1)/L2;	//实线终点的增量(L2-DK)/dx=L2/(Yx-x1)
                        Xdy = (L2-DK)*(Yy-y1)/L2;
                        x = x1 + Round(Xdx);		//画第一笔实部
                        y = y1 + Round(Xdy);
                        pt[Pnum].x = Round(x);  pt[Pnum++].y = Round(y);
                    }
                    L1 -= L2;						//修正L1长度
                    x1 = Yx;	    		      	//修正新起点
                    y1 = Yy;
                }
                else
                {         					//线段L1不够画完余数所剩虚线段
                    L3 = 1.0*DL-Yu;				//L3为剩余实部的长度
                    if (L3>0) //L1线段内有实部
                    {
                        if (i==1)
                        {
                            if(Pnum)
                            {
                                LineOut=0;
                            }
                            Pnum=0;
                            pt[Pnum].x = x1;
                            pt[Pnum++].y = y1;
                        }
                        if (L3<L1)
                        {   	      //剩余实部全画
                            Xdx = L3*(x2-x1)/L1;	//实部终点的增量	L3/dx=L1/(x2-x1)
                            Xdy = L3*(y2-y1)/L1;
                            x = x1 + Round(Xdx);
                            y = y1 + Round(Xdy);
                            pt[Pnum].x = Round(x);  pt[Pnum++].y = Round(y);
                        }
                        else
                        {  	            //有剩余实部,但不够全画,或刚够
                            pt[Pnum].x = x2;
                            pt[Pnum++].y = y2;
                        }
                    }
                    Yu += L1;  						//修正线段余数
                    x1 = x2;             		//下个线段的起点
                    y1 = y2;
                    continue;						//返回到for,开始下一笔
                }

            }
            //------处理前余数结束-------

            Qx = x1;   		                	//新线段(也是画虚线的)起点
            Qy = y1;
            if (L1<Xu_L)
                N=0;
            else
            {
                Dn = L1/Xu_L;		            //线段内虚线段的个数
                Xdx = (x2-x1)/Dn;      			//每个虚线段的增量(含空)
                Xdy = (y2-y1)/Dn;
                Sdx = (Xdx*DL)/Xu_L;	     		//每个虚线段内实部终点的增量
                Sdy = (Xdy*DL)/Xu_L;
                N = Dn;
                for (j=0; j<N; j++) //画每个虚线段的实部
                {
                    if(Pnum)
                    {
                        LineOut=0;
                    }
                    Pnum=0;
                    pt[Pnum].x = Round(Qx);
                    pt[Pnum++].y = Round(Qy);
                    Zx = Qx+Sdx;            	//每个虚线段的实部终点
                    Zy = Qy+Sdy;
                    pt[Pnum].x = Round(Zx);
                    pt[Pnum++].y = Round(Zy);
                    Qx += Xdx;               	//下个虚线段的起点
                    Qy += Xdy;
                }
            }

            //--------处理后余数, x2y2之前的画线---------
            Yu = L1 - N*Xu_L;	  					//本大线段余数
            if (Yu>0) //最后一笔处理
            {
                if(Pnum)
                {
                    LineOut=0;
                }
                Pnum=0;
                pt[Pnum].x = Round(Qx);
                pt[Pnum++].y = Round(Qy);
                if (Yu > DL) //够画满实部
                {
                    Xdx = DL*(x2-Qx)/Yu;	  		//实部终点的增量 DL/dx=Yu/(x2-Qx)
                    Xdy = DL*(y2-Qy)/Yu;
                    Zx = Qx+Xdx;
                    Zy = Qy+Xdy;
                    pt[Pnum].x = Round(Zx);
                    pt[Pnum++].y = Round(Zy);
                }
                else
                {                    		//Yu<=DL 画不满实部
                    pt[Pnum].x = x2;
                    pt[Pnum++].y = y2;
                }
            }
            //------处理后余数结束-------
            x1 = x2;                			//下个线段的起点
            y1 = y2;
        }//for

        if(Pnum)
        {
        }
    }
}

//==============================
//
//	    	线型：14-16 点虚线
//
//==============================
void CLineJB::Draw_XuXian2(int DL,int DK,int DD,POINT * D,int num)
{
    double Qx,Qy,Zx,Zy;			//每个虚线段的起点、实部终点
    double Xdx,Xdy;  			//每个虚线段的增量(含空),(余数点的增量)
    double Sdx,Sdy;			//每个虚线段内实部的增量
    double Ddx,Ddy,Zdx,Zdy;		//每个虚线段内实点起、终点的增量
    double Dn,Xu_L,L1,L2,L3,L4,L5;
    double Yx,Yy;          		//计入余数后,画线的新起点
    double x,y;
    int x1,y1,x2,y2;			//画线线段两端点
    int i,j,N;
    POINT pt[2000];
    int Pnum=0;

    Xu_L = DL+DK+DD+DK;				//虚线段全长
    x1 = D->x;           				//整个线的起始点
    y1 = D->y;
    if (abs(x1-x_end)>SearchXY  || abs(y1-y_end)>SearchXY)
    {
        Yu=0;
    }
    D++;
    for (i=1; i<num; i++,D++)
    {
        x2 = x_end = D->x;     				//线段的下一拐点
        y2 = y_end = D->y;

        L1 = sqrt(1.0*(x1-x2)*(x1-x2)+1.0*(y1-y2)*(y1-y2)); //线段长度

        //--------处理前余数, 新起点YxYy之前的画线---------
        if (Yu>0) {						//上笔(次)画线完保留Yu,重新计算L2
            L2 = Xu_L - Yu;					//起点x1y1点到新起点的长度
            if (L1>=L2){      				//线段L1够画完余数所剩虚线段
                M_fd(L2,x1,y1,x2,y2,L1,&Yx,&Yy);	//YxYy新起点
                if (L2>(2*DK+DD)) { 	     	//有剩余实部
                    if (i==1){
                        if(Pnum) {
                            LineOut=0;
                        }
                        Pnum = 0;
                        pt[Pnum].x = x1;  pt[Pnum++].y = y1;
                    }
                    M_fd(L2-(2*DK+DD),x1,y1,Yx,Yy,L2,&x,&y); //实部终点
                    pt[Pnum].x = Round(x);
                    pt[Pnum++].y = Round(y);
                }
                if (L2>DK) {			//有实点
                    if (L2>=(DK+DD)) {		//够画全部实点
                        M_fd(L2-(DK+DD),x1,y1,Yx,Yy,L2,&x,&y); //实点起点
                        if(Pnum){
                            LineOut=0;
                        }
                        Pnum = 0;
                        pt[Pnum].x = x;
                        pt[Pnum++].y = y;
                    } else {                      //L2<(DK+DD),没有实部仅有部分实点
                        if (i==1){
                            if(Pnum){
                                LineOut=0;
                            }
                            Pnum = 0;
                            pt[Pnum].x = x1;
                            pt[Pnum++].y = y1;
                        }
                    }
                    M_fd(L2-DK,x1,y1,Yx,Yy,L2,&x,&y); //实点终点
                    pt[Pnum].x = Round(x);
                    pt[Pnum++].y = Round(y);
                }
                L1 -= L2;			//修正L1长度
                x1 = Yx;	    		//修正新起点
                y1 = Yy;

            } else {         			//线段L1不够画完余数所剩虚线段
                if (Yu<DL) {            //L1线段内有实部
                    if (i==1){
                        if(Pnum) {
                            LineOut=0;
                        }
                        Pnum = 0;
                        pt[Pnum].x = x1;
                        pt[Pnum++].y = y1;
                    }
                    L3 = 1.0*DL-Yu;					//L3>0为剩余实部的长度
                    if (L3<L1) {                  //剩余实部全画
                        M_fd(L3,x1,y1,x2,y2,L1,&x,&y); //实部终点
                        pt[Pnum].x = Round(x);
                        pt[Pnum++].y = Round(y);
                    } else{     	            		//有剩余实部,但不够全画,或刚够
                        pt[Pnum].x = Round(x2);
                        pt[Pnum++].y = Round(y2);
                    }
                }
                if (Yu<(DL+DK)){     				//L1线段内可能有完整实点
                    L4 = 1.0*(DL+DK)-Yu;				//L4>0为实点起点处的长度
                    if (L4<L1) {    	         	//L1线段内实点起点
                        M_fd(L4,x1,y1,x2,y2,L1,&x,&y);
                        if(Pnum) {
                            LineOut=0;
                        }
                        Pnum = 0;
                        pt[Pnum].x = x;
                        pt[Pnum++].y = y;

                        L5 = 1.0*(DL+DK+DD)-Yu;		//为实点终点处的长度
                        if (L5<L1) {    	         //L1线段内实点终点
                            M_fd(L5,x1,y1,x2,y2,L1,&x,&y); //实点终点
                            pt[Pnum].x = Round(x);
                            pt[Pnum++].y = Round(y);
                        } else {
                            pt[Pnum].x = x2;
                            pt[Pnum++].y = y2;
                        }
                    }
                }
                else if (Yu>=(DL+DK) && Yu<(DL+DK+DD)) {     //L1线段内有实点
                    if(Pnum) {
                        LineOut=0;
                    }
                    Pnum = 0;
                    pt[Pnum].x = x1;
                    pt[Pnum++].y = y1;
                    L4 = 1.0*(DL+DK+DD)-Yu;			//L4>0为实点终点处的长度
                    if (L4<L1) {            		//L1线段内有实点终点
                        M_fd(L4,x1,y1,x2,y2,L1,&x,&y); //实点终点
                        pt[Pnum].x = Round(x);
                        pt[Pnum++].y = Round(y);
                    } else {
                        pt[Pnum].x = x2;  pt[Pnum++].y = y2;
                    }
                }
                Yu += L1;  					//修正线段余数
                x1 = x2;             				//下个线段的起点
                y1 = y2;
                continue;					//返回到for,开始下一笔
            }
        }
        //------处理前余数结束-------

        Qx = x1;   		                			//新线段(也是画虚线的)起点
        Qy = y1;
        if (L1<Xu_L)
            N=0;
        else {
            Dn = L1/Xu_L;         					//线段内虚线段的个数
            Xdx = (x2-x1)/Dn;      					//每个虚线段的增量(含空)
            Xdy = (y2-y1)/Dn;
            Sdx = (Xdx*DL)/Xu_L; 					//每个虚线段内实部终点的增量
            Sdy = (Xdy*DL)/Xu_L;						//Sdx/Xdx=DL/Xu_L
            Ddx = (Xdx*(DL+DK))/Xu_L; 				//每个虚线段内实点起点的增量
            Ddy = (Xdy*(DL+DK))/Xu_L;				//Ddx/Xdx=(DL+DK)/(DL+2*DK+DD)
            Zdx = (Xdx*(DL+DK+DD))/Xu_L;			//每个虚线段内实点终点的增量
            Zdy = (Xdy*(DL+DK+DD))/Xu_L;			//Zdx/Xdx=(DL+DK+DD)/Xu_L
            N = Dn;
            for (j=0; j<N; j++) {					//画每个虚线段的实部
                if(Pnum) {
                    LineOut=0;
                }
                Pnum = 0;
                pt[Pnum].x = Round(Qx);
                pt[Pnum++].y = Round(Qy);
                Zx = Qx+Sdx;            			//每个虚线段的实部终点
                Zy = Qy+Sdy;
                pt[Pnum].x = Round(Zx);  pt[Pnum++].y = Round(Zy);

                Zx = Qx+Ddx;            			//每个虚线段的实点起点
                Zy = Qy+Ddy;
                if(Pnum) {
                    LineOut=0;
                }
                Pnum=0;
                pt[Pnum].x = Round(Zx);
                pt[Pnum++].y = Round(Zy);
                Zx = Qx+Zdx;            			//每个虚线段的实点终点
                Zy = Qy+Zdy;
                pt[Pnum].x = Round(Zx);  pt[Pnum++].y = Round(Zy);
                Qx += Xdx;               			//下个虚线段的起点
                Qy += Xdy;
            }
        }

        //--------处理后余数, x2y2之前的画线---------
        Yu = L1 - N*Xu_L;								//本大线段余数
        if (Yu>0) {                	   		//最后一笔处理
            if(Pnum) {
                LineOut=0;
            }
            Pnum=0;
            pt[Pnum].x = Round(Qx);
            pt[Pnum++].y = Round(Qy);
            if (Yu > DL) {        	  				//够画满实部
                M_fd(DL,Qx,Qy,x2,y2,Yu,&Zx,&Zy); //实部终点
                pt[Pnum].x = Round(Zx);
                pt[Pnum++].y = Round(Zy);
                if (Yu>(DL+DK)){           		//余数线段内有实点
                    M_fd(DL+DK,Qx,Qy,x2,y2,Yu,&x,&y);   //实点起点
                    if(Pnum) {
                        LineOut=0;
                    }
                    Pnum=0;
                    pt[Pnum].x = x;  pt[Pnum++].y = y;

                    if (Yu>(DL+DK+DD)) {    		//实点全画
                        M_fd(DL+DK+DD,Qx,Qy,x2,y2,Yu,&x,&y);   //实点终点
                        pt[Pnum].x = Round(x);
                        pt[Pnum++].y = Round(y);
                    } else{
                        pt[Pnum].x = x2;  pt[Pnum++].y = y2;
                    }
                }
            }
            else
            {                    				//Yu<=DL 画不满实部
                pt[Pnum].x = x2;  pt[Pnum++].y = y2;
            }
        }
        //------处理后余数结束-------
        x1 = x2;                					//下个线段的起点
        y1 = y2;
    }//for
    if(Pnum)
    {
    }
}

//==============================
//
//   		线型:26-28 铁路线;
//   		线型:38 X线; 48,49
//       线型90 不划实线，为双线铁路用
//==============================
void CLineJB::Draw_XuXian3(int DL,int DW, POINT * D,int num,int LineType)
{
    double Qx,Qy;     			//每个虚线段的起点
    double Xdx,Xdy;  			//每个虚线段的增量(含空),余数点的增量
    double Sdx,Sdy;				//每个虚线段内实部的增量
    double Dn,Xu_L,L1,L2,L3;
    double Yx,Yy;                           //计入余数后,画线的新起点
    double x,y;
    int x1,y1,x2,y2;			//画线线段两端点
    int i,j,N,DK;

    Xu_L = DL;

    if(LineType == 90){
        DL = DL/1.4;
        DK = Xu_L - DL;
    } else {
        if (LineType!=38) DL = Round(DL/4);
        DK = Xu_L - DL;
    }

    if(LineType==90){
        LineType=31;
    } else {
        if(D[0].x==D[num-1].x && D[0].y==D[num-1].y)      //先画实线
        {
        }
        else
        {
        }
    }

    x1 = D->x;           						//整个线的起始点
    y1 = D->y;
    D++;
    if (abs(x1-x_end)>SearchXY || abs(y1-y_end)>SearchXY) Yu=0;
    for (i=1; i<num; i++,D++){
        x2 = x_end = D->x;     				//线段的下一拐点
        y2 = y_end = D->y;
        L1 = sqrt(1.0*(x1-x2)*(x1-x2)+1.0*(y1-y2)*(y1-y2));//线段长度

        //--------处理前余数, 新起点YxYy之前的横线---------
        if (Yu>0) {								//上笔(次)画线完保留Yu
            L2 = Xu_L - Yu; 					//起点x1y1点到新起点的长度
            if (L1>=L2){      				//线段L1够余数所剩虚线段
                M_fd(L2,x1,y1,x2,y2,L1,&Yx,&Yy);	//YxYy新起点

                if (L2>DK) {            	//L2含横线
                    M_fd(L2-DK,x1,y1,Yx,Yy,L2,&x,&y); //横线点

                    switch(LineType) {
                    case 38:    Drw_X(DW,x2,y2,x,y,1,LineType);
                        break;
                    case 48:
                    case 50:    BT_2d(DW,x2,y2,x,y,11);
                        break;
                    case 49:
                    case 51:    BT_2d(DW,x2,y2,x,y,10);
                        break;
                    case 26:
                        BT_2d(DW,x2,y2,x,y,1);	//画第一笔横线
                        break;
                    case 31:    // 双线铁路
                        BT_2d(DW,x2,y2,x,y,1);	//画第一笔横线
                        M_fd(L2-DK*0.75,x1,y1,Yx,Yy,L2,&x,&y); //横线点
                        BT_2d(DW,x2,y2,x,y,1);	//画第二笔横线
                        break;
                    }
                }
                L1 -= L2;						//修正L1长度
                x1 = Yx;	    		      	//修正新起点
                y1 = Yy;
            } else {         					//线段L1不够余数所剩虚线段
                L3 = 1.0*DL-Yu;
                if (L3>0) {             	//L3内有横线
                    if (L3<L1) {   	      //L1线段内有横线
                        M_fd(L3,x1,y1,x2,y2,L1,&x,&y);

                        switch(LineType) {
                        case 38:	Drw_X(DW,x1,y1,x,y,0,LineType);
                            break;
                        case 48:
                        case 50:    BT_2d(DW,x1,y1,x,y,10);
                            break;
                        case 49:
                        case 51:    BT_2d(DW,x1,y1,x,y,11);
                            break;
                        case 26:	BT_2d(DW,x1,y1,x,y,0);	//画第一笔横线
                            break;
                        case 31:    // 双线铁路
                            BT_2d(DW,x1,y1,x,y,0);	//画第一笔横线
                            M_fd(L3+DK*0.25,x1,y1,x2,y2,L1,&x,&y); //横线点
                            BT_2d(DW,x1,y1,x,y,0);	//画第二笔横线
                            break;
                        }
                    }
                }
                Yu += L1;                       //修正线段余数
                x1 = x2;                        //下个线段的起点
                y1 = y2;
                continue;                       //返回到for,开始下一笔
            }
        }
        //------处理前余数结束-------

        Qx = x1;   		                	//新线段(也是虚线的)起点
        Qy = y1;
        if (L1<Xu_L) N=0;
        else {
            Dn = L1/Xu_L;		            //线段内虚线段的个数
            Xdx = (x2-x1)/Dn;      			//每个虚线段的增量(含空)
            Xdy = (y2-y1)/Dn;
            N = Dn;
            for (j=0; j<N; j++) {
                Sdx = DL*(x2-x1)/L1;			//横线处的增量	DL/dx=L1/(x2-x1)
                Sdy = DL*(y2-y1)/L1;
                x = Qx + Sdx;
                y = Qy + Sdy;
                switch(LineType) {
                case 38:	Drw_X(DW,x1,y1,x,y,0,LineType);
                    break;
                case 48:
                case 50: BT_2d(DW,x1,y1,x,y,10);
                    break;
                case 49:
                case 51: BT_2d(DW,x1,y1,x,y,11);
                    break;
                case 26:	BT_2d(DW,x1,y1,x,y,0);	//画横线
                    break;
                case 31:
                    BT_2d(DW,x1,y1,x,y,0);	//画横线
                    x = Qx + Sdx*1.8;
                    y = Qy + Sdy*1.8;
                    BT_2d(DW,x1,y1,x,y,0);	//画横线
                    break;
                }

                x += Xdx;               	//下个虚线段的起点
                Qy += Xdy;
            }
        }

        //--------处理后余数, x2y2之前的横线---------
        Yu = L1 - N*Xu_L;	  					//本大线段余数
        if (Yu>0) {                   	//最后一笔处理
            if (Yu >= DL) {        	  		//够画横线
                M_fd(DL,Qx,Qy,x2,y2,Yu,&x,&y);
                switch(LineType) {
                case 38:	Drw_X(DW,x1,y1,x,y,0,LineType);
                    break;
                case 48:
                case 50:    BT_2d(DW,x1,y1,x,y,10);
                    break;
                case 49:
                case 51:    BT_2d(DW,x1,y1,x,y,11);
                    break;
                case 26:	BT_2d(DW,x1,y1,x,y,0);	//画此笔最后一个横线
                    break;
                case 31:    // 双线铁路
                    BT_2d(DW,x1,y1,x,y,0);	//画第一笔横线
                    M_fd(DL+DK*0.25,Qx,Qy,x2,y2,Yu,&x,&y); //横线点
                    BT_2d(DW,x1,y1,x,y,0);	//画第二笔横线
                    break;
                }
            }
        }
        //------处理后余数结束-------
        x1 = x2;                			//下个线段的起点
        y1 = y2;
    }//for
}

//==============================
//
//   		线型:36-37 国界线
//
//==============================
void CLineJB::Draw_XuXian4(int DL,int DK,int DW, POINT * D,int num)
{
    double Qx,Qy,Zx,Zy;			//每个虚线段的起点、实部终点
    double Xdx,Xdy;  			//每个虚线段的增量(含空),(余数点的增量)
    double Sdx,Sdy;			//每个虚线段内实部的增量
    double Dn,Xu_L,L1,L2,L3;
    double Yx,Yy;          		//计入余数后,画线的新起点
    double x,y;
    int x1,y1,x2,y2;			//画线线段两端点
    int i,j,N;

    POINT pt[2000];
    int Pnum=0;


    Xu_L = DL+DK;
    x1 = D->x;           						//整个线的起始点
    y1 = D->y;
    if (abs(x1-x_end)>SearchXY || abs(y1-y_end)>SearchXY) Yu=0;
    D++;
    for (i=1; i<num; i++,D++) {
        x2 = x_end = D->x;     				//线段的下一拐点
        y2 = y_end = D->y;

        L1 = sqrt(1.0*(x1-x2)*(x1-x2)+1.0*(y1-y2)*(y1-y2));//线段长度

        //--------处理前余数, 新起点YxYy之前的画线---------
        if (Yu>0)
        {								//上笔(次)画线完保留Yu,重新计算L2
            L2 = Xu_L - Yu; 					//起点x1y1点到新起点的长度
            if (L1>=L2)
            {      				//线段L1够画完余数所剩虚线段
                M_fd(L2,x1,y1,x2,y2,L1,&Yx,&Yy);	//新起点
                if (L2>DK)
                {            	//有剩余实部
                    if (i==1)
                    {
                        if(Pnum)
                        {
                        }
                        Pnum=0;
                        pt[Pnum].x = x1;
                        pt[Pnum++].y = y1;
                    }

                    M_fd(L2-DK,x1,y1,Yx,Yy,L2,&x,&y);	//实线终点
                    pt[Pnum].x = Round(x);
                    pt[Pnum++].y = Round(y);
                }
                L1 -= L2;						//修正L1长度
                x1 = Yx;	    		      	//修正新起点
                y1 = Yy;
            }
            else
            {         					//线段L1不够画完余数所剩虚线段
                L3 = 1.0*DL-Yu;				//L3为剩余实部的长度
                if (L3>0)
                {             	//L1线段内有实部
                    if (i==1)
                    {
                        if(Pnum)
                        {
                        }
                        Pnum=0;
                        pt[Pnum].x = x1;
                        pt[Pnum++].y = y1;
                    }

                    if (L3<=L1)
                    {       		//剩余实部全画
                        M_fd(L3,x1,y1,x2,y2,L1,&x,&y);	//实线终点
                        pt[Pnum].x = Round(x);
                        pt[Pnum++].y = Round(y);
                    }
                    else
                    {                 	//有剩余实部,但不够全画
                        pt[Pnum].x = Round(x2);
                        pt[Pnum++].y = Round(y2);
                    }
                }
                Yu += L1;                           //修正线段余数
                x1 = x2;             		//下个线段的起点
                y1 = y2;
                continue;                           //返回到for,开始下一笔
            }
        }
        //------处理前余数结束-------

        Qx = x1;   		                	//新线段(也是画虚线的)起点
        Qy = y1;
        if (L1<Xu_L)
            N=0;
        else
        {
            Dn = L1/Xu_L;		            //线段内虚线段的个数
            Xdx = (x2-x1)/Dn;      			//每个虚线段的增量(含空)
            Xdy = (y2-y1)/Dn;
            Sdx = (Xdx*DL)/Xu_L;	     		//每个虚线段内实部终点的增量
            Sdy = (Xdy*DL)/Xu_L;
            N = Dn;
            for (j=0; j<N; j++)
            {			//画每个虚线段的实部
                if(Pnum)
                {
                    LineOut=0;
                }
                Pnum=0;
                pt[Pnum].x = Qx;
                pt[Pnum++].y = Qy;
                Zx = Qx+Sdx;            	//每个虚线段的实部终点
                Zy = Qy+Sdy;
                pt[Pnum].x = Round(Zx);
                pt[Pnum++].y = Round(Zy);

                Qx += Xdx;               	//下个虚线段的起点
                Qy += Xdy;
            }
        }

        //--------处理后余数, x2y2之前的画线---------
        Yu = L1 - N*Xu_L;	  					//本大线段余数
        if (Yu>0)
        {
            if(Pnum)
            {
                LineOut=0;
            }
            Pnum=0;
            pt[Pnum].x = Qx;
            pt[Pnum++].y = Qy;
            if (Yu >= DL)
            {        	  		//够画满实部
                M_fd(DL,Qx,Qy,x2,y2,Yu,&x,&y);	//实部终点
                pt[Pnum].x = Round(x);
                pt[Pnum++].y = Round(y);
            }
            else
            {                   		//Yu<DL 画不满实部
                pt[Pnum].x = Round(x2);  pt[Pnum++].y = Round(y2);
            }
        }
        //------处理后余数结束-------
        x1 = x2;                			//下个线段的起点
        y1 = y2;
    }
    if(Pnum)
    {
        LineOut=0;
    }
}

//==============================
//
//	    	线型：39,40
//
//==============================
void CLineJB::Draw_XuXian5(int DL,int DK,int DW,POINT * D,int num,int LineType)
{
    double Qx,Qy,Zx,Zy;			//每个虚线段的起点、实部终点
    double Xdx,Xdy;  				//每个虚线段的增量(含空),(余数点的增量)
    double Sdx,Sdy;				//每个虚线段内实部的增量
    double Zdx,Zdy;				//每个虚线段内实点的增量
    double Dn,Xu_L,L1,L2,L3,L4;
    double Yx,Yy;          		//计入余数后,画线的新起点
    double x,y;
    int x1,y1,x2,y2;				//画线线段两端点
    int i,j,N;

    POINT pt[2000];
    int Pnum=0;


    /// 虚线段全长
    Xu_L = DL+DK+1+DK;

    /// 整个线的起始点
    x1 = D->x;
    y1 = D->y;

    if (abs(x1-x_end)>SearchXY  || abs(y1-y_end)>SearchXY)
    {
        Yu=0;
    }

    /// 移动数据
    ++D;
    for (i=1; i<num; ++i,++D)
    {
        x2 = x_end = D->x;     				//线段的下一拐点
        y2 = y_end = D->y;

        L1 = sqrt(double((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)));//线段长度

        /// --------处理前余数, 新起点YxYy之前的画线---------
        if (Yu>0)
        {
            /// 上笔(次)画线完保留Yu,重新计算L2
            /// 起点x1y1点到新起点的长度
            L2 = Xu_L - Yu;

            ///  线段L1够画完余数所剩虚线段
            if (L1>=L2)
            {
                M_fd(L2,x1,y1,x2,y2,L1,&Yx,&Yy);	//YxYy新起点

                if (L2>(2*DK+1))
                { 	     	//有剩余实部
                    if (i==1)
                    {                 //首笔落笔,其它均不落笔
                        if(Pnum)
                        {
                        }
                        Pnum=0;
                        pt[Pnum].x = x1;
                        pt[Pnum++].y = y1;
                    }
                    M_fd(L2-(2*DK+1),x1,y1,Yx,Yy,L2,&x,&y); //实部终点
                    pt[Pnum].x = Round(x);
                    pt[Pnum++].y = Round(y); //画实部
                }

                if (L2>DK)
                {					//有实点
                    M_fd(L2-DK,x1,y1,x2,y2,L1,&x,&y); //实点点
                    Drw_X(DW,x2,y2,x,y,1,LineType);
                }
                L1 -= L2;                                           //修正L1长度
                x1 = Yx;                                            //修正新起点
                y1 = Yy;

            }
            else
            {    //线段L1不够画完余数所剩虚线段

                if (Yu<DL)
                {             			//L1线段内有实部
                    if (i==1)
                    {
                        if(Pnum)
                        {
                        }
                        Pnum=0;
                        pt[Pnum].x = x1;
                        pt[Pnum++].y = y1;
                    }
                    L3 = 1.0*DL-Yu;					//L3>0为剩余实部的长度
                    if (L3<L1)
                    {                  //剩余实部全画
                        M_fd(L3,x1,y1,x2,y2,L1,&x,&y); //实部终点
                        pt[Pnum].x = Round(x);
                        pt[Pnum++].y = Round(y);
                    } else
                    {    	            		//有剩余实部,但不够全画,或刚够
                        pt[Pnum].x = Round(x2);  pt[Pnum++].y = Round(y2);
                    }
                }

                if (Yu<(DL+DK+1))
                {     				//L1线段内可能有实点
                    L4 = 1.0*(DL+DK+1)-Yu;			//L4>0为x1点到实点处的距离(含X点)
                    if (L4==L1)     	         	//x2为实点
                        Drw_X(DW,x1,y1,x2,y2,0,LineType);
                    else if (L4<L1)
                    {    	         	//L1线段内有实点(含X2点,这样处理前余数就不能含X1点)
                        M_fd(L4,x1,y1,x2,y2,L1,&x,&y);
                        /*							L5 = sqrt(1.0*(x-x2)*(x-x2)+1.0*(y-y2)*(y-y2));
                                                            if (L5<2) Drw_X(hdc,DW,x1,y1,x,y,0,LineType); //离x2近
                                                                    else*/
                        Drw_X(DW,x2,y2,x,y,1,LineType);	//离x1近
                    }
                }

                Yu += L1;  					//修正线段余数
                x1 = x2;             				//下个线段的起点
                y1 = y2;
                continue;		//返回到for,开始下一笔
            }
        }
        /// ------处理前余数结束-------

        Qx = x1;   		                			//新线段(也是画虚线的)起点
        Qy = y1;
        if (L1<Xu_L)
        {
            N=0;
        }
        else
        {
            Dn = L1/Xu_L;         					//线段内虚线段的个数
            Xdx = (x2-x1)/Dn;      					//每个虚线段的增量(含空)
            Xdy = (y2-y1)/Dn;
            Sdx = (Xdx*DL)/Xu_L; 					//每个虚线段内实部终点的增量
            Sdy = (Xdy*DL)/Xu_L;						//Sdx/Xdx=DL/Xu_L

            Zdx = (Xdx*(DL+DK+1))/Xu_L;			//每个虚线段内X点的增量
            Zdy = (Xdy*(DL+DK+1))/Xu_L;			//Zdx/Xdx=(DL+DK+DD)/Xu_L
            N = Dn;
            for (j=0; j<N; j++)
            {					//画每个虚线段的实部
                if(Pnum)
                {
                }
                Pnum=0;
                pt[Pnum].x = Qx;
                pt[Pnum++].y = Qy;
                Zx = Qx+Sdx;            			//每个虚线段的实部终点
                Zy = Qy+Sdy;
                pt[Pnum].x = Round(Zx);
                pt[Pnum++].y = Round(Zy);

                Zx = Qx+Zdx;            			//每个虚线段的X点
                Zy = Qy+Zdy;
                Drw_X(DW,x1,y1,Zx,Zy,0,LineType);

                Qx += Xdx;               			//下个虚线段的起点
                Qy += Xdy;
            }
        }

        /// --------处理后余数, x2y2之前的画线---------
        Yu = L1 - N*Xu_L;                                       //本大线段余数
        if (Yu>0)
        {                	   		//最后一笔处理
            if(Pnum)
            {
            }
            Pnum=0;
            pt[Pnum].x = Qx;
            pt[Pnum++].y = Qy;
            //	         MoveToEx(hdc,Round(Qx),Round(Qy),NULL); //余数起点
            if (Yu > DL)
            {        	  				//够画满实部
                M_fd(DL,Qx,Qy,x2,y2,Yu,&Zx,&Zy); //实部终点
                pt[Pnum].x = Round(Zx);
                pt[Pnum++].y = Round(Zy);

                if (Yu==DL+DK+1)	              	//x2为实点
                    Drw_X(DW,x1,y1,x2,y2,0,LineType);
                else if (Yu>(DL+DK+1))
                {          		//余数段内有实点(含X2点,这样处理前余数就不能含X1点)
                    M_fd(DL+DK+1,Qx,Qy,x2,y2,Yu,&x,&y); //实点点
                    Drw_X(DW,x2,y2,x,y,1,LineType);	//离x1近
                }
            } else
            {                    				//Yu<=DL 画不满实部
                pt[Pnum].x = Round(x2);
                pt[Pnum++].y = Round(y2);
            }
        }
        //------处理后余数结束-------
        x1 = x2;                					//下个线段的起点
        y1 = y2;
    }//for

    if(Pnum)
    {
    }
}

//======================================================
//
//   		线型:63,64 双线;  75 铁路  76 复线铁路
//
//======================================================
void CLineJB::Draw_XuXian7(int Width,COLORREF Color, COLORREF Color1,int DW, POINT * D1,int num,int LineType)
{
    double xl,yl,xr,yr;
    double xl2,yl2,xr2,yr2;
    double jdx1,jdy1,jdx2,jdy2,len;
    int x1,y1,x2,y2,x3,y3;
    int i,num1=0,num2=0;
    POINT  *Gl,*Gr,*Gp1,*Gp2,*D=D1;
    POINT  *E;
    BOOL rv;
    int WidthR,WidthL,FlagZY=0,Width1;

    int JL,JK;										//为画虚线设
    Width1 = Width/2.5;
    if(Width1<=0) Width1=1;


    if(num<=1) return;
    while(D->x==(D+1)->x && D->y==(D+1)->y)
    {  // 消除起始重点
        D++; num--;
        if(num<=1) return;
    }

    if(Width==1){
        return;
    }

    if (LineType==75 || LineType==76)
    {
        Width1=Width;					//保留原设置
        Width=DW;						//铁路中心虚线的线宽、颜色

        E=D;
        JL=30*Width1/4;  if(JL<=30) JL=30;
        JK=30*Width1/4;  if(JK<=30) JK=30;
        LWidth=Width; LColor = Color;

        //=============先画实线(底)===============

        if(D[0].x==D[num-1].x && D[0].y==D[num-1].y)
        {
        }
        else
            //         g.DrawLines(&Pen2,(Gdiplus::Point *)D,num);//原zuo
            //=============先画虚线===============
            Draw_XuXian1(JL,JK,E,num);

        if(LineType==76)
        { // 复线铁路，画短线
            LWidth /=2;
            Draw_XuXian3(JL+JK,Width/2,E,num,90);
            LineType=75;
            LWidth = Width;
        }
        //=============画虚线结束===============
        Width=Width1;
    }

    if(Width %2)
    {
        WidthR=WidthL=Width /2;
    }
    else
    {
        WidthL=Width /2;
        WidthR=WidthL-1;
    }

    Gl = new POINT[num*6+10];
    Gr = new POINT[num*3+10];
    for (i=0; i<num; i++,D++)
    {
        if (i==0)
        {
            x1 = D->x;           					//整个线的起始点
            y1 = D->y;
            continue;
        }

        if(i==1)
        {
            x2 = D->x;           					//第二个线
            y2 = D->y;
            BT_3d(WidthL,x1,y1,x2,y2,&xl,&yl,&xr,&yr);//求第一点处垂直点
            Gl[0].x = Round(x1-x2+xl); Gl[0].y = Round(y1-y2+yl);

            if(WidthR==WidthL)
            {
                Gr[0].x = Round(x1-x2+xr); Gr[0].y = Round(y1-y2+yr);
            }
            else
            {
                BT_3d(WidthR,x1,y1,x2,y2,&xl2,&yl2,&xr,&yr);//求第一点处垂直点
                Gr[0].x = Round(x1-x2+xr); Gr[0].y = Round(y1-y2+yr);
            }
            num1++; num2++;
            xl2 = x1-x2+xl;  yl2 = y1-y2+yl;
            xr2 = x1-x2+xr;  yr2 = y1-y2+yr;
            if(num==2){
                jdx1=x1; jdy1=y1;
                break;
            }
            jdx1=x1; jdy1=y1;
            continue;
        }

        /// x3为当前点, x2,x1为前两点
        x3 = D->x;
        y3 = D->y;
        if(abs(x3-x2)<2 && abs(y3-y2)<2)
        {
            continue;
        }

        BT_3d(WidthR,x1,y1,x2,y2,&xl,&yl,&xr,&yr);//求第一点处垂直点
        if(WidthR!=WidthL)
        {
            BT_3d(WidthL,x1,y1,x2,y2,&xl,&yl,&jdx1,&jdy1);//求第一点处垂直点
        }
        BT_3d(WidthR,x3,y3,x2,y2,&xr2,&yr2,&xl2,&yl2);//第二线处垂直点
        if(WidthR!=WidthL)
        {
            BT_3d(WidthL,x3,y3,x2,y2,&jdx1,&jdy1,&xl2,&yl2);//求第一点处垂直点
        }

        FlagZY=BT_lr(x1,y1,x2,y2,x3,y3);
        if(FlagZY)
        {	  //p3在p1-p2的右侧
            rv=Line2Dot(x1-x2+xr,y1-y2+yr,xr,yr,x3-x2+xr2,y3-y2+yr2,xr2,yr2,jdx1,jdy1);
            if(rv==false) goto goloop;   // 三点一线
            if(GETLEN(x2,y2,jdx1,jdy1)>WidthL*10)
            {  // 内外侧交点太长,缩短
                N_fd(WidthL*10,x2,y2,jdx1,jdy1,&jdx1,&jdy1);
            }
            if(i==2){
                len = (jdx1-x2)*(jdx1-x2)+(jdy1-y2)*(jdy1-y2);
                // 前一点的长度太短,前点的右边去掉,只留交点
                if(len - WidthR*WidthR > (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)) num2--;
            }
            Gr[num2].x = Round(jdx1);
            Gr[num2].y = Round(jdy1);
            num2++;

            Line2Dot(x1-x2+xl,y1-y2+yl,xl,yl,x3-x2+xl2,y3-y2+yl2,xl2,yl2,jdx2,jdy2);
            Gl[num1].x = Round(xl); Gl[num1].y = Round(yl);
            num1++;
            if(GETLEN(x2,y2,jdx2,jdy2)>WidthL*5)
            {  // 外侧交点太长,缩短
                N_fd(WidthL*5,x2,y2,jdx2,jdy2,&jdx2,&jdy2);
            }
            Gl[num1].x = Round(jdx2); Gl[num1].y = Round(jdy2); num1++;
            Gl[num1].x = Round(xl2); Gl[num1].y = Round(yl2); num1++;

        }
        else
        {   //p3在p1-p2的左侧
            rv=Line2Dot(x1-x2+xl,y1-y2+yl,xl,yl,x3-x2+xl2,y3-y2+yl2,xl2,yl2,jdx1,jdy1);
            if(rv==false) goto goloop;   // 三点一线
            if(GETLEN(x2,y2,jdx1,jdy1)>WidthL*10)
            {  // 内外侧交点太长,缩短
                N_fd(WidthL*10,x2,y2,jdx1,jdy1,&jdx1,&jdy1);
            }
            if(i==2)
            {
                len = (jdx1-x2)*(jdx1-x2)+(jdy1-y2)*(jdy1-y2);
                // 前一点的长度太短,前点的右边去掉,只留交点
                if(len - WidthL*WidthL > (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2))
                {
                    num1--;
                }
            }
            Gl[num1].x = Round(jdx1); Gl[num1].y = Round(jdy1); num1++;

            Line2Dot(x1-x2+xr,y1-y2+yr,xr,yr,x3-x2+xr2,y3-y2+yr2,xr2,yr2,jdx2,jdy2);
            Gr[num2].x = Round(xr); Gr[num2].y = Round(yr); num2++;
            if(GETLEN(x2,y2,jdx2,jdy2)>WidthR*5){  // 外侧交点太长,缩短
                N_fd(WidthL*5,x2,y2,jdx2,jdy2,&jdx2,&jdy2);
            }
            Gr[num2].x = Round(jdx2); Gr[num2].y = Round(jdy2); num2++;
            Gr[num2].x = Round(xr2); Gr[num2].y = Round(yr2); num2++;
        }
goloop:
        x1 = x2;
        y1 = y2;
        x2 = x3;
        y2 = y3;
    }

    len = (jdx1-x1)*(jdx1-x1)+(jdy1-y1)*(jdy1-y1);
    if(num1<2 || FlagZY==1 || len - WidthL*WidthL <= (x2-x1+xl2-x1)*(x2-x1+xl2-x1)+(y2-y1+yl2-y1)*(y2-y1+yl2-y1)){  // 防止最后一点折返
        Gl[num1].x = Round(x2-x1+xl2); Gl[num1].y = Round(y2-y1+yl2); num1++;
    }

    if(num2<2 || FlagZY==0 || len - WidthR*WidthR <= (x2-x1+xr2-x1)*(x2-x1+xr2-x1)+(y2-y1+yr2-y1)*(y2-y1+yr2-y1)){  // 防止最后一点折返
        Gr[num2].x = Round(x2-x1+xr2); Gr[num2].y = Round(y2-y1+yr2); num2++;
    }

    if(LineType==65 || LineType==66){
        Gp1 = Gl+num1; Gp2= Gr+num2-1;
        for(i=0; i<num2; i++,Gp1++,Gp2--){
            Gp1->x=Gp2->x;
            Gp1->y=Gp2->y;
        }
        Gp1->x=Gl->x;  Gp1->y=Gl->y;
    }

    if(Gl[0].x==Gl[num1-1].x && Gl[0].y==Gl[num1-1].y)
    {
    }
    else
    {
    }

    if(Gr[0].x==Gr[num2-1].x && Gr[0].y==Gr[num2-1].y)
    {
    }
    else
    {
    }

    if (LineType==64 || LineType==66){	//先画整个线
        if(D1[0].x==D1[num-1].x && D1[0].y==D1[num-1].y)
        {
        }
        else
        {
        }

    }

    delete Gl;
    delete Gr;

    return;
}

//==============================
//
//			线型：101 虚线
//
//==============================
void CLineJB::Draw_XuXian101(int Width,POINT * D,int num)
{
    double Qx,Qy,Zx,Zy;			//每个虚线段的起点、实部终点
    double Xdx,Xdy;                     //每个虚线段的增量(含空),余数点的增量
    double Sdx,Sdy;                     //每个虚线段内实部的增量
    double Xu_L,L1,L2,L3;
    double Yx,Yy;          		//计入余数后,画线的新起点
    double x,y;
    double x1,y1,x2,y2;                 //画线线段两端点
    int Dn,i,j,N,Pnum=0;
    int DL, DK;
    //    Gdiplus::PointF pt[2000];
    FPOINT pt[2000];
    double qdx,qdy,dx1,dy1,dx2,dy2;


    if (Width>=1) {
        DL = (int)(12*Width*0.6);
        DK = (int)(5*Width*0.6);
    }


    Xu_L = DL+DK;
    x1 = D->x;           				//整个线的起始点
    y1 = D->y;
    D++;
    if (abs(Round(x1)-x_end)>SearchXY || abs(Round(y1)-y_end)>SearchXY) Yu=0;
    for (i=1; i<num; i++,D++){
        x2 = x_end = D->x;     				//线段的下一拐点
        y2 = y_end = D->y;

        L1 = sqrt(1.0*(x1-x2)*(x1-x2)+1.0*(y1-y2)*(y1-y2));//线段长度

        //--------处理前余数, 新起点YxYy之前的画线---------
        if (Yu>0) {                                         //上笔(次)画线完保留Yu,重新计算L2
            L2 = Xu_L - Yu;                                 //起点x1y1点到新起点的长度
            if (L1>=L2){                                    //线段L1够画完余数所剩虚线段
                M_fd(L2,x1,y1,x2,y2,L1,&Yx,&Yy);	//YxYy新起点
                if (L2>DK) {                            //有剩余实部
                    if (i==1) {
                        if(Pnum) {
                            LineOut=0;
                            Pnum=0;
                            pt[Pnum].x = x1;  pt[Pnum++].y = y1;
                        }
                        Xdx = (L2-DK)*(Yx-x1)/L2;	//实线终点的增量(L2-DK)/dx=L2/(Yx-x1)
                        Xdy = (L2-DK)*(Yy-y1)/L2;
                        x = x1 + Xdx;		//画第一笔实部
                        y = y1 + Xdy;
                        pt[Pnum].x = x;
                        pt[Pnum++].y = y;

                        BT_3d(6.0,qdx,qdy,(qdx+x)/2,(qdy+y)/2,&dx1,&dy1,&dx2,&dy2);
                    }
                    L1 -= L2;					//修正L1长度
                    x1 = Yx;	    		      	//修正新起点
                    y1 = Yy;
                } else {         				//线段L1不够画完余数所剩虚线段
                    L3 = 1.0*DL-Yu;				//L3为剩余实部的长度
                    if (L3>0) {                             //L1线段内有实部
                        if (i==1){//首笔落笔,其它均不落笔
                            if(Pnum) {
                                LineOut=0;
                            }
                            Pnum=0;
                            pt[Pnum].x = x1;
                            pt[Pnum++].y = y1;
                        }
                        if (L3<L1) {   	      //剩余实部全画
                            Xdx = L3*(x2-x1)/L1;	//实部终点的增量	L3/dx=L1/(x2-x1)
                            Xdy = L3*(y2-y1)/L1;
                            x = x1 + Xdx;
                            y = y1 + Xdy;
                            pt[Pnum].x = x;
                            pt[Pnum++].y = y;

                            BT_3d(6.0,qdx,qdy,(qdx+x)/2,(qdy+y)/2,&dx1,&dy1,&dx2,&dy2);
                        } else {     	            //有剩余实部,但不够全画,或刚够
                            pt[Pnum].x = x2;  pt[Pnum++].y = y2;
                        }
                    }
                    Yu += L1;                       //修正线段余数
                    x1 = x2;             		//下个线段的起点
                    y1 = y2;
                    continue;                       //返回到for,开始下一笔
                }
            }
            //------处理前余数结束-------

            Qx = x1;   		                	//新线段(也是画虚线的)起点
            Qy = y1;
            if (L1<Xu_L) N=0;
            else {
                //            Dn = L1/Xu_L;                               //线段内虚线段的个数
                Dn = (int)(L1/Xu_L);                               //线段内虚线段的个数
                Xdx = (x2-x1)/Dn;      			//每个虚线段的增量(含空)
                Xdy = (y2-y1)/Dn;
                Sdx = (Xdx*DL)/Xu_L;	     		//每个虚线段内实部终点的增量
                Sdy = (Xdy*DL)/Xu_L;
                N = Dn;
                for (j=0; j<N; j++) {			//画每个虚线段的实部
                    if(Pnum) {
                        LineOut=0;
                    }
                    Pnum=0;
                    pt[Pnum].x = Qx;
                    pt[Pnum++].y = Qy;
                    qdx=Qx; qdy=Qy;

                    Zx = Qx+Sdx;            	//每个虚线段的实部终点
                    Zy = Qy+Sdy;
                    pt[Pnum].x = Zx;
                    pt[Pnum++].y = Zy;
                    Qx += Xdx;               	//下个虚线段的起点
                    Qy += Xdy;

                    BT_3d(6.0,qdx,qdy,(qdx+Zx)/2,(qdy+Zy)/2,&dx1,&dy1,&dx2,&dy2);
                }
            }

            //--------处理后余数, x2y2之前的画线---------
            Yu = L1 - N*Xu_L;	  					//本大线段余数
            if (Yu>0) {                   	//最后一笔处理
                if(Pnum) {
                    LineOut=0;
                }
                Pnum=0;
                pt[Pnum].x = Qx;  pt[Pnum++].y = Qy;
                qdx=Qx; qdy=Qy;

                if (Yu > DL) {        	  		//够画满实部
                    Xdx = DL*(x2-Qx)/Yu;	  		//实部终点的增量 DL/dx=Yu/(x2-Qx)
                    Xdy = DL*(y2-Qy)/Yu;
                    Zx = Qx+Xdx;
                    Zy = Qy+Xdy;
                    pt[Pnum].x = Zx;  pt[Pnum++].y = Zy;

                    BT_3d(6.0,qdx,qdy,(qdx+Zx)/2,(qdy+Zy)/2,&dx1,&dy1,&dx2,&dy2);
                } else {                    		//Yu<=DL 画不满实部
                    pt[Pnum].x = x2;
                    pt[Pnum++].y = y2;
                }
            }
            //------处理后余数结束-------
            x1 = x2;                			//下个线段的起点
            y1 = y2;

        }//for
        if(Pnum){
        }
    }
}

//==========================================
//    GDI 改造 虚线
//==========================================
void CLineJB::Draw_XuXian8(float JL,float JK,POINT * outd, int num)
{
    //    QVector<qreal> vec;
    //    vec<<JL<<JK;
    //    QPen pen;
    //    pen.setStyle(Qt::DashLine);
    //    pen.setDashPattern(vec);
    //    pen.setColor(MakeColor(LColor));
    //    pen.setWidth(LWidth);
    //   pen.setDashPattern(vec);
    //    painter->setPen(pen);
    //    QPoint * qpt = new QPoint[num];
    //    for(int i=0; i<num; i++) {
    //        qpt[i].setX(outd[i].x);
    //        qpt[i].setY(outd[i].y);
    //    }
    //    painter->drawLines(qpt, num);
    //    delete [] qpt;

}

//==========================================
//    GDI 改造 点划线
//==========================================
void CLineJB::Draw_XuXian9(int LType,POINT * outd,int num)
{
    //QPen pen;
    //pen.setColor(MakeColor(LColor));
    //pen.setWidth(LWidth);
    //QVector<qreal> dash;

    //QVector<qreal> dashVals1;
    //dashVals1 << 11 << 5 << 1 << 5;
    //QVector<qreal> dashVals2;
    //dashVals2 << 18 << 3 << 1 << 3;
    //QVector<qreal> dashVals3;
    //dashVals3 << 18 << 3 << 4 << 3;
    //QVector<qreal> dashVals4;
    //dashVals4 << 32 << 12 << 6 << 12;
    //QVector<qreal> dashVals5;
    //dashVals5 << 32 << 6 << 4 << 6 << 4 << 6;
    //QVector<qreal> dashVals6;
    //dashVals6 << 32 << 6 << 4 << 6 << 4 << 6;
    //QVector<qreal> dashVals7;
    //dashVals7 << 11 << 5 << 1 << 5 << 1 << 5;
    //QVector<qreal> dashVals8;
    //dashVals8 << 20 << 4 << 1 << 4 << 1 << 4;
    //QVector<qreal> dashVals9;
    //dashVals9 << 20 << 4 << 1 << 4 << 1 << 4 << 1 << 4;
    //QVector<qreal> dashVals10;
    //dashVals10 << 6 << 2 << 1 << 2;
    //QVector<qreal> dashVals11;
    //dashVals11 << 6 << 2 << 1 << 2 << 1 << 2 ;
    //QVector<qreal> dashVals12;
    //dashVals12 << 10 << 2 << 1 << 2 << 4 << 2 << 1 << 2;

    //   switch(LType){
    //      case 14: dash = dashVals1; break;
    //      case 15: dash = dashVals2; break;
    //      case 16: dash = dashVals3; break;
    //      case 17: dash = dashVals4; break;
    //      case 18: dash = dashVals5; break;
    //      case 19: dash = dashVals6; break;
    //      case 20: dash = dashVals7; break;
    //      case 21: dash = dashVals8; break;
    //      case 22: dash = dashVals9; break;
    //      case 23: dash = dashVals10; break;
    //      case 24: dash = dashVals11; break;
    //      case 25: dash = dashVals12; break;
    //      default:
    //            dash = dashVals1; break;
    //   }
    //   pen.setDashPattern(dash);

    //    painter->setPen(pen);
    //    QPoint * qpt = new QPoint[num];
    //    for(int i=0; i<num; i++) {
    //        qpt[i].setX(outd[i].x);
    //        qpt[i].setY(outd[i].y);
    //    }
    //    painter->drawLines(qpt, num);
    //    delete [] qpt;

}
//==========================================
//    GDI 改造 线铁路（线条）
//==========================================
void CLineJB::Draw_XuXian10(int LType,POINT * outd,int num)
{
    int lw[4]={9,15,23};
    ////    Gdiplus::Pen pen(MakeColor(LColor),LWidth);
    //    QPen pen;
    //    pen.setColor(MakeColor(LColor));
    //    pen.setWidth(LWidth);
    ////    Gdiplus::Pen pen1(MakeColor(LColor),5.0*LWidth);
    //    QPen pen1;
    //    pen.setColor(MakeColor(LColor));
    //    pen.setWidth(5*LWidth);
    //    int i;
    //    float offset;
    ////    Gdiplus::REAL dashVals[4];
    //    QVector<qreal> dashVals;
    //    double len=0;

    //    for(i=0; i<num-1; i++){
    //        len += GETLEN(outd[i].x,outd[i].y,outd[i+1].x,outd[i+1].y);
    //    }

    //    if(LType==31){
    //        i = 2;
    ////        dashVals[0] = 1/10.0;
    ////        dashVals[1] = 3/10.0;
    ////        dashVals[2] = 1/10.0;
    ////        dashVals[3] = (lw[i]-3)/5.0;
    //        dashVals << 1/10.0 << 3/10.0 << 1/10.0 << (lw[i]-3)/5.0;
    ////        pen1.SetDashPattern(dashVals, 4);
    //        pen1.setDashPattern(dashVals);
    //        offset = lw[2]/2.0;
    ////        pen1.SetDashOffset((offset)/5.0);
    //        pen1.setDashOffset((offset)/5.0);
    //    } else {
    //        i = LType-26;
    ////        dashVals[0] = 1/10.0;
    ////        dashVals[1] = lw[i]/5.0;
    //        dashVals << 1/10.0 << lw[i]/5.0;
    ////        pen1.SetDashPattern(dashVals, 2);
    //        pen1.setDashPattern(dashVals);
    //        offset = lw[i]/2.0;
    ////        pen1.SetDashOffset((offset)/5.0);
    //        pen1.setDashOffset((offset)/5.0);
    //    }

    ////    g.DrawLines(&pen,(Gdiplus::Point *)outd,num);
    //    painter->setPen(pen);
    //    QPoint * qpt = new QPoint[num];
    //    for(int i=0; i<num; i++) {
    //        qpt[i].setX(outd[i].x);
    //        qpt[i].setY(outd[i].y);
    //    }
    //    painter->drawLines(qpt, num);
    //    delete [] qpt;
    //    if(len>(offset+1)*LWidth)
    //    {
    ////        g.DrawLines(&pen1,(Gdiplus::Point *)outd,num);
    //        painter->setPen(pen);
    //        QPoint * qpt = new QPoint[num];
    //        for(int i=0; i<num; i++) {
    //            qpt[i].setX(outd[i].x);
    //            qpt[i].setY(outd[i].y);
    //        }
    //        painter->drawLines(qpt, num);
    //        delete [] qpt;
    //    }
}

//==========================================
//    国界线   34 - 37
//==========================================
void CLineJB::Draw_XuXian11(int LType,POINT * outd,int num)
{
    //    QPen pen;
    //    pen.setColor(MakeColor(LColor));
    //    pen.setWidth(LWidth);
    //    QPen pen1;
    //    pen1.setColor(MakeColor(LColor));
    //    pen1.setWidth(LWidth);

    //    QVector<qreal> lpDash;
    //    QVector<qreal> dashVals1;
    //    dashVals1 << 9 << 5;
    //    QVector<qreal> dashVals2;
    //    dashVals2 << 15 << 6;
    //    QVector<qreal> dashVals3;
    //    dashVals3 << 9 << 1 << 1 <<2;
    //    QVector<qreal> dashVals4;
    //    dashVals4 << 15 << 2 << 1 <<2;

    //    QVector<qreal> W_dashVals;

    //   if(LType == 34 || LType == 35) {
    //      if(LType == 34)
    //          lpDash = dashVals3;
    //      else
    //          lpDash = dashVals4;
    //      pen.setDashPattern(lpDash);

    //      W_dashVals << 0.1;
    //      W_dashVals << (lpDash[0])/5.0-0.1;
    //      W_dashVals << 0.1;
    //      W_dashVals << (lpDash[1]+lpDash[2]+lpDash[3])/5.0-0.1;
    //      pen1.setDashPattern(W_dashVals);

    //   } else if(LType == 36 || LType == 37){
    //      if(LType == 36)
    //          lpDash = dashVals1;
    //      else
    //          lpDash = dashVals2;
    //      pen.setDashPattern(lpDash);

    //      W_dashVals << 0.1;
    //      W_dashVals << (lpDash[0])/5.0-0.1;
    //      W_dashVals << 0.1;
    //      W_dashVals << (lpDash[1])/5.0-0.1;
    //      pen1.setDashPattern(W_dashVals);
    //   }

    //    painter->setPen(pen);
    //    QPoint * qpt = new QPoint[num];
    //    for(int i=0; i<num; i++) {
    //        qpt[i].setX(outd[i].x);
    //        qpt[i].setY(outd[i].y);
    //    }
    //    painter->drawLines(qpt, num);
    //    painter->setPen(pen1);
    //    painter->drawLines(qpt, num);
    //    delete [] qpt;

}

//==========================================
//    双线63，65， 双线加轴64-66  铁路线  75 - 76
//==========================================
void CLineJB::Draw_XuXian12(int LType,COLORREF Color1,POINT * outd,int num)
{
    //    QPen WPen;
    //    WPen.setColor(QColor(255, 0, 255));
    //    WPen.setWidth(LWidth);
    //    QPainterPath GpPath;
    //    QPainterPath path1;
    //    QVector<qreal> dashVals1;
    //    QVector<qreal> dashVals2;
    //    QPen pen1;
    //    pen1.setColor(MakeColor(LColor));
    //    pen1.setWidth(LWidth);
    //    float bl;
    //    int i;

    ////    GpPath.StartFigure();
    ////    GpPath.AddLines((Gdiplus::Point*) outd,num);
    //    GpPath.moveTo(outd[0].x, outd[0].y);
    //    for(i=1; i<num; i++) {
    //        GpPath.lineTo(outd[i].x, outd[i].y);
    //    }

    ////    path1 = GpPath.Clone();
    ////    GpPath.Widen(&WPen);
    ////    GpPath.Outline();
    //    path1 = GpPath;

    ////    Gdiplus::SolidBrush brush(MakeColor(Color1));
    //    QBrush brush;
    //    brush.setColor(MakeColor(Color1));
    ////    g.FillPath(&brush,&GpPath);
    //    QBrush oldbrush = painter->brush();
    //    painter->setBrush(brush);
    //    painter->drawPath(GpPath);
    //    painter->setBrush(oldbrush);

    ////    Gdiplus::Pen pen(MakeColor(LColor),LWidth*0.3);  // 画边线
    //    QPen pen;
    //    pen.setColor(MakeColor(LColor));
    //    pen.setWidthF(LWidth*0.3);
    ////    g.DrawPath(&pen,&GpPath);
    //    painter->setPen(pen);
    //    painter->drawPath(GpPath);

    //    switch(LType){
    //        case 75:
    //            dashVals1 << 20/5.0;
    //            dashVals1 << 20/5.0;
    //            pen1.setDashPattern(dashVals1);
    ////            g.DrawPath(&pen1,path1);
    //            painter->setPen(pen1);
    //            painter->drawPath(path1);
    //            break;
    //        case 76:
    //            bl = 20*LWidth/5.0;
    //            dashVals2 << bl;
    //            dashVals2 << bl/2;
    //            dashVals2 << 0.4;
    //            dashVals2 << 0.6;
    //            dashVals2 << 0.4;
    //            dashVals2 << bl/2;
    //            pen1.setDashPattern(dashVals2);
    ////            g.DrawPath(&pen1,path1);
    //            painter->setPen(pen1);
    //            painter->drawPath(path1);
    //            break;
    //        default:
    //            break;
    //    }
    ////    if(path1)  delete path1;

}
