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
 *          2005-11-21 Added documentation.                             *
 *                     Added missing exception handling                 *
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
 * \brief An object representing a specific region in a data set.
 *
 * CDataSetRegion can be used to define a specific region of a dataset. There is no direct
 * connection between data sets and regions, both are defined independently and can be used
 * jointly in classes like CDataSetRegionIterator.
 * \todo Write test cases and example code
 */
class CDataSetRegion : public CBase
{
private:
	/// Standard constructor
	CDataSetRegion();	
public:
/** \name Structors */
  //@{
	/// Constructor
  CDataSetRegion( const std::vector<size_t>& extentsVec_, const std::vector<size_t>& originVec_ ) 
		throw( OutOfRangeException );
	/// Constructor
	CDataSetRegion( const size_t* extentsArr_, const size_t* originArr_, const size_t dimensionSize ) 
		throw();
	/// Copy constructor
	CDataSetRegion( const CDataSetRegion& aNewRegion )
		throw();
	/// Destructor
  ~CDataSetRegion() 
		throw();
  //@}
/** \name Accessors */
  //@{
  /// Get origin position
  size_t getOrigin( ushort usOriginIndex ) 
    throw( OutOfRangeException );
  /// \overload
  std::vector<size_t> getOrigin() 
    throw();
  /// Get region extents
  size_t getExtent( ushort usExtentIndex ) 
    throw( OutOfRangeException );
  /// \overload
  std::vector<size_t> getExtents() 
    throw();
  //@}
/** \name Other methods */
  //@{
	/// Resets the origin position, but not the extents of the current region
	void resetOrigin( const std::vector<size_t>& originVec_ ) 
		throw( OutOfRangeException );
	/// \overload
	void resetOrigin( const size_t* originArr_, const size_t dimensionSize )	
		throw( OutOfRangeException );
	/// Resets the extents, but not the origin of the current region
	void resetExtents( const std::vector<size_t>& extentsVec_ ) 
		throw( OutOfRangeException );
	/// \overload
	void resetExtents( const size_t* extentsArr_, const size_t dimensionSize )	
		throw( OutOfRangeException );
	/// Resets the complete region
	void resetRegion( const std::vector<size_t>& originVec_, const std::vector<size_t>& extentsVec_ ) 
		throw( OutOfRangeException );
	/// \overload
	void resetRegion( const size_t* extentsArr_, const size_t* originArr_, const size_t dimensionSize )
		throw();
  const std::string dump() const
    throw();
  //@}  
private:
	std::vector<size_t> extentsVec; ///< Vector of region extents in dataset
	std::vector<size_t> originVec;  ///< Vector of region origin position in dataset
};

#include <cdatasetregion.tpp>

};

#endif
