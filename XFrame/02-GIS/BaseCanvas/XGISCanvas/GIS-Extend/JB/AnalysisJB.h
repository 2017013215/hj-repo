#ifndef ANALYSIS_JB_H
#define ANALYSIS_JB_H

#include "JBType.h"

struct POINT;
struct RECT;

class CAnalysisJB
{
public:
    virtual ~CAnalysisJB(){}

    void DrawGroup(const QT_GROUP& rGroup);
    void DrawPoint(JB_UNIT_HEAD *rJB);

protected:

    /**
     * @brief 根据军标中的属性
     *             设置绘画的样式
     * @param pLineAttr
     */
    void SetLineAtter(LINE_ATTR* pLineAttr);

    /**
     * @brief 根据tag文件生成BLine数据
     */
    void CallBLine(int* pData, bool bFill, int nClose);

    /**
     * @brief 根据tag文件生成 BBLine数据
     * @param pData
     * @param nClose
     * @param bFill
     */
    void CallBBLine(int *pData, bool bFill, int nClose);

    /**
     * @brief 根据tag文件生成PLine数据
     * @param pData
     * @param nNum
     */
    void CallPLine(POINT *pData, int nNum);

    /**
     * @brief 获取矩形的数据
     * @param pData
     */
    void CallRect(POINT* pData);

    /**
     * @brief 获取椭圆数据
     * @param pData
     */
    void CallEllipse(int* pData);

    /**
     * @brief 获取饼图数据
     * @param pData
     */
    void CallPie(int *pData,int nFlag);

    /**
     * @brief 获取多边形
     * @param pData
     */
    void CallPgon(int* pData);

    /**
     * @brief 字符
     * @param pText
     */
    void CallText(POINT* pText);

    /**
     * @brief 面要素裁切
     * @param in
     * @param out
     * @param count
     * @param r
     * @return
     */
    int ClipArea(POINT * in,POINT * out,int count,RECT r);

    int intersect_x(POINT a,POINT b,POINT *c);
    int inside(POINT p,POINT *c);
    int intersect_y(POINT a,POINT b,POINT *c);

    int S_H_CLIP(POINT *in_v,POINT * out_v,int in_length,POINT *clip);
};

#endif // ANALYSIS_JB_H
