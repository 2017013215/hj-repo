#include "ByyEllipse.h"
#include "coreUtils.h"
#include <QStringList>

ByyEllipse::ByyEllipse() 
	:ByyLocalizedCtrlObject()
	,myMajorSemiAxis(0)
	,myMinorSemiAxis(0)
	,myRotationAngle(0)
{
	myItemClass="JSEllipse";
}

ByyEllipse::~ByyEllipse()
{

}

double ByyEllipse::majorSemiAxis()
{
	return myMajorSemiAxis;
}

void ByyEllipse::setMajorSemiAxis( double val )
{
	if (val!=myMajorSemiAxis)
	{
		myMajorSemiAxis=val;

		emit majorSemiAxisChanged(myMajorSemiAxis);
	}
}

double ByyEllipse::minorSemiAxis()
{
	return myMinorSemiAxis;
}

void ByyEllipse::setMinorSemiAxis( double val )
{
	if (val!=myMinorSemiAxis)
	{
		myMinorSemiAxis=val;

		emit minorSemiAxisChanged(myMinorSemiAxis);
	}
}

const QString& ByyEllipse::type() const
{
	return theType();
}

const QString& ByyEllipse::theType()
{
	static QString _type=QString::fromLocal8Bit("Õ÷‘≤");

	return _type;
}

double ByyEllipse::rotationAngle()
{
	return myRotationAngle;
}

void ByyEllipse::setRotationAngle( double val )
{
	if (val!=myRotationAngle)
	{
		myRotationAngle=val;

		emit rotationAngleChanged(val);
	}
}

ByyKeyValueList ByyEllipse::getConfig()
{
	ByyKeyValueList keyValues=__super::getConfig();
	//keyValues.add("CenterPos_Temp",myPosition);
	keyValues.add("MajorSemiAxis",myMajorSemiAxis);
	keyValues.add("MinorSemiAxis",myMinorSemiAxis);
	keyValues.add("RotationAngle",myRotationAngle);

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
	keyValues.add("CenterPos",point);

	return keyValues;
}

void ByyEllipse::fromConfig(ByyKeyValueList& keyValues )
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
	keyValues.get("MajorSemiAxis",myMajorSemiAxis);
	keyValues.get("MinorSemiAxis",myMinorSemiAxis);
	keyValues.get("RotationAngle",myRotationAngle);
}

QString ByyEllipse::getAttributeValueSet()
{
	QStringList attList;
	attList.push_back(QString::number(forceType()));
	attList.push_back(name());
	attList.push_back("");
	attList.push_back("");
	attList.push_back(vec3d2Str(swapXY(myPosition)));
	attList.push_back(QString::number(myMajorSemiAxis));
	attList.push_back(QString::number(myMinorSemiAxis));

	return attList.join(";");
}