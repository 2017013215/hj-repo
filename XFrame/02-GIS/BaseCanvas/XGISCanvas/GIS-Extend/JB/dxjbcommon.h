#ifndef DXJB_COMMON_H
#define DXJB_COMMON_H

#ifndef MAX_PATH
#define MAX_PATH 512
#endif

typedef unsigned int UINT;
typedef unsigned long COLORREF;
typedef unsigned char BYTE;
typedef unsigned int DWORD;
typedef unsigned short WORD;
typedef bool BOOL;
#define LONG long

#define Round(X) (int)(((X) > 0)? ((X) + 0.5) : (((X) == 0)? 0 : ((X) - 0.5)))
#define min(a, b)  (((a) < (b)) ? (a) : (b))
#define max(a, b)  (((a) > (b)) ? (a) : (b))

#define MAKELONG(a, b) \
    ((LONG) (((WORD) (a)) | ((DWORD) ((WORD) (b))) << 16))

#define RGB(r, g ,b)  ((DWORD) (((BYTE) (r) | \
    ((WORD) (g) << 8)) | \
    (((DWORD) (BYTE) (b)) << 16)))

#define GetRValue(rgb)   ((BYTE) (rgb))
#define GetGValue(rgb)   ((BYTE) (((WORD) (rgb)) >> 8))
#define GetBValue(rgb)   ((BYTE) ((rgb) >> 16))

#define LOWORD(l)   ((unsigned int) (l) & 0xffff)
#define HIWORD(l)   (((unsigned int) (l) >> 16) & 0xFFFF)

#define TRUE true
#define FALSE false

enum MAPMODE{DEVICE,LOG_DLG,LOG_mM,LOG_uM,LOG_DOT};

/// DL+DK为虚线段总长
struct DLK
{
    int nDL; /// 虚线段实部长
    int nDK; /// 虚线段空长
};

struct BITMAPINFOHEADER
{
    DWORD  biSize;
    LONG   biWidth;
    LONG   biHeight;
    WORD   biPlanes;
    WORD   biBitCount;
    DWORD  biCompression;
    DWORD  biSizeImage;
    LONG   biXPelsPerMeter;
    LONG   biYPelsPerMeter;
    DWORD  biClrUsed;
    DWORD  biClrImportant;
} ;

struct RGBQUAD
{
    BYTE    rgbBlue;
    BYTE    rgbGreen;
    BYTE    rgbRed;
    BYTE    rgbReserved;
};

/// 浮点 点结构
struct FPOINT
{
        float x;
        float y;
};

/// 浮点 线结构
struct FLINE
{
    FPOINT stP1;
    FPOINT stP2;
};

/// 浮点 矩形结构
struct FRECT
{
   float left;
   float top;
   float right;
   float bottom;
};

/// 整型 点结构
struct POINT
{
    long    x;
    long    y;
};

/// 整型 线结构
struct LINE
{
    POINT stP1;
    POINT stP2;
};

/// 整型 矩形结构
struct RECT
{
    long    left;
    long    top;
    long    right;
    long    bottom;
};

bool PtInRect(RECT *rect, POINT *pt);

void CopyRect(RECT *rect, RECT *srcRect);

void ZeroMemory(void * p, UINT size);

void UnionRect(RECT *rcDst, RECT *rcSrc1, RECT *rcSrc2);

bool IntersectRect(RECT *rcDst, const RECT *rcSrc1, const RECT *rcSrc2);

bool EqualRect(const RECT *lprc1, const RECT *lprc2);

void SetRect(RECT* rect, int left,int top, int right, int bottom);

void SetRectEmpty(RECT *rect);

bool IsRectEmpty(RECT *rect);

int _Min(int num, long* x);

int _Max(int num, long* x);

RECT GetRect(POINT * pt, int num);

BOOL InflateRect(RECT *lprc, int dx, int dy);

//==================================================
//		逻辑坐标到设备坐标(单点)
//==================================================
POINT LPtoDP(int xWin,int yWin);

//==================================================
//		逻辑坐标到设备坐标(多点)
//==================================================
BOOL LPtoDP(POINT * pt,int n,POINT * outpt);

//==================================================
//		设备坐标到逻辑坐标(单点)
//==================================================
POINT DPtoLP(int xView,int yView);

//==================================================
//		设备坐标到逻辑坐标(多点)
//==================================================
BOOL DPtoLP(POINT *pt,int n,POINT * outpt);
#endif
