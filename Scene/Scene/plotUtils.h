#ifndef PLOTUTILS_H
#define PLOTUTILS_H

#include <QtCore/qglobal.h>
#include <QVector>
#include <osg/vec3d>

/*
#ifdef SNPLOTUTILS_LIB
# define SNPLOTUTILS_EXPORT Q_DECL_EXPORT
#else
# define SNPLOTUTILS_EXPORT Q_DECL_IMPORT
#endif*/

typedef QVector<osg::Vec3d> ByyVec3dArray;

class /*SNPLOTUTILS_EXPORT*/ ByyPlotUtils
{
public:
	static ByyVec3dArray generateFineArrow(const ByyVec3dArray& points);
	static ByyVec3dArray generateDoubleArrow(const ByyVec3dArray& points);
	static ByyVec3dArray generateGatheringPlace(const ByyVec3dArray& points);
	static ByyVec3dArray generateAttackArrow(const ByyVec3dArray& points);
	static ByyVec3dArray generateStraightArrow(const ByyVec3dArray& points);

	static double distance(const osg::Vec3d& pnt1,const osg::Vec3d& pnt2);
	static double wholeDistance(const ByyVec3dArray& points);
	static double getBaseLength(const ByyVec3dArray& points);

	static osg::Vec3d getThirdPoint(const osg::Vec3d& startPnt, const osg::Vec3d& endPnt, double angle, double distance, bool clockWise);

	static double getAzimuth(const osg::Vec3d& startPnt, const osg::Vec3d& endPnt);

	static osg::Vec3d mid(const osg::Vec3d& pnt1, const osg::Vec3d& pnt2);

	static double getAngleOfThreePoints(const osg::Vec3d& pntA, const osg::Vec3d& pntB, const osg::Vec3d& pntC);

	static ByyVec3dArray getBezierPoints(const ByyVec3dArray& points);

	static double getBinomialFactor(int n,int index);

	static int getFactorial(int n);

	static bool isClockWise(const osg::Vec3d& pnt1, const osg::Vec3d& pnt2,const osg::Vec3d& pnt3);

	static ByyVec3dArray getBisectorNormals(double t,const osg::Vec3d&  pnt1,const osg::Vec3d&  pnt2,const osg::Vec3d&  pnt3);

	static osg::Vec3d getNormal(const osg::Vec3d& pnt1, const osg::Vec3d& pnt2, const osg::Vec3d& pnt3);
	static osg::Vec3d getCubicValue(double t,const osg::Vec3d& startPnt,const osg::Vec3d& cPnt1,const osg::Vec3d& cPnt2,const osg::Vec3d& endPnt);

	static ByyVec3dArray getQBSplinePoints(const ByyVec3dArray& points);

	static double getQuadricBSplineFactor(int k,double t);
};

#endif // PLOTUTILS_H
