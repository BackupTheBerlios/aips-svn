#ifdef WIN32
#pragma once
#endif

/************************************************************************
 * File: cvector.h                                                      *
 * Project: AIPS                                                        *
 * Description: A class for the representation of mathematical vectors  *
 *              This class will only be used in the case that AIPS is   *
 *              compiled without Blitz++ support. Otherwise we use      *
 *              Blitz::TinyVector.                                      *
 *                                                                      *
 * Author: Hendrik Belitz (hbelitz@users.berlios.de)                    *
 *                                                                      *
 * Version: 0.5                                                         *
 * Created: 2004-01-04                                                  *
 * Changed: 2004-04-19 Division operator added                          *
 *          2004-04-20 CVector is no longer derived from CBase          *
 *                      (to save memory in large vector arrays)         *
 *                     Class now stores the correct data type           *
 *                     Method getType added                             *
 *          2004-04-28 Updated documentation                            *
 *          2005-11-18 Added T_numtype for Blitz++ compability          *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CVECTOR_H
#define CVECTOR_H

#include <aipsbase_config.h>

#ifndef USE_BLITZ
#define CVECTOR_VERSION "0.5"

namespace aips 
{

/**
 * A class for the representation of mathematical vectors.
 * Only used if compiled without Blitz++ support
 */
template<typename DataType, unsigned short dimension> 
class CVector
{
public:
	typedef DataType T_numtype; //< Data type of the actual CVector. Inserted for compability with Blitz++
/* Structors */
  /// Constructor
  CVector( DataType dX = 0.0, DataType dY = 0.0, DataType dZ = 0.0 ) throw();
  /// Copy constructor
  CVector( const CVector<DataType, dimension>& aVector ) 
		throw();
  /// Destructor
  ~CVector() 
		throw();
/* Operators/Assignment */
  /// Operator= for CVector
  CVector<DataType, dimension>& operator=( const CVector<DataType, dimension>& aVector ) 
		throw();
  /// Operator= for scalar type
  CVector<DataType, dimension>& operator=( const DataType aScalar ) 
		throw();
  /// Operator *=
  CVector<DataType, dimension>& operator*=( const DataType aScalar ) 
		throw();
  /// Operator /=
  CVector<DataType, dimension>& operator/=( const DataType aScalar ) 
		throw();
  /// Operator +=
  CVector<DataType, dimension>& operator+=( const CVector<DataType, dimension>& aVector ) 
		throw();
  /// Operator -=
  CVector<DataType, dimension>& operator-=( const CVector<DataType, dimension>& aVector ) 
		throw();
/* Operators/Access */
  /// Access operator without range checking
  DataType& operator[]( const unsigned short usIndex ) throw();
  /// Const access operator without range checking
  const DataType operator[]( const unsigned short usIndex ) const throw();
/* Operators/Identity */
  /// Operator== to check for equality of two vectors
  bool operator==( CVector<DataType, dimension>& aVector ) const throw();
/* Accessors */
	/// Returns the data type of the stored elements
  const std::type_info& getType() const
    throw();
private:
  DataType theData[dimension]; ///< Actual vector components
};

/* Arithmetic operations for CVector */
/// Addition of two vectors
template<typename DataType, unsigned short  dimension> inline 
CVector<DataType, dimension> operator+
  ( const CVector<DataType, dimension>& aVector, const CVector<DataType, dimension>& bVector )
	throw();
/// Difference of two vectors
template<typename DataType, unsigned short  dimension> inline 
CVector<DataType, dimension> operator-
  ( const CVector<DataType, dimension>& aVector, const CVector<DataType,dimension>& bVector ) 
	throw();
/// Multiplication of vector and scalar
template<typename DataType, unsigned short  dimension> inline 
CVector<DataType, dimension> operator*
  ( const CVector<DataType, dimension>& aVector, const DataType aScalar ) 
	throw();
/// Division of vector and scalar
template<typename DataType, unsigned short  dimension> inline 
CVector<DataType, dimension> operator/
  ( const CVector<DataType, dimension>& aVector, const DataType aScalar ) 
	throw();
/// Multiplication of scalar and vector
template<typename DataType, unsigned short  dimension> inline 
CVector<DataType, dimension> operator*
  ( const DataType aScalar, const CVector<DataType, dimension>& aVector ) 
	throw();

/// Scalar product of two vectors
template<typename DataType, unsigned short dimension> inline 
DataType dot( const CVector<DataType, dimension>& leftVector, 
	const CVector<DataType, dimension>& rightVector ) throw();

} // namespace aips

/* Inline implementation of the above functions */
#include "cvectorinlines.tpp"

#endif // USE_BLITZ
#endif // CVECTOR_H
