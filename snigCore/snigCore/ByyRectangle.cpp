#include "ByyRectangle.h"
#include "coreUtils.h"

#include <osgEarth/GeoMath>
#include <QStringList>

ByyRectangle::ByyRectangle() 
	: ByyPolygon()
{
	myItemClass="JSRectangle";
}

ByyRectangle::~ByyRectangle()
{

}

const QString& ByyRectangle::type() const
{
	return theType();
}

const QString& ByyRectangle::theType()
{
	static QString _type=QString::fromLocal8Bit("¾ØÐÎ");

	return _type;
}

QString ByyRectangle::getAttributeValueSet()
{
	QStringList attList;
	attList.push_back(QString::number(forceType()));
	attList.push_back(name());
	attList.push_back("0");
	attList.push_back("0");

	for (int i=0;i!=myPoints.size();++i)
	{
		attList.push_back(vec3d2Str(swapXY(myPoints[i])));
	}

	

	/*double majorEdge=osgEarth::GeoMath::distance(myPoints[0],myPoints[1],osgEarth::SpatialReference::get("wgs84"));
	double minorAdeg=osgEarth::GeoMath::distance(myPoints[0],myPoints[3],osgEarth::SpatialReference::get("wgs84"));

	attList.push_back(QString::number(majorEdge));
	attList.push_back(QString::number(minorAdeg));*/

	return attList.join(";");
}

bool ByyRectangle::operator!=( const ByyRectangle& e )
{
	return myPoints!=e.myPoints;
}

ByyRectangle& ByyRectangle::operator=( const ByyRectangle& e )
{
	myPoints=e.myPoints;
	return *this;
}

bool ByyRectangle::operator==( const ByyRectangle& e )
{
	return myPoints==e.myPoints;
}
