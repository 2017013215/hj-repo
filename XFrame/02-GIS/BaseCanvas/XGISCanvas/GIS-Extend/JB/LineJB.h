#ifndef LINEJB_H
#define LINEJB_H
#include"dxjbcommon.h"

class CLineJB
{
public:
    CLineJB();

    static int  BT_lr(double x1,double y1,double x2,double y2,double x3,double y3);
    static void M_fd(double L2,double x1,double y1,double x2,double y2,double L,double *x0,double *y0);
    static void Drw_X(double L2,double x1,double y1,double x2,double y2,int lr,int LineType);
    static void BT_2d(double L2,double x1,double y1,double x2,double y2,int lr);
    static void BT_3d(double L2,double x1,double y1,double x2,double y2,double *xl,double *yl,double *xr,double *yr);
    static void N_fd(double L2,double x1,double y1,double x2,double y2,double *x0,double *y0);
    static void W_fd(double L2,double x1,double y1,double x2,double y2,double *x0,double *y0);
    static bool Line2Dot(double x11,double y11,double x12,double y12,double x21,double y21,double x22,double y22,double & x,double & y);

    static void Draw_XuXian1(int DL,int DK,POINT * D,int num);
    static void Draw_XuXian2(int DL,int DK,int DD,POINT * D,int num);
    static void Draw_XuXian3(int DL,int DW, POINT * D,int num,int LineType);
    static void Draw_XuXian4(int DL,int DK,int DW, POINT * D,int num);
    static void Draw_XuXian5(int DL,int DK,int DW,POINT * D,int num,int LineType);
    static void Draw_XuXian7(int Width,COLORREF Color, COLORREF Color1,int DW, POINT * D1,int num,int LineType);
    static void Draw_XuXian8(float JL,float JK,POINT * outd,int num);
    static void Draw_XuXian9(int LType,POINT * outd,int num);
    static void Draw_XuXian10(int LType,POINT * outd,int num);
    static void Draw_XuXian11(int LType,POINT * outd,int num);
    static void Draw_XuXian12(int LType,COLORREF Color1,POINT * outd,int num);
    static void Draw_XuXian101(int Width,POINT * D,int num);
};

#endif // LINEJB_H
