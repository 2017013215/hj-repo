#ifndef BYYPOINTSET_H
#define BYYPOINTSET_H

#include "ByyControlObject.h"

typedef QVector<double> ByyDoubleArray;
class SNIGCORE_EXPORT ByyPointSet : public ByyControlObject
{
	Q_OBJECT
public:
	ByyPointSet();
	virtual ~ByyPointSet();

	virtual ByyVec3dArray points()const;
	virtual void setPoints(const ByyVec3dArray& ps);
	virtual void setPoints_xy(const ByyVec3dArray& ps); // xks,20240708, 只设置xy的值

	void clear();

	void append(const osg::Vec3d& pnt);

signals:
	void pointsChanged();

protected:
	ByyVec3dArray myPoints;


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

	private slots:
		void initDoubleArray();
protected:
	ByyDoubleArray mySpeedS;
	ByyDoubleArray myBCrossS; 
	ByyDoubleArray myWaitTimeS;
	ByyDoubleArray myBTimeCtrlS;
	ByyDoubleArray myTimeArriveS;
	ByyDoubleArray myIRoundS;
	ByyDoubleArray myRSurroundS;
	
};

#endif // POINTSET_H
