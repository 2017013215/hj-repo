#include "dxjbcommon.h"
#include <cstdlib>
#include <cstring>

bool PtInRect(RECT *rect, POINT *pt)
{
    return ((rect->left < pt->x && pt->x < rect->right) && (rect->top < pt->y && pt->y < rect->bottom));
}

void CopyRect(RECT *rect, RECT *srcRect)
{
    rect->left = srcRect->left;
    rect->right = srcRect->right;
    rect->top = srcRect->top;
    rect->bottom = srcRect->bottom;
}

void ZeroMemory(void * p, UINT size)
{
    memset(p, 0, size);
}

void UnionRect(RECT *rcDst, RECT *rcSrc1, RECT *rcSrc2)
{
    if (rcSrc1->right > rcSrc2->right)
        rcDst->right = rcSrc1->right;
    else
        rcDst->right = rcSrc2->right;
    if (rcSrc1->left < rcSrc2->left)
        rcDst->left = rcSrc1->left;
    else
        rcDst->left = rcSrc2->left;
    if (rcSrc1->top < rcSrc2->top)
        rcDst->top = rcSrc1->top;
    else
        rcDst->top = rcSrc2->top;
    if (rcSrc1->bottom > rcSrc2->bottom)
        rcDst->bottom = rcSrc1->bottom;
    else
        rcDst->bottom = rcSrc2->bottom;
}

bool IntersectRect(RECT *rcDst, const RECT *rcSrc1, const RECT *rcSrc2)
{
    return(true);
}

bool EqualRect(const RECT *rc1, const RECT *rc2)
{
    return (rc1->left == rc2->left)
            && (rc1->right == rc2->right)
            && (rc1->top == rc2->top)
            && (rc1->bottom == rc2->bottom);
}

//=============设置一个矩形的左，上，右，下坐标================
void SetRect(RECT* rect, int left,int top, int right, int bottom)
{
    rect->left = left;
    rect->top = top;
    rect->right = right;
    rect->bottom = bottom;
}

void SetRectEmpty(RECT *rect)
{
    rect->left = 0;
    rect->top = 0;
    rect->right = 0;
    rect->bottom = 0;
}

bool IsRectEmpty(RECT *rect)
{
    return (rect->right <= rect->left || rect->bottom <= rect->top);
}

//===================================================
//	找最小 重写_Min(int num, ...)
//===================================================
int _Min(int num, long* x)
{
    int tmp;
    if (num > 0)
        tmp = x[0];
    else
        return -1;
    int i;
    for (i = 0; i < num; i++)
    {
        if (tmp > x[i])
            tmp = x[i];
    }
    return tmp;
}

//===================================================
//	找最大 重写_Max(int num, ...)
//===================================================
int _Max(int num, long* x)
{
    int tmp;
    if (num > 0)
        tmp = x[0];
    else
        return -1;
    int i;
    for (i = 0; i < num; i++)
    {
        if (tmp < x[i])
            tmp = x[i];
    }
    return tmp;
}

RECT GetRect(POINT * pt, int num)
{
    long * ptxs = (long *)malloc(sizeof(long) * num);
    long * ptys = (long *)malloc(sizeof(long) * num);
    for(int i=0; i<num; i++)
    {
        ptxs[i] = pt[i].x;
        ptys[i] = pt[i].y;
    }
    long minX = _Min(num, ptxs);
    long minY = _Min(num, ptys);
    long maxX = _Max(num, ptxs);
    long maxY = _Max(num, ptys);

    RECT r;
    r.left = minX;
    r.right = maxX;
    r.top = minY;
    r.bottom = maxY;

    return r;
}

BOOL InflateRect(RECT *lprc, int dx, int dy)
{
    lprc->left -= dx;
    lprc->top -= dy;
    lprc->right += dx;
    lprc->bottom += dy;
    return(1);
}

//==================================================
//
//		逻辑坐标到设备坐标(单点)
//
//==================================================
int Resolution = 1;
POINT LPtoDP(int xWin,int yWin)
{
    double xViewport,yViewport;
    POINT pt;
    double Rx,Ry;
    int xWinOrg=0,yWinOrg=0;

    Rx=Resolution;
    Ry=Resolution;

    xViewport=(xWin-xWinOrg)*Rx;
    yViewport=(yWin-yWinOrg)*(-Ry);
    pt.x = Round(xViewport);
    pt.y = Round(yViewport);
    return pt;
}
