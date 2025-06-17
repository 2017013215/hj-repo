#ifndef JBPIXMAPHELPER_H
#define JBPIXMAPHELPER_H

#include <QObject>
#include <QPixmap>
#include <QColor>

#include "JBWindow/JunBiaoInfo.hpp"

// 前置声明
//class CJBPixmapWidget;
class CJBTreeModel;
struct QT_GROUP;

//=====================================================================
// 类名：CJBPixmapHelper
// 时间：2017年9月7日
// 使用方法：--------------------------------------------
//
//		1.QPixmap pixmap = GetJBPixmap(uId,160,160);
//	当前获得的是一个160x160大小的军标图元数据.
//
//		2.CJBPixmapHelper::SetBackColor
//	设置整体背景颜色,可放在初始化的位置.
// -------------------------------------------------------

/// -- CJBPixmapHelper
/// -- 军标图样获取辅助类
///
class CJBPixmapHelper : public QObject
{
public:
    ~CJBPixmapHelper();

    static CJBPixmapHelper *Instance();

    // 设置背景颜色
    void setBackColor(QColor color);

    // 获取军标（自定义大小）
    QPixmap getPixmap(const QT_GROUP &vGroup, int width, int height,const QColor& color = QColor(),int nLineWidth = -1);
    QPixmap getPixmap(const QT_GROUP &vGroup, QSize size, const QColor& color = QColor(),int nLineWidth = -1);
    QPixmap getPixmap(const JB_GROUP &vGroup, QSize size, const QColor& color = QColor(), int nLinewidth = -1);
private:
    explicit CJBPixmapHelper(QObject *parent= 0);

    static CJBPixmapHelper      *m_pInstance;

	QColor						m_color;		///< 背景颜色

    //CJBPixmapWidget             *m_pPixmapWidget;
};

#define GetJBPixmap(id,width,height)\
    CJBPixmapHelper::Instance()->getPixmap(id,width,height)

#endif // JBPIXMAPHELPER_H
