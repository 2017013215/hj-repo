#include "localizedCtrlObject.h"

ByyLocalizedCtrlObject::ByyLocalizedCtrlObject() 
	: ByyControlObject()
{
	mySpeedS.resize(1);
	myBCrossS.resize(1);
	myWaitTimeS.resize(1);
	myBTimeCtrlS.resize(1);
	myTimeArriveS.resize(1);
	myIRoundS.resize(1);
	myRSurroundS.resize(1);

	mySpeedS.append(0);
	myBCrossS.append(0);
	myWaitTimeS.append(0);
	myBTimeCtrlS.append(0);
	myTimeArriveS.append(0);
	myIRoundS.append(0);
	myRSurroundS.append(0);
}

ByyLocalizedCtrlObject::~ByyLocalizedCtrlObject()
{

}

void ByyLocalizedCtrlObject::setPosition( const osg::Vec3d& p )
{
	if (p!=myPosition)
	{
		myPosition=p;
		//myPosition.z()=10000;
		emit positionChanged(myPosition);
	}
}

osg::Vec3d& ByyLocalizedCtrlObject::position()
{
	return myPosition;
}


ByyVec3dArray ByyLocalizedCtrlObject::points() const
{
	return ByyVec3dArray()<<myPosition;
}

void ByyLocalizedCtrlObject::setPoints( const ByyVec3dArray& ps )
{
	if (ps.size()>0)
	{
		setPosition(ps[0]);
	}
}

ByyKeyValueList ByyLocalizedCtrlObject::getConfig()
{
	ByyKeyValueList keyValues=__super::getConfig();

	keyValues.add("speed",mySpeedS[0]);
	keyValues.add("bcross",myBCrossS[0]);
	keyValues.add("waittime",myWaitTimeS[0]);
	keyValues.add("btimectrl",myBTimeCtrlS[0]);
	keyValues.add("arrivetime",myTimeArriveS[0]);
	keyValues.add("iround",myIRoundS[0]);
	keyValues.add("rsurround",myRSurroundS[0]);

	return keyValues;
}

void ByyLocalizedCtrlObject::fromConfig(ByyKeyValueList& keyValues )
{
	__super::fromConfig(keyValues);

	{
		QList<double> speeds=keyValues.getValues<double>("speed");

		mySpeedS = speeds.toVector();
		if(mySpeedS.size() == 0)
			mySpeedS.append(0);

		QList<double> BCrossS=keyValues.getValues<double>("bcross");

		myBCrossS = BCrossS.toVector();
		if(myBCrossS.size() == 0)
			myBCrossS.append(0);

		QList<double> WaitTimeS=keyValues.getValues<double>("waittime");

		myWaitTimeS = WaitTimeS.toVector();
		if(myWaitTimeS.size() == 0)
			myWaitTimeS.append(0);

		QList<double> BTimeCtrlS=keyValues.getValues<double>("btimectrl");

		myBTimeCtrlS = BTimeCtrlS.toVector();
		if(myBTimeCtrlS.size() == 0)
			myBTimeCtrlS.append(0);

		QList<double> TimeArriveS=keyValues.getValues<double>("arrivetime");

		myTimeArriveS = TimeArriveS.toVector();
		if(myTimeArriveS.size() == 0)
			myTimeArriveS.append(0);

		QList<double> IRoundS=keyValues.getValues<double>("iround");

		myIRoundS = IRoundS.toVector();
		if(myIRoundS.size() == 0)
			myIRoundS.append(0);

		QList<double> RSurroundS=keyValues.getValues<double>("rsurround");

		myRSurroundS = RSurroundS.toVector();
		if(myRSurroundS.size() == 0)
			myRSurroundS.append(0);
	}
}


ByyDoubleArray ByyLocalizedCtrlObject::speedS() const
{
	return mySpeedS;
}

void ByyLocalizedCtrlObject::setSpeedS( const ByyDoubleArray& s )
{
	if (s!=mySpeedS)
	{
		mySpeedS=s;
		//emit pointsChanged();
	}
}

ByyDoubleArray ByyLocalizedCtrlObject::bCrossS() const
{
	return myBCrossS;
}

void ByyLocalizedCtrlObject::setBCrossS( const ByyDoubleArray& tr )
{
	if (tr!=myBCrossS)
	{
		myBCrossS=tr;
	}
}

ByyDoubleArray ByyLocalizedCtrlObject::waitTimeS() const
{
	return myWaitTimeS;
}

void ByyLocalizedCtrlObject::setWaitTimeS( const ByyDoubleArray& tr )
{
	if (tr!=myWaitTimeS)
	{
		myWaitTimeS=tr;
	}
}

ByyDoubleArray ByyLocalizedCtrlObject::bTimeCtrlS()const
{
	return myBTimeCtrlS;
}
void ByyLocalizedCtrlObject::setBTimeCtrlS(const ByyDoubleArray& tr)
{
	if(tr!=myBTimeCtrlS)
	{
		myBTimeCtrlS=tr;
	}
}


ByyDoubleArray ByyLocalizedCtrlObject::timeArriveS() const
{
	return myTimeArriveS;
}

void ByyLocalizedCtrlObject::setTimeArriveS( const ByyDoubleArray& tr )
{
	if (tr!=myTimeArriveS)
	{
		myTimeArriveS=tr;
	}
}

ByyDoubleArray ByyLocalizedCtrlObject::iRoundS() const
{
	return myIRoundS;
}

void ByyLocalizedCtrlObject::setIRoundS( const ByyDoubleArray& tr )
{
	if (tr!=myIRoundS)
	{
		myIRoundS=tr;
	}
}

ByyDoubleArray ByyLocalizedCtrlObject::rSurroundS() const
{
	return myRSurroundS;
}

void ByyLocalizedCtrlObject::setRSurroundS( const ByyDoubleArray& tr )
{
	if (tr!=myRSurroundS)
	{
		myRSurroundS=tr;
	}
}