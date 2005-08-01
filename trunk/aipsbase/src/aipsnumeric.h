/************************************************************************
 * File: aipsnumeric.h                                                  *
 * Project: AIPS                                                        *
 * Description: Some useful numerical constants and simple functions    *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.12                                                        *
 * Status: Beta                                                         *
 * Created: 2004-01-16                                                  *
 * Changed:                                                             *
 *        2004-01-19 CVector implementation, dot and vecProd moved to   *
 *                    aipsnumericinlines.h                              *
 *        2004-01-20 Added arithmetic operators and == to CVector2D     *
 *                   Moved vecProd back to this file                    *
 *                   Added swapEndianess()                              *
 *                   Templatized CVector2D and CVector3D to CVector     *
 *                   Moved CVector to seperate file cvector.h           *
 *        2004-01-21 Added function round()                             *
 *                   Moved definitions to aipsnumericinlines.cpp        *
 *        2004-04-22 Added further documentation                        *
 *        2004-05-06 Added specializations of abs() for int and long    *
 *                   Added function in()                                *
 *        2004-08-11 Added TPoint2D, TPoint3D and TField typedefs       *
 *        2004-11-14 TField is now of type double instead of float      *
 *        2004-12-15 Added shared_ptr types for fields and images       *
 *        2005-01-26 Changed data type for TImage to short              *
 *                   Added typedef TComplex and TComplexImage           *
 *                    for complex datasets                              *
 *        2005-07-06 Added new field type TStringField and pointer      *
 *                    types for all single value types                  *
 *        2005-07-07 Added new single value type TSingleString          *
 *        2005-08-01 Added compile-dependent definition of standard     *
 *                    floating point type                               *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef AIPSNUMERIC_H
#define AIPSNUMERIC_H
// System includes
#include <stdint.h> // Primitive types with definitive size

// Standard library includes
#include <cmath>    // floor(), sqrt()
#include <complex>  // complex<>

// AIPS includes
#include "ctypeddata.h"
#include "csinglevalue.h"
 
#ifdef USE_BLITZ
// Blitz includes
#include <blitz/tinyvec.h>       // blitz::TinyVector
#include <blitz/tinyvec-et.h>    // blitz::TinyVector mathematical operators
#include <blitz/array/storage.h> // blitz::GeneralArrayStorage
#else
// Use standalone vector class 
#include "cvector.h"
#endif

#ifdef USE_DOUBLE
typedef double TFloatType;
#else
typedef float TFloatType;
#endif

namespace aips {

#ifdef USE_BLITZ
/* Typedefs for vectors if AIPS get compiled with Blitz++ support */
typedef blitz::TinyVector<TFloatType,2> TVector2D; ///< Typedef for a mathematical 2D vector
typedef blitz::TinyVector<TFloatType,3> TVector3D; ///< Typedef for a mathematical 3D vector
typedef blitz::TinyVector<int,2>    		TPoint2D;  ///< Typedef for a discrete 2D point
typedef blitz::TinyVector<int,3>    		TPoint3D;  ///< Typedef for a discrete 3D point

/**
 * Definition of our own storage format. Using x as the first index AND the fastest
 * varying index is more useful when working with images. Except for the base flag
 * this is identical to a FortranArray of Blitz++
 */
template<int N_rank>
class AIPSArray : public blitz::GeneralArrayStorage<N_rank>
{
private:
  typedef blitz::GeneralArrayStorage<N_rank> T_base;
  typedef _bz_typename T_base::noInitializeFlag noInitializeFlag;
  using T_base::ordering_;
  using T_base::ascendingFlag_;
  using T_base::base_;
public:
  AIPSArray()
    : blitz::GeneralArrayStorage<N_rank>( noInitializeFlag() )
  {
    for (int i=0; i < N_rank; ++i)
      ordering_(i) = i;
    ascendingFlag_ = true;
    base_ = 0;
  }
};
#else
/* Typedefs for vectors if AIPS get compiled without Blitz++ support */
typedef CVector<TFloatType,2> TVector2D; ///< Typedef for a mathematical 2D vector
typedef CVector<TFloatType,3> TVector3D; ///< Typedef for a mathematical 3D vector
typedef CVector<int,2>    		TPoint2D;  ///< Typedef for discrete 2D point
typedef CVector<int,3>	   		TPoint3D;  ///< Typedef for discrete 3D point
#endif

/*******************
 * Useful typedefs *
 *******************/

/// A single signed integer value for pipeline usage
typedef CSingleValue<long> TInteger; 
/// A single double value for pipeline usage
typedef CSingleValue<TFloatType> TDouble;
/// A single complex value for pipeline usage
typedef CSingleValue<std::complex<TFloatType> > TComplex;
/// A single string value for pipeline usage
typedef CSingleValue<std::string> TSingleString;
/// A scalar integer field for pipeline usage
typedef CTypedData<short> TImage;
/// A scalar floating point field for pipeline usage
typedef CTypedData<TFloatType> TField;
/// A complex field for pipeline usage
typedef CTypedData<std::complex<TFloatType> > TComplexImage;
/// A field of 2D vectors for pipeline usage
typedef CTypedData<TVector2D> TField2D;
/// A field of 3D vectors for pipeline usage
typedef CTypedData<TVector3D> TField3D;
/// A field of strings for pipeline usage
typedef CTypedData<std::string> TStringField;

typedef boost::shared_ptr<CDataSet>           TDataSetPtr; ///< Smart pointer to an unspecified data set
typedef boost::shared_ptr<TImage>               TImagePtr; ///< Smart pointer to integer images
typedef boost::shared_ptr<TField>               TFieldPtr; ///< Smart pointer to floating point images
typedef boost::shared_ptr<TComplexImage> TComplexImagePtr; ///< Smart pointer to complex images
typedef boost::shared_ptr<TField2D>           TField2DPtr; ///< Smart pointer to 2D vector fields
typedef boost::shared_ptr<TField3D>           TField3DPtr; ///< Smart pointer to 3D vector fields
typedef boost::shared_ptr<TStringField>       TStringFieldPtr; ///< Smart pointer to string arrays
typedef boost::shared_ptr<TInteger>           TIntegerPtr; ///< Smart pointer to a single integer value
typedef boost::shared_ptr<TDouble>           	TDoublePtr; ///< Smart pointer to a single double value
typedef boost::shared_ptr<TComplex>           TComplexPtr; ///< Smart pointer to a single complex value

/********************
 * Useful constants *
 ********************/

// Some zero length vectors ( for convenience )

const TVector2D VEC_ZERO2D; ///< Zero vector for two dimensions
const TVector3D VEC_ZERO3D; ///< Zero vector for three dimensions

/************************************************************
 * Useful mathematical and information theoretical funcions *
 ************************************************************/

/// Swaps the bytes of a data type from little to big endian and vice versa
template<typename T> inline void swapEndianess( T& value ) 
	throw();
	
#ifndef __GLIBCPP__
/// Templated version of abs ( compute absolute value )
template<typename T> inline T abs( const T value ) 
	throw();
#endif 

/// Templated round
template<typename T> inline T round( const T value ) 
	throw();

/// Checks if the given value lies in the interval [min,max]
template<typename T> inline bool in( const T value, const T min, const T max )
	throw();
/// Checks if the given value lies in the interval (min,max)
template<typename T> inline bool trueIn( const T value, const T min, const T max )
	throw();

/// Approximated equality for double precision floating points
inline bool eq( const double lhs, const double rhs,
	const double epsilon = std::numeric_limits<double>::epsilon() )
	throw();
	
#ifdef USE_STANDALONE
/// Norm function for data types with defined dot() function
template<typename T> inline double norm( const T& aVector ) 
	throw();
#endif
	
/// Quadratic norm function for data types with defined dot() function.
template<typename T> inline double quadNorm( const T& aVector ) 
	throw();
	
/// Outer product for 3D vectors
inline TVector3D vecProd( const TVector3D& leftVector, const TVector3D& rightVector ) 
	throw();

 
#include<aipsnumericinlines.tpp>

}
#endif
