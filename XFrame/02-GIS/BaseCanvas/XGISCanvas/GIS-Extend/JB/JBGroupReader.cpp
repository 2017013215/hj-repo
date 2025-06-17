#include <QDataStream>
#include <QDebug>

#include "JBGroupReader.h"
#include "JBWindow/JunBiaoInfo.hpp"
#include "Common_Base.h"

JBGroupReader::JBGroupReader()
{

}

QByteArray ArgReader::readGroup(QDomElement &elem)
{
    QByteArray array;
    QDataStream out(&array,QIODevice::WriteOnly);
    ShapeArg stArg;

    stArg.rtArg.fLeft = elem.attribute("left").toFloat();
    stArg.rtArg.fRight = elem.attribute("right").toFloat();
    stArg.rtArg.fTop = elem.attribute("top").toFloat();
    stArg.rtArg.fBottom = elem.attribute("bottom").toFloat();
    stArg.nStartAngle = elem.attribute("starAngle").toFloat();
    stArg.nEndAngle = elem.attribute("spanAngle").toFloat();
    out << SpArg << stArg;
    return array;
}

QByteArray TextReader::readGroup(QDomElement &elem)
{
    QByteArray array;
    QDataStream out(&array,QIODevice::WriteOnly);
    ShapeText  stText;
    stText.nX = elem.attribute("PointX").toFloat();
    stText.nY = elem.attribute("PointY").toFloat();
    stText.nSize = elem.attribute("TextSize").toFloat();
    stText.strText = elem.attribute("Text");
    out <<SpText <<stText;
    return array;
}

QByteArray RectReader::readGroup(QDomElement &elem)
{
    QByteArray array;
    QDataStream out(&array,QIODevice::WriteOnly);
    ShapeRect stRect;
    stRect.fLeft = elem.attribute("left").toFloat();
    stRect.fRight = elem.attribute("right").toFloat();
    stRect.fTop = elem.attribute("top").toFloat();
    stRect.fBottom = elem.attribute("bottom").toFloat();
    out << SpRect;
    out << stRect;
    return array;
}

QByteArray PolygonReader::readGroup(QDomElement &elem)
{
    QByteArray array;
    QDataStream out(&array,QIODevice::WriteOnly);
    ShapePolygon polygon;
    QDomNodeList nodeList = elem.childNodes();
    for(int i = 0;i < nodeList.count();++i)
    {
        if(nodeList.at(i).toElement().tagName() == tr8("Point"))
        {
            QPointF point(nodeList.at(i).toElement().attribute("X").toFloat(),nodeList.at(i).toElement().attribute("Y").toFloat());
            polygon.lsPoint.push_back(point);
        }
    }

    out << SpPolygon << polygon;
    return array;
}

QByteArray LineReader::readGroup(QDomElement &elem)
{
    QByteArray array;
    QDataStream out(&array,QIODevice::WriteOnly);
    ShapeLine stLine;
    stLine.ptStart.setX(elem.attribute("X1").toFloat());
    stLine.ptStart.setY(elem.attribute("Y1").toFloat());
    stLine.ptEnd.setX(elem.attribute("X2").toFloat());
    stLine.ptEnd.setY(elem.attribute("Y2").toFloat());
    out << SpLine << stLine;
    return array;
}

QByteArray EllipseReader::readGroup(QDomElement &elem)
{
    QByteArray array;
    QDataStream out(&array,QIODevice::WriteOnly);
    ShapeEllipse stEllipse;
    stEllipse.rtEll.fLeft = elem.attribute("left").toFloat();
    stEllipse.rtEll.fRight = elem.attribute("right").toFloat();
    stEllipse.rtEll.fTop = elem.attribute("top").toFloat();
    stEllipse.rtEll.fBottom = elem.attribute("bottom").toFloat();
    out << SpEllipse << stEllipse;
    return array;
}
