/************************************************************************
 * File: cdatasetregion.tpp                                             *
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

/** 
 * \param usOriginIndex Dimensional index of request
 * \returns Position of origin for the given index
 * \throws OutOfRangeException if given index is out of range
 */
inline size_t CDataSetRegion::getOrigin( unsigned short usOriginIndex ) throw( OutOfRangeException )
{ 
	if ( usOriginIndex > originVec.size() ) 
		throw( OutOfRangeException( SERROR("dimensionSize too big"), CException::RECOVER, ERR_BADDIMENSION ) );
		
	return originVec[usOriginIndex];
}			

/**
 * \param usExtentIndex Dimensional index of request
 * \returns Region extent for the given index
 * \throws OutOfRangeException if given index is out of range
 */ 
inline size_t CDataSetRegion::getExtent( unsigned short usExtentIndex ) throw( OutOfRangeException )
{ 
	if ( usExtentIndex > extentsVec.size() ) 
		throw( OutOfRangeException( SERROR("dimensionSize too big"), CException::RECOVER, ERR_BADDIMENSION ) );
		
	return extentsVec[usExtentIndex];
}		
	
/** \returns Position of region origin */
inline std::vector<size_t> CDataSetRegion::getOrigin() throw()
{ 
	return originVec; 
}

/** \returns Region extents */	
inline std::vector<size_t> CDataSetRegion::getExtents() throw() 
{ 
	return extentsVec; 
}
