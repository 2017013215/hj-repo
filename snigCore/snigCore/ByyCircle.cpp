#include "ByyCircle.h"
#include "coreUtils.h"
#include <QStringList>

ByyCircle::ByyCircle() 
	:ByyLocalizedCtrlObject()
{
	myItemClass="JSCircle";
}

ByyCircle::~ByyCircle()
{

}

void ByyCircle::setRadius( double rad )
{
	if (rad!=myRadius)
	{
		myRadius=rad;
		emit radiusChanged(myRadius);
	}
}

double ByyCircle::radius()
{
	return myRadius;
}

const QString& ByyCircle::type() const
{
	return theType();
}

const QString& ByyCircle::theType()
{
	static QString _type=QString::fromLocal8Bit("т╡пн");

	return _type;
}

ByyKeyValueList ByyCircle::getConfig()
{
	ByyKeyValueList keyValues=__super::getConfig();
	//keyValues.add("CenterPos_Temp",myPosition);
	keyValues.add("Radius",myRadius);

	PointStu point;
	point.lon = myPosition.x();
	point.lat = myPosition.y();
	point.elev = myPosition.z();
	point.speed = mySpeedS[0];
	point.bcross = myBCrossS[0];
	point.waittime = myWaitTimeS[0];
	point.btimectrl=myBTimeCtrlS[0];
	point.arrivetime = myTimeArriveS[0];
	point.iround = myIRoundS[0];
	point.rsurround = myRSurroundS[0];
	keyValues.add("CenterPos",point);

	return keyValues;
}

void ByyCircle::fromConfig(ByyKeyValueList& keyValues )
{
	PointStu point;
	keyValues.get("CenterPos",point);

	keyValues.add("speed",point.speed);
	keyValues.add("bcross",point.bcross);
	keyValues.add("waittime",point.waittime);
	keyValues.add("btimectrl",point.btimectrl);
	keyValues.add("arrivetime",point.arrivetime);
	keyValues.add("iround",point.iround);
	keyValues.add("rsurround",point.rsurround);

	__super::fromConfig(keyValues);
	keyValues.get("CenterPos",myPosition);
	keyValues.get("Radius",myRadius);
}

QString ByyCircle::getAttributeValueSet()
{
	QStringList attList;
	attList.push_back(QString::number(forceType()));
	attList.push_back(name());
	attList.push_back("");
	attList.push_back("");
	attList.push_back(vec3d2Str(swapXY(myPosition)));
	attList.push_back(QString::number(myRadius));

	return attList.join(";");
}