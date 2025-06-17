#include "stdafx.h"
#include "plotUtils.h"
#include <math.h>

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define M_PI       3.14159265358979323846
#define TWO_PI M_PI*2
#define HALF_PI M_PI/2
#define ZERO_TOLERANCE 0.0001  
#define FITTING_COUNT 100

ByyVec3dArray ByyPlotUtils::generateFineArrow( const ByyVec3dArray& points )
{
	ByyVec3dArray result;

	if(points.count() < 2) {
		return result;
	}

// 	static double tailWidthFactor = 0.15;
// 	static double neckWidthFactor = 0.2;
// 	static double headWidthFactor = 0.25;
	static double tailWidthFactor = 0.10;
	static double neckWidthFactor = 0.2;
	static double headWidthFactor = 0.25;
	static double headAngle = osg::PI / 8.5;
	static double neckAngle = osg::PI / 13;

	const osg::Vec3d& pnt1 = points[0];
	const osg::Vec3d& pnt2 = points[1];

	double len =ByyPlotUtils::getBaseLength(points);
	double tailWidth = len * tailWidthFactor;
	double neckWidth = len * neckWidthFactor;
	double headWidth = len * headWidthFactor;

	result.resize(7);

	result[0] = ByyPlotUtils::getThirdPoint(pnt2, pnt1, HALF_PI, tailWidth, true); //tailLeft
	result[1] = ByyPlotUtils::getThirdPoint(pnt1, pnt2, neckAngle, neckWidth, false);//neckLeft
	result[2] = ByyPlotUtils::getThirdPoint(pnt1, pnt2, headAngle, headWidth, false); //headLeft

	result[3] = pnt2;

	result[4] = ByyPlotUtils::getThirdPoint(pnt1, pnt2, headAngle, headWidth, true); //headRight

	result[5] = ByyPlotUtils::getThirdPoint(pnt1, pnt2, neckAngle, neckWidth, true);//neckRight

	result[6] = ByyPlotUtils::getThirdPoint(pnt2, pnt1, HALF_PI, tailWidth, false); //tailRight

	return result;
}

double ByyPlotUtils::distance( const osg::Vec3d& pnt1,const osg::Vec3d& pnt2 )
{
	return sqrt(pow((pnt1[0] - pnt2[0]), 2) + pow((pnt1[1] - pnt2[1]), 2));
}

double ByyPlotUtils::wholeDistance( const ByyVec3dArray& points )
{
	double _distance = 0;
	for(int i=0; i<points.size()-1; i++)
		_distance += ByyPlotUtils::distance(points[i], points[i+1]);
	return _distance;
}

double ByyPlotUtils::getBaseLength( const ByyVec3dArray& points )
{
	return pow(ByyPlotUtils::wholeDistance(points), 0.99);
}

osg::Vec3d ByyPlotUtils::getThirdPoint( const osg::Vec3d& startPnt, const osg::Vec3d& endPnt, double angle, double distance, bool clockWise )
{
	double azimuth=ByyPlotUtils::getAzimuth(startPnt, endPnt);
	double alpha = clockWise ? azimuth+angle : azimuth-angle;
	double dx=distance * cos(alpha);
	double dy=distance * sin(alpha);

	return osg::Vec3d(endPnt[0] + dx, endPnt[1] + dy,0); 
}

double ByyPlotUtils::getAzimuth( const osg::Vec3d& startPnt, const osg::Vec3d& endPnt )
{
	double azimuth;
	double angle=asin(abs(endPnt[1] - startPnt[1]) / ByyPlotUtils::distance(startPnt, endPnt));
	if (endPnt[1] >= startPnt[1] && endPnt[0] >= startPnt[0])
		azimuth=angle + M_PI;
	else if (endPnt[1] >= startPnt[1] && endPnt[0] < startPnt[0])
		azimuth=TWO_PI - angle;
	else if (endPnt[1] < startPnt[1] && endPnt[0] < startPnt[0])
		azimuth=angle;
	else if (endPnt[1] < startPnt[1] && endPnt[0] >= startPnt[0])
		azimuth=M_PI - angle;
	return azimuth;
}

osg::Vec3d getTempPoint4(const osg::Vec3d& linePnt1,const osg::Vec3d& linePnt2,const osg::Vec3d& point)
{
	osg::Vec3d midPnt=ByyPlotUtils::mid(linePnt1, linePnt2);
	double len=ByyPlotUtils::distance(midPnt, point);
	double angle=ByyPlotUtils::getAngleOfThreePoints(linePnt1, midPnt, point);
	osg::Vec3d symPnt,mid;
	double distance1, distance2;
	if (angle < HALF_PI)
	{
		distance1=len * sin(angle);
		distance2=len * cos(angle);
		mid=ByyPlotUtils::getThirdPoint(linePnt1, midPnt, HALF_PI, distance1, false);
		symPnt=ByyPlotUtils::getThirdPoint(midPnt, mid, HALF_PI, distance2, true);
	}
	else if (angle >= HALF_PI && angle < M_PI)
	{
		distance1=len * sin(M_PI - angle);
		distance2=len * cos(M_PI - angle);
		mid=ByyPlotUtils::getThirdPoint(linePnt1, midPnt, HALF_PI, distance1, false);
		symPnt=ByyPlotUtils::getThirdPoint(midPnt, mid, HALF_PI, distance2, false);
	}
	else if (angle >= M_PI && angle < M_PI * 1.5)
	{
		distance1=len * sin(angle - M_PI);
		distance2=len * cos(angle - M_PI);
		mid=ByyPlotUtils::getThirdPoint(linePnt1, midPnt,HALF_PI, distance1, true);
		symPnt=ByyPlotUtils::getThirdPoint(midPnt, mid, HALF_PI, distance2, true);
	}
	else
	{
		distance1=len * sin(M_PI * 2 - angle);
		distance2=len * cos(M_PI * 2 - angle);
		mid=ByyPlotUtils::getThirdPoint(linePnt1, midPnt, HALF_PI, distance1, true);
		symPnt=ByyPlotUtils::getThirdPoint(midPnt, mid, HALF_PI, distance2, false);
	}
	return symPnt;
}

ByyVec3dArray getArrowHeadPoints(const ByyVec3dArray& points, const osg::Vec3d& tailLeft, const osg::Vec3d& tailRight)
{
	double headHeightFactor = 0.25;
	double headWidthFactor = 0.3;
	double neckHeightFactor = 0.85;
	double neckWidthFactor = 0.15;

	double len =ByyPlotUtils::getBaseLength(points);
	double headHeight = len * headHeightFactor;
	osg::Vec3d headPnt = points[points.size()-1];
	double tailWidth = ByyPlotUtils::distance(tailLeft, tailRight);
	double headWidth = headHeight * headWidthFactor;
	double neckWidth = headHeight * neckWidthFactor;
	double neckHeight = headHeight *neckHeightFactor;

	ByyVec3dArray result(5);

	result[2]=headPnt;

	osg::Vec3d headEndPnt = ByyPlotUtils::getThirdPoint(points[points.size()-2], headPnt, 0, headHeight, true); //headEndPnt
	osg::Vec3d neckEndPnt = ByyPlotUtils::getThirdPoint(points[points.size()-2], headPnt, 0, neckHeight, true); //neckEndPnt
	result[1] = ByyPlotUtils::getThirdPoint(headPnt, headEndPnt, HALF_PI, headWidth, false); //headLeft
	result[3] = ByyPlotUtils::getThirdPoint(headPnt, headEndPnt, HALF_PI, headWidth, true);//headRight
	result[0] = ByyPlotUtils::getThirdPoint(headPnt, neckEndPnt, HALF_PI, neckWidth, false);//neckLeft
	result[4] = ByyPlotUtils::getThirdPoint(headPnt, neckEndPnt, HALF_PI, neckWidth, true);//neckRight

	return result;
};

ByyVec3dArray getArrowBodyPoints(const ByyVec3dArray& points,const osg::Vec3d& neckLeft,const osg::Vec3d& neckRight,double tailWidthFactor)
{
	double allLen = ByyPlotUtils::wholeDistance(points);
	double len = ByyPlotUtils::getBaseLength(points);
	double tailWidth = len * tailWidthFactor;
	double neckWidth = ByyPlotUtils::distance(neckLeft, neckRight);
	double widthDif = (tailWidth - neckWidth) / 2;
	double tempLen = 0;

	ByyVec3dArray leftBodyPnts,rightBodyPnts;

	for(int i=1; i<points.size()-1; i++)
	{
		double angle=ByyPlotUtils::getAngleOfThreePoints(points[i-1], points[i], points[i+1]) / 2;
		tempLen += ByyPlotUtils::distance(points[i-1], points[i]);
		double w = (tailWidth/2 - tempLen / allLen * widthDif) / sin(angle);
		osg::Vec3d left = ByyPlotUtils::getThirdPoint(points[i-1], points[i], M_PI-angle, w, true);
		osg::Vec3d right = ByyPlotUtils::getThirdPoint(points[i-1], points[i], angle, w, false);
		leftBodyPnts.append(left);
		rightBodyPnts.append(right);
	}
	return leftBodyPnts+rightBodyPnts;
};
ByyVec3dArray getArrowPoints(const osg::Vec3d& pnt1, const osg::Vec3d& pnt2, const osg::Vec3d& pnt3, bool clockWise)
{
	osg::Vec3d midPnt=ByyPlotUtils::mid(pnt1, pnt2);
	double len=ByyPlotUtils::distance(midPnt, pnt3);
	osg::Vec3d midPnt1=ByyPlotUtils::getThirdPoint(pnt3, midPnt, 0, len * 0.3, true);
	osg::Vec3d midPnt2=ByyPlotUtils::getThirdPoint(pnt3, midPnt, 0, len * 0.5, true);
	//var midPnt3=PlotUtils.getThirdPoint(pnt3, midPnt, 0, len * 0.7, true);
	midPnt1=ByyPlotUtils::getThirdPoint(midPnt, midPnt1, HALF_PI, len / 5, clockWise);
	midPnt2=ByyPlotUtils::getThirdPoint(midPnt, midPnt2, HALF_PI, len / 4, clockWise);
	//midPnt3=PlotUtils.getThirdPoint(midPnt, midPnt3, Constants.HALF_PI, len / 5, clockWise);

	ByyVec3dArray points(4);

	points[0]=midPnt;
	points[1]=midPnt1;
	points[2]=midPnt2;
	points[3]=pnt3;

	double headHeightFactor = 0.25;
	double headWidthFactor = 0.3;
	double neckHeightFactor = 0.85;
	double neckWidthFactor = 0.15;

	// 计算箭头部分
	ByyVec3dArray arrowPnts=getArrowHeadPoints(points, osg::Vec3d(headHeightFactor, headWidthFactor,0) ,osg::Vec3d(neckHeightFactor, neckWidthFactor,0));
	
	osg::Vec3d neckLeftPoint=arrowPnts[0];
	osg::Vec3d neckRightPoint=arrowPnts[4];
	// 计算箭身部分
	double tailWidthFactor=ByyPlotUtils::distance(pnt1, pnt2) / ByyPlotUtils::getBaseLength(points) / 2;
	ByyVec3dArray bodyPnts=getArrowBodyPoints(points, neckLeftPoint, neckRightPoint, tailWidthFactor);
	int n=bodyPnts.size();
	ByyVec3dArray lPoints=bodyPnts.mid(0, n / 2);
	ByyVec3dArray rPoints=bodyPnts.mid(n / 2, n);
	lPoints.append(neckLeftPoint);
	rPoints.append(neckRightPoint);

	std::reverse(lPoints.begin(),lPoints.end());
	lPoints.append(pnt2);
	std::reverse(rPoints.begin(),rPoints.end());
	rPoints.append(pnt1);

	std::reverse(lPoints.begin(),lPoints.end());

	return lPoints+arrowPnts+rPoints;
};

ByyVec3dArray ByyPlotUtils::generateDoubleArrow( const ByyVec3dArray& points )
{
	ByyVec3dArray result;

	int count = points.size();

	if(count<3) return result;

	
	osg::Vec3d connPoint;
	osg::Vec3d tempPoint4;

	const osg::Vec3d& pnt1 = points[0];
	const osg::Vec3d& pnt2 = points[1];
	const osg::Vec3d& pnt3 = points[2];


	if(count == 3)
		tempPoint4 = getTempPoint4(pnt1, pnt2, pnt3);
	else
		tempPoint4 = points[3];
	if(count==3 || count==4)
		connPoint = ByyPlotUtils::mid(pnt1, pnt2);
	else
		connPoint = points[4];
	ByyVec3dArray leftArrowPnts, rightArrowPnts;
	if(ByyPlotUtils::isClockWise(pnt1, pnt2, pnt3)){
		leftArrowPnts = getArrowPoints(pnt1, connPoint, tempPoint4, false);
		rightArrowPnts = getArrowPoints(connPoint, pnt2, pnt3, true);
	}else{
		leftArrowPnts = getArrowPoints(pnt2, connPoint, pnt3, false);
		rightArrowPnts = getArrowPoints(connPoint, pnt1, tempPoint4, true);
	}
	int m = leftArrowPnts.size();
	int t = (m - 5) / 2;

	ByyVec3dArray llBodyPnts = leftArrowPnts.mid(0 ,t);
	ByyVec3dArray lArrowPnts = leftArrowPnts.mid(t, 5);
	ByyVec3dArray lrBodyPnts = leftArrowPnts.mid(t+5, /*m*/-1);

	ByyVec3dArray rlBodyPnts = rightArrowPnts.mid(0 ,t);
	ByyVec3dArray rArrowPnts = rightArrowPnts.mid(t,5);
	ByyVec3dArray rrBodyPnts = rightArrowPnts.mid(t+5, /*m*/-1);

	rlBodyPnts = ByyPlotUtils::getBezierPoints(rlBodyPnts);
	ByyVec3dArray bodyPnts = ByyPlotUtils::getBezierPoints(rrBodyPnts+llBodyPnts.mid(1));
	lrBodyPnts = ByyPlotUtils::getBezierPoints(lrBodyPnts);

	return rlBodyPnts+rArrowPnts+bodyPnts+lArrowPnts+lrBodyPnts;
}

osg::Vec3d ByyPlotUtils::mid( const osg::Vec3d& pnt1, const osg::Vec3d& pnt2 )
{
	return osg::Vec3d((pnt1[0]+pnt2[0])/2, (pnt1[1]+pnt2[1])/2,0);
}

double ByyPlotUtils::getAngleOfThreePoints( const osg::Vec3d& pntA, const osg::Vec3d& pntB, const osg::Vec3d& pntC )
{
	double angle=ByyPlotUtils::getAzimuth(pntB, pntA) - ByyPlotUtils::getAzimuth(pntB, pntC);
	return (angle<0 ? angle + TWO_PI : angle);
}

ByyVec3dArray ByyPlotUtils::getBezierPoints( const ByyVec3dArray& points )
{
	if (points.size() <= 2)
		return points;

	ByyVec3dArray bezierPoints;
	int n=points.size() - 1;
	for (double t=0; t <= 1; t+=0.01){
		double x=0,y=0;
		for (int index=0; index <= n; index++){
			double factor=ByyPlotUtils::getBinomialFactor(n, index);
			double a=pow(t, index);
			double b=pow((1 - t), (n - index));
			x+=factor * a * b * points[index][0];
			y+=factor * a * b * points[index][1];
		}
		bezierPoints.append(osg::Vec3d(x, y,0));
	}
	bezierPoints.append(points[n]);
	return bezierPoints;
}

double ByyPlotUtils::getBinomialFactor( int n,int index )
{
	return ByyPlotUtils::getFactorial(n) / (ByyPlotUtils::getFactorial(index) * ByyPlotUtils::getFactorial(n - index));
}

int ByyPlotUtils::getFactorial( int n )
{
	if (n <= 1)
		return 1;
	if (n == 2)
		return 2;
	if (n == 3)
		return 6;
	if (n == 4)
		return 24;
	if (n == 5)
		return 120;
	int result=1;
	for (int i=1; i <= n; i++)
		result*=i;
	return result;
}

bool ByyPlotUtils::isClockWise( const osg::Vec3d& pnt1, const osg::Vec3d& pnt2,const osg::Vec3d& pnt3 )
{
	return ((pnt3[1]-pnt1[1])*(pnt2[0]-pnt1[0]) > (pnt2[1]-pnt1[1])*(pnt3[0]-pnt1[0]));
}

ByyVec3dArray ByyPlotUtils::generateGatheringPlace( const ByyVec3dArray& points )
{
	ByyVec3dArray pnts = points;

	if(pnts.size()<2)
		return ByyVec3dArray();

	double t=0.4;

	if(points.size()==2)
	{
		osg::Vec3d mid = ByyPlotUtils::mid(pnts[0], pnts[1]);
		double d = ByyPlotUtils::distance(pnts[0], mid)/0.9;
		osg::Vec3d pnt = ByyPlotUtils::getThirdPoint(pnts[0], mid, HALF_PI, d, true);

		pnts.insert(1,pnt);
	}

	osg::Vec3d mid = ByyPlotUtils::mid(pnts[0], pnts[2]);

	pnts.append(mid);
	pnts.append(pnts[0]);
	pnts.append(pnts[1]);

	ByyVec3dArray normals;

	for(int i=0; i<pnts.size()-2; i++)
	{
		const osg::Vec3d& pnt1 = pnts[i];
		const osg::Vec3d& pnt2 = pnts[i+1];
		const osg::Vec3d& pnt3 = pnts[i+2];
		ByyVec3dArray normalPoints = ByyPlotUtils::getBisectorNormals(t, pnt1, pnt2, pnt3);
		normals+=normalPoints;
	}

	int count = normals.size();

	//normals = [normals[count-1]]+normals.mid(0, count-1);

	normals.push_front(normals.last());
	normals.pop_back();	

	ByyVec3dArray pList;

	for(int i=0; i<pnts.size()-2; i++)
	{
		const osg::Vec3d& pnt1 = pnts[i];
		const osg::Vec3d& pnt2 = pnts[i+1];
		pList.append(pnt1);
		for(int t=0; t<=FITTING_COUNT; t++)
		{
			osg::Vec3d pnt = ByyPlotUtils::getCubicValue(t*1.0/FITTING_COUNT, pnt1, normals[i*2], normals[i*2+1], pnt2);
			pList.append(pnt);
		}
		pList.append(pnt2);
	}
	return pList;
}

ByyVec3dArray ByyPlotUtils::getBisectorNormals( double t,const osg::Vec3d& pnt1,const osg::Vec3d& pnt2,const osg::Vec3d& pnt3 )
{
	osg::Vec3d normal = ByyPlotUtils::getNormal(pnt1, pnt2, pnt3);
	double dist = sqrt(normal[0]*normal[0] + normal[1]*normal[1]);
	double uX = normal[0]/dist;
	double uY = normal[1]/dist;
	double d1 = distance(pnt1, pnt2);
	double d2 = distance(pnt2, pnt3);
	osg::Vec3d bisectorNormalLeft,bisectorNormalRight;

	if(dist > ZERO_TOLERANCE){
		if(ByyPlotUtils::isClockWise(pnt1, pnt2, pnt3)){
			double dt = t * d1;
			double x = pnt2[0] - dt*uY;
			double y = pnt2[1] + dt*uX;
			bisectorNormalRight=osg::Vec3d(x, y,0);
			dt = t * d2;
			x = pnt2[0] + dt*uY;
			y = pnt2[1] - dt*uX;
			bisectorNormalLeft=osg::Vec3d(x, y,0);
		}
		else{
			double dt = t * d1;
			double x = pnt2[0] + dt*uY;
			double y = pnt2[1] - dt*uX;
			bisectorNormalRight =osg::Vec3d(x, y,0);
			dt = t * d2;
			x = pnt2[0] - dt*uY;
			y = pnt2[1] + dt*uX;
			bisectorNormalLeft =osg::Vec3d(x, y,0);
		}
	}
	else{
		double x = pnt2[0] + t*(pnt1[0] - pnt2[0]);
		double y = pnt2[1] + t*(pnt1[1] - pnt2[1]);
		bisectorNormalRight = osg::Vec3d(x, y,0);
		x = pnt2[0] + t*(pnt3[0] - pnt2[0]);
		y = pnt2[1] + t*(pnt3[1] - pnt2[1]);
		bisectorNormalLeft = osg::Vec3d(x, y,0);
	}

	ByyVec3dArray result(2);

	result[0]=bisectorNormalRight;
	result[1]=bisectorNormalLeft;

	return result;
}

osg::Vec3d ByyPlotUtils::getNormal( const osg::Vec3d& pnt1, const osg::Vec3d& pnt2, const osg::Vec3d& pnt3 )
{
	double dX1 = pnt1[0] - pnt2[0];
	double dY1 = pnt1[1] - pnt2[1];
	double d1 = sqrt(dX1*dX1 + dY1*dY1);
	dX1 /= d1;
	dY1 /= d1;

	double dX2 = pnt3[0] - pnt2[0];
	double dY2 = pnt3[1] - pnt2[1];
	double d2 = sqrt(dX2*dX2 + dY2*dY2);
	dX2 /= d2;
	dY2 /= d2;

	double uX = dX1 + dX2;
	double uY = dY1 + dY2;
	return osg::Vec3d(uX, uY,0);
}

osg::Vec3d ByyPlotUtils::getCubicValue( double t,const osg::Vec3d& startPnt,const osg::Vec3d& cPnt1,const osg::Vec3d& cPnt2,const osg::Vec3d& endPnt )
{
	t = max(min(t, 1), 0);
	double tp = 1 - t;
	double t2 = t * t;
	double t3 = t2 * t;
	double tp2 = tp * tp;
	double tp3 = tp2 * tp;
	double x = (tp3*startPnt[0]) + (3*tp2*t*cPnt1[0]) + (3*tp*t2*cPnt2[0]) + (t3*endPnt[0]);
	double y = (tp3*startPnt[1]) + (3*tp2*t*cPnt1[1]) + (3*tp*t2*cPnt2[1]) + (t3*endPnt[1]);
	return osg::Vec3d(x, y,0);
}

ByyVec3dArray getArrowHeadPoints1(const ByyVec3dArray& points, const osg::Vec3d& tailLeft, const osg::Vec3d& tailRight)
{
	double headHeightFactor = 0.18;
	double headWidthFactor = 0.3;
	double neckHeightFactor = 0.85;
	double neckWidthFactor = 0.15;
	double headTailFactor = 0.8;

	double len = ByyPlotUtils::getBaseLength(points);
	double headHeight = len *headHeightFactor;
	osg::Vec3d headPnt = points[points.size() - 1];
	len = ByyPlotUtils::distance(headPnt, points[points.size() - 2]);
	double tailWidth =ByyPlotUtils::distance(tailLeft, tailRight);
	if (headHeight > tailWidth * headTailFactor) 
	{
		headHeight = tailWidth * headTailFactor;
	}
	double headWidth = headHeight * headWidthFactor;
	double neckWidth = headHeight * neckWidthFactor;
	headHeight = headHeight > len ? len : headHeight;
	double neckHeight = headHeight * neckHeightFactor;

	ByyVec3dArray result(5);

	result[2]=headPnt;

	osg::Vec3d headEndPnt = ByyPlotUtils::getThirdPoint(points[points.size() - 2], headPnt, 0, headHeight, true);
	osg::Vec3d neckEndPnt = ByyPlotUtils::getThirdPoint(points[points.size() - 2], headPnt, 0, neckHeight, true);
	result[1] = ByyPlotUtils::getThirdPoint(headPnt, headEndPnt, HALF_PI, headWidth, false);
	result[3] = ByyPlotUtils::getThirdPoint(headPnt, headEndPnt, HALF_PI, headWidth, true);
	result[0] = ByyPlotUtils::getThirdPoint(headPnt, neckEndPnt, HALF_PI, neckWidth, false);
	result[4] = ByyPlotUtils::getThirdPoint(headPnt, neckEndPnt, HALF_PI, neckWidth, true);
	

	return result;
};

ByyVec3dArray ByyPlotUtils::generateAttackArrow( const ByyVec3dArray& points )
{
	if (points.size() < 2)
	{
		return ByyVec3dArray();
	}

	if (points.size() == 2) 
	{
		return points;
	}

	ByyVec3dArray pnts = points;

	// 计算箭尾
	osg::Vec3d tailLeft = pnts[0];
	osg::Vec3d tailRight = pnts[1];

	if (ByyPlotUtils::isClockWise(pnts[0], pnts[1], pnts[2])) 
	{
		tailLeft = pnts[1];
		tailRight = pnts[0];
	}
	osg::Vec3d midTail = ByyPlotUtils::mid(tailLeft, tailRight);
	ByyVec3dArray bonePnts;
	bonePnts.append(midTail);

	bonePnts+=pnts.mid(2);

	// 计算箭头
	ByyVec3dArray headPnts = getArrowHeadPoints1(bonePnts, tailLeft, tailRight);
	osg::Vec3d neckLeft = headPnts[0];
	osg::Vec3d neckRight = headPnts[4];
	double tailWidthFactor = ByyPlotUtils::distance(tailLeft, tailRight) / ByyPlotUtils::getBaseLength(bonePnts);
	// 计算箭身
	ByyVec3dArray bodyPnts = getArrowBodyPoints(bonePnts, neckLeft, neckRight, tailWidthFactor);
	// 整合
	int count = bodyPnts.size();
	ByyVec3dArray leftPnts;
	leftPnts.push_back(tailLeft);

	leftPnts+=bodyPnts.mid(0, count / 2.0);

	leftPnts.append(neckLeft);

	ByyVec3dArray rightPnts;
	rightPnts.append(tailRight);

	rightPnts+=bodyPnts.mid(count / 2.0);

	rightPnts.append(neckRight);

	leftPnts = ByyPlotUtils::getQBSplinePoints(leftPnts);
	rightPnts = ByyPlotUtils::getQBSplinePoints(rightPnts);

	std::reverse(rightPnts.begin(),rightPnts.end());

	return leftPnts+headPnts+rightPnts;
}

ByyVec3dArray ByyPlotUtils::getQBSplinePoints( const ByyVec3dArray& points )
{
	if (points.size() <= 2 )
		return points;

	int n = 2;

	ByyVec3dArray bSplinePoints;

	int m=points.size() - n - 1;

	bSplinePoints.append(points[0]);

	for (int i=0; i <= m; i++)
	{
		for (double t=0; t <= 1; t+=0.05)
		{
			double x=0,y=0;
			for (int k=0; k <= n; k++){
				double factor=ByyPlotUtils::getQuadricBSplineFactor(k, t);
				x+=factor * points[i + k][0];
				y+=factor * points[i + k][1];
			}
			bSplinePoints.append(osg::Vec3d(x, y,0));
		}
	}
	bSplinePoints.append(points[points.size() - 1]);
	return bSplinePoints;
}

double ByyPlotUtils::getQuadricBSplineFactor( int k,double t )
{
	if (k == 0)
		return pow(t - 1, 2) / 2.0;
	if (k == 1)
		return (-2 * pow(t, 2) + 2 * t + 1) / 2.0;
	if (k == 2)
		return pow(t, 2) / 2.0;
	return 0;
}

ByyVec3dArray ByyPlotUtils::generateStraightArrow( const ByyVec3dArray& points )
{
	if(points.size()<2)
	{
		return points;
	}

	double maxArrowLength = 3000000;
	double arrowLengthScale = 5;

	ByyVec3dArray pnts = points;

	const osg::Vec3d& pnt1 = pnts[0];
	const osg::Vec3d& pnt2 = pnts[1];
	double distance = ByyPlotUtils::distance(pnt1, pnt2);

	double len = distance / arrowLengthScale;

	len = len > maxArrowLength ? maxArrowLength : len;
	osg::Vec3d leftPnt = ByyPlotUtils::getThirdPoint(pnt1, pnt2, M_PI/15, len, false);
	osg::Vec3d rightPnt = ByyPlotUtils::getThirdPoint(pnt1, pnt2, M_PI/15, len, true);

	ByyVec3dArray result(5);

	result[0]=pnt1;
	result[1]=pnt2;
	result[2]=leftPnt;
	result[3]=pnt2;
	result[4]=rightPnt;


	return result;
}
