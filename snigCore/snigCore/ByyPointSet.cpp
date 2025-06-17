#include "ByyPointSet.h"

ByyPointSet::ByyPointSet()
{

	mySpeedS.resize(0);
	myBCrossS.resize(0);
	myWaitTimeS.resize(0);
	myBTimeCtrlS.resize(0);
	myTimeArriveS.resize(0);
	myIRoundS.resize(0);
	myRSurroundS.resize(0);

	connect(this,SIGNAL(pointsChanged()),this,SLOT(initDoubleArray()));
}

ByyPointSet::~ByyPointSet()
{

}

ByyVec3dArray ByyPointSet::points() const
{
	return myPoints;
}

void ByyPointSet::setPoints( const ByyVec3dArray& ps )
{
	if (ps!=myPoints)
	{
		myPoints=ps;
		emit pointsChanged();
	}
}

void ByyPointSet::setPoints_xy( const ByyVec3dArray& ps )
{
	if (ps!=myPoints)
	{
		for(int i=0; i<ps.size() ; i++ )
		{
			myPoints[i].x()=ps.at(i).x();
			myPoints[i].y()=ps.at(i).y();
		}
		emit pointsChanged();
	}
}

void ByyPointSet::clear()
{
	myPoints.clear();
}

void ByyPointSet::append( const osg::Vec3d& pnt )
{
	myPoints.append(pnt);
}




ByyKeyValueList ByyPointSet::getConfig()
{
	ByyKeyValueList keyValues=__super::getConfig();

	for (int i=0;i!=myPoints.size();++i)
	{

		keyValues.add("speed",mySpeedS[i]);
		keyValues.add("bcross",myBCrossS[i]);
		keyValues.add("waittime",myWaitTimeS[i]);
		keyValues.add("btimectrl",myBTimeCtrlS[i]);
		keyValues.add("arrivetime",myTimeArriveS[i]);
		keyValues.add("iround",myIRoundS[i]);
		keyValues.add("rsurround",myRSurroundS[i]);
	}

	return keyValues;
}

void ByyPointSet::fromConfig(ByyKeyValueList& keyValues )
{
	__super::fromConfig(keyValues);

	{
		QList<double> speeds=keyValues.getValues<double>("speed");

		mySpeedS = speeds.toVector();

		QList<double> BCrossS=keyValues.getValues<double>("bcross");

		myBCrossS = BCrossS.toVector();

		QList<double> WaitTimeS=keyValues.getValues<double>("waittime");

		myWaitTimeS = WaitTimeS.toVector();

		QList<double> BTimeCtrlS=keyValues.getValues<double>("btimectrl");

		myBTimeCtrlS = BTimeCtrlS.toVector();

		QList<double> TimeArriveS=keyValues.getValues<double>("arrivetime");

		myTimeArriveS = TimeArriveS.toVector();

		QList<double> IRoundS=keyValues.getValues<double>("iround");

		myIRoundS = IRoundS.toVector();

		QList<double> RSurroundS=keyValues.getValues<double>("rsurround");

		myRSurroundS = RSurroundS.toVector();
	}
}


ByyDoubleArray ByyPointSet::speedS() const
{
	return mySpeedS;
}

void ByyPointSet::setSpeedS( const ByyDoubleArray& s )
{
	if (s!=mySpeedS)
	{
		mySpeedS=s;
		//emit pointsChanged();
	}
}

ByyDoubleArray ByyPointSet::bCrossS() const
{
	return myBCrossS;
}

void ByyPointSet::setBCrossS( const ByyDoubleArray& tr )
{
	if (tr!=myBCrossS)
	{
		myBCrossS=tr;
	}
}

ByyDoubleArray ByyPointSet::waitTimeS() const
{
	return myWaitTimeS;
}

void ByyPointSet::setWaitTimeS( const ByyDoubleArray& tr )
{
	if (tr!=myWaitTimeS)
	{
		myWaitTimeS=tr;
	}
}

ByyDoubleArray ByyPointSet::bTimeCtrlS()const
{
	return myBTimeCtrlS;
}
void ByyPointSet::setBTimeCtrlS(const ByyDoubleArray& tr)
{
	if(tr!=myBTimeCtrlS)
	{
		myBTimeCtrlS=tr;
	}
}


ByyDoubleArray ByyPointSet::timeArriveS() const
{
	return myTimeArriveS;
}

void ByyPointSet::setTimeArriveS( const ByyDoubleArray& tr )
{
	if (tr!=myTimeArriveS)
	{
		myTimeArriveS=tr;
	}
}

ByyDoubleArray ByyPointSet::iRoundS()const
{
	return myIRoundS;
}
void ByyPointSet::setIRoundS(const ByyDoubleArray& tr)
{
	if (tr!=myIRoundS)
	{
		myIRoundS=tr;
	}
}

ByyDoubleArray ByyPointSet::rSurroundS()const
{
	return myRSurroundS;
}
void ByyPointSet::setRSurroundS(const ByyDoubleArray& tr)
{
	if (tr!=myRSurroundS)
	{
		myRSurroundS=tr;
	}
}


void ByyPointSet::initDoubleArray()
{
	if(mySpeedS.size() == myPoints.size())
		return;

	mySpeedS.resize(myPoints.size());
	myBCrossS.resize(myPoints.size());
	myWaitTimeS.resize(myPoints.size());
	myBTimeCtrlS.resize(myPoints.size());
	myTimeArriveS.resize(myPoints.size());
	myIRoundS.resize(myPoints.size());
	myRSurroundS.resize(myPoints.size());
	for (int i =0;i<myPoints.size();++i)
	{
		mySpeedS.append(0);
		myBCrossS.append(0);
		myWaitTimeS.append(0);
		myBTimeCtrlS.append(0);
		myTimeArriveS.append(0);
		myIRoundS.append(0);
		myRSurroundS.append(0);
	}
}