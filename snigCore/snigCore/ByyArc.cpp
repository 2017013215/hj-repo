#include "ByyArc.h"
#include <QStringList>

ByyArc::ByyArc(void)
{
	myItemClass="CArc";
}


ByyArc::~ByyArc(void)
{
}



const QString& ByyArc::type() const
{
	return theType();
}

const QString& ByyArc::theType()
{
	static QString _type=QString::fromLocal8Bit("»¡ÐÎ");

	return _type;
}

ByyKeyValueList ByyArc::getConfig()
{
	ByyKeyValueList keyValues=ByyCircle::getConfig();
	keyValues.add("StartAngle",myStartAngle);
	keyValues.add("EndAngle",myEndAngle);

	return keyValues;
}

void ByyArc::fromConfig(ByyKeyValueList& keyValues )
{
	ByyCircle::fromConfig(keyValues);
	keyValues.get("StartAngle",myStartAngle);
	keyValues.get("EndAngle",myEndAngle);
}

QString ByyArc::getAttributeValueSet()
{
	QString str=ByyCircle::getAttributeValueSet();
	QStringList ret;
	ret<<str;
	ret<<QString::number(myStartAngle,'f',4);
	ret<<QString::number(myEndAngle,'f',4);
	
	return ret.join(";");
}

void ByyArc::setStartAngle(double angle)
{
	if(myStartAngle!=angle)
	{
		myStartAngle=angle;
		emit startAngleChanged(angle);
	}
}

void ByyArc::setEndAngle(double angle)
{
	if(myEndAngle!=angle)
	{
		myEndAngle=angle;
		emit endAngleChanged(angle);
	}
}