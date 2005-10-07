/************************************************************************
 * File: aipsnumericinlines.cpp                                         *
 * Project: AIPS                                                        *
 * Description: Implementation of aipsnumeric inline functions          *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 2004-01-21                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

/**
 * \param value data to swap (will be overwritten)
 */
template<typename T> inline void swapEndianess( T& value ) throw()
{
  uint8_t* itBegin = static_cast<uint8_t*>( static_cast<void*>( &value ) );
  uint8_t* itEnd = itBegin + sizeof( value );
  std::reverse( itBegin, itEnd );
}

/**
 * Specialization of the function template for TVector2D type
 * \param value data to swap (will be overwritten)
 */
inline void swapEndianess( TVector2D& value ) throw()
{
  uint8_t* itBegin = static_cast<uint8_t*>( static_cast<void*>( &value[0] ) );
  uint8_t* itEnd = itBegin + sizeof( value[0] );
  std::reverse( itBegin, itEnd );
  itBegin = static_cast<uint8_t*>( static_cast<void*>( &value[1] ) );
  itEnd = itBegin + sizeof( value[1] );
  std::reverse( itBegin, itEnd );	
}

/**
 * Specialization of the function template for TVector3D type
 * \param value data to swap (will be overwritten)
 */
inline void swapEndianess( TVector3D& value ) throw()
{
  uint8_t* itBegin = static_cast<uint8_t*>( static_cast<void*>( &value[0] ) );
  uint8_t* itEnd = itBegin + sizeof( value[0] );
  std::reverse( itBegin, itEnd );
  itBegin = static_cast<uint8_t*>( static_cast<void*>( &value[1] ) );
  itEnd = itBegin + sizeof( value[1] );
  std::reverse( itBegin, itEnd );	
  itBegin = static_cast<uint8_t*>( static_cast<void*>( &value[2] ) );
  itEnd = itBegin + sizeof( value[2] );
  std::reverse( itBegin, itEnd );	
}

#ifndef __GLIBCPP__
/* The following stuff is only needed if we don't compile in an GCC-Environment */

/**
 * All types which are comparable to 0L are supported
 * \param value a value
 * \return absolute value
 */
template<typename T> inline T abs( const T value ) throw()
{
  if ( value >= static_cast<T>( 0L ) )
    return value;
  else
    return -value;
}

/** Specialized template of abs<int>. Uses the cmath abs function */
inline int abs( const int value ) throw()
{
  return std::abs( value );
}

/** Specialized template of abs<double>. Uses the cmath fabs function */
inline double abs( const double value ) throw()
{
  return std::fabs( value );
}

/** Specialized template of abs<float>. Uses the cmath fabsf function */
inline float abs( const float value ) throw()
{
  return fabsf( value );
}

/** Specialized template of abs<long>. Uses the cmath labs function */
inline long abs( const long value ) throw()
{
  return std::labs( value );
}
#endif /* #ifndef __GLIBCPP__ */

/** 
 * \param value value to check
 * \param min minimum 
 * \param max maximum 
 * \returns true if min <= value <= max
 */
template<typename T> inline bool in( const T value, const T min, const T max ) throw()
{
	return ( ( value >= min ) && ( value <= max ) );
}

/** 
 * \param value value to check
 * \param min minimum 
 * \param max maximum 
 * \returns true if min < value < max
 */
template<typename T> inline bool trueIn( const T value, const T min, const T max ) throw()
{
	return ( ( value > min ) && ( value < max ) );
}

#ifdef USE_STANDALONE
/**
 * \param value value to be rounded
 * \return rounded value
 */
template<typename T> inline T round( const T value ) throw()
{
  return static_cast<T>( floor( value + 0.5f ) );
}

/**
 * \param aVector vector to compute norm from
 * \return Norm of given vector
 */
template<typename T> inline double norm( const T& aVector )
{
  return sqrt( dot( aVector, aVector ) );
}
#endif /* #ifdef USE_STANDALONE */

/**
 * \param lhs first value to compare
 * \param rhs second value to compare
 * \param epsilon allowed error
 */
inline bool equal( const double lhs, const double rhs, const double epsilon ) throw()
{
	return ( fabs( lhs-rhs ) <= epsilon );
}

/**
 * This is faster than a simple norm function since sqrt() is not used.
 * Prefer this function if you need to compare two vectors,
 * but are not interested in the actual values.
 * \param aVector vector to compute norm from
 * \return Norm^2 of given vector
 */
template<typename T> inline double quadNorm( const T& aVector ) throw()
{
  return dot( aVector, aVector );
}

/**
 * \param leftVector first Vector for product
 * \param rightVector second Vector for product
 */
inline TVector3D vecProd( const TVector3D& leftVector, const TVector3D& rightVector ) throw()
{
  return TVector3D( leftVector[1] * rightVector[2] - leftVector[2] * rightVector[1],
    leftVector[2] * rightVector[0] - leftVector[0] * rightVector[2],
    leftVector[0] * rightVector[1] - leftVector[1] * rightVector[0] );
}
