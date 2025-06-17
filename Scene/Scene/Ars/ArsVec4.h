
#include "ArsBase.h"
#include "ArsVec3.h"
#include "ArsMath.h"

#ifndef __Vector4_H__
#define __Vector4_H__

namespace ArsBase
{
	//--------------------------------------------------------------------------------
	// 作者：皮学贤 
	// 日期：2010-05-25 
	// 功能：标准四维矢量
	//================================================================================
    class  ARS_BASE_API Vec4
    {
    public:
        Real x, y, z, w;

    public:
        inline Vec4()
        {
        }

        inline Vec4( const Real fX, const Real fY, const Real fZ, const Real fW )
            : x( fX ), y( fY ), z( fZ ), w( fW)
        {
        }

        inline explicit Vec4( const Real afCoordinate[4] )
            : x( afCoordinate[0] ),
              y( afCoordinate[1] ),
              z( afCoordinate[2] ),
              w( afCoordinate[3] )
        {
        }

        inline explicit Vec4( const int afCoordinate[4] )
        {
            x = (Real)afCoordinate[0];
            y = (Real)afCoordinate[1];
            z = (Real)afCoordinate[2];
            w = (Real)afCoordinate[3];
        }

        inline explicit Vec4( Real* const r )
            : x( r[0] ), y( r[1] ), z( r[2] ), w( r[3] )
        {
        }

        inline explicit Vec4( const Real scaler )
            : x( scaler )
            , y( scaler )
            , z( scaler )
            , w( scaler )
        {
        }

        inline explicit Vec4(const Vec3& rhs)
            : x(rhs.x), y(rhs.y), z(rhs.z), w(1.0f)
        {
        }

		inline Real operator [] ( const size_t i ) const
        {
            assert( i < 4 );

            return *(&x+i);
        }

		inline Real& operator [] ( const size_t i )
        {
            assert( i < 4 );

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
        inline Vec4& operator = ( const Vec4& rkVector )
        {
            x = rkVector.x;
            y = rkVector.y;
            z = rkVector.z;
            w = rkVector.w;

            return *this;
        }

		inline Vec4& operator = ( const Real fScalar)
		{
			x = fScalar;
			y = fScalar;
			z = fScalar;
			w = fScalar;
			return *this;
		}

        inline bool operator == ( const Vec4& rkVector ) const
        {
            return ( x == rkVector.x &&
                y == rkVector.y &&
                z == rkVector.z &&
                w == rkVector.w );
        }

        inline bool operator != ( const Vec4& rkVector ) const
        {
            return ( x != rkVector.x ||
                y != rkVector.y ||
                z != rkVector.z ||
                w != rkVector.w );
        }

        inline Vec4& operator = (const Vec3& rhs)
        {
            x = rhs.x;
            y = rhs.y;
            z = rhs.z;
            w = 1.0f;
            return *this;
        }

        // arithmetic operations
        inline Vec4 operator + ( const Vec4& rkVector ) const
        {
            return Vec4(
                x + rkVector.x,
                y + rkVector.y,
                z + rkVector.z,
                w + rkVector.w);
        }

        inline Vec4 operator - ( const Vec4& rkVector ) const
        {
            return Vec4(
                x - rkVector.x,
                y - rkVector.y,
                z - rkVector.z,
                w - rkVector.w);
        }

        inline Vec4 operator * ( const Real fScalar ) const
        {
            return Vec4(
                x * fScalar,
                y * fScalar,
                z * fScalar,
                w * fScalar);
        }

        inline Vec4 operator * ( const Vec4& rhs) const
        {
            return Vec4(
                rhs.x * x,
                rhs.y * y,
                rhs.z * z,
                rhs.w * w);
        }

        inline Vec4 operator / ( const Real fScalar ) const
        {
            assert( fScalar != 0.0 );

            Real fInv = 1.0 / fScalar;

            return Vec4(
                x * fInv,
                y * fInv,
                z * fInv,
                w * fInv);
        }

        inline Vec4 operator / ( const Vec4& rhs) const
        {
            return Vec4(
                x / rhs.x,
                y / rhs.y,
                z / rhs.z,
                w / rhs.w);
        }

        inline const Vec4& operator + () const
        {
            return *this;
        }

        inline Vec4 operator - () const
        {
            return Vec4(-x, -y, -z, -w);
        }

        inline friend Vec4 operator * ( const Real fScalar, const Vec4& rkVector )
        {
            return Vec4(
                fScalar * rkVector.x,
                fScalar * rkVector.y,
                fScalar * rkVector.z,
                fScalar * rkVector.w);
        }

        inline friend Vec4 operator / ( const Real fScalar, const Vec4& rkVector )
        {
            return Vec4(
                fScalar / rkVector.x,
                fScalar / rkVector.y,
                fScalar / rkVector.z,
                fScalar / rkVector.w);
        }

        inline friend Vec4 operator + (const Vec4& lhs, const Real rhs)
        {
            return Vec4(
                lhs.x + rhs,
                lhs.y + rhs,
                lhs.z + rhs,
                lhs.w + rhs);
        }

        inline friend Vec4 operator + (const Real lhs, const Vec4& rhs)
        {
            return Vec4(
                lhs + rhs.x,
                lhs + rhs.y,
                lhs + rhs.z,
                lhs + rhs.w);
        }

        inline friend Vec4 operator - (const Vec4& lhs, Real rhs)
        {
            return Vec4(
                lhs.x - rhs,
                lhs.y - rhs,
                lhs.z - rhs,
                lhs.w - rhs);
        }

        inline friend Vec4 operator - (const Real lhs, const Vec4& rhs)
        {
            return Vec4(
                lhs - rhs.x,
                lhs - rhs.y,
                lhs - rhs.z,
                lhs - rhs.w);
        }

        // arithmetic updates
        inline Vec4& operator += ( const Vec4& rkVector )
        {
            x += rkVector.x;
            y += rkVector.y;
            z += rkVector.z;
            w += rkVector.w;

            return *this;
        }

        inline Vec4& operator -= ( const Vec4& rkVector )
        {
            x -= rkVector.x;
            y -= rkVector.y;
            z -= rkVector.z;
            w -= rkVector.w;

            return *this;
        }

        inline Vec4& operator *= ( const Real fScalar )
        {
            x *= fScalar;
            y *= fScalar;
            z *= fScalar;
            w *= fScalar;
            return *this;
        }

        inline Vec4& operator += ( const Real fScalar )
        {
            x += fScalar;
            y += fScalar;
            z += fScalar;
            w += fScalar;
            return *this;
        }

        inline Vec4& operator -= ( const Real fScalar )
        {
            x -= fScalar;
            y -= fScalar;
            z -= fScalar;
            w -= fScalar;
            return *this;
        }

        inline Vec4& operator *= ( const Vec4& rkVector )
        {
            x *= rkVector.x;
            y *= rkVector.y;
            z *= rkVector.z;
            w *= rkVector.w;

            return *this;
        }

        inline Vec4& operator /= ( const Real fScalar )
        {
            assert( fScalar != 0.0 );

            Real fInv = 1.0 / fScalar;

            x *= fInv;
            y *= fInv;
            z *= fInv;
            w *= fInv;

            return *this;
        }

        inline Vec4& operator /= ( const Vec4& rkVector )
        {
            x /= rkVector.x;
            y /= rkVector.y;
            z /= rkVector.z;
            w /= rkVector.w;

            return *this;
        }

        /** Calculates the dot (scalar) product of this vector with another.
            @param
                vec Vector with which to calculate the dot product (together
                with this one).
            @returns
                A float representing the dot product value.
        */
        inline Real dotProduct(const Vec4& vec) const
        {
            return x * vec.x + y * vec.y + z * vec.z + w * vec.w;
        }

        /*
		* Function for writing to a stream.
        */
        //inline friend std::ostream& operator <<
        //    ( std::ostream& o, const Vec4& v )
        //{
        //    o << "Vec4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
        //    return o;
        //}
        // special

        static const Vec4 ZERO;
    };

}
#endif
