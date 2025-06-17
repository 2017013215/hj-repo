#ifndef PLOTUTILS_H
#define PLOTUTILS_H

#include <QtCore/qglobal.h>
#include <QVector>
#include <QVector3D>

/*
#ifdef SNPLOTUTILS_LIB
# define SNPLOTUTILS_EXPORT Q_DECL_EXPORT
#else
# define SNPLOTUTILS_EXPORT Q_DECL_IMPORT
#endif*/

typedef QVector<QVector3D> ByyVec3dArray;

class /*SNPLOTUTILS_EXPORT*/ ByyPlotUtils
{
public:
	static ByyVec3dArray generateFineArrow(const ByyVec3dArray& points);
	static ByyVec3dArray generateDoubleArrow(const ByyVec3dArray& points);
	static ByyVec3dArray generateGatheringPlace(const ByyVec3dArray& points);
	static ByyVec3dArray generateAttackArrow(const ByyVec3dArray& points);
	static ByyVec3dArray generateStraightArrow(const ByyVec3dArray& points);

	static double distance(const QVector3D& pnt1,const QVector3D& pnt2);
	static double wholeDistance(const ByyVec3dArray& points);
	static double getBaseLength(const ByyVec3dArray& points);

	static QVector3D getThirdPoint(const QVector3D& startPnt, const QVector3D& endPnt, double angle, double distance, bool clockWise);

	static double getAzimuth(const QVector3D& startPnt, const QVector3D& endPnt);

	static QVector3D mid(const QVector3D& pnt1, const QVector3D& pnt2);

	static double getAngleOfThreePoints(const QVector3D& pntA, const QVector3D& pntB, const QVector3D& pntC);

	static ByyVec3dArray getBezierPoints(const ByyVec3dArray& points);

	static double getBinomialFactor(int n,int index);

	static int getFactorial(int n);

	static bool isClockWise(const QVector3D& pnt1, const QVector3D& pnt2,const QVector3D& pnt3);

	static ByyVec3dArray getBisectorNormals(double t,const QVector3D&  pnt1,const QVector3D&  pnt2,const QVector3D&  pnt3);

	static QVector3D getNormal(const QVector3D& pnt1, const QVector3D& pnt2, const QVector3D& pnt3);
	static QVector3D getCubicValue(double t,const QVector3D& startPnt,const QVector3D& cPnt1,const QVector3D& cPnt2,const QVector3D& endPnt);

	static ByyVec3dArray getQBSplinePoints(const ByyVec3dArray& points);

	static double getQuadricBSplineFactor(int k,double t);
};

#endif // PLOTUTILS_H
