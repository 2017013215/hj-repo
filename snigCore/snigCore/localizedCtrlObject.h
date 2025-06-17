#ifndef LOCALIZEDCTRLOBJECT_H
#define LOCALIZEDCTRLOBJECT_H

#include "ByyControlObject.h"

typedef QVector<double> ByyDoubleArray;
class SNIGCORE_EXPORT ByyLocalizedCtrlObject : public ByyControlObject
{
	Q_OBJECT
public:
	ByyLocalizedCtrlObject();
	virtual ~ByyLocalizedCtrlObject();

	void setPosition(const osg::Vec3d& p);
	osg::Vec3d& position();

	virtual ByyVec3dArray points()const;
	void setPoints(const ByyVec3dArray& ps);

signals:
	void positionChanged(const osg::Vec3d&);

protected:
	osg::Vec3d myPosition;


	//add by xks 20221113
public:
	virtual ByyKeyValueList getConfig();
	virtual void fromConfig(ByyKeyValueList& keyValues);

	ByyDoubleArray speedS()const;
	void setSpeedS(const ByyDoubleArray& s);

	ByyDoubleArray bCrossS()const;
	void setBCrossS(const ByyDoubleArray& tr);

	ByyDoubleArray waitTimeS()const;
	void setWaitTimeS(const ByyDoubleArray& tr);

	ByyDoubleArray bTimeCtrlS()const;
	void setBTimeCtrlS(const ByyDoubleArray& tr);

	ByyDoubleArray timeArriveS()const;
	void setTimeArriveS(const ByyDoubleArray& tr);

	ByyDoubleArray iRoundS()const;
	void setIRoundS(const ByyDoubleArray& tr);

	ByyDoubleArray rSurroundS()const;
	void setRSurroundS(const ByyDoubleArray& tr);

protected:
	ByyDoubleArray mySpeedS;
	ByyDoubleArray myBCrossS; 
	ByyDoubleArray myWaitTimeS;
	ByyDoubleArray myBTimeCtrlS;
	ByyDoubleArray myTimeArriveS;
	ByyDoubleArray myIRoundS;
	ByyDoubleArray myRSurroundS;
};

#endif // LOCALIZEDCTRLOBJECT_H
