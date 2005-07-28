/************************************************************************
 * File: cdatasetregioniterator.tpp                                     *
 * Project: AIPS                                                        *
 * Description: Iterates over a specific image region                   *
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

/*************
 * Structors *
 *************/
 
/**
 * \param aDataSet associated dataset
 * \param aRegion associated region 
 */
template<typename valueType>    
CDataSetRegionIterator::CDataSetRegionIterator( CTypedData<valueType>& aDataSet, const CDataSetRegion& aRegion )
	: theDataSet( aDataSet ), dataIterator( aDataSet.begin() ), theRegion( aRegion ), 
		counterVec( aDataSet.getDimension() ), dimensionSize( aDataSet.getDimension()-1 )
{
	ulong ulZeroOffset = 0;
	ulong ulEndOffset = 0;
	jumpOffsetVec.resize( theDataSet.getDimension() );
	for( ushort i = aDataSet.getDimension() - 1; i < aDataSet.getDimension(); --i )
	{
		if ( i == 0 )
		{
			ulZeroOffset += theRegion.getOrigin( 0 );
			ulEndOffset += theRegion.getExtent( 0 );
		}
		else if ( i == aDataSet.getDimension() - 1 )
		{
			ulZeroOffset += theRegion.getOrigin( i ) * theDataSet.getExtent( i - 1 );
			ulEndOffset += theRegion.getExtent( i ) * theDataSet.getExtent( i - 1 );
		}
		else
		{
			ulZeroOffset += theRegion.getOrigin( i );
			ulZeroOffset *= theDataSet.getExtent( i - 1 );
			ulEndOffset += theRegion.getExtent( i );
			ulEndOffset *= theDataSet.getExtent( i - 1 );
		}
		ulong ulOffset = 0; ulong ulOp = 1; ulong ulOm = 1;
		for( ushort j = i; j < theDataSet.getDimension(); j-- )
		{
			ulOp *= theDataSet.getExtent( j );							
			if ( j == i ) ulOm *= theRegion.getExtent( j ) + 1;
			else ulOm *= theDataSet.getExtent( j );
			ulOffset = ulOp - ulOm;
		}				
		jumpOffsetVec[i] = ulOffset;
	}
	
	regionBegin = theDataSet.begin() + ulZeroOffset;
	regionEnd = regionBegin + ulEndOffset + 1;
	dataIterator = regionBegin;
	for( int i = 0; i < dimensionSize + 1; ++i )
		regionExtentVec.push_back( theRegion.getExtent( i ) );
}

template<typename valueType>
CDataSetRegionIterator::~CDataSetRegionIterator()
{
}

/*************
 * Operators *
 *************/

/** \returns actual value at iterator position */
template<typename valueType>		
valueType& CDataSetRegionIterator::operator*() 
{ 
	return *dataIterator; 
}

/** \returns actual iterator */
template<typename valueType>		
typename CTypedData<valueType>::iterator& CDataSetRegionIterator::operator->() 
{ 
	return dataIterator; 
}

template<typename valueType>		
void CDataSetRegionIterator::operator++() 
{ 
	counterVec[0]++; dataIterator++;
	if ( counterVec[0] > regionExtentVec[0] )
	{
		for( ushort i = 0; i < dimension; ++i )
		{
			if ( counterVec[i] > regionExtentVec[i] )
			{
				counterVec[i] = 0;
				counterVec[i+1]++;
				dataIterator += jumpOffsetVec[i];						
			}
		}
		if ( counterVec[dimension] > regionExtentVec[dimension] )
			dataIterator = regionEnd;
	}			
}

/*****************
 * Other methods *
 *****************/
 
template<typename valueType>		
void CDataSetRegionIterator::setToBegin()
{
	dataIterator = regionBegin;
	for( ushort i = 0; i < dimension+1; ++i )
		ct[i]=0;
}
    
/** \returns new iterator poiting to region start */
template<typename valueType>
CDataSetRegionIterator::CDataSetRegionIterator<valueType> begin() 
{ 
	CDataSetRegionIterator<valueType> newIterator( *this );
	newIterator.dataIterator = regionBegin;
	return newIterator;
}

/** \returns new iterator poiting to region end */
template<typename valueType>		
CDataSetRegionIterator::CDataSetRegionIterator<valueType> end() 
{ 
	CDataSetRegionIterator<valueType> newIterator( *this );
	newIterator.dataIterator = regionEnd;
	return newIterator;
}

/** \returns true if dataIterator is at end of region */
template<typename valueType>		
bool CDataSetRegionIterator::atEnd()
{
	return ( dataIterator == regionEnd );
}

/** 
 * \param usX reference to store x position of iterator in
 * \param usY reference to store y position of iterator in
 */
template<typename valueType>		
void CDataSetRegionIterator::getPos( ushort& usX, ushort& usY )
{
	dataIterator.getPos( usX, usY );
}

/** 
 * \param usX reference to store x position of iterator in
 * \param usY reference to store y position of iterator in
 * \param usZ reference to store z position of iterator in
 */
template<typename valueType>		
void CDataSetRegionIterator::getPos( ushort& usX, ushort& usY, ushort& usZ )
{
	dataIterator.getPos( usX, usY, usZ );
}

/** 
 * \param usX reference to store x position of iterator in
 * \param usY reference to store y position of iterator in
 * \param usZ reference to store z position of iterator in
 * \param usW reference to store w position of iterator in
 */
template<typename valueType>		
void CDataSetRegionIterator::getPos( ushort& usX, ushort& usY, ushort& usZ, ushort& usW )
{
	dataIterator.getPos( usX, usY, usZ, usW );
}
