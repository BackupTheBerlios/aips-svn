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
 * \param dataDimensionSize_ dimension of each field entry (defaults to 1)
 * \param sClassName_ name of the class (type information)
 * \param sClassVersion_ version number of the class (type information)
 * \param sDerivedFrom_ name of the classes this class is derived from (type information)
 * \post All member variables are initialised.
 * \post Origin initialised with \f$ \vec{0} \f$.
 * \post Base element dimension initialised with \f$ \vec{1} \f$.
 */
CDataSet::CDataSet( const ushort usDimension_, const size_t* extentArr_,
  const size_t dataDimensionSize_, const std::string &sClassName_, 
	const std::string &sClassVersion_,
  const std::string &sDerivedFrom_ ) throw ()
  : CBase( sClassName_, sClassVersion_, sDerivedFrom_ ), usDimension( usDimension_ ),
  extentVec( usDimension_ + 1 ), baseElementDimensionsVec( usDimension_ ), originVec( usDimension_ ), dataDimensionSize( dataDimensionSize_ )
{
FBEGIN;
  for ( ushort i = 0; i < usDimension; i++ )
  {
    extentVec[i] = extentArr_[i];
    baseElementDimensionsVec[i] = 1.0;
    originVec[i] = 0.0;
  }
	extentVec[usDimension] = dataDimensionSize;
  if ( isVerbose() )
  {
    alog << LINFO << "Creating instance " << static_cast<void*>( this ) << " of class CDataSet" << endl;
    alog << "- Dimension " << usDimension << endl;
    alog << "- Extents ";
    for ( uint i = 0; i < usDimension; ++i )
      alog << extentVec[i] << " ";
    alog << endl << "- Origin ";
    for ( uint i = 0; i < usDimension; ++i )
      alog << originVec[i] << " ";
    alog << endl << "- Origin ";
    for ( uint i = 0; i < usDimension; ++i )
      alog << baseElementDimensionsVec[i] << " ";
    alog << endl << "- Data dimension " << dataDimensionSize << endl;    
  }
FEND;  
}

/**
 * \param usDimension_ field dimension
 * \param extentVec_ field extents ( == range of each dimension )
 * \param dataDimensionSize_ dimension of each field entry (defaults to 1)
 * \param sClassName_ name of the class (type information)
 * \param sClassVersion_ version number of the class (type information)
 * \param sDerivedFrom_ name of the classes this class is derived from (type information)
 * \post All member variables are initialised.
 * \post Origin initialised with \f$ \vec{0} \f$.
 * \post Base element dimension initialised with \f$ \vec{1} \f$.
 */
CDataSet::CDataSet( const ushort usDimension_, const vector<size_t> extentVec_,
  const size_t dataDimensionSize_, const std::string &sClassName_, 
	const std::string &sClassVersion_ ,
  const std::string &sDerivedFrom_ ) throw ()
  : CBase( sClassName_, sClassVersion_, sDerivedFrom_ ),
  usDimension( usDimension_ ), extentVec( extentVec_ ), baseElementDimensionsVec( usDimension_ ), originVec( usDimension_ ),
  dataDimensionSize( dataDimensionSize_ )  
{
FBEGIN;
	for ( ushort i = 0; i < usDimension; i++ )
  {
    baseElementDimensionsVec[i] = 1.0;
    originVec[i] = 0.0;
  }
  extentVec.push_back( dataDimensionSize );
  if ( isVerbose() )
  {
    alog << LINFO << "Creating instance " << static_cast<void*>( this ) << " of class CDataSet" << endl;
    alog << dump() << endl;
  }
FEND;
}

/**
 * \param usDimension_ field dimension
 * \param extentVec_ field extents ( == range of each dimension )
 * \param dataDimensionSize_ dimension of each field entry (defaults to 1)
 * \param sClassName_ name of the class (type information)
 * \param sClassVersion_ version number of the class (type information)
 * \param sDerivedFrom_ name of the classes this class is derived from (type information)
 * \post All member variables are initialised.
 * \post usDimension defaults to 1.
 * \post Origin initialised with 0.
 * \post Base element dimension initialised with 1.
 */
CDataSet::CDataSet( const size_t extent_, const size_t dataDimensionSize_,
	const std::string &sClassName_, const std::string &sClassVersion_, const std::string &sDerivedFrom_ ) throw()
  : CBase( sClassName_, sClassVersion_, sDerivedFrom_ ), usDimension( 1 ), extentVec( 2 ),
  	baseElementDimensionsVec( 1 ), originVec( 1 ), dataDimensionSize( dataDimensionSize_ )
{
FBEGIN;
	extentVec[0] = extent_;
	extentVec[1] = dataDimensionSize_;
  baseElementDimensionsVec[0] = 1.0;
  originVec[0] = 0.0;
  if ( isVerbose() )
  {
    alog << LINFO << "Creating instance " << static_cast<void*>( this ) << " of class CDataSet" << endl;
    alog << dump() << endl;
  }
FEND;
}

/**
 * \param aDataSet data set to copy.
 * \post All member variables are initialised with the copied values.
 */
CDataSet::CDataSet( const CDataSet& aDataSet ) throw()
  : CBase( "CDataSet", "0.4", "CBase" )
{
FBEGIN;
  usDimension = aDataSet.usDimension;
  dataDimensionSize = aDataSet.dataDimensionSize;
  extentVec = aDataSet.extentVec;
  originVec = aDataSet.originVec;
  baseElementDimensionsVec = aDataSet.baseElementDimensionsVec;
  if ( isVerbose() )
  {
    alog << LINFO << "Creating instance " << static_cast<void*>( this ) << " of class CDataSet" << endl;
    alog << dump() << endl;
  }
FEND;
}

CDataSet::~CDataSet() throw()
{
FBEGIN;
  if ( isVerbose() )
    alog << LINFO << "Destroying instance " << static_cast<void*>( this ) << " of class CDataSet" << endl;
  extentVec.clear();
  baseElementDimensionsVec.clear();
FEND;  
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
double CDataSet::getBaseElementDimension( const ushort usIndex ) const
  throw( OutOfRangeException )
{
  if ( usIndex > ( usDimension - 1 ) )
    throw( OutOfRangeException( SERROR("Index out of range"), CException::RECOVER, ERR_BADDIMENSION ) );
  return baseElementDimensionsVec[usIndex];
}

/** \returns vector of origin coordinates */
std::vector<double> CDataSet::getOrigin() const throw()
{
  return originVec;
}

/**
 * \param usIndex index of requested dimension
 * \returns origin coordinate of the requested dimension
 * \throws OutOfRangeException if usIndex is greater than dataset dimension minus one
 */
double CDataSet::getOrigin( const ushort usIndex ) const
  throw( OutOfRangeException )
{
  if ( usIndex > ( usDimension - 1 ) )
    throw( OutOfRangeException( SERROR("Index out of range"), CException::RECOVER, ERR_BADDIMENSION ) );
  return originVec[usIndex];
}

/************
 * Mutators *
 ************/

/**
 * \param usIndex index of dimension to update
 * \param dValue new dimension value
 * \throws OutOfRangeException if usIndex is greater than dataset dimension minus one
 * \post given base element dimension was updated
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
 * \post base element dimensions were updated
 */
void CDataSet::setBaseElementDimensions( const std::vector<double> dimensionsVec ) throw( OutOfRangeException )
{
  if ( dimensionsVec.size() != baseElementDimensionsVec.size() )
    throw( OutOfRangeException( SERROR("Dimension vector has wrong number of elements"),
    	CException::RECOVER, ERR_BADDIMENSION ) );
	baseElementDimensionsVec = dimensionsVec;
}

/**
 * \param dimensionsVec double array of updated origin
 * \throws OutOfRangeException if vector of updated dimensions has the wrong size
 * \post base element dimensions were updated
 */
void CDataSet::setBaseElementDimensions( const double* dimensionsVec_ ) throw()
{
  for( uint i = 0; i < usDimension; ++i )
    baseElementDimensionsVec[i] = dimensionsVec_[i];
}

/**
 * \param usIndex index of dimension to update
 * \param dValue new dimension value
 * \throws OutOfRangeException if usIndex is greater than dataset dimension minus one
 * \post origin coordinate was updated
 */
void CDataSet::setOrigin( const ushort usIndex, const double dValue ) throw( OutOfRangeException )
{
  if ( usIndex > ( usDimension - 1 ) )
    throw( OutOfRangeException( SERROR("Index out of range"), CException::RECOVER, ERR_BADDIMENSION ) );
  originVec[usIndex] = dValue;
}

/**
 * \param originVec_ vector of updated origin
 * \throws OutOfRangeException if vector of updated dimensions has the wrong size
 * \post origin coordinates were updated
 */
void CDataSet::setOrigin( const std::vector<double> originVec_ ) throw( OutOfRangeException )
{
  if ( usDimension != originVec_.size() )
    throw( OutOfRangeException( SERROR("Origin vector has wrong number of elements"),
      CException::RECOVER, ERR_BADDIMENSION ) );
  originVec = originVec_;
}

/**
 * \param dimensionsVec double array of updated origin
 * \throws OutOfRangeException if vector of updated dimensions has the wrong size
 * \post origin coordinates were updated
 */
void CDataSet::setOrigin( const double* originVec_ ) throw()
{
  for( uint i = 0; i < usDimension; ++i )
    originVec[i] = originVec_[i];
}

/**************************
 * Operators (Assignment) *
 **************************/
 
/**
 * \param aDataSet data set to be assigned
 * \post All values were initialised with the new values
 */
CDataSet& CDataSet::operator=( CDataSet& aDataSet ) throw()
{  
  if ( &aDataSet == this )
    return *this;
      
  usDimension = aDataSet.usDimension;
  extentVec = aDataSet.extentVec;
  dataDimensionSize = aDataSet.dataDimensionSize;
  originVec = aDataSet.originVec;

  if ( isVerbose() )
    alog << LINFO << dump() << endl;
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
  ostringstream os;
  
  os << "- Dimension " << usDimension << endl;
  os << "- Extents ";
  for ( uint i = 0; i < usDimension; ++i )
    os << extentVec[i] << " ";
  os << endl << "- Origin ";
  for ( uint i = 0; i < usDimension; ++i )
    os << originVec[i] << " ";
  os << endl << "- Base element dimension ";
  for ( uint i = 0; i < usDimension; ++i )
    os << baseElementDimensionsVec[i] << " ";
  os << endl << "- Data dimension " << dataDimensionSize << endl;

  return CBase::dump() + os.str();
}
