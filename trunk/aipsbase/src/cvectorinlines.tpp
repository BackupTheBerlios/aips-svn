/************************************************************************
 * File: cvectorinlines.cpp                                             *
 * Project: AIPS                                                        *
 * Description: A class for the representation of mathematical vectors  *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 2004-01-20                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

/*************
 * Structors *
 *************/
 
/**
 * \param dX first component
 * \param dY second component
 * \param dZ third component
 */
template<typename DataType, ushort dimension> inline
CVector<DataType, dimension>::CVector ( double dX, double dY, double dZ ) throw()
{
  theData[0] = dX;
  if ( 1 < dimension ) 
		theData[1] = dY;
  if ( 2 < dimension ) 
		theData[2] = dZ;
}

/**
 * Copy constructor
 * \param aVector Vector to be copied
 */
template<typename DataType, ushort dimension> inline 
CVector<DataType, dimension>::CVector( const CVector& aVector ) throw()
{
  for ( int i = 0; i < dimension; i++ )
    theData[i] = aVector.theData[i];
}

/** Destructor */
template<typename DataType, ushort dimension> inline 
CVector<DataType, dimension>::~CVector() throw()
{
}

/************************
 * Operators/assignment *
 ************************/
 
/**
 * \param aVector Vector2D to get values from
 * \return reference to assignment target
 */
template<typename DataType, ushort dimension> inline 
CVector<DataType, dimension>& CVector<DataType, dimension>::operator=
  ( const CVector<DataType, dimension>& aVector ) throw()
{
  if ( &aVector == this ) 
		return *this;
  for ( int i = 0; i < dimension; i++ )  
    theData[i] = aVector.theData[i];
  return *this;
}

/**
 * \param aScalar Value to assign to all vector components
 * \return reference to assignment target
 */
template<typename DataType, ushort dimension> inline
	CVector<DataType, dimension>& CVector<DataType, dimension>::operator=( const DataType aScalar )
	throw()
{
  for ( int i = 0; i < dimension; i++ )
    theData[i] = aScalar;
  return *this;
}

/********************
 * Operators/access *
 ********************/

/**
 * \param usIndex access index, should be 0 or 1
 * \return value at given index
 */
template<typename DataType, ushort dimension> inline 
	double& CVector<DataType, dimension>::operator[]( const ushort usIndex ) throw()
{
  return theData[usIndex];
}

/**
 * \param usIndex access index, should be 0 or 1
 * \return value at given index
 */
template<typename DataType, ushort dimension> inline 
	const double CVector<DataType, dimension>::operator[]( const ushort usIndex ) const throw()
{
  return theData[usIndex];
}

/************************
 * Operators/arithmetic *
 ************************/

/**
 * \param aScalar number to multiply with
 */
template<typename DataType, ushort dimension> inline
  CVector<DataType, dimension>& CVector<DataType, dimension>::operator*=( const DataType aScalar )
	throw()  
{
  for ( int i = 0; i < dimension; i++ )
    theData[i]*= aScalar;
  return *this;
}

/**
 * \param aScalar number to divide by
 */
template<typename DataType, ushort dimension> inline 
  CVector<DataType, dimension>& CVector<DataType, dimension>::operator/=( const DataType aScalar )
	throw()  
{
  for ( int i = 0; i < dimension; i++ )
    theData[i]/= aScalar;
  return *this;
}

/**
 * \param aVector vector to add
 */
template<typename DataType, ushort dimension> inline
  CVector<DataType, dimension>& CVector<DataType, dimension>::operator+=
	( const CVector<DataType, dimension>& aVector ) throw()
{
  for ( int i = 0; i < dimension; i++ )
    theData[i]+= aVector.theData[i];
  return *this;
}

/**
 * \param aVector vector to add
 */
template<typename DataType, ushort dimension> inline
  CVector<DataType, dimension>& CVector<DataType, dimension>::operator-=
	( const	CVector<DataType, dimension>& aVector ) throw()
{
  for ( int i = 0; i < dimension; i++ )
    theData[i]-= aVector.theData[i];    
  return *this;
}

/*****************
 * Other methods *
 *****************/
 
/**
 * \returns the data type of the stored elements
 */
template<typename DataType, ushort dimension> inline 
	const std::type_info& CVector<DataType, dimension>::getType() const	throw()
{
	return typeid( theData[0] );
}

/*********************************
 * External arithmetic functions *
 *********************************/

/**
 * \param aVector first vector to add
 * \param bVector second vector to add
 * \return sum of the two vectors
 */
template<typename DataType, ushort dimension> inline 
	CVector<DataType, dimension> operator+( const CVector<DataType, dimension>& aVector, 
	const CVector<DataType, dimension>& bVector ) throw()
{
  CVector<DataType, dimension> newVector( aVector );
  for ( int i = 0; i < dimension; ++i )  
    newVector[i] += bVector[i];
  return newVector;
}

/**
 * \param aVector first vector to substract from
 * \param bVector second vector to substract
 * \return difference of the two vectors
 */
template<typename DataType, ushort dimension> inline 
	CVector<DataType, dimension> operator-( const CVector<DataType, dimension>& aVector, 
	const CVector<DataType, dimension>& bVector ) throw()
{
  CVector<DataType, dimension> newVector( aVector );
  for ( int i = 0; i < dimension; ++i )
    newVector[i] -= bVector[i];
  return newVector;
}

/**
 * Although this is not mathematically defined, it might be useful sometimes
 * \param aVector vector to multiply
 * \param aScalar scalar to multiply
 * \return aVector * aScalar
 */
template<typename DataType, ushort dimension> inline 
	CVector<DataType, dimension> operator*( const CVector<DataType, dimension>& aVector, 
	const DataType aScalar ) throw()
{
  CVector<DataType, dimension> newVector( aVector );
  for ( int i = 0; i < dimension; ++i )
    newVector[i] *= aScalar;
  return newVector;
}

/**
 * \param aVector vector to divide
 * \param aScalar divisor
 * \return aVector / aScalar
 */
template<typename DataType, ushort dimension> inline 
	CVector<DataType, dimension> operator/( const CVector<DataType, dimension>& aVector, 
	const DataType aScalar ) throw()
{
  CVector<DataType, dimension> newVector( aVector );
  for ( int i = 0; i < dimension; ++i )
    newVector[i] /= aScalar;
  return newVector;
}

/**
 * \param aScalar scalar to multiply
 * \param aVector vector to multiply
 * \return aVector * aScalar
 */
template<typename DataType, ushort dimension> inline 
	CVector<DataType, dimension> operator*( const DataType aScalar, 
	const CVector<DataType, dimension>& aVector ) throw()
{
  CVector<DataType, dimension> newVector( aVector );
  for ( int i = 0; i < dimension; ++i )
    newVector[i] *= aScalar;
  return newVector;
}

/**
 * \param aVector vector to compare with
 */
template<typename DataType, ushort dimension> inline 
	bool CVector<DataType, dimension>::operator==( CVector<DataType, dimension>& aVector ) 
	const throw()
{
  if ( &aVector == this )
    return true;
  bool bEqual = true;
  for ( int i = 0; i < dimension && bEqual; ++i )
    bEqual = ( aVector.theData[i] == theData[i] );
  return bEqual;
}

/**
 * \param leftVector first Vector for product
 * \param rightVector second Vector for product
 */
template<typename DataType, ushort dimension> inline DataType 
	dot( const CVector<DataType, dimension>& leftVector, 
	const CVector<DataType, dimension>& rightVector ) throw()
{
  DataType result = 0;
  for ( int i = 0; i < dimension; ++i )
    result += leftVector[i] * rightVector[i];
  return result;
}
