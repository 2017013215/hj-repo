#include "JunBiaoPainter.h"
#include "ParseJunBiao.h"
#include <QDebug>
#include <qmath.h>
JunBiaoRender::JunBiaoRender(QPainter& painter)
    : m_painter(painter)
    , m_color(QColor())
    , m_nLineWidth(1)
{

}

void JunBiaoRender::setColor(const QColor &color)
{
    if(color.isValid())
    {
        m_color = color;
    }
}

void JunBiaoRender::setLineWidth(int nLineWidth)
{
    if(nLineWidth != -1)
    {
        m_nLineWidth = nLineWidth;
    }
}

void JunBiaoRender::draw(const QByteArray &array, QRect rect)
{
    m_painter.save();
    m_painter.translate(rect.topLeft());
    ShapeType type;
    qint32 nType;
    m_rect.setWidth(rect.width() );
    m_rect.setHeight(rect.height() );
    m_rect.setTopLeft(QPoint(0,0));
    QDataStream in(array);
    QPen pen = m_painter.pen();
    pen.setColor(m_color.isValid() ? m_color:Qt::red);
    pen.setWidth(m_nLineWidth);
    m_painter.setPen(pen);
    while(!in.atEnd())
    {
        in >> nType;
        type = (ShapeType)nType;
        parse(type,in);
    }
    m_painter.restore();
}

void JunBiaoRender::parse(ShapeType type,QDataStream& in)
{
    switch(type)
    {
    case SpRect:
        drawRect(in);
        break;
    case SpEllipse:
        drawEllipse(in);
        break;
    case SpArg:
        drawArg(in);
        break;
    case SpText:
        drawText(in);
        break;
    case SpPolygon:
        drawPolygon(in);
        break;
    case SpLine:
        drawLine(in);
        break;
    }
}

void JunBiaoRender::drawRect(QDataStream &in)
{
    ShapeRect rect = ParseJunBiao::GetRect(in);
    QRect rtScreen;
    rtScreen.setLeft(m_rect.width()*rect.fLeft);
    rtScreen.setRight(m_rect.width()*rect.fRight);
    rtScreen.setTop(m_rect.height()*rect.fTop);
    rtScreen.setBottom(m_rect.height()*rect.fBottom);
    m_painter.drawRect(rtScreen);
}

void JunBiaoRender::drawEllipse(QDataStream &in)
{
    ShapeEllipse ell = ParseJunBiao::getEllipse(in);
    QRect rtScreen;
    rtScreen.setLeft(m_rect.width()*ell.rtEll.fLeft);
    rtScreen.setRight(m_rect.width()*ell.rtEll.fRight);
    rtScreen.setTop(m_rect.height()*ell.rtEll.fTop);
    rtScreen.setBottom(m_rect.height()*ell.rtEll.fBottom);
    m_painter.drawEllipse(rtScreen);
}

void JunBiaoRender::drawArg(QDataStream &in)
{
      QRect rtScreen;
      ShapeArg arg = ParseJunBiao::getArg(in);
      rtScreen.setLeft(m_rect.width()*arg.rtArg.fLeft);
      rtScreen.setRight(m_rect.width()*arg.rtArg.fRight);
      rtScreen.setTop(m_rect.width()*arg.rtArg.fTop);
      rtScreen.setBottom(m_rect.width()*arg.rtArg.fBottom);
      m_painter.drawArc(rtScreen, 16*arg.nStartAngle, 16*arg.nEndAngle);
}

void JunBiaoRender::drawText(QDataStream &in)
{
    ShapeText text;
    QFont oldFont = m_painter.font();
    QFont font;
    text = ParseJunBiao::getText(in);
    font.setPixelSize(m_rect.width()*m_rect.height() * text.nSize);
    m_painter.setFont(font);
    m_painter.drawText(m_rect.width() * text.nX,
                                    m_rect.height() * text.nY,
                                    text.strText);
    m_painter.setFont(oldFont);
}

void JunBiaoRender::drawPolygon(QDataStream &in)
{
    ShapePolygon poly;

    poly  = ParseJunBiao::getPolygon(in);
    int count = poly.lsPoint.count();
    QPointF *pPoints = new QPointF[count];
    for(int i = 0;i < count;++i)
    {
        pPoints[i].setX( poly.lsPoint.at(i).x()*m_rect.width());
        pPoints[i].setY(poly.lsPoint.at(i).y()*m_rect.height());
    }
    m_painter.drawPolygon(pPoints,count);
    delete []pPoints;
}

void JunBiaoRender::drawLine(QDataStream &in)
{
    ShapeLine line;
    line = ParseJunBiao::getLine(in);
    QPoint start;
    QPoint end;
    start.setX(line.ptStart.x() * m_rect.width());
    start.setY(line.ptStart.y() * m_rect.height());
    end.setX(line.ptEnd.x() * m_rect.width());
    end.setY(line.ptEnd.y() * m_rect.height());
    m_painter.drawLine(start,end);
}
