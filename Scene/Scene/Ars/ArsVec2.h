
#include "ArsBase.h"
#include "ArsMath.h"
#include <assert.h>

#ifndef __Vector2_H__
#define __Vector2_H__

namespace ArsBase
{
	//--------------------------------------------------------------------------------
	// 作者：皮学贤 
	// 日期：2010-05-25 
	// 功能：标准二维矢量  
	//================================================================================
    class ARS_BASE_API Vec2
    {
    public:
        Real x, y;

    public:
        inline Vec2()
        {
        }

        inline Vec2(const Real fX, const Real fY )
            : x( fX ), y( fY )
        {
        }

        inline explicit Vec2( const Real scaler )
            : x( scaler), y( scaler )
        {
        }

        inline explicit Vec2( const Real afCoordinate[2] )
            : x( afCoordinate[0] ),
              y( afCoordinate[1] )
        {
        }

        inline explicit Vec2( const int afCoordinate[2] )
        {
            x = (Real)afCoordinate[0];
            y = (Real)afCoordinate[1];
        }

        inline explicit Vec2( Real* const r )
            : x( r[0] ), y( r[1] )
        {
        }

		inline Real operator [] ( const size_t i ) const
        {
            assert( i < 2 );
            return *(&x+i);
        }

		inline Real& operator [] ( const size_t i )
        {
            assert( i < 2 );
            return *(&x+i);
        }

		/// Pointer accessor for direct copying
		inline Real* ptr()
		{
			return &x;
		}

		/// Pointer accessor for direct copying
		inline const Real* ptr() const
		{
			return &x;
		}

        /** Assigns the value of the other vector.
            @param
                rkVector The other vector
        */
        inline Vec2& operator = ( const Vec2& rkVector )
        {
            x = rkVector.x;
            y = rkVector.y;

            return *this;
        }

		inline Vec2& operator = ( const Real fScalar)
		{
			x = fScalar;
			y = fScalar;

			return *this;
		}

        inline bool operator == ( const Vec2& rkVector ) const
        {
            return ( x == rkVector.x && y == rkVector.y );
        }

        inline bool operator != ( const Vec2& rkVector ) const
        {
            return ( x != rkVector.x || y != rkVector.y  );
        }

        // arithmetic operations
        inline Vec2 operator + ( const Vec2& rkVector ) const
        {
            return Vec2(
                x + rkVector.x,
                y + rkVector.y);
        }

        inline Vec2 operator - ( const Vec2& rkVector ) const
        {
            return Vec2(
                x - rkVector.x,
                y - rkVector.y);
        }

        inline Vec2 operator * ( const Real fScalar ) const
        {
            return Vec2(
                x * fScalar,
                y * fScalar);
        }

        inline Vec2 operator * ( const Vec2& rhs) const
        {
            return Vec2(
                x * rhs.x,
                y * rhs.y);
        }

        inline Vec2 operator / ( const Real fScalar ) const
        {
           assert( fScalar != 0.0 );

            Real fInv = 1.0 / fScalar;

            return Vec2(
                x * fInv,
                y * fInv);
        }

        inline Vec2 operator / ( const Vec2& rhs) const
        {
            return Vec2(
                x / rhs.x,
                y / rhs.y);
        }

        inline const Vec2& operator + () const
        {
            return *this;
        }

        inline Vec2 operator - () const
        {
            return Vec2(-x, -y);
        }

        // overloaded operators to help Vec2
        inline friend Vec2 operator * ( const Real fScalar, const Vec2& rkVector )
        {
            return Vec2(
                fScalar * rkVector.x,
                fScalar * rkVector.y);
        }

        inline friend Vec2 operator / ( const Real fScalar, const Vec2& rkVector )
        {
            return Vec2(
                fScalar / rkVector.x,
                fScalar / rkVector.y);
        }

        inline friend Vec2 operator + (const Vec2& lhs, const Real rhs)
        {
            return Vec2(
                lhs.x + rhs,
                lhs.y + rhs);
        }

        inline friend Vec2 operator + (const Real lhs, const Vec2& rhs)
        {
            return Vec2(
                lhs + rhs.x,
                lhs + rhs.y);
        }

        inline friend Vec2 operator - (const Vec2& lhs, const Real rhs)
        {
            return Vec2(
                lhs.x - rhs,
                lhs.y - rhs);
        }

        inline friend Vec2 operator - (const Real lhs, const Vec2& rhs)
        {
            return Vec2(
                lhs - rhs.x,
                lhs - rhs.y);
        }
        // arithmetic updates
        inline Vec2& operator += ( const Vec2& rkVector )
        {
            x += rkVector.x;
            y += rkVector.y;

            return *this;
        }

        inline Vec2& operator += ( const Real fScaler )
        {
            x += fScaler;
            y += fScaler;

            return *this;
        }

        inline Vec2& operator -= ( const Vec2& rkVector )
        {
            x -= rkVector.x;
            y -= rkVector.y;

            return *this;
        }

        inline Vec2& operator -= ( const Real fScaler )
        {
            x -= fScaler;
            y -= fScaler;

            return *this;
        }

        inline Vec2& operator *= ( const Real fScalar )
        {
            x *= fScalar;
            y *= fScalar;

            return *this;
        }

        inline Vec2& operator *= ( const Vec2& rkVector )
        {
            x *= rkVector.x;
            y *= rkVector.y;

            return *this;
        }

        inline Vec2& operator /= ( const Real fScalar )
        {
            assert( fScalar != 0.0 );

            Real fInv = 1.0 / fScalar;

            x *= fInv;
            y *= fInv;

            return *this;
        }

        inline Vec2& operator /= ( const Vec2& rkVector )
        {
            x /= rkVector.x;
            y /= rkVector.y;

            return *this;
        }

        /** Returns the length (magnitude) of the vector.
            @warning
                This operation requires a square root and is expensive in
                terms of CPU operations. If you don't need to know the exact
                length (e.g. for just comparing lengths) use squaredLength()
                instead.
        */
        inline Real length () const
        {
			return 0.0;
            //return Math::Sqrt( x * x + y * y );
        }

        /** Returns the square of the length(magnitude) of the vector.
            @remarks
                This  method is for efficiency - calculating the actual
                length of a vector requires a square root, which is expensive
                in terms of the operations required. This method returns the
                square of the length of the vector, i.e. the same as the
                length but before the square root is taken. Use this if you
                want to find the longest / shortest vector without incurring
                the square root.
        */
        inline Real squaredLength () const
        {
            return x * x + y * y;
        }

        /** Calculates the dot (scalar) product of this vector with another.
            @remarks
                The dot product can be used to calculate the angle between 2
                vectors. If both are unit vectors, the dot product is the
                cosine of the angle; otherwise the dot product must be
                divided by the product of the lengths of both vectors to get
                the cosine of the angle. This result can further be used to
                calculate the distance of a point from a plane.
            @param
                vec Vector with which to calculate the dot product (together
                with this one).
            @returns
                A float representing the dot product value.
        */
        inline Real dotProduct(const Vec2& vec) const
        {
            return x * vec.x + y * vec.y;
        }

        /** Normalises the vector.
            @remarks
                This method normalises the vector such that it's
                length / magnitude is 1. The result is called a unit vector.
            @note
                This function will not crash for zero-sized vectors, but there
                will be no changes made to their components.
            @returns The previous length of the vector.
        */
        inline Real normalise()
        {
            Real fLength = 0; //Math::Sqrt( x * x + y * y);

            // Will also work for zero-sized vectors, but will change nothing
            if ( fLength > 1e-08 )
            {
                Real fInvLength = 1.0 / fLength;
                x *= fInvLength;
                y *= fInvLength;
            }

            return fLength;
        }



        /** Returns a vector at a point half way between this and the passed
            in vector.
        */
        inline Vec2 midPoint( const Vec2& vec ) const
        {
            return Vec2(
                ( x + vec.x ) * 0.5,
                ( y + vec.y ) * 0.5 );
        }

        /** Returns true if the vector's scalar components are all greater
            that the ones of the vector it is compared against.
        */
        inline bool operator < ( const Vec2& rhs ) const
        {
            if( x < rhs.x && y < rhs.y )
                return true;
            return false;
        }

        /** Returns true if the vector's scalar components are all smaller
            that the ones of the vector it is compared against.
        */
        inline bool operator > ( const Vec2& rhs ) const
        {
            if( x > rhs.x && y > rhs.y )
                return true;
            return false;
        }

        /** Sets this vector's components to the minimum of its own and the
            ones of the passed in vector.
            @remarks
                'Minimum' in this case means the combination of the lowest
                value of x, y and z from both vectors. Lowest is taken just
                numerically, not magnitude, so -1 < 0.
        */
        inline void makeFloor( const Vec2& cmp )
        {
            if( cmp.x < x ) x = cmp.x;
            if( cmp.y < y ) y = cmp.y;
        }

        /** Sets this vector's components to the maximum of its own and the
            ones of the passed in vector.
            @remarks
                'Maximum' in this case means the combination of the highest
                value of x, y and z from both vectors. Highest is taken just
                numerically, not magnitude, so 1 > -3.
        */
        inline void makeCeil( const Vec2& cmp )
        {
            if( cmp.x > x ) x = cmp.x;
            if( cmp.y > y ) y = cmp.y;
        }

        /** Generates a vector perpendicular to this vector (eg an 'up' vector).
            @remarks
                This method will return a vector which is perpendicular to this
                vector. There are an infinite number of possibilities but this
                method will guarantee to generate one of them. If you need more
                control you should use the Quaternion class.
        */
        inline Vec2 perpendicular(void) const
        {
            return Vec2 (-y, x);
        }

        /** Calculates the 2 dimensional cross-product of 2 vectors, which results
			in a single floating point value which is 2 times the area of the triangle.
        */
        inline Real crossProduct( const Vec2& rkVector ) const
        {
            return x * rkVector.y - y * rkVector.x;
        }

        /** Generates a new random vector which deviates from this vector by a
            given angle in a random direction.
            @remarks
                This method assumes that the random number generator has already
                been seeded appropriately.
            @param
                angle The angle at which to deviate in radians
            @param
                up Any vector perpendicular to this one (which could generated
                by cross-product of this vector and any other non-colinear
                vector). If you choose not to provide this the function will
                derive one on it's own, however if you provide one yourself the
                function will be faster (this allows you to reuse up vectors if
                you call this method more than once)
            @returns
                A random vector which deviates from this vector by angle. This
                vector will not be normalised, normalise it if you wish
                afterwards.
        */
        inline Vec2 randomDeviant(
            Real angle) const
        {

            //angle *=  Math::UnitRandom() * Math::TWO_PI;
            Real cosa = cos(angle);
            Real sina = sin(angle);
            return  Vec2(cosa * x - sina * y,
                            sina * x + cosa * y);
        }

        /** Returns true if this vector is zero length. */
        inline bool isZeroLength(void) const
        {
            Real sqlen = (x * x) + (y * y);
            return (sqlen < (1e-06 * 1e-06));

        }

        /** As normalise, except that this vector is unaffected and the
            normalised vector is returned as a copy. */
        inline Vec2 normalisedCopy(void) const
        {
            Vec2 ret = *this;
            ret.normalise();
            return ret;
        }

        /** Calculates a reflection vector to the plane with the given normal .
        @remarks NB assumes 'this' is pointing AWAY FROM the plane, invert if it is not.
        */
        inline Vec2 reflect(const Vec2& normal) const
        {
            return Vec2( *this - ( 2 * this->dotProduct(normal) * normal ) );
        }

        // special points
        static const Vec2 ZERO;
        static const Vec2 UNIT_X;
        static const Vec2 UNIT_Y;
        static const Vec2 NEGATIVE_UNIT_X;
        static const Vec2 NEGATIVE_UNIT_Y;
        static const Vec2 UNIT_SCALE;

        // Function for writing to a stream.
        //inline __Export friend std::ostream& operator <<
        //    ( std::ostream& o, const Vec2& v )
        //{
        //    o << "Vec2(" << v.x << ", " << v.y <<  ")";
        //    return o;
        //}
    };
}
#endif
