#include "ByyPolygon.h"
#include "coreUtils.h"

#include <QStringList>

ByyPolygon::ByyPolygon() 
	: ByyPointSet()
{
	myItemClass="JSPolygon";
}

ByyPolygon::~ByyPolygon()
{

}

const QString& ByyPolygon::type() const
{
	return theType();
}

const QString& ByyPolygon::theType()
{
	static QString _type=QString::fromLocal8Bit("ÇøÓò");

	return _type;
}

ByyKeyValueList ByyPolygon::getConfig()
{
	ByyKeyValueList keyValues=__super::getConfig();
	keyValues.add("PointCount",QString::number(myPoints.size()));

	for (int i=0;i!=myPoints.size();++i)
	{
		//keyValues.add("WayPoint_Temp",myPoints[i]);

		PointStu point;
		point.lon = myPoints[i].x();
		point.lat = myPoints[i].y();
		point.elev = myPoints[i].z();
		point.speed = mySpeedS[i];
		point.bcross = myBCrossS[i];
		point.waittime = myWaitTimeS[i];
		point.btimectrl = myBTimeCtrlS[i];
		point.arrivetime = myTimeArriveS[i];
		point.iround = myIRoundS[i];
		point.rsurround = myRSurroundS[i];

		//keyValues.add("WayPoint",myPoints[i]);
		keyValues.add("WayPoint",point);
	}

	return keyValues;
}

void ByyPolygon::fromConfig(ByyKeyValueList& keyValues )
{
	QList<PointStu> pointList=keyValues.getValues<PointStu>("WayPoint");
	for(int index = 0; index < pointList.size(); ++index)
	{
		//keyValues.add("WayPoint_Temp",osg::Vec3d(pointList.at(index).lon,pointList.at(index).lat,pointList.at(index).elev));
		keyValues.add("speed",pointList.at(index).speed);
		keyValues.add("bcross",pointList.at(index).bcross);
		keyValues.add("waittime",pointList.at(index).waittime);
		keyValues.add("btimectrl",pointList.at(index).btimectrl);
		keyValues.add("arrivetime",pointList.at(index).arrivetime);
		keyValues.add("iround",pointList.at(index).iround);
		keyValues.add("rsurround",pointList.at(index).rsurround);
	}

	__super::fromConfig(keyValues);

	QList<osg::Vec3d> points=keyValues.getValues<osg::Vec3d>("WayPoint");

	myPoints=points.toVector();

	emit pointsChanged();
}

QString ByyPolygon::getAttributeValueSet()
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
