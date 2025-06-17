//---------------------------------------------------------------------------
#ifndef CalcLibH
#define CalcLibH
#define GETLEN(x1,y1,x2,y2) sqrt((double)((x1)-(x2))*((x1)-(x2))+(double)((y1)-(y2))*((y1)-(y2)))

#include "JBType.h"
#include "dxjbcommon.h"

#define  quxian  0x100
#define  guanghua  0x8000
#define  guanlian  0x10000

int  BrowseLine(int * Data,int flag);
int Bezier(POINT * pt,int n,POINT *pxy0);
void BT_2_Point(float,int,int,int,int,int,int,int,int*,int*,int*,int*);
void BT_1_Point(float,int,int,int,int,int,int,int*,int*);
void BT_YC_Point(float Rate,int con,int p3x,int p3y,int p1x,int p1y,int p2x,int p2y,int *x1,int *y1);
int  BT_lr(int,int,int,int,int,int);
void BT_3d(float,int,int,int,int,int *,int *,int *,int *);
void BT_pxx(float,float, int, int, int, int, int, int, int *, int *, int *,int *);
void BT_dyd(float, int, int, int, int, int, int, int *, int *);
void N_fd(float ,int ,int ,int ,int ,int *,int *);
void W_fd(float ,int ,int ,int ,int ,int *,int *);

void FenJiao1(double L,int x1,int y1,int x2,int y2,int x3,int y3,int *xl,int *yl,int *xr,int *yr);
float Q_Bezier(POINT * ,long,float jd=0.5);
int FindLineDot(POINT * pt,POINT *pt0,int wc);
void FindPtInCurve(POINT *,int ,int );
float FindBezierDot(POINT * Bzpt,POINT * pt1,int wc);
float GetBezierLen(POINT * Bzpt,float t,int jd);
float GetBezierLen1(POINT * Bzpt,float t0,float t1,int jd);
void AddDotOnBezier(POINT * pt,float t1);

POINT Dcd(POINT d1,POINT d2,POINT d3);
POINT Cxjd(POINT d1,POINT d2,POINT d3);

void Fl(POINT d1,POINT d2,POINT d3,double * l1,double * l2);
RECT CalcLineUnit(int * Data,int CloseFlag);
RECT CalcPLineB(POINT * Data,int n);
double CalcPLineL(POINT * Data,int n);
int FindPLineD(POINT * Data,int *n,double Len);
POINT JiaoD(POINT d1,POINT d2,POINT d3,POINT d4);
float CalcCLineLen(FPOINT * s,int N);
float CalcCLineLen(POINT * s,int N);

void EllipseDataToBLine(int * d0,int *d1);
void PieDataToBLine(int * d0,int *d1,int flag);

int GetCutDot(float x0,float y0,float r,float x1,float y1,float * x2,float * y2,float *x3, float *y3);
void RotateLine(float x0,float y0,float x1,float y1,float jiao,float * x2,float * y2);
float CalcAngle(float x1,float y1,float x2,float y2);
bool IsClockwisePoly(FPOINT * fpt,int n);
float SumPolyInterAngle(FPOINT * fpt,int n);
float GetLineAngle(int x1,int y1,int x2,int y2,int x3,int y3);

float HLS_3(float sz[3][3]);
float HLS_4(float sz[4][4]);

float Cal_AG_Length(FPOINT A,FPOINT B,FPOINT C,float alpha);
int Dot_In_Triangle(FPOINT G,FPOINT A,FPOINT B,FPOINT C);
float LD_JIAJIAO(FPOINT a,FPOINT b);
void RotatePoly(POINT * pp,int N,float x0,float y0,float jiao);

//========================================
//          获取多个点的外接矩形
//========================================
RECT GetRect(POINT * pt);

//========================================
//    旋转、缩放和平移
//========================================
void TransformUnit(int * Array,int * TranE,int num,int x,int y,float Scale,float Sin,float Cos,int TranFlag);

//---------------------------------------------------------------------------
#endif
