#include "ByyWaypoint.h"
#include <QStringList>
#include "coreUtils.h"

ByyWaypoint::ByyWaypoint()
	:ByyLocalizedCtrlObject()
{
	myItemClass="JSWayPoint";
}

ByyWaypoint::~ByyWaypoint()
{

}

const QString& ByyWaypoint::type() const
{
	return theType();
}

const QString& ByyWaypoint::theType()
{
	static QString _type=QString::fromLocal8Bit("Â·¾­µã");

	return _type;
}

ByyKeyValueList ByyWaypoint::getConfig()
{
	ByyKeyValueList keyValues=__super::getConfig();
	//keyValues.add("Point_Temp",myPosition);

	PointStu point;
	point.lon = myPosition.x();
	point.lat = myPosition.y();
	point.elev = myPosition.z();
	point.speed = mySpeedS[0];
	point.bcross = myBCrossS[0];
	point.waittime = myWaitTimeS[0];
	point.btimectrl = myBTimeCtrlS[0];
	point.arrivetime = myTimeArriveS[0];
	point.iround = myIRoundS[0];
	point.rsurround = myRSurroundS[0];
	keyValues.add("Point",point);

	return keyValues;
}

void ByyWaypoint::fromConfig(ByyKeyValueList& keyValues )
{
	PointStu point;
	keyValues.get("Point",point);

	keyValues.add("speed",point.speed);
 	keyValues.add("bcross",point.bcross);
 	keyValues.add("waittime",point.waittime);
 	keyValues.add("btimectrl",point.btimectrl);
 	keyValues.add("arrivetime",point.arrivetime);
 	keyValues.add("iround",point.iround);
 	keyValues.add("rsurround",point.rsurround);

	__super::fromConfig(keyValues);

	keyValues.get("Point",myPosition);
}

QString ByyWaypoint::getAttributeValueSet()
{
	QStringList attList;
	attList.push_back(QString::number(forceType()));
	attList.push_back(name());
	attList.push_back(vec3d2Str(swapXY(myPosition)));

	return attList.join(";");
}
