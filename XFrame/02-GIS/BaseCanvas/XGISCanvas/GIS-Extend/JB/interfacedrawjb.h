#ifndef INTERFACEDRAWJB_H
#define INTERFACEDRAWJB_H

#include "JBType.h"
/**
 * @brief 绘制军标的接口文件
 */
class InterfaceDrawJB
{
public:
    virtual ~InterfaceDrawJB(){}

    /**
     * @brief 绘制军标
     * @param 军标元组数据
     */
    virtual bool Draw(const QT_GROUP& rJBGroup)=0;

    /**
     * @brief 开始绘制军标消息
     */
    virtual void BeforeDraw()=0;

    /**
     * @brief 结束绘制军标消息
     */
    virtual void AfterDraw()=0;
};

#endif // INTERFACEDRAWJB_H
