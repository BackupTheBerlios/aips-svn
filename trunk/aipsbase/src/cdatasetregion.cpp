/************************************************************************
 * File: cdatasetregion.cpp                                             *
 * Project: AIPS                                                        *
 * Description: Defines a sub region of a given dataset                 *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Created: 2004-09-17                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#include "cdatasetregion.h"

namespace aips {

/*************
 * Structors *
 *************/

using namespace std;

/**
 * \param extentsVec_ vector representing the extents of the region
 * \param originVec_ vector representing the point of origin of the region 
 * \throws OutOfRangeException if dimensions of extents and origin differ
 * \post All data members are initialised properly.
 */
CDataSetRegion::CDataSetRegion( const std::vector<size_t>& extentsVec_, 
	const std::vector<size_t>& originVec_ ) throw( OutOfRangeException ) 	
	: CBase( "CDataSetRegion","0.1","CBase" ), extentsVec( extentsVec_ ), originVec( originVec_ )
{
  if ( extentsVec.size() != originVec.size() )
    throw( OutOfRangeException( SERROR("Dimensions of extents and origin are not equal"),
      CException::RECOVER, ERR_BADDIMENSION ) );
  if ( isVerbose() )
    alog << LINFO << dump() << endl;
}
	
/**
 * \param extentsArr_ array representing the extents of the region
 * \param originArr_ array representing the point of origin of the region
 * \param dimensionSize size of the given arrays
 * \post All data members are initialised properly.
 */
CDataSetRegion::CDataSetRegion( const size_t* extentsArr_, const size_t* originArr_, 
	const size_t dimensionSize ) throw() : CBase( "CDataSetRegion","0.1","CBase" )
{
	for( size_t i = 0; i < dimensionSize; ++i )
	{
		extentsVec.push_back( extentsArr_[i] );
		originVec.push_back( originArr_[i] );
	}
  if ( isVerbose() )
    alog << LINFO << dump() << endl;
}
 
/**
 * \param aNewRegion CDataSetRegion instance to copy
 * \post All data members are initialised properly.
 */
CDataSetRegion::CDataSetRegion( const CDataSetRegion& aNewRegion ) throw()
	: CBase( "CDataSetRegion","0.1","CBase" ), extentsVec( aNewRegion.extentsVec ),
		originVec( aNewRegion.originVec )
{
  if ( isVerbose() )
    alog << LINFO << dump() << endl;
}
  
CDataSetRegion::~CDataSetRegion() throw()
{
  if ( isVerbose() )
    alog << LINFO << "Deleting instance " << static_cast<void*>( this ) << " of CDataSetRegion" << endl;
}
	
/*****************
 * Other methods *
 *****************/

/**
 * \param originVec_ new position of region origin
 * \throws OutOfRangeException if dimension of parameter doesn't match that of the region's extents
 * \post new origin set
 */
void CDataSetRegion::resetOrigin( const std::vector<size_t>& originVec_ )
  throw( OutOfRangeException )
{
  if ( originVec_.size() != extentsVec.size() )
    throw( OutOfRangeException( SERROR("Dimensions of extents and origin are not equal"),
      CException::RECOVER, ERR_BADDIMENSION ) );
	originVec = originVec_;
  if ( isVerbose() )
    alog << LINFO << dump() << endl;
}
	
/**
 * \param originArr_ new position of region origin
 * \param dimensionSize size of originArr_
 * \throws OutOfRangeException if dimension of parameter doesn't match that of the region's extents
 * \post new origin set
 */ 
void CDataSetRegion::resetOrigin( const size_t* originArr_, const size_t dimensionSize )	
	throw( OutOfRangeException )	
{
	if ( dimensionSize != originVec.size() )
		throw( OutOfRangeException( SERROR("Dimensions of extents and origin are not equal"),
      CException::RECOVER, ERR_BADDIMENSION ) );
		
	for( size_t i = 0; i < dimensionSize; ++i )
	{
		originVec[i] = originArr_[i];
	}
  if ( isVerbose() )
    alog << LINFO << dump() << endl;
}

/**
 * \param extentsVec_ new extents of the region
 * \throws OutOfRangeException if dimension of parameter doesn't match that of the region's extents
 * \post new extents set
 */
void CDataSetRegion::resetExtents( const std::vector<size_t>& extentsVec_ )
  throw( OutOfRangeException )
{
	if ( extentsVec_.size() != extentsVec.size() )
		throw( OutOfRangeException( SERROR("Dimensions of extents and origin are not equal"),
			CException::RECOVER, ERR_BADDIMENSION ) );
		
	extentsVec = extentsVec_;
  if ( isVerbose() )
    alog << LINFO << dump() << endl;
}

/**
 * \param extentsArr_ new extents of the region
 * \param dimensionSize size of extentsArr_
 * \throws OutOfRangeException if dimension of parameter doesn't match that of the region's extents
 * \post new extents set
 */  
void CDataSetRegion::resetExtents( const size_t* extentsArr_, const size_t dimensionSize )	
	throw( OutOfRangeException )
{
	if ( dimensionSize != extentsVec.size() )
		throw( OutOfRangeException( SERROR("Dimensions of extents and origin are not equal"),
      CException::RECOVER, ERR_BADDIMENSION ) );
		
	for( size_t i = 0; i < dimensionSize; ++i )
	{
		extentsVec[i] = extentsArr_[i];
	}
  if ( isVerbose() )
    alog << LINFO << dump() << endl;
}	

/**
 * \param extentsVec_ vector representing the extents of the region
 * \param originVec_ vector representing the point of origin of the region 
 * \throws OutOfRangeException if dimensions of extents and origin differ
 * \post All data members are resetted properly.
 */
void CDataSetRegion::resetRegion( const std::vector<size_t>& originVec_,
	const std::vector<size_t>& extentsVec_ ) throw( OutOfRangeException )
{
  if ( extentsVec.size() != originVec.size() )
    throw( OutOfRangeException( SERROR("Dimensions of extents and origin are not equal"),
      CException::RECOVER, ERR_BADDIMENSION ) );
	originVec = originVec_;
	extentsVec = extentsVec_;
  if ( isVerbose() )
    alog << LINFO << dump() << endl;
}

/**
 * \param extentsArr_ array representing the extents of the region
 * \param originArr_ array representing the point of origin of the region
 * \param dimensionSize size of the given arrays
 * \post All data members are resetted properly.
 */ 
void CDataSetRegion::resetRegion( const size_t* extentsArr_, const size_t* originArr_, 
	const size_t dimensionSize ) throw()		
{
	for( size_t i = 0; i < dimensionSize; ++i )
	{
		extentsVec[i] = extentsArr_[i];
		originVec[i] = originArr_[i];
	}	
  if ( isVerbose() )
    alog << LINFO << dump() << endl;
}

/**
 * \returns information string
 */
const std::string CDataSetRegion::dump() const throw()
{
  ostringstream os;
  unsigned short usDimension = extentsVec.size();
  os << "- Dimension " << usDimension << endl;
  os << "- Extents ";
  for ( unsigned int i = 0; i < usDimension; ++i )
    os << extentsVec[i] << " ";
  os << endl << "- Origin ";
  for ( unsigned int i = 0; i < usDimension; ++i )
    os << originVec[i] << " ";
  os << endl;

  return CBase::dump() + os.str();
}
};
