
#ifndef __Math_H__
#define __Math_H__

#include "ArsBase.h"
#include <xutility>
#include <limits>
#include <map>
#include <set>
#include <list>
#include <assert.h>

//--------------------------------------------------------------------------------
// 作者  皮学贤 
// 日期：2010-06-07 
// 功能：常用数学对象及数学函数(改自OpenGRE)  
//================================================================================

using namespace std;
namespace ArsBase
{
	//前置声明
	class Degree;
	class Angle;
	class Radian;
	class Vec2 ;
	class Vec3 ;
	class Vec4 ;

	/*
	* “弧度”---- 可实现与“度”之间的互相切换
	*/
	class Radian
	{
		Real mRad;

	public:
		explicit Radian ( Real r=0 ) : mRad(r) {}
		Radian ( const Degree& d );
		Radian& operator = ( const Real& f ) { mRad = f; return *this; }
		Radian& operator = ( const Radian& r ) { mRad = r.mRad; return *this; }
		Radian& operator = ( const Degree& d );

		Real valueDegrees() const;
		Real valueRadians() const { return mRad; }
		Real valueAngleUnits() const;

		const Radian& operator + () const { return *this; }
		Radian operator + ( const Radian& r ) const { return Radian ( mRad + r.mRad ); }
		Radian operator + ( const Degree& d ) const;
		Radian& operator += ( const Radian& r ) { mRad += r.mRad; return *this; }
		Radian& operator += ( const Degree& d );
		Radian operator - () const { return Radian(-mRad); }
		Radian operator - ( const Radian& r ) const { return Radian ( mRad - r.mRad ); }
		Radian operator - ( const Degree& d ) const;
		Radian& operator -= ( const Radian& r ) { mRad -= r.mRad; return *this; }
		Radian& operator -= ( const Degree& d );
		Radian operator * ( Real f ) const { return Radian ( mRad * f ); }
		Radian operator * ( const Radian& f ) const { return Radian ( mRad * f.mRad ); }
		Radian& operator *= ( Real f ) { mRad *= f; return *this; }
		Radian operator / ( Real f ) const { return Radian ( mRad / f ); }
		Radian& operator /= ( Real f ) { mRad /= f; return *this; }

		bool operator <  ( const Radian& r ) const { return mRad <  r.mRad; }
		bool operator <= ( const Radian& r ) const { return mRad <= r.mRad; }
		bool operator == ( const Radian& r ) const { return mRad == r.mRad; }
		bool operator != ( const Radian& r ) const { return mRad != r.mRad; }
		bool operator >= ( const Radian& r ) const { return mRad >= r.mRad; }
		bool operator >  ( const Radian& r ) const { return mRad >  r.mRad; }
	};

	/*
	* “度”---- 可实现与“弧度”之间的互相切换
	*/
	class Degree
	{
		Real mDeg; 

	public:
		explicit Degree ( Real d=0 ) : mDeg(d) {}
		Degree ( const Radian& r ) : mDeg(r.valueDegrees()) {}
		Degree& operator = ( const Real& f ) { mDeg = f; return *this; }
		Degree& operator = ( const Degree& d ) { mDeg = d.mDeg; return *this; }
		Degree& operator = ( const Radian& r ) { mDeg = r.valueDegrees(); return *this; }

		Real valueDegrees() const { return mDeg; }
		Real valueRadians() const; 
		Real valueAngleUnits() const;

		const Degree& operator + () const { return *this; }
		Degree operator + ( const Degree& d ) const { return Degree ( mDeg + d.mDeg ); }
		Degree operator + ( const Radian& r ) const { return Degree ( mDeg + r.valueDegrees() ); }
		Degree& operator += ( const Degree& d ) { mDeg += d.mDeg; return *this; }
		Degree& operator += ( const Radian& r ) { mDeg += r.valueDegrees(); return *this; }
		Degree operator - () const { return Degree(-mDeg); }
		Degree operator - ( const Degree& d ) const { return Degree ( mDeg - d.mDeg ); }
		Degree operator - ( const Radian& r ) const { return Degree ( mDeg - r.valueDegrees() ); }
		Degree& operator -= ( const Degree& d ) { mDeg -= d.mDeg; return *this; }
		Degree& operator -= ( const Radian& r ) { mDeg -= r.valueDegrees(); return *this; }
		Degree operator * ( Real f ) const { return Degree ( mDeg * f ); }
		Degree operator * ( const Degree& f ) const { return Degree ( mDeg * f.mDeg ); }
		Degree& operator *= ( Real f ) { mDeg *= f; return *this; }
		Degree operator / ( Real f ) const { return Degree ( mDeg / f ); }
		Degree& operator /= ( Real f ) { mDeg /= f; return *this; }

		bool operator <  ( const Degree& d ) const { return mDeg <  d.mDeg; }
		bool operator <= ( const Degree& d ) const { return mDeg <= d.mDeg; }
		bool operator == ( const Degree& d ) const { return mDeg == d.mDeg; }
		bool operator != ( const Degree& d ) const { return mDeg != d.mDeg; }
		bool operator >= ( const Degree& d ) const { return mDeg >= d.mDeg; }
		bool operator >  ( const Degree& d ) const { return mDeg >  d.mDeg; }
	};


	//--------------------------------------------------------------------------------
	// 作者  皮学贤 
	// 日期：2010-06-07 
	// 功能：定义“角”  
	//================================================================================

	class Angle
	{
		Real mAngle;

	public:
		explicit Angle ( Real angle ) : mAngle(angle) {}
		operator Radian() const;
		operator Degree() const;
	};

	inline Radian::Radian ( const Degree& d ) : mRad(d.valueRadians()) {
	}
	inline Radian& Radian::operator = ( const Degree& d ) {
		mRad = d.valueRadians(); return *this;
	}
	inline Radian Radian::operator + ( const Degree& d ) const {
		return Radian ( mRad + d.valueRadians() );
	}
	inline Radian& Radian::operator += ( const Degree& d ) {
		mRad += d.valueRadians();
		return *this;
	}
	inline Radian Radian::operator - ( const Degree& d ) const {
		return Radian ( mRad - d.valueRadians() );
	}
	inline Radian& Radian::operator -= ( const Degree& d ) {
		mRad -= d.valueRadians();
		return *this;
	}

	/*
	* Math 获取部分数学功能函数
	*/
	class ARS_BASE_API Math 
	{
	public:
		enum AngleUnit
		{
			AU_DEGREE,
			AU_RADIAN
		};

	protected:
		// API所用的角度单位( angle units used by the API )
		static AngleUnit msAngleUnit;

		// 构造函数所用的查找表表长( Size of the trig tables as determined by constructor.)
		static int mTrigTableSize;

		/// Radian -> index factor value ( mTrigTableSize / 2 * ONE_PI )
		static Real mTrigTableFactor;
		static Real* mSinTable;
		static Real* mTanTable;

		// Private function to build trig tables.
		void buildTrigTables();

		static Real SinTable (Real fValue);
		static Real TanTable (Real fValue);

	public:
		/* 
		*	三角函数表*用于快速计算正弦Sin、余弦Cos、正切Tan等，表的大小可选
		*/
		Math( unsigned int trigTableSize = 4096);

		~Math();

		static int ISign (int iValue);
		static Radian ACos (Real fValue);
		static Radian ASin (Real fValue);
		static inline int IAbs (int iValue) { return ( iValue >= 0 ? iValue : -iValue ); }
		static inline int ICeil (float fValue) { return int(ceil(fValue)); }
		static inline int IFloor (float fValue) { return int(floor(fValue)); }
		static inline Real Abs (Real fValue) { return Real(fabs(fValue)); }
		static inline Degree Abs (const Degree& dValue) { return Degree(fabs(dValue.valueDegrees())); }
		static inline Radian Abs (const Radian& rValue) { return Radian(fabs(rValue.valueRadians())); }
		static inline Radian ATan (Real fValue) { return Radian(atan(fValue)); }
		static inline Radian ATan2 (Real fY, Real fX) { return Radian(atan2(fY,fX)); }
		static inline Real Ceil (Real fValue) { return Real(ceil(fValue)); }

		//计算Cosine，fValue Angle in radians
		static inline Real Cos (const Radian& fValue, bool useTables = false) {
			return (!useTables) ? Real(cos(fValue.valueRadians())) : SinTable(fValue.valueRadians() + HALF_PI);
		}

		//计算Cosine，fValue Angle in radians
		static inline Real Cos (Real fValue, bool useTables = false) {
			return (!useTables) ? Real(cos(fValue)) : SinTable(fValue + HALF_PI);
		}

		static inline Real Exp (Real fValue) { return Real(exp(fValue)); }

		static inline Real Floor (Real fValue) { return Real(floor(fValue)); }

		static inline Real Log (Real fValue) { return Real(log(fValue)); }

		static inline Real Pow (Real fBase, Real fExponent) { return Real(pow(fBase,fExponent)); }

		static Real Sign (Real fValue);

		static inline Radian Sign ( const Radian& rValue )
		{
			return Radian(Sign(rValue.valueRadians()));
		}
		static inline Degree Sign ( const Degree& dValue )
		{
			return Degree(Sign(dValue.valueDegrees()));
		}

		//计算Sine，fValue Angle in radians
		static inline Real Sin (const Radian& fValue, bool useTables = false) {
			return (!useTables) ? Real(sin(fValue.valueRadians())) : SinTable(fValue.valueRadians());
		}

		//计算Sine，fValue Angle in radians
		static inline Real Sin (Real fValue, bool useTables = false) {
			return (!useTables) ? Real(sin(fValue)) : SinTable(fValue);
		}

		static inline Real Sqr (Real fValue) { return fValue*fValue; }

		static inline Real Sqrt (Real fValue) { return Real(sqrt(fValue)); }

		static inline Radian Sqrt (const Radian& fValue) { return Radian(sqrt(fValue.valueRadians())); }

		static inline Degree Sqrt (const Degree& fValue) { return Degree(sqrt(fValue.valueDegrees())); }

		// 计算平方根之倒数 Inverse square root i.e. 1 / Sqrt(x), 用于矢量归一化
		static Real InvSqrt(Real fValue);					

		static Real UnitRandom ();							// in [0,1]

		static Real RangeRandom (Real fLow, Real fHigh);	// in [fLow,fHigh]

		static Real SymmetricRandom ();						// in [-1,1]

		//计算Tangent，fValue Angle in radians
		static inline Real Tan (const Radian& fValue, bool useTables = false) {
			return (!useTables) ? Real(tan(fValue.valueRadians())) : TanTable(fValue.valueRadians());
		}

		//计算Tangent，fValue Angle in radians
		static inline Real Tan (Real fValue, bool useTables = false) {
			return (!useTables) ? Real(tan(fValue)) : TanTable(fValue);
		}

		static inline Real DegToRad(Real degrees) { return degrees * fDeg2Rad; }
		static inline Real RadToDeg(Real radians) { return radians * fRad2Deg; }

		/** These functions used to set the assumed angle units (radians or degrees) 
		expected when using the Angle type.
		@par
		You can set this directly after creating a new Root, and also before/after resource creation,
		depending on whether you want the change to affect resource files.
		*/
		//下列函数使用时，基于角度单位已经设定
		static void setAngleUnit( AngleUnit unit);

		//  Get the unit being used for angles. 
		static AngleUnit getAngleUnit(void);

		//** Convert from the current AngleUnit to radians./
		static Real AngleUnitsToRadians(Real units);

		//** Convert from radians to the current AngleUnit ./
		static Real RadiansToAngleUnits(Real radians);

		//** Convert from the current AngleUnit to degrees. /
		static Real AngleUnitsToDegrees(Real units);

		//** Convert from degrees to the current AngleUnit. /
		static Real DegreesToAngleUnits(Real degrees);

		//判断指定点是否在三角形内部(迪卡尔坐标系)，三角形的顶点必须是逆时针顺序或顺时针
		//若是，返回“真” 
		static bool pointInTri2D(const Vec2& p, const Vec2& a, const Vec2& b, const Vec2& c);

		//判断指定点是否在三角形内部(迪卡尔坐标系)，三角形的顶点必须是逆时针顺序或顺时针
		//若是，返回“真” ( 点必须与三角形仅次于同一平面)
		static bool pointInTri3D(const Vec3& p, const Vec3& a, 
			const Vec3& b, const Vec3& c, const Vec3& normal);

		///** Ray / plane intersection, returns boolean result and distance. */
		//static std::pair<bool, Real> intersects(const Ray& ray, const Plane& plane);

		///** Ray / sphere intersection, returns boolean result and distance. */
		//static std::pair<bool, Real> intersects(const Ray& ray, const Sphere& sphere, 
		//    bool discardInside = true);
		//
		///** Ray / box intersection, returns boolean result and distance. */
		//static std::pair<bool, Real> intersects(const Ray& ray, const AABox& box);

		/** Ray / box intersection, returns boolean result and two intersection distance.
		@param
		ray The ray.
		@param
		box The box.
		@param
		d1 A real pointer to retrieve the near intersection distance
		from the ray origin, maybe <b>null</b> which means don't care
		about the near intersection distance.
		@param
		d2 A real pointer to retrieve the far intersection distance
		from the ray origin, maybe <b>null</b> which means don't care
		about the far intersection distance.
		@returns
		If the ray is intersects the box, <b>true</b> is returned, and
		the near intersection distance is return by <i>d1</i>, the
		far intersection distance is return by <i>d2</i>. Guarantee
		<b>0</b> <= <i>d1</i> <= <i>d2</i>.
		@par
		If the ray isn't intersects the box, <b>false</b> is returned, and
		<i>d1</i> and <i>d2</i> is unmodified.
		*/
		//static bool intersects(const Ray& ray, const AABox & box,
		//    Real* d1, Real* d2);

		/** Ray / triangle intersection, returns boolean result and distance.
		@param
		ray The ray.
		@param
		a The triangle's first vertex.
		@param
		b The triangle's second vertex.
		@param
		c The triangle's third vertex.
		@param 
		normal The triangle plane's normal (passed in rather than calculated
		on demand since the caller may already have it), doesn't need
		normalised since we don't care.
		@param
		positiveSide Intersect with "positive side" of the triangle
		@param
		negativeSide Intersect with "negative side" of the triangle
		@returns
		If the ray is intersects the triangle, a pair of <b>true</b> and the
		distance between intersection point and ray origin returned.
		@par
		If the ray isn't intersects the triangle, a pair of <b>false</b> and
		<b>0</b> returned.
		*/

		////直线与三角形的交点，返回“是否相交”及距离。
		// static std::pair<bool, Real> intersects(const Ray& ray, const Vec3& a,
		//     const Vec3& b, const Vec3& c, const Vec3& normal,
		//     bool positiveSide = true, bool negativeSide = true);

		/** Ray / triangle intersection, returns boolean result and distance.
		@param
		ray The ray.
		@param
		a The triangle's first vertex.
		@param
		b The triangle's second vertex.
		@param
		c The triangle's third vertex.
		@param
		positiveSide Intersect with "positive side" of the triangle
		@param
		negativeSide Intersect with "negative side" of the triangle
		@returns
		If the ray is intersects the triangle, a pair of <b>true</b> and the
		distance between intersection point and ray origin returned.
		@par
		If the ray isn't intersects the triangle, a pair of <b>false</b> and
		<b>0</b> returned.
		*/
		//static std::pair<bool, Real> intersects(const Ray& ray, const Vec3& a,
		//    const Vec3& b, const Vec3& c,
		//    bool positiveSide = true, bool negativeSide = true);

		///** Sphere / box intersection test. */
		//static bool intersects(const Sphere& sphere, const AABox& box);

		///** Plane / box intersection test. */
		//static bool intersects(const Plane& plane, const AABox& box);

		///** Ray / convex plane list intersection test. 
		//@param ray The ray to test with
		//@param plaeList List of planes which form a convex volume
		//@param normalIsOutside Does the normal point outside the volume
		//*/
		//static std::pair<bool, Real> intersects(
		//    const Ray& ray, const std::vector<Plane>& planeList, 
		//    bool normalIsOutside);
		///** Ray / convex plane list intersection test. 
		//@param ray The ray to test with
		//@param plaeList List of planes which form a convex volume
		//@param normalIsOutside Does the normal point outside the volume
		//*/
		//static std::pair<bool, Real> intersects(
		//    const Ray& ray, const std::list<Plane>& planeList, 
		//    bool normalIsOutside);

		///** Sphere / plane intersection test. 
		//@remarks NB just do a plane.getDistance(sphere.getCenter()) for more detail!
		//*/
		//static bool intersects(const Sphere& sphere, const Plane& plane);

		/** Compare 2 reals, using tolerance for inaccuracies.
		*/
		static bool RealEqual(Real a, Real b, Real tolerance = std::numeric_limits<Real>::epsilon());       

		///** Calculates the tangent space vector for a given set of positions / texture coords. */
		//static Vec3 calculateTangentSpaceVector(
		//    const Vec3& position1, const Vec3& position2, const Vec3& position3,
		//    Real u1, Real v1, Real u2, Real v2, Real u3, Real v3);
		///** Build a reflection matrix for the passed in plane. */
		//static Matrix4 buildReflectionMatrix(const Plane& p);
		///** Calculate a face normal, including the w component which is the offset from the origin. */
		//static Vec4 calculateFaceNormal(const Vec3& v1, const Vec3& v2, const Vec3& v3);
		///** Calculate a face normal, no w-information. */
		//static Vec3 calculateBasicFaceNormal(const Vec3& v1, const Vec3& v2, const Vec3& v3);
		///** Calculate a face normal without normalize, including the w component which is the offset from the origin. */
		//static Vec4 calculateFaceNormalWithoutNormalize(const Vec3& v1, const Vec3& v2, const Vec3& v3);
		///** Calculate a face normal without normalize, no w-information. */
		//static Vec3 calculateBasicFaceNormalWithoutNormalize(const Vec3& v1, const Vec3& v2, const Vec3& v3);

		/*
		* Generates a value based on the Gaussian (normal) distribution
		* function with the given offset and scale parameters.
		* 生成基于高斯分布(正态分布)函数的概率值
		*/
		static Real gaussianDistribution( Real x, Real offset = 0.0f, Real scale = 1.0f );

		/** Clamp a value within an inclusive range. */
		template <typename T>
		static T Clamp(T val, T minval, T maxval)
		{
			assert  (minval < maxval && "Invalid clamp range");
			return  max( min(val, maxval), minval );
		}

		//static Matrix4 makeViewMatrix(const Vec3& position, const Quaternion& orientation, const Matrix4* reflectMatrix = 0);

		static const Real POS_INFINITY;
		static const Real NEG_INFINITY;
		static const Real ONE_PI;
		static const Real TWO_PI;
		static const Real HALF_PI;
		static const Real fDeg2Rad;
		static const Real fRad2Deg;
	};

	//下列函数依赖于角度单位转换，所以定义于下
	inline Real Radian::valueDegrees() const
	{
		return Math::RadToDeg ( mRad );
	}

	inline Real Radian::valueAngleUnits() const
	{
		return Math::RadiansToAngleUnits ( mRad );
	}

	inline Real Degree::valueRadians() const
	{
		return Math::DegToRad ( mDeg );
	}

	inline Real Degree::valueAngleUnits() const
	{
		return Math::DegreesToAngleUnits ( mDeg );
	}

	inline Angle::operator Radian() const
	{
		return Radian(Math::AngleUnitsToRadians(mAngle));
	}

	inline Angle::operator Degree() const
	{
		return Degree(Math::AngleUnitsToDegrees(mAngle));
	}

	inline Radian operator * ( Real a, const Radian& b )
	{
		return Radian ( a * b.valueRadians() );
	}

	inline Radian operator / ( Real a, const Radian& b )
	{
		return Radian ( a / b.valueRadians() );
	}

	inline Degree operator * ( Real a, const Degree& b )
	{
		return Degree ( a * b.valueDegrees() );
	}

	inline Degree operator / ( Real a, const Degree& b )
	{
		return Degree ( a / b.valueDegrees() );
	}
}

#endif
