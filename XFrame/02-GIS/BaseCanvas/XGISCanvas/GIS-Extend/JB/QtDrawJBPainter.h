#ifndef QTDRAWJBPAINTER_H
#define QTDRAWJBPAINTER_H

#include <QPainter>
#include <QPainterPath>
#include "dxjbcommon.h"
#include "JBType.h"

#define GetAValue(rgba)  ((BYTE)((rgba) >> 24))
#define GetRValue(rgb)   ((BYTE) (rgb))
#define GetGValue(rgb)   ((BYTE) (((WORD) (rgb)) >> 8))
#define GetBValue(rgb)   ((BYTE) ((rgb) >> 16))
#define MakeColor(rgba)   QColor(GetRValue(rgba),GetGValue(rgba),GetBValue(rgba),255-GetAValue(rgba))

/// 对军标进行绘制的画笔,适应所有PaintDevice设备
class CQtDrawJBPainter : public QPainter
{

public:
    CQtDrawJBPainter(QPaintDevice *pDevice);

public:
	/// 绘制军标
    void DrawGroup(const QT_GROUP &rGroup);

	/// 设置军标的尺寸, (相当于在画布的在绘制的大小)
 	void SetJBSize(QSize rSize);
 	void SetJBSize(int nWidth, int nHeight);
	
	/// 贝塞尔精度,值越高越平滑（建议：10-20之间）
	void SetBezierPrecision(int nPrecision);
	void SetUserColor(const QColor& rColor);

    void setUseUserColor(bool b);
private:
    /// 初始化常量
    void init();
	void DrawPoint(JB_UNIT_HEAD *rJB);

private:
    void DrawBLine(JB_UNIT_HEAD *rJB);
    void DrawBBline(JB_UNIT_HEAD *rJB);
    void DrawPLine(JB_UNIT_HEAD *rJB);
    void DrawRect(JB_UNIT_HEAD *rJB);
    void DrawEllipse(JB_UNIT_HEAD *rJB);
    void DrawArc(JB_UNIT_HEAD *rJB);
    void DrawPie(JB_UNIT_HEAD *rJB);
//    void DrawChord();
    void DrawText(JB_UNIT_HEAD *rJB);
//    void DrawBMP();
    void DrawPolygon(JB_UNIT_HEAD *rJB);
//    void DrawPoint();
//    void DrawLine();
//    void DrawTextBorder();
//    void DrawBitmap();

private:
    void SetLineAttr(LINE_ATTR *LA);
	/// 生成贝塞尔曲线顶点
	void Bezier(POINT *pt, int n, POINT *pxy0);
	/// 计算顶点数量
    int BrowseLine(int *Data, int flag);

    int Quadrant(int x, int y);

private:
	QColor m_userColor;         /// 用户自定义颜色
	QRect m_rectBox;			///< 包围盒
	QSize m_sizeJB;				///< 图标尺寸
    float m_dScale;             ///< 为保持纵横比
	
	float m_dJBFactor;			///< 军标缩放引子
	float m_dTextSizeFactor;	///< 文字缩放引子	

	int m_nBezierPrecision;		///< 贝塞尔曲线精度 (默认：15）
	bool m_bUseUserColor;       /// 是否用用户自定义颜色
};

#endif // QTDRAWJBPAINTER_H
