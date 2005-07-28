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

CDataSetRegion::CDataSetRegion( const std::vector<size_t>& extentsVec_, 
	const std::vector<size_t>& originVec_ ) throw() 	
	: CBase( "CDataSetRegion","0.1","CBase" ), extentsVec( extentsVec_ ), originVec( originVec_ )
{
}
	
CDataSetRegion::CDataSetRegion( const size_t* extentsArr_, const size_t* originArr_, 
	const size_t dimensionSize ) throw() : CBase( "CDataSetRegion","0.1","CBase" )
{
	for( size_t i = 0; i < dimensionSize; ++i )
	{
		extentsVec.push_back( extentsArr_[i] );
		originVec.push_back( originArr_[i] );
	}
}
	
CDataSetRegion::CDataSetRegion( const CDataSetRegion& newRegion ) throw()
	: CBase( "CDataSetRegion","0.1","CBase" ), extentsVec( newRegion.extentsVec ), 
		originVec( newRegion.originVec )
{
}
  
CDataSetRegion::~CDataSetRegion() throw()
{
}
	
void CDataSetRegion::resetOrigin( const std::vector<size_t>& originVec_ ) throw( OutOfRangeException )
{
	originVec = originVec_;
}
	
void CDataSetRegion::resetOrigin( const size_t* originArr_, const size_t dimensionSize )	
	throw( OutOfRangeException )	
{
	if ( dimensionSize != originVec.size() )
		throw( OutOfRangeException( SERROR("dimensionSize too big"), CException::RECOVER, ERR_BADDIMENSION ) );
		
	for( size_t i = 0; i < dimensionSize; ++i )
	{
		originVec[i] = originArr_[i];
	}
}
	
void CDataSetRegion::resetExtents( const std::vector<size_t>& extentsVec_ ) throw( OutOfRangeException )
{
	if ( extentsVec_.size() != extentsVec.size() )
		throw( OutOfRangeException( SERROR("Extents vector has wrong dimension"),
			CException::RECOVER, ERR_BADDIMENSION ) );
		
	extentsVec = extentsVec_;
}
	
void CDataSetRegion::resetExtents( const size_t* extentsArr_, const size_t dimensionSize )	
	throw( OutOfRangeException )
{
	if ( dimensionSize != extentsVec.size() )
		throw( OutOfRangeException( SERROR("dimensionSize too big"), CException::RECOVER, ERR_BADDIMENSION ) );
		
	for( size_t i = 0; i < dimensionSize; ++i )
	{
		extentsVec[i] = extentsArr_[i];
	}
}	
	
void CDataSetRegion::resetRegion( const std::vector<size_t>& originVec_, 
	const std::vector<size_t>& extentsVec_ ) throw( OutOfRangeException )
{
	originVec = originVec_;
	extentsVec = extentsVec_;
}
	
void CDataSetRegion::resetRegion( const size_t* extentsArr_, const size_t* originArr_, 
	const size_t dimensionSize ) throw( OutOfRangeException )		
{
	if ( dimensionSize != extentsVec.size() )
		throw( OutOfRangeException( SERROR("dimensionSize too big"), CException::RECOVER, ERR_BADDIMENSION ) );
	for( size_t i = 0; i < dimensionSize; ++i )
	{
		extentsVec[i] = extentsArr_[i];
		originVec[i] = originArr_[i];
	}	
}

};
