#include "ByyPhaseLine.h"
#include "coreUtils.h"
#include <QStringList>

ByyPhaseLine::ByyPhaseLine() 
	: ByyControlObject()
{
	myItemClass="JSPhaseline";
}

ByyPhaseLine::~ByyPhaseLine()
{

}

void ByyPhaseLine::setStart( const osg::Vec3d& p )
{
	if (myStart!=p)
	{
		myStart=p;
		emit startChanged(myStart);
	}
}

osg::Vec3d& ByyPhaseLine::start()
{
	return myStart;
}

void ByyPhaseLine::setEnd( const osg::Vec3d& p )
{
	if (myEnd!=p)
	{
		myEnd=p;
		emit endChanged(myEnd);
	}
}

osg::Vec3d& ByyPhaseLine::end()
{
	return myEnd;
}

const QString& ByyPhaseLine::type() const
{
	return theType();
}

const QString& ByyPhaseLine::theType()
{
	static QString _type=QString::fromLocal8Bit("œ‡Œªœﬂ");

	return _type;
}

ByyKeyValueList ByyPhaseLine::getConfig()
{
	ByyKeyValueList keyValues=__super::getConfig();
	keyValues.add("StartPoint",myStart);
	keyValues.add("EndPoint",myEnd);
	return keyValues;
}

void ByyPhaseLine::fromConfig(ByyKeyValueList& keyValues )
{
	__super::fromConfig(keyValues);
	keyValues.get("StartPoint",myStart);
	keyValues.get("EndPoint",myEnd);
}

QString ByyPhaseLine::getAttributeValueSet()
{
	QStringList attList;
	attList.push_back(QString::number(forceType()));
	attList.push_back(name());
	attList.push_back(vec3d2Str(swapXY(myStart)));
	attList.push_back(vec3d2Str(swapXY(myEnd)));

	return attList.join(";");
}

ByyVec3dArray ByyPhaseLine::points() const
{
	return ByyVec3dArray()<<myStart<<myEnd;
}

void ByyPhaseLine::setPoints( const ByyVec3dArray& ps )
{
	if (ps.size()>=2)
	{
		setStart(ps[0]);
		setEnd(ps[1]);
	}
}

void ByyPhaseLine::setPoints_xy( const ByyVec3dArray& ps )
{
	if (ps.size()>=2)
	{
		myStart.x() = ps[0].x();
		myStart.y() = ps[0].y();
		myEnd.x() = ps[1].x();
		myEnd.y() = ps[1].y();
	}
}
