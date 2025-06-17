#ifndef JunBiaoInfo_h__
#define JunBiaoInfo_h__

#include <QPoint>
#include <QDataStream>
#include <QString>
#include <vector>

enum ShapeType
{
	SpRect = 32,
	SpEllipse,
	SpArg,
    SpText,
    SpPolygon,
    SpLine
};

///矩形
struct ShapeRect
{
	float fLeft;
	float fRight;
	float fTop;
	float fBottom;
	friend QDataStream& operator<< (QDataStream& out, const ShapeRect& rect)
	{
		out << rect.fLeft << rect.fRight << rect.fTop << rect.fBottom;
		return out;
	}
	friend QDataStream& operator >> (QDataStream& in, ShapeRect& rect)
	{
		in >> rect.fLeft >> rect.fRight >> rect.fTop >> rect.fBottom;
		return in;
	}
};
///圆型
struct ShapeEllipse
{
	ShapeRect rtEll;
	friend QDataStream& operator << (QDataStream& out, const ShapeEllipse& ellipse)
	{
        out << ellipse.rtEll;
		return out;
	}
	friend QDataStream& operator >> (QDataStream &in, ShapeEllipse& ellipse)
	{
        in >> ellipse.rtEll;
		return in;
	}
};
///圆弧图形
struct ShapeArg
{
	int nStartAngle;
	int nEndAngle;
	ShapeRect rtArg;
	friend QDataStream& operator << (QDataStream &out, const ShapeArg &arg)
	{
		out << arg.nStartAngle << arg.nEndAngle << arg.rtArg;
		return out;
	}
	friend QDataStream& operator >> (QDataStream& in, ShapeArg& arg)
	{
		in >> arg.nStartAngle >> arg.nEndAngle >> arg.rtArg;
		return in;
	}
};

///文字
struct ShapeText
{
	QString strText;
	float nX;
	float nY;
    float   nSize;
	friend QDataStream& operator << (QDataStream& out, const ShapeText& text)
	{
		out << text.strText << text.nX << text.nY << text.nSize;
        return out;
	}
	friend QDataStream& operator >> (QDataStream& in,ShapeText& text)
	{
		in >> text.strText >> text.nX >> text.nY >> text.nSize;
		return in;
	}
};
///多边形
struct ShapePolygon
{
   QList<QPointF> lsPoint;
   friend QDataStream& operator << (QDataStream& out,ShapePolygon& other)
   {
       out << other.lsPoint;
       return out;
   }
   friend QDataStream& operator >> (QDataStream& in,ShapePolygon& other)
   {
       in >> other.lsPoint;
       return in;
   }
};

struct ShapeLine
{
    QPointF ptStart;
    QPointF ptEnd;
    friend QDataStream& operator << (QDataStream& out,ShapeLine& other)
    {
        out << other.ptStart << other.ptEnd;
        return out;
    }
    friend QDataStream& operator >> (QDataStream& in,ShapeLine& other)
    {
        in >> other.ptStart >> other.ptEnd;
        return in;
    }
};

///图元信息，包含图元类型和数据
//struct ShapeInfo
//{
//	QByteArray byteInfo;
//	ShapeType   emJBType;
//};

///军标信息，包含多个图元组成的军标
struct JB_GROUP
{
    JB_GROUP() : unTagId(0),unGroupId(0){ }
    QString					strName;
    quint32					unTagId;
    quint32                unGroupId;
    QByteArray          shapeInfo;
};
#endif // JunBiaoInfo_h__
