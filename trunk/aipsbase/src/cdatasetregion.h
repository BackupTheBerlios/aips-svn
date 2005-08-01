/************************************************************************
 * File: cdatasetregion.h                                               *
 * Project: AIPS                                                        *
 * Description: Defines a sub region of a given dataset                 *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Version: 0.2                                                         *
 * Status : Alpha                                                       *
 * Created: 2004-09-17                                                  *
 * Changed: 2004-12-22 Moved inline members to cdatasetregion.tpp       *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef AIPSCDATASETREGION_H
#define AIPSCDATASETREGION_H

#include <cexception.h>
#include <cdataset.h>
#include <aipsnumeric.h>

namespace aips {

/**
@author HendrikBelitz
*/
class CDataSetRegion : public CBase
{
private:
	/// Standard constructor
	CDataSetRegion();	
public:
	/// Constructor
  CDataSetRegion( const std::vector<size_t>& extentsVec_, const std::vector<size_t>& originVec_ ) 
		throw();
	/// Constructor
	CDataSetRegion( const size_t* extentsArr_, const size_t* originArr_, const size_t dimensionSize ) 
		throw();
	/// Copy constructor
	CDataSetRegion( const CDataSetRegion& newRegion ) 
		throw();
	/// Destructor
  ~CDataSetRegion() 
		throw();
	/// Resets the origin position of the current region
	void resetOrigin( const std::vector<size_t>& originVec_ ) 
		throw( OutOfRangeException );
	/// Resets the origin position of the current region
	void resetOrigin( const size_t* originArr_, const size_t dimensionSize )	
		throw( OutOfRangeException );
	/// Resets the extents of the current region
	void resetExtents( const std::vector<size_t>& extentsVec_ ) 
		throw( OutOfRangeException );
	/// Resets the extents of the current region
	void resetExtents( const size_t* extentsArr_, const size_t dimensionSize )	
		throw( OutOfRangeException );
	/// Resets the complete region
	void resetRegion( const std::vector<size_t>& originVec_, const std::vector<size_t>& extentsVec_ ) 
		throw( OutOfRangeException );
	/// Resets the complete region
	void resetRegion( const size_t* extentsArr_, const size_t* originArr_, const size_t dimensionSize )
		throw( OutOfRangeException );	
	/// Get origin position
	size_t getOrigin( ushort usOriginIndex ) 
		throw( OutOfRangeException );
	/// Get region extents
	size_t getExtent( ushort usExtentIndex ) 
		throw( OutOfRangeException );
	/// Get origin position
	std::vector<size_t> getOrigin()	
		throw();
	/// Get region extents
	std::vector<size_t> getExtents() 
		throw();
private:
	std::vector<size_t> extentsVec; ///< Vector of region extents in dataset
	std::vector<size_t> originVec;  ///< Vector of region origin position in dataset
};

#include <cdatasetregion.tpp>

};

#endif
