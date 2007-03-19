/************************************************************************
 * File: csinglevalueinlines.cpp                                        *
 * Project: AIPS                                                        *
 * Description: A dataset representing a single value                   *
 *                                                                      *
 * Author: Hendrik Belitz (hbelitz@users.berlios.de)                          *
 *                                                                      *
 * Created: 20.01.04                                                    *
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
 * \param theValue_ Initial value of scalar
 */
template<typename valueType> CSingleValue<valueType>::CSingleValue( valueType theValue_ )
  throw() : CDataSet( 0, NULL, 1, "CSingleValue", CSINGLEVALUE_VERSION, "CDataSet" )
{
  valueVec.resize(1);
  valueVec[0] = theValue_;
}

/**
 * \param theValue_ Array of values that equals the vectors components
 * \param dataDimensionSize Dimension of the vector
 */
template<typename valueType> CSingleValue<valueType>::CSingleValue( valueType* valueArr_,
  size_t dataDimensionSize_ ) throw() 
  : CDataSet( 0, NULL, dataDimensionSize_, "CSingleValue", CSINGLEVALUE_VERSION, "CDataSet" )
{
  valueVec.resize( dataDimensionSize );
  for ( size_t i = 0; i < dataDimensionSize; i++ )
    valueVec[i] = valueArr_[i];
}

/**
 * \param aSingleValue instance to contruct object from
 */
template<typename valueType> CSingleValue<valueType>::CSingleValue
  ( const CSingleValue& aSingleValue ) throw()
  : CDataSet( 0, NULL, aSingleValue.dataDimensionSize, "CSingleValue", CSINGLEVALUE_VERSION, "CDataSet" )  
{
FBEGIN;
  valueVec.resize( dataDimensionSize );
  valueVec = aSingleValue.valueVec;
FEND;
}

template<typename valueType> CSingleValue<valueType>::~CSingleValue() throw()
{
  valueVec.clear();
}

/*************
 * Operators *
 *************/

/**
 * \param aSingleValue instance to assign data from
 */
template<typename valueType> inline CSingleValue<valueType>& CSingleValue<valueType>::operator=
  ( const CSingleValue& aSingleValue ) throw()
{
  if ( &aSingleValue == this ) return *this;
  CDataSet::operator=( aSingleValue );
  valueVec.resize(aSingleValue.usDimension);
  valueVec = aSingleValue.valueVec;
  return *this;
}

/*************
 * Accessors *
 *************/
 
/** \returns the type of the single value */
template<typename valueType> const std::type_info& CSingleValue<valueType>::getType()
  const throw()
{
  return typeid( valueType );
}

/** \returns a handle to the void array of the data */
template<typename valueType> void* CSingleValue<valueType>::getVoidArray() throw()
{
  return static_cast<void*>( &( valueVec[0] ) );
}

/** \returns a handle to the typed array of the data */
template<typename valueType> inline valueType* CSingleValue<valueType>::getArray() throw()
{
  return valueVec;
}

/********************
 * Access operators *
 ********************/
 
/**
 * \param usIndex Index of member (only use if CSingleValue is a vector!)
 */
template<typename valueType> inline valueType& CSingleValue<valueType>::operator()
  ( const unsigned short usIndex ) throw()
{
  return valueVec[usIndex];
}

/**
 * \param usIndex Index of member (only use if CSingleValue is a vector!)
 */
template<typename valueType> inline const valueType& CSingleValue<valueType>::operator()
  ( const unsigned short usIndex ) const throw()
{
  return valueVec[usIndex];
}

/**
 * \param usIndex index of data member (only use if CSingleValue is a vector!)
 * \exception OutOfRangeException if usIndex is greater than dataDimensionSize
 */
template<typename valueType> inline valueType& CSingleValue<valueType>::at
  ( const unsigned short usIndex ) throw( OutOfRangeException )
{
  if ( usIndex > ( dataDimensionSize - 1 ) )
    throw( OutOfRangeException( SERROR("Index out of range"), ERR_BADDIMENSION, 
    CException::RECOVER ) );
  return valueVec[usIndex];
}

/**
 * \param usIndex index of data member (only use if CSingleValue is a vector!)
 * \exception OutOfRangeException if usIndex is greater than dataDimensionSize
 */
template<typename valueType> inline const valueType& CSingleValue<valueType>::at
  ( const unsigned short usIndex ) const throw( OutOfRangeException )
{
  if ( usIndex > ( dataDimensionSize - 1 ) )
    throw( OutOfRangeException( SERROR("Index out of range"), ERR_BADDIMENSION, 
    CException::RECOVER ) );
  return valueVec[usIndex];
}

/*****************
 * Other methods *
 *****************/

template<typename valueType> const std::string CSingleValue<valueType>::dump() const throw()
{
  return CDataSet::dump() + "valueVec size: " + boost::lexical_cast<std::string>( valueVec.size() ) + "\n";
}

/**
 * \param aDataSet the other data set
 */
template<typename valueType> inline 
void CSingleValue<valueType>::swap( CSingleValue<valueType>& aDataSet ) throw()
{
	valueVec.swap( aDataSet.valueVec );
	std::swap( usDimension, aDataSet.usDimension );
  extentVec.swap( aDataSet.extentVec ); 
  std::swap( dataDimensionSize, aDataSet.dataDimensionSize );
}
