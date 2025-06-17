#include "ByyStraightArrow.h"
#include "coreUtils.h"
ByyStraightArrow::ByyStraightArrow()
{
	myItemClass="ByyStraightArrow";
}

ByyStraightArrow::~ByyStraightArrow()
{

}

const QString& ByyStraightArrow::type() const
{
	return theType();
}

const QString& ByyStraightArrow::theType()
{
	static QString _type=QString::fromLocal8Bit("Ö±¼ýÍ·");

	return _type;
}


void ByyStraightArrow::fromConfig(ByyKeyValueList& keyValues )
{
	__super::fromConfig(keyValues);

	QList<osg::Vec3d> points=keyValues.getValues<osg::Vec3d>("WayPoint");

	myPoints=points.toVector();
}

#include <QStringList>

QString ByyStraightArrow::getAttributeValueSet()
{
	QStringList attList;
	attList.push_back(QString::number(forceType()));
	attList.push_back(name());
	attList.push_back("0");
	attList.push_back("0");
	attList.push_back(QString::number(myPoints.size()));

	for (int i=0;i!=myPoints.size();++i)
	{
		attList.push_back(vec3d2Str(swapXY(myPoints[i])));
	}

	return attList.join(";");
}


ByyKeyValueList ByyStraightArrow::getConfig()
{
	ByyKeyValueList keyValues=__super::getConfig();
	keyValues.add("PointCount",QString::number(myPoints.size()));

	for (int i=0;i!=myPoints.size();++i)
	{
		keyValues.add("WayPoint",myPoints[i]);
	}

	return keyValues;
}