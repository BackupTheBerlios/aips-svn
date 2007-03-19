/************************************************************************
 * File: aipsnumericinlines.cpp                                         *
 * Project: AIPS                                                        *
 * Description: Implementation of aipsnumeric inline functions          *
 *                                                                      *
 * Author: Hendrik Belitz (hbelitz@users.berlios.de)                    *
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
  boost::uint8_t* itBegin = static_cast<uint8_t*>( static_cast<void*>( &value ) );
  boost::uint8_t* itEnd = itBegin + sizeof( value );
  std::reverse( itBegin, itEnd );
}

/**
 * Specialization of the function template for TVector2D type
 * \param value data to swap (will be overwritten)
 */
inline void swapEndianess( TVector2D& value ) throw()
{
  boost::uint8_t* itBegin = static_cast<boost::uint8_t*>( static_cast<void*>( &value[0] ) );
  boost::uint8_t* itEnd = itBegin + sizeof( value[0] );
  std::reverse( itBegin, itEnd );
  itBegin = static_cast<boost::uint8_t*>( static_cast<void*>( &value[1] ) );
  itEnd = itBegin + sizeof( value[1] );
  std::reverse( itBegin, itEnd );	
}

/**
 * Specialization of the function template for TVector3D type
 * \param value data to swap (will be overwritten)
 */
inline void swapEndianess( TVector3D& value ) throw()
{
  boost::uint8_t* itBegin = static_cast<boost::uint8_t*>( static_cast<void*>( &value[0] ) );
  boost::uint8_t* itEnd = itBegin + sizeof( value[0] );
  std::reverse( itBegin, itEnd );
  itBegin = static_cast<boost::uint8_t*>( static_cast<void*>( &value[1] ) );
  itEnd = itBegin + sizeof( value[1] );
  std::reverse( itBegin, itEnd );	
  itBegin = static_cast<boost::uint8_t*>( static_cast<void*>( &value[2] ) );
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
 * \param value number to round
 * \returns nearest integer of input
 */
template<typename T> inline T round( const T value ) throw()
{
	return static_cast<T>( static_cast<double>( value ) + 0.5 );
}

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


/**
 * \param lhs first value to compare
 * \param rhs second value to compare
 * \param epsilon allowed error
 */
inline bool equal( const double lhs, const double rhs, const double epsilon ) throw()
{
	return ( fabs( lhs-rhs ) <= epsilon );
}
