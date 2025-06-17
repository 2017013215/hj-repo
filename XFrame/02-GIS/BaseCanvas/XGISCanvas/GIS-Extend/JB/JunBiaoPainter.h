#ifndef JUNBIAOPAINTER_H
#define JUNBIAOPAINTER_H
#include <QPainter>
#include <QByteArray>
#include "JBWindow/JunBiaoInfo.hpp"
class JunBiaoRender
{
public:
    JunBiaoRender(QPainter& painter);
    void setColor(const QColor &color);
    void setLineWidth(int nLineWidth);

    void draw(const QByteArray& array,QRect rect);
private:
    void parse(ShapeType type, QDataStream &in);

    void drawRect(QDataStream& in);
    void drawEllipse(QDataStream& in);
    void drawArg(QDataStream& in);
    void drawText(QDataStream& in);
    void drawPolygon(QDataStream& in);
    void drawLine(QDataStream& in);
private:
    QColor      m_color;
    int             m_nLineWidth;

    QPainter& m_painter;
    QRect m_rect;
};

#endif // JUNBIAOPAINTER_H
