/************************************************************************
 * File: aipsdistancefunctions.h                                        *
 * Project: AIPS                                                        *
 * Description: Declaration of common distance functions                *
 *                                                                      *
 * Author: Hendrik Belitz (hbelitz@users.berlios.de)                          *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status: Alpha                                                        *
 * Created: 2005-11-18                                                  *
 * Changed:                                                             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef AIPSDISTANCEFUNCTIONS_H
#define AIPSDISTANCEFUNCTIONS_H

namespace aips {

/**
 * \defgroup distancefunctions Distance function policy and implementations
 * This group contains several distance functions following the distance function policy
 * This policy describes a group of classes defining distance functions.
 * The structure of the policy interface to be followed is
 * \code
 * class CDistanceFunction
 * {
 * public:
 *   static appropiateDistanceType distance( const actualDataType& a, const actualDataType& b );
 *   static appropiateDistanceType distanceFromZero( const actualDataType& a );
 * }
 * \endcode
 */

/**
 * \ingroup distancefunctions
 * \brief Base template for a variety of default distance functions. 
 */
template <typename T>
struct SDefaultDistFunc
{
	/** Returns the distance between two given values. In this case, the distance is equal to
	 *  the absolute difference of the two values.
	 *  \param a first value
	 *  \param b second value
	 *  \returns the distance between the two values
	 */
	static T distance( const T& a, const T& b )
	{
		return ( std::abs( a - b ) );
	}
	/** Returns the distance of the given values to zero. In this case, the distance is equal to
	 *  the absolute value.
	 *  \param a the value
	 *  \returns the absolute value of \a a
	 */
	static T distanceFromZero( const T& a )
	{
		return ( std::abs( a ) );
	}
};

/** 
 * \ingroup distancefunctions
 * \brief Default distance function for discrete 2D points. 
 *
 * Specialised version of SDefaultDistFunc.
 * This distance measure is equal to the chessboard metric 
 * \f$ \max( \mid a_x - b_x \mid, \mid a_y - b_y \mid ) \f$.
 */
template <>
struct SDefaultDistFunc<TPoint2D>
{	
	static TPoint2D::T_numtype distance( const TPoint2D& a, const TPoint2D& b )
	{
		return ( std::max( static_cast<TPoint2D::T_numtype>( std::abs( static_cast<long int>( a[0]-b[0] ) ) ), 
			static_cast<TPoint2D::T_numtype>( std::abs( static_cast<long int>( a[1]-b[1] ) ) ) ) );
	}
	static TPoint2D::T_numtype distanceFromZero( const TPoint2D& a )
	{
		return ( std::max( static_cast<TPoint2D::T_numtype>( std::abs( static_cast<long int>( a[0] ) ) ), 
			static_cast<TPoint2D::T_numtype>( std::abs( static_cast<long int>( a[1] ) ) ) ) );
	}
};

/** 
 * \ingroup distancefunctions
 * \brief Default distance function for discrete 3D points. 
 *
 * Specialised version of SDefaultDistFunc.
 * This distance measure is equal to the chessboard metric 
 * \f$ \max( \mid a_x - b_x \mid, \mid a_y - b_y \mid, \mid a_z - b_z \mid ) \f$.
 */
template <>
struct SDefaultDistFunc<TPoint3D>
{	
	static TPoint3D::T_numtype distance( const TPoint3D& a, const TPoint3D& b )
	{
		return ( std::max( std::max( static_cast<TPoint2D::T_numtype>( std::abs( static_cast<long int>( a[0]-b[0] ) ) ), 
			static_cast<TPoint2D::T_numtype>( std::abs( static_cast<long int>( a[1]-b[1] ) ) ) ), 
			static_cast<TPoint2D::T_numtype>( std::abs( static_cast<long int>( a[2]-b[2] ) ) ) ) );
	}
	static TPoint3D::T_numtype distanceFromZero( const TPoint3D& a )
	{
		return ( std::max( std::max( static_cast<TPoint2D::T_numtype>( std::abs( static_cast<long int>( a[0] ) ) ),
			static_cast<TPoint2D::T_numtype>( std::abs( static_cast<long int>( a[1] ) ) ) ), 
			static_cast<TPoint2D::T_numtype>( std::abs( static_cast<long int>( a[2] ) ) ) ) );
	}
};

/** 
 * \ingroup distancefunctions
 * \brief Default distance function for 2D vectors.
 *
 * Specialised version of SDefaultDistFunc.
 * This distance measure is equal to the norm of the difference vector 
 * \f$ \mid\mid \mathbf{a} - \mathbf{b} \mid\mid \f$.
 */
template <>
struct SDefaultDistFunc<TVector2D>
{	
	static TVector2D::T_numtype distance( const TVector2D& a, const TVector2D& b )
	{
		return ( norm( a - b )  );
	}
	static TVector2D::T_numtype distanceFromZero( const TVector2D& a )
	{
		return ( norm( a )  );
	}
};

/** 
 * \ingroup distancefunctions
 * \brief Default distance function for 3D vectors.
 *
 * Specialised version of SDefaultDistFunc.
 * This distance measure is equal to the norm of the difference vector 
 * \f$ \mid\mid \mathbf{a} - \mathbf{b} \mid\mid \f$.
 */
template <>
struct SDefaultDistFunc<TVector3D>
{	
	static TVector3D::T_numtype distance( const TVector3D& a, const TVector3D& b )
	{
		return ( norm( a - b )  );
	}
	static TVector3D::T_numtype distanceFromZero( const TVector3D& a )
	{
		return ( norm( a )  );
	}
};

/** 
 * \ingroup distancefunctions
 * \brief Default distance function for complex numbers.
 *
 * Specialised version of SDefaultDistFunc.
 * This distance measure is equal to the modulus \f$ \mid a - b \mid \f$.
 */
template <>
struct SDefaultDistFunc<std::complex<TFloatType> >
{	
	static TFloatType distance( const std::complex<TFloatType>& a, const std::complex<TFloatType>& b )
	{
		return ( sqrt( norm( a - b ) ) );
	}
	static TFloatType distanceFromZero( const std::complex<TFloatType>& a )
	{
		return ( sqrt( norm( a ) ) );
	}
};

}

#endif
