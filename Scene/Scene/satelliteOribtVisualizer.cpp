#include "StdAfx.h"
#include "satelliteOribtVisualizer.h"
#include "ByySatelliteSR.h"
#include "entityObject.h"
#include "Ars/ArsVec3.h"
#include "Ars/ArsMatrix3.h"
#include "entitySettings.h"
#include "utils.h"
#include "ByyIG.h"
#include "ByyHdose.h"
#include "scenarioManager.h"
#include "scenarioV2.h"
#include "ByyCore.h"
#include "localObjectManager.h"
#include "entityObject.h"
#include "localEntity.h"
#include <QList>

bool sv_from_coe( const double coe[] ,ArsBase::Vec3& r,ArsBase::Vec3& v);
double LST(int y,int m,int d,double uth,double utm,double uts,double ELd,double ELm,double ELs);

ByySatelliteOribtVisualizer::ByySatelliteOribtVisualizer( ByyEntityObject& entObj,ByyIG& ig ) 
	: ByyStateVisualizer(entObj,ig)
{
	ByyScenarioV2 *scn=myIG.hdose().scenarioManager().scenario();
	ByyLocalEntity* satobj = (ByyLocalEntity*)myIG.core().localObjectManager().findEntity(entObj.name());
	if( !satobj )
		return;
	ByyParameter* noworbit=satobj->getParameters().findParameter("Orbit");
	if( !noworbit )
		return;
	QStringList list = noworbit->getValue().split(',');
	double a =  list.at(0).toDouble();
	double e = list.at(1).toDouble();
	double ra = osg::DegreesToRadians( list.at(2).toDouble());
	double incl = osg::DegreesToRadians( list.at(3).toDouble());
	double w = osg::DegreesToRadians( list.at(4).toDouble());
	double TA = osg::DegreesToRadians( list.at(5).toDouble());

	ByySatelliteSR *ssr=dynamic_cast<ByySatelliteSR*>(myEntObj.getExtendEsr(ByySatelliteSR::theType()));

	if (ssr)
	{
		osg::ref_ptr<osg::Geode> pGeode = new osg::Geode;

		pGeometry0 = new osg::Geometry;

		pGeometry0->setUseDisplayList(false);
		pGeometry0->setUseVertexBufferObjects(true);

		pGeometry0->getOrCreateStateSet()->setMode( GL_LIGHTING,	osg::StateAttribute::OFF);
		pGeometry0->getOrCreateStateSet()->setMode( GL_DEPTH_TEST,	osg::StateAttribute::ON );
		pGeometry0->getOrCreateStateSet()->setMode( GL_BLEND,		osg::StateAttribute::ON );
		pGeometry0->getOrCreateStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

		if(ssr)
		{
			ByySatelliteSR::ByyOrbitData oribt=ssr->orbitData();

			a = oribt.m_a;
			e = oribt.m_e;
			ra = osg::DegreesToRadians(oribt.m_RA);
			incl = osg::DegreesToRadians(oribt.m_Incl);
			w = osg::DegreesToRadians(oribt.m_w);
		}


		// 取样数量
		const int N = 400; 
		osg::Vec3Array  * vertices = new osg::Vec3Array( N + 1 );

		for( int i = 0; i <= N; i ++ )
		{
			double ta = osg::PI * 2.0  * (double)i/(double)N ;

			ArsBase::Vec3 r, v;
			double coe[6] = { a, e, ra,  incl,  w, ta } ;			

			sv_from_coe( coe, r, v );

			// sv_from_coe 计算出来的坐标单位为公里，故乘以1000，以转化为米
			double cx = r.x* 1000.0;
			double cy = r.y * 1000.0;
			double cz = r.z * 1000.0;

			(*vertices)[ i ] = osg::Vec3f( cx, cy, cz );
		}

		double alpha = 0.9 ;
		colors0 = new osg::Vec4Array( 1 );
		(*colors0)[0]=forceTypeColorAlpha(myEntObj.forceType(),alpha);/*osg::Vec4f( 0.0, 0.9, 0.0, alpha );*/
		pGeometry0->setColorArray( colors0 );
		pGeometry0->setColorBinding( osg::Geometry::BIND_OVERALL );
		pGeometry0->setVertexArray( vertices );
		pGeometry0->addPrimitiveSet( new osg::DrawArrays( GL_LINE_STRIP, 0, N+1 ) );

		// 将几何对象加入到几何节点				
		pGeode->addDrawable( pGeometry0 );

		osg::ref_ptr<osg::MatrixTransform> mtGrp = new osg::MatrixTransform;
		mtGrp->addChild( pGeode.get() );

		ByyScenarioV2::ScenarioInfo scnInfo;
		scnInfo= scn->getScnInfo();

		QDateTime dateTime = scnInfo.myStartDateTime; //QDateTime::currentDateTime();
		QDate date = dateTime.date();
		QTime time = dateTime.time();

		int year = date.year();
		int month = date.month();
		int day = date.day();
		double hour = time.hour();
		double minute = time.minute();
		double second = time.second();

		double gst = LST( year, month, day, hour, minute, second, 0, 0, 0 );
		gst = osg::DegreesToRadians( gst );

		double ptr[ 16 ];
		ptr[ 0  ] =  cos( gst ),	ptr[ 1  ] = -sin( gst ),		ptr[  2 ] = 0,		ptr[ 3 ] = 0;
		ptr[ 4  ] =  sin( gst ),	ptr[ 5  ] = cos( gst ),		ptr[  6 ] = 0,		ptr[ 7 ] = 0;
		ptr[ 8  ] =  0,				ptr[ 9  ] = 0,				ptr[ 10 ] = 1,		ptr[ 11] = 0;
		ptr[ 12 ] =  0,				ptr[ 13 ] = 0,				ptr[ 14 ] = 0,		ptr[ 15] = 1;

		osg::Matrix mat;
		mat.set( ptr );

		mtGrp->setMatrix(mat);

		root()->addChild(mtGrp);
	}

	setVisible(ByyEntitySettings::instance().satelliteOribtVisible());

	connect(&ByyEntitySettings::instance(),SIGNAL(satelliteOribtVisibleChanged(bool)),this,SLOT(setVisible(bool)));
	connect(&myEntObj,SIGNAL(forceTypeChanged(ByyForceType)),this,SLOT(updateForceType()));

}

ByySatelliteOribtVisualizer::~ByySatelliteOribtVisualizer()
{

}

const QString& ByySatelliteOribtVisualizer::theType()
{
	static QString _type("SatelliteOribt");
	return _type;
}

const QString& ByySatelliteOribtVisualizer::type()
{
	return theType();
}

void ByySatelliteOribtVisualizer::updateForceType()
{
	colors0 = new osg::Vec4Array( 1 ); //xks
	(*colors0)[0]=forceTypeColorAlpha(myEntObj.forceType(),0.9);
	colors0->dirty();
	pGeometry0 = new osg::Geometry; //xks
	pGeometry0->dirtyBound();
}

bool sv_from_coe( const double coe[] ,ArsBase::Vec3& r,ArsBase::Vec3& v)
{
	// 	% ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// 		function [r, v] = sv_from_coe(coe)
	// 		% ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// 		% This function computes the state vector (r,v) from the
	// 		% classical orbital elements (coe).
	// 		%
	// 		% mu - gravitational parameter (km?3; s?2)
	// 		% coe - orbital elements [h e RA incl w TA]
	// 	% where
	// 		% h= angular momentum (km^2/s)，Note: 现在由长半轴a替换
	// 		% e = eccentricity
	// 		% RA = right ascension of the ascending node (rad)
	// 		% incl = inclination of the orbit (rad)
	// 		% w = argument of perigee (rad)
	// 		% TA = true anomaly (rad)
	// 		% R3_w - Rotation matrix about the z-axis through the angle w
	// 		% R1_i - Rotation matrix about the x-axis through the angle i
	// 		% R3_W - Rotation matrix about the z-axis through the angle RA
	// 		% Q_pX - Matrix of the transformation from perifocal to
	// 		% geocentric equatorial frame
	// 		% rp - position vector in the perifocal frame (km)
	// 		% vp - velocity vector in the perifocal frame (km/s)
	// 		% r - position vector in the geocentric equatorial frame
	// 		% (km)
	// 		% v - velocity vector in the geocentric equatorial frame
	// 		% (km/s)
	// 		%
	// 		% User M-functions required: none
	// 		% ------------------------------------------------------------
	double mu;
	mu=398600;
	double h=sqrt(coe[0]*mu*(1-pow(coe[1],2)));
	double e = coe[1];
	double RA = coe[2];
	double incl = coe[3];
	double w = coe[4];
	double TA = coe[5];


	ArsBase::Vec3 tempV1(1,0,0);
	ArsBase::Vec3 cTA=cos(TA)*tempV1;
	ArsBase::Vec3 tempV2(0,1,0);
	ArsBase::Vec3 sTA=sin(TA)*tempV2;
	// %...Equations 4.37 and 4.38 (rp and vp are column vectors):
	ArsBase::Vec3 rp = (pow(h,2)/mu) * (1/(1 + e*cos(TA))) * (cTA + sTA);
	sTA=-sin(TA)*tempV1;
	cTA=(e + cos(TA))*tempV2;
	ArsBase::Vec3 vp = (mu/h) * (sTA + cTA);

	// %...Equation 4.39:
	ArsBase::Matrix3 R3_W (	cos(RA) ,sin(RA), 0.0,
		-sin(RA), cos(RA), 0.0,
		0,		0,			1.0);
	// %...Equation 4.40:
	ArsBase::Matrix3 R1_i (1,   0 ,		  0,
		0, cos(incl), sin(incl),
		0, -sin(incl), cos(incl));
	// %...Equation 4.41:
	ArsBase::Matrix3 R3_w (cos(w), sin(w) ,0,
		-sin(w), cos(w), 0,
		0 ,		0 ,		1);

	// %...Equation 4.44:
	ArsBase::Matrix3 Tr_R3_W=R3_W.Transpose();
	ArsBase::Matrix3 Tr_R1_i=R1_i.Transpose();
	ArsBase::Matrix3 Tr_R3_w=R3_w.Transpose();
	ArsBase::Matrix3 Q_pX = Tr_R3_W*Tr_R1_i*Tr_R3_w;

	// %...Equations 4.46 (r and v are column vectors):
	r = Q_pX*rp;
	v = Q_pX*vp;

	return true;
}

double ClampZeroTo360(double x)
{
	// % This function reduces an angle to the range
	// 	% 0 - 360 degrees.
	// 	%
	// 	% x - The angle (degrees) to be reduced
	// 	% y - The reduced value
	// 	%
	// 	% ------------------------------------------------------------
	if (x >= 360)
		x = x - (int)(x/360)*360;
	else if (x < 0)
		x = x - ((int)(x/360) - 1)*360;
	return x;
}

bool J0(int year, int month, int day,double &j0)
{
	// % ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// 	% ...
	// 	% This function computes the Julian day number at 0 UT for any
	// 	% year between 1900 and 2100 using Equation 5.48.
	// 	%
	// 	% j0 - Julian day at 0 hr UT (Universal Time)
	// 	% year - range: 1901 - 2099
	// 	% month - range: 1 - 12
	// 	% day - range: 1 - 31
	// 	%
	// 	% User M-functions required: none
	// 	% ------------------------------------------------------------
	///判断输入的年月日是否正确；若不正确则返回错；

	j0 = 367*year - (int)(7*(year + (int)((month + 9)/12))/4) + (int)(275*month/9) + day + 1721013.5;
	return true;
}

// 地球惯性坐标系与地心坐标系之夹角
double LST(int y,int m,int d,double uth,double utm,double uts,double ELd,double ELm,double ELs)
{
	// % This function calculates the local sidereal time.
	// 
	// % output:lst - local sidereal time (degrees)
	//	% y - year - range: 1901 - 2099
	//	% m - month - range: 1 - 12
	//	% d - day range: 1 - 31
	//	% ut - Universal Time (hours) (0 - 23)
	//	% utm - Universal Time (minutes) (0 - 60)
	//	% uts - Universal Time (seconds) (0 - 60)
	// % EL - east longitude of the site (west longitude is
	//	% negative):
	//	% ELd - east longitude (degrees) (0 - 360)
	//	% ELm - east longitude (minutes) (0 - 60)
	//	% ELs - east longitude (seconds) (0 - 60)
	//	% j0 - Julian day number at 0 hr UT
	//	% j - number of centuries since J2000
	//	% g0 - Greenwich sidereal time (degrees) at 0 hr UT
	//	% gst - Greenwich sidereal time (degrees) at the specified UT
	// % User unction required: J0, ClampZeroTo360
	// % ------------------------------------------------------------
	//	% --by xks------
	//	% East longitude:
	double degrees = ELd;
	double minutes = ELm;
	double seconds = ELs;
	// % Universal time:
	double hour = uth;
	double minute = utm;
	double second = uts;
	//%...Convert negative (west) longitude to east longitude:
	if (degrees < 0)
		degrees = degrees + 360;
	// %...Express the longitudes as decimal numbers:
	double EL = degrees + minutes/60 + seconds/3600;
	// %WL = 360 - EL;
	// %...Express universal time as a decimal number:
	double ut = hour + minute/60 + second/3600;
	// % --by xks-----

	double j0;
	// %...Equation 5.48;
	J0(y, m, d,j0);
	// %...Equation 5.49:
	double j = (j0 - 2451545)/36525;
	// %...Equation 5.50:
	double g0 = 100.4606184 + 36000.77004*j + 0.000387933*pow(j,2) - 2.583e-8*pow(j,3);

	// %...Reduce g0 so it lies in the range 0 - 360 degrees
	double ng0 = ClampZeroTo360(g0);
	// %...Equation 5.51:
	double gst = ng0 + 360.98564724*ut/24;
	// %...Equation 5.52:
	double lst = gst + EL;
	// %...Reduce lst to the range 0 - 360 degrees:
	lst = lst - 360*((int)(lst/360));

	return lst;
}