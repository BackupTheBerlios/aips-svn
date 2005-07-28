/************************************************************************
 * File: cdataset.cpp                                                   *
 * Project: AIPS                                                        *
 * Description: Base class for generic data fields                      *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 20.01.04                                                    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "cdataset.h"

using namespace std;
using namespace aips;

/*************
 * Structors *
 *************/

/**
 * \param usDimension_ field dimension
 * \param extentArr_ field extents ( == range of each dimension )
 * \param dataDimensionSize_ dimension of each field entry
 * \param sClassName_ name of the class (type information)
 * \param sClassVersion_ version number of the class (type information)
 * \param sDerivedFrom_ name of the classes this class is derived from (type information)
 */
CDataSet::CDataSet( const ushort usDimension_, const size_t* extentArr_,
  const size_t dataDimensionSize_, const std::string &sClassName_, 
		const std::string &sClassVersion_ ,
    const std::string &sDerivedFrom_ ) throw ()
  : CBase( sClassName_, sClassVersion_, sDerivedFrom_ ), usDimension( usDimension_ ),
  extentVec( usDimension_ + 1 ), dataDimensionSize( dataDimensionSize_ ), baseElementDimensionsVec( usDimension_ )
{
  for ( ushort i = 0; i < usDimension; i++ )
  {
    extentVec[i] = extentArr_[i];
    baseElementDimensionsVec[i] = 1.0;
  }
	extentVec[usDimension] = dataDimensionSize;
}

/**
 * \param usDimension_ field dimension
 * \param extentVec_ field extents ( == range of each dimension )
 * \param dataDimensionSize_ dimension of each field entry
 * \param sClassName_ name of the class (type information)
 * \param sClassVersion_ version number of the class (type information)
 * \param sDerivedFrom_ name of the classes this class is derived from (type information)
 */
CDataSet::CDataSet( const ushort usDimension_, const vector<size_t> extentVec_,
  const size_t dataDimensionSize_, const std::string &sClassName_, 
	const std::string &sClassVersion_ ,
  const std::string &sDerivedFrom_ ) throw ()
  : CBase( sClassName_, sClassVersion_, sDerivedFrom_ ),
  usDimension( usDimension_ ), extentVec( extentVec_ ), dataDimensionSize( dataDimensionSize_ ),
  baseElementDimensionsVec( usDimension_ )
{
	for ( ushort i = 0; i < usDimension; i++ )
  {
    baseElementDimensionsVec[i] = 1.0;
  }
}

/// Constructor (for one-dimensional data sets)
CDataSet::CDataSet( const size_t extent_, const size_t dataDimensionSize_,
	const std::string &sClassName_, const std::string &sClassVersion_, const std::string &sDerivedFrom_ ) throw()
  : CBase( sClassName_, sClassVersion_, sDerivedFrom_ ), usDimension( 1 ), extentVec( 1 ),
  	dataDimensionSize( dataDimensionSize_ ), baseElementDimensionsVec( 1 )
{
	extentVec[0] = extent_;
}

CDataSet::CDataSet( const CDataSet& aDataSet ) throw()
  : CBase( "CDataSet", "0.4", "CBase" )
{
FBEGIN;
  usDimension = aDataSet.usDimension;
  dataDimensionSize = aDataSet.dataDimensionSize;
  extentVec = aDataSet.extentVec;
  baseElementDimensionsVec = aDataSet.baseElementDimensionsVec;
FEND;
}

CDataSet::~CDataSet() throw()
{
  extentVec.clear();
  baseElementDimensionsVec.clear();
}

/*************
 * Accessors *
 *************/

/** \returns the Dimension of the data set */
ushort CDataSet::getDimension() const throw()
{
  return usDimension;
}

/** \returns the data dimension of each field entry of the data set */
size_t CDataSet::getDataDimension() const throw()
{
  return dataDimensionSize;
}

/**
 * \returns the extent of the given dimension
 * \param usIndex dimensional index with valid values from [0..(dim-1)]. Defaults to 0.
 * \throws OutOfRangeException if usIndex is greater than dataset dimension minus one
 */
size_t CDataSet::getExtent( const ushort usIndex ) const
  throw( OutOfRangeException )
{
  if ( usIndex > ( usDimension - 1 ) )
    throw( OutOfRangeException( SERROR("Index out of range"), CException::RECOVER, ERR_BADDIMENSION ) );
  return extentVec[usIndex];
}

/** \returns vector of dimension sizes */
const vector<size_t> CDataSet::getExtents() const throw()
{
	return extentVec;
}

/** \returns vector of base element dimensions */
std::vector<double> CDataSet::getBaseElementDimensions() const throw()
{
	return baseElementDimensionsVec;
}

/**
 * \param usIndex index of requested dimension
 * \returns value of the requested dimension
 * \throws OutOfRangeException if usIndex is greater than dataset dimension minus one
 */
double CDataSet::getBaseElementDimension( const ushort usIndex ) const throw( OutOfRangeException )
{
  if ( usIndex > ( usDimension - 1 ) )
    throw( OutOfRangeException( SERROR("Index out of range"), CException::RECOVER, ERR_BADDIMENSION ) );
  return baseElementDimensionsVec[usIndex];
}
  
/************
 * Mutators *
 ************/

/**
 * \param usIndex index of dimension to update
 * \param dValue new dimension value
 * \throws OutOfRangeException if usIndex is greater than dataset dimension minus one
 */
void CDataSet::setBaseElementDimension( const ushort usIndex, const double dValue ) throw( OutOfRangeException )
{
  if ( usIndex > ( usDimension - 1 ) )
    throw( OutOfRangeException( SERROR("Index out of range"), CException::RECOVER, ERR_BADDIMENSION ) );
  baseElementDimensionsVec[usIndex] = dValue;
}

/**
 * \param dimensionsVec vector of updated dimensions
 * \throws OutOfRangeException if vector of updated dimensions has the wrong size
 */
void CDataSet::setBaseElementDimensions( const std::vector<double> dimensionsVec ) throw( OutOfRangeException )
{
  if ( dimensionsVec.size() != baseElementDimensionsVec.size() )
    throw( OutOfRangeException( SERROR("Dimension vector has wrong number of elements"),
    	CException::RECOVER, ERR_BADDIMENSION ) );
	baseElementDimensionsVec = dimensionsVec;
}

/**************************
 * Operators (Assignment) *
 **************************/
 
/**
 * \param aDataSet dataset to be assigned
 */
CDataSet& CDataSet::operator=( CDataSet& aDataSet ) throw()
{
  if ( &aDataSet == this )
    return *this;
    
  usDimension = aDataSet.usDimension;
  extentVec = aDataSet.extentVec;
  dataDimensionSize = aDataSet.dataDimensionSize;
  return *this;
}

/***************** 
 * Other Methods *
 *****************/

/**
 * \returns information string
 */
const string CDataSet::dump() const throw()
{
  return CBase::dump() + "Dimension: " + boost::lexical_cast<string>( usDimension ) + " DataDimension " 
		+ boost::lexical_cast<string>( dataDimensionSize ) + "\n" + "Extend size: " 
		+ boost::lexical_cast<string>( extentVec.size() ) + "\n";
}
