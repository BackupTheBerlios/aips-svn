/************************************************************************
 * File: cdatasetregioniterator.h                                       *
 * Project: AIPS                                                        *
 * Description: Iterates over a specific image region                   *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status : Alpha                                                       *
 * Created: 2004-09-17                                                  *
 * Changed: 2004-12-22 Moved inline members to cdatasetregioniterator.tp*
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef AIPSCDATASETREGIONITERATOR_H
#define AIPSCDATASETREGIONITERATOR_H

#include <ctypeddata.h>
#include <cdatasetregion.h>

namespace aips {

/**
@author HendrikBelitz
*/
template<typename valueType>
class CDataSetRegionIterator
{
private:
	/// Standard constructor
	CDataSetRegionIterator();
	/// Copy constructor
	CDataSetRegionIterator( const CDataSetRegionIterator& );
	/// Assignment operator
	CDataSetRegionIterator operator=( const CDataSetRegionIterator& );
public:
/* Structors */
	/// Constructor
  CDataSetRegionIterator( CTypedData<valueType>& aDataSet, const CDataSetRegion& aRegion );
	/// Destructor
	~CDataSetRegionIterator();
/* Operators */	
	/// Dereferecing operator
	valueType& operator*();
	/// Dereferecing operator
	typename CTypedData<valueType>::iterator& operator->();
	/// Increment operator
	void operator++();
	//	valueType operator--();
/* Other Methods */	
	/// Sets iterator to region beginning
	void setToBegin();
	/// Returns an iterator pointing to region start
  CDataSetRegionIterator<valueType> begin();
	/// Returns an iterator pointing to region end
	CDataSetRegionIterator<valueType> end();
	/// Returns true if iterator is at region end
	bool atEnd();
	/// Returns actual position of the iterator
	void getPos( ushort& usX, ushort& usY );
	/// Returns actual position of the iterator
	void getPos( ushort& usX, ushort& usY, ushort& usZ );
	/// Returns actual position of the iterator
	void getPos( ushort& usX, ushort& usY, ushort& usZ, ushort& usW );
private:
	CTypedData<valueType>& theDataSet; ///< Reference to associated dataset
	typename CTypedData<valueType>::iterator dataIterator; ///< Actual iterator
	typename CTypedData<valueType>::iterator regionBegin;  ///< Iterator pointing to region begin
	typename CTypedData<valueType>::iterator regionEnd;    ///< Iterator pointing to region end
	CDataSetRegion theRegion; ///< Associated set region
	const size_t dimensionSize; ///< Dimension of dataset
	std::vector<size_t> counterVec; ///< Internal counter for fast positioning
	std::vector<size_t> regionExtentVec; ///< Region extents
	std::vector<ulong> jumpOffsetVec; ///< Precomputed jump offsets for fast positioning
};

#include <cdatasetregioniterator.tpp>

};

#endif
