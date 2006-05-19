/************************************************************************
 * File: ctypeddatainlines.cpp                                          *
 * Project: AIPS                                                        *
 * Description: A dataset representing an array of a specific type      *
 *                                                                      *
 * Author: Hendrik Belitz (hbelitz@users.berlios.de)                          *
 *                                                                      *
 * Created: 2004-01-20                                                  *
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
 * \param usDimension_ Dimension of data field
 * \param extentArr_ Extends of field dimensions
 * \param dataDimensionSize_ Dimension of each field entry (for nonscalar fields)
 */
template<typename TValue>
CTypedData<TValue>::CTypedData( const ushort usDimension_,
  const size_t* extentArr_,
  const size_t dataDimensionSize_ ) throw() 
	: CDataSet( usDimension_, extentArr_, dataDimensionSize_,
   "CTypedData", CTYPEDDATA_VERSION, "CDataSet" )
{
	// Compute array size and resize internal vector
  arraySize = 1;
  for ( ushort i = 0; i < usDimension; i++ )
    arraySize *= extentVec[i];
  arraySize *= dataDimensionSize;
  dataVec.resize( arraySize );
  memset( &dataVec[0], 0, arraySize * sizeof( TValue ) );
  setDataRange( TTraitType::ZERO(), TTraitType::ONE() );
}

/**
 * \param usDimension_ Dimension of data field
 * \param extentVec_ Extends of field dimensions
 * \param dataDimensionSize_ Dimension of each field entry (for nonscalar fields)
 */
template<typename TValue>
CTypedData<TValue>::CTypedData( const ushort usDimension_,
  const std::vector<size_t> extentVec_,
  const size_t dataDimensionSize_ ) throw()
  : CDataSet( usDimension_, extentVec_, dataDimensionSize_, "CTypedData",
    CTYPEDDATA_VERSION, "CDataSet" )
{
	// Compute array size and resize internal vector
  arraySize = 1;
  for ( ushort i = 0; i < usDimension; i++ )
    arraySize *= extentVec[i];
  arraySize *= dataDimensionSize;
  dataVec.resize( arraySize );
  memset( &dataVec[0], 0, arraySize * sizeof( TValue ) );
  setDataRange( TTraitType::ZERO(), TTraitType::ONE() );
}

/**
 * Convienience constructor for one-dimensional data fields
 * \param extent_ extent of field dimension
 * \param dataDimensionSize_ Dimension of each field entry (for nonscalar fields)
 */
template<typename TValue>
CTypedData<TValue>::CTypedData( const size_t extent_, const size_t dataDimensionSize_ ) throw()
	: CDataSet( extent_, dataDimensionSize_, "CTypedData", CTYPEDDATA_VERSION,
	"CDataSet" )
{
	arraySize = extent_;
	arraySize *= dataDimensionSize;
  dataVec.resize( arraySize );
  memset( &dataVec[0], 0, arraySize * sizeof( TValue ) );
  setDataRange( TTraitType::ZERO(), TTraitType::ONE() );
}

/**
 * \param aDataSet Object to copy
 */
template<typename TValue>
CTypedData<TValue>::CTypedData ( const CTypedData<TValue>& aDataSet ) throw()
  : CDataSet( aDataSet.usDimension, aDataSet.extentVec, aDataSet.dataDimensionSize, 
		"CTypedData", CTYPEDDATA_VERSION, "CDataSet" )
{
  arraySize = aDataSet.arraySize;
  dataVec.resize( arraySize );
  dataVec = aDataSet.dataVec;
  theDataRange = aDataSet.theDataRange;
}

template<typename TValue>
CTypedData<TValue>::~CTypedData() throw()
{
  dataVec.clear();
}

/*************
 * Operators *
 *************/

/**
 * \param aDataSet new value set
 */
template<typename TValue> inline
CTypedData<TValue>& CTypedData<TValue>::operator=
  ( const CTypedData<TValue> &aDataSet ) throw()
{
  if ( &aDataSet == this ) return *this;

  usDimension = aDataSet.usDimension;
  dataDimensionSize = aDataSet.dataDimensionSize;
  extentVec = aDataSet.extentVec;
  theDataRange = aDataSet.theDataRange;
  arraySize = aDataSet.arraySize;
  dataVec.resize( arraySize );
  dataVec.assign( aDataSet.dataVec.begin(), aDataSet.dataVec.end() );  
  return *this;  
}

/**
 * All field elements will be set to newDefault.
 * \param newDefault The new value for all data set elements
 */
template<typename TValue> inline
CTypedData<TValue>& CTypedData<TValue>::operator=
  ( const TValue newDefault ) throw()
{
  for ( ulong i = 0; i < arraySize; i++ )
  {
    dataVec[i] = newDefault;
  }  
  return *this;
}

/*************
 * Accessors *
 *************/

/** \returns the type of the data objects stored inside the dataset */
template<typename TValue> inline
const std::type_info& CTypedData<TValue>::getType() const throw()
{
  return typeid( TValue );
}

/**
 * Get operator. This operator is slow but does range checking.
 * \param usX x-coordinate of element
 * \param usY y-coordinate of element
 * \param usZ z-coordinate of element
 * \param usW w-coordinate of element
 * \returns value of the indexed element
 */
template<typename TValue> inline
const TValue& CTypedData<TValue>::get( const ushort usX, const ushort usY, const ushort usZ,
	const ushort usW ) const throw(OutOfRangeException)
{
  if ( usX > extentVec[0] || usY > extentVec[1] || usZ > extentVec[2] || usW > extentVec[3] )
    throw( OutOfRangeException( SERROR( "Index out of range"), CException::RECOVER, ERR_BADCOORDS ) );
  return dataVec[usX + usY * extentVec[0] + usZ * extentVec[0] * extentVec[1]
    + usW * extentVec[0] * extentVec[1] * extentVec[2]];
}

/**
 * Get operator. This operator is slow but does range checking.
 * \param aPosition spatial coordinates of the element
 * \returns value of the indexed element
 */
template<typename TValue> inline
const TValue& CTypedData<TValue>::get( const TPoint2D aPosition )
  const throw(OutOfRangeException)
{
  if ( aPosition[0] < 0 || aPosition[0] > extentVec[0] )
    throw( OutOfRangeException( SERROR( "Index out of range"), CException::RECOVER, ERR_BADCOORDS ) );
  else if ( ( this->getDimension < 2 && aPosition[1] != 0 )
    || aPosition[1] < 0 || aPosition[1] > extentVec[1] )
    throw( OutOfRangeException( SERROR( "Index out of range"), CException::RECOVER, ERR_BADCOORDS ) );
  return dataVec[aPosition[0] + aPosition[1] * extentVec[0]];
}

/**
 * Get operator. This operator is slow but does range checking.
 * \param aPosition spatial coordinates of the element
 * \returns value of the indexed element
 */
template<typename TValue> inline
const TValue& CTypedData<TValue>::get( const TPoint3D aPosition )
  const throw(OutOfRangeException)
{
  if ( aPosition[0] < 0 || aPosition[0] > extentVec[0] )
    throw( OutOfRangeException( SERROR( "Index out of range"), CException::RECOVER, ERR_BADCOORDS ) );
  else if ( ( this->getDimension < 2 && aPosition[1] != 0 )
    || aPosition[1] < 0 || aPosition[1] > extentVec[1] )
    throw( OutOfRangeException( SERROR( "Index out of range"), CException::RECOVER, ERR_BADCOORDS ) );
  else if ( ( this->getDimension < 3 && aPosition[2] != 0 )
    || aPosition[2] < 0 || aPosition[2] > extentVec[2] )
    throw( OutOfRangeException( SERROR( "Index out of range"), CException::RECOVER, ERR_BADCOORDS ) );
  return dataVec[aPosition[0] + aPosition[1] * extentVec[0] + aPosition[2] * extentVec[0] * extentVec[1]];
}

/**
 * \param usChannel data channel to be retrieved ( ignore for scalar data )
 * \returns a typed handle to the data array
 */
template<typename TValue> inline
TValue* CTypedData<TValue>::getArray( ushort usChannel ) throw( OutOfRangeException )
{
  if ( usChannel >= dataDimensionSize )
    throw( OutOfRangeException( SERROR("Data dimension out of range"),
      CException::RECOVER, ERR_BADDIMENSION ) );
  return &(dataVec[arraySize / dataDimensionSize * usChannel]);
}

/** \returns a void handle to the data array */
template<typename TValue> inline
void* CTypedData<TValue>::getVoidArray() throw()
{
  return static_cast<void*>( &( dataVec[0] ) );
}

/** \returns the size of the internal Array (no. of elements) */
template<typename TValue> inline
ulong CTypedData<TValue>::getArraySize() const throw()
{
  return arraySize;
}

/** \returns the size of the data block (in bytes) */
template<typename TValue> inline
ulong CTypedData<TValue>::getDataSize() const throw()
{
	return arraySize * sizeof( TValue );
}

/************
 * Mutators *
 ************/
 
/**
 * Set operator. This operator is slow but does range checking and will
 * assign new minimum and maximum values for the field automatically.
 * \param usX x-coordinate of element
 * \param usY y-coordinate of element
 * \param usZ z-coordinate of element
 * \param usW w-coordinate of element
 * \param newValue value of the indexed element
 */
template<typename TValue> inline
void CTypedData<TValue>::set( const ushort usX, const ushort usY, const ushort usZ,
	const ushort usW, const TValue newValue ) throw(OutOfRangeException)
{
  if ( usX > extentVec[0] || usY > extentVec[1] || usZ > extentVec[2] || usW > extentVec[3] )
    throw( OutOfRangeException( SERROR( "Index out of range"), CException::RECOVER, ERR_BADCOORDS ) );
  dataVec[usX + usY * extentVec[0] + usZ * extentVec[0] * extentVec[1]
    + usW * extentVec[0] * extentVec[1] * extentVec[2]] = newValue;
  theDataRange.updateRange( newValue );
}

/**
 * Set operator. This operator is slow but does range checking and will
 * assign new minimum and maximum values for the field automatically.
 * \param aPosition spatial coordinates of element
 * \param newValue value of the indexed element
 */
template<typename TValue> inline
void CTypedData<TValue>::set( const TPoint2D aPosition, const TValue newValue )
  throw(OutOfRangeException)
{
  if ( aPosition[0] < 0 || aPosition[0] > extentVec[0] )
    throw( OutOfRangeException( SERROR( "Index out of range"), CException::RECOVER, ERR_BADCOORDS ) );
  else if ( ( this->getDimension < 2 && aPosition[1] != 0 )
    || aPosition[1] < 0 || aPosition[1] > extentVec[1] )
    throw( OutOfRangeException( SERROR( "Index out of range"), CException::RECOVER, ERR_BADCOORDS ) );
  dataVec[aPosition[0] + aPosition[1] * extentVec[0]] = newValue;
  theDataRange.updateRange( newValue );
}

/// Access operator with range checking and automatic min/max assignment
template<typename TValue> inline
void CTypedData<TValue>::set( const TPoint3D aPosition, const TValue newValue )
  throw(OutOfRangeException)
{
  if ( aPosition[0] < 0 || aPosition[0] > extentVec[0] )
    throw( OutOfRangeException( SERROR( "Index out of range"), CException::RECOVER, ERR_BADCOORDS ) );
  else if ( ( this->getDimension < 2 && aPosition[1] != 0 )
    || aPosition[1] < 0 || aPosition[1] > extentVec[1] )
    throw( OutOfRangeException( SERROR( "Index out of range"), CException::RECOVER, ERR_BADCOORDS ) );
  else if ( ( this->getDimension < 3 && aPosition[2] != 0 )
    || aPosition[2] < 0 || aPosition[2] > extentVec[2] )
    throw( OutOfRangeException( SERROR( "Index out of range"), CException::RECOVER, ERR_BADCOORDS ) );
  dataVec[aPosition[0] + aPosition[1] * extentVec[0] + aPosition[2] * extentVec[0] * extentVec[1]]
    = newValue;
  theDataRange.updateRange( newValue );
}

/**
 * \param newMinimum the new Minimum
 */
template<typename TValue> inline
void CTypedData<TValue>::setMinimum( const TValue newMinimum ) throw()
{
  theDataRange.setMinimum( newMinimum );
}

/**
 * \param newMaximum the new Maximum
 */
template<typename TValue> inline
void CTypedData<TValue>::setMaximum( const TValue newMaximum ) throw()
{
  theDataRange.setMaximum( newMaximum );
}

	/// Set a new minimum and maximum at once
template<typename TValue> inline
void CTypedData<TValue>::setDataRange( const TValue newMinimum, const TValue newMaximum ) throw()
{
	setMaximum( newMaximum );
	setMinimum( newMinimum );
}

/// Adjust range so that the given value lies definitely in data range
template<typename TValue> inline
void CTypedData<TValue>::adjustDataRange( const TValue theValue ) throw()
{
	theDataRange.updateRange( theValue );
}

/**
 * \param extentArr_ Array of new dataset extents
 * \param alignment Alignment of old data in the new dataset
 */
template<typename TValue> inline
void CTypedData<TValue>::resize( const size_t* extentArr_, const EDataAlign
	alignment ) throw()
{
	size_t newArraySize = 1;
  for ( ushort i = 0; i < usDimension; i++ )
    newArraySize *= extentArr_[i];
  newArraySize *= dataDimensionSize;	
  std::vector<TValue> newDataVec( arraySize );
	size_t maxExtent[usDimension];
	for( size_t dims = 0; dims < dataDimensionSize; ++dims )
	{
		for ( ushort d = 0; d < usDimension; d++ )	
		{
			maxExtent[d] = std::min( extentArr_[d], extentVec[d] );
		}
		ushort maxW = 1;		
		if ( usDimension > 3 ) maxW = maxExtent[3];
		ushort maxZ = 1;		
		if ( usDimension > 2 ) maxW = maxExtent[2];
		ushort maxY = 1;		
		if ( usDimension > 1 ) maxW = maxExtent[1];
		ushort maxX = maxExtent[0];		
		for( ushort w = 0; w < maxW; ++w )
		 for( ushort z = 0; z < maxZ; ++z )
		  for( ushort y = 0; y < maxY; ++y )
		   for( ushort x = 0; x < maxX; ++x )
			   newDataVec[ x + maxX * y + ( maxX * maxY ) * z + ( maxX * maxY * maxZ ) * w] 
				 	= dataVec[ x + maxX * y + ( maxX * maxY ) * z + ( maxX * maxY * maxZ ) * w];
	}
	dataVec = newDataVec;
}

/**
 * \param extentVec_ Vector of new dataset extents
 * \param alignment Alignment of old data in the new dataset
 */
template<typename TValue> inline
void CTypedData<TValue>::resize( const std::vector<size_t> extentVec_,
	const EDataAlign alignment )	throw()
{
	size_t newArraySize = 1;
  for ( ushort i = 0; i < usDimension; i++ )
    newArraySize *= extentVec_[i];
  newArraySize *= dataDimensionSize;	
  std::vector<TValue> newDataVec( arraySize );
	size_t maxExtent[usDimension];
	for( size_t dims = 0; dims < dataDimensionSize; ++dims )
	{
		for ( ushort d = 0; d < usDimension; d++ )	
		{
			maxExtent[d] = std::min( extentVec_[d], extentVec[d] );
		}
		ushort maxW = 1;		
		if ( usDimension > 3 ) maxW = maxExtent[3];
		ushort maxZ = 1;		
		if ( usDimension > 2 ) maxW = maxExtent[2];
		ushort maxY = 1;		
		if ( usDimension > 1 ) maxW = maxExtent[1];
		ushort maxX = maxExtent[0];		
		for( ushort w = 0; w < maxW; ++w )
		 for( ushort z = 0; z < maxZ; ++z )
		  for( ushort y = 0; y < maxY; ++y )
		   for( ushort x = 0; x < maxX; ++x )
			   newDataVec[ x + maxX * y + ( maxX * maxY ) * z + ( maxX * maxY * maxZ ) * w] 
				 	= dataVec[ x + maxX * y + ( maxX * maxY ) * z + ( maxX * maxY * maxZ ) * w];
	}
	dataVec = newDataVec;
}
	
/**
 * \param addToDataDimension Increase of dimension
 */
template<typename TValue> inline
void CTypedData<TValue>::increaseDataDimension( const size_t addToDataDimension ) throw()
{
	if ( addToDataDimension == 0 ) 
		return;
	ulong dimensionSize = 1;
	for( uint i = 0; i < usDimension; ++i )
		dimensionSize *= getExtent( i );
	dataDimensionSize += addToDataDimension;
	dataVec.resize( dimensionSize * dataDimensionSize );
}

/**
 * \param subFromDataDimension Decrease of dimension
 */
template<typename TValue> inline
void CTypedData<TValue>::decreaseDataDimension( const size_t subFromDataDimension ) throw()
{
	typedef typename std::vector<TValue>::iterator localIterator;
	
	if ( subFromDataDimension == 0 ) 
		return;
	ulong dimensionSize = 1;
	for( uint i = 0; i < usDimension; ++i )
		dimensionSize *= getExtent( i );
	dataDimensionSize -= subFromDataDimension;	
	std::vector<TValue> newDataVec( dimensionSize * dataDimensionSize );
	localIterator newIt = newDataVec.begin();
	localIterator oldIt = dataVec.begin();
	while( newIt != newDataVec.end() )
	{
		*newIt = *oldIt;
		++newIt; ++oldIt;
	}
	dataVec = newDataVec;
}

/********************
 * Access operators *
 ********************/

/**
 * Access operator for 1D-fields. No range checking is done.
 * \param usX index of element
 * \returns a reference to the indexed element
 */
template<typename TValue> inline
TValue& CTypedData<TValue>::operator()( const ushort usX ) throw()
{
  return dataVec[usX];
}

/**
 * Access operator for 2D-fields. No range checking is done.
 * \param usX x-coordinate of element
 * \param usY y-coordinate of element
 * \returns a reference to the indexed element
 */
template<typename TValue> inline
TValue& CTypedData<TValue>::operator()( const ushort usX, const ushort usY ) throw()
{
  return dataVec[usX + usY * extentVec[0]];
}

/**
 * Access operator for 3D-fields. No range checking is done.
 * \param usX x-coordinate of element
 * \param usY y-coordinate of element
 * \param usZ z-coordinate of element
 * \returns a reference to the indexed element
 */
template<typename TValue> inline
TValue& CTypedData<TValue>::operator()( const ushort usX, const ushort usY,
	const ushort usZ ) throw()
{
  return dataVec[usX + usY * extentVec[0] + usZ * extentVec[0] * extentVec[1]];
}

/**
 * Access operator for 4D-fields. No range checking is done.
 * \param usX x-coordinate of element
 * \param usY y-coordinate of element
 * \param usZ z-coordinate of element
 * \param usW w-coordinate of element
 * \returns a reference to the indexed element
 */
template<typename TValue> inline
TValue& CTypedData<TValue>::operator()( const ushort usX, const ushort usY,
	const ushort usZ, const ushort usW ) throw()
{
  return dataVec[usX + usY * extentVec[0] + usZ * extentVec[0] * extentVec[1]
    + usW * extentVec[0] * extentVec[1] * extentVec[2]];
}

/**
 * Access operator for 1D-fields. No range checking is done. Constant version.
 * \param usX x-coordinate of element
 * \returns a reference to the indexed element
 */
template<typename TValue> inline
const TValue& CTypedData<TValue>::operator()( const ushort usX ) const throw()
{
  return dataVec[usX];
}

/**
 * Access operator for 2D-fields. No range checking is done. Constant version.
 * \param usX x-coordinate of element
 * \param usY y-coordinate of element
 * \returns a reference to the indexed element
 */
template<typename TValue> inline
const TValue& CTypedData<TValue>::operator()( const ushort usX, const ushort usY )
	const throw()
{
  return dataVec[usX + usY * extentVec[0]];
}

/**
 * Access operator for 3D-fields. No range checking is done. Constant version.
 * \param usX x-coordinate of element
 * \param usY y-coordinate of element
 * \param usZ z-coordinate of element
 * \returns a reference to the indexed element
 */
template<typename TValue> inline
const TValue& CTypedData<TValue>::operator()( const ushort usX, const ushort usY,
	const ushort usZ ) const throw()
{
  return dataVec[usX + usY * extentVec[0] + usZ * extentVec[0] * extentVec[1]];
}

/**
 * Access operator for 4D-fields. No range checking is done. Constant version.
 * \param usX x-coordinate of element
 * \param usY y-coordinate of element
 * \param usZ z-coordinate of element
 * \param usW w-coordinate of element
 * \returns a reference to the indexed element
 */
template<typename TValue> inline
const TValue& CTypedData<TValue>::operator()( const ushort usX, const ushort usY,
	const ushort usZ, const ushort usW ) const throw()
{
  return dataVec[usX + usY * extentVec[0] + usZ * extentVec[0] * extentVec[1]
    + usW * extentVec[0] * extentVec[1] * extentVec[2]];
}

/** \param ulIndex index to retrieve */
template<typename TValue> inline
TValue& CTypedData<TValue>::operator[]( const ulong ulIndex ) throw()
{
	return dataVec[ ulIndex ];
}

/** \param aPosition spatial coordinates of element to retrieve */
template<typename TValue> inline
TValue& CTypedData<TValue>::operator[]( const TPoint2D aPosition ) throw()
{
  return dataVec[ aPosition[0] + aPosition[1] * extentVec[0] ];
}
  
  /** \param aPosition spatial coordinates of element to retrieve */
template<typename TValue> inline
TValue& CTypedData<TValue>::operator[]( const TPoint3D aPosition ) throw()
{
  return dataVec[ aPosition[0] + aPosition[1] * extentVec[0]
    + aPosition[2] * extentVec[0] * extentVec[1] ];
}

/** \param ulIndex index to retrieve */
template<typename TValue> inline
const TValue& CTypedData<TValue>::operator[]( const ulong ulIndex ) const throw()
{
	return dataVec[ ulIndex ];
}		

/** \param ulIndex index to retrieve */
template<typename TValue> inline
const TValue& CTypedData<TValue>::operator[]( const TPoint2D aPosition ) const throw()
{
  return dataVec[ aPosition[0] + aPosition[1] * extentVec[0] ];
}   

/** \param ulIndex index to retrieve */
template<typename TValue> inline
const TValue& CTypedData<TValue>::operator[]( const TPoint3D aPosition ) const throw()
{
  return dataVec[ aPosition[0] + aPosition[1] * extentVec[0]
    + aPosition[2] * extentVec[0] * extentVec[1] ];
}   

/*****************
 * Other methods *
 *****************/

template<typename TValue> const std::string CTypedData<TValue>::dump() const throw()
{
  std::ostringstream os;
  os << "ulArraySize: " << arraySize << " dataVec ptr : " << &dataVec 
    << std::endl;
  return CDataSet::dump() + os.str();
}

/**
 * \param aDataSet the other data set
 */
template<typename TValue> inline
void CTypedData<TValue>::swap( CTypedData<TValue>& aDataSet ) throw()
{
// 	std::swap( theMinimum, aDataSet.theMinimum );
// 	std::swap( theMaximum, aDataSet.theMaximum );
	std::swap( theDataRange, aDataSet.theDataRange );
	std::swap( arraySize, aDataSet.arraySize );
	dataVec.swap( aDataSet.dataVec );
	std::swap( usDimension, aDataSet.usDimension );
  extentVec.swap( aDataSet.extentVec ); 
  std::swap( dataDimensionSize, aDataSet.dataDimensionSize );
}

/*********************************
 * CTypedData::TypedDataIterator *
 *********************************/

/*******************************
 * Iterator generating methods *
 *******************************/

/** \returns iterator for begin of array */
template<typename TValue> inline
typename CTypedData<TValue>::iterator CTypedData<TValue>::begin() throw()
{
  return ( iterator( &dataVec[0], this ) );
}

/** \returns iterator for end of array */
template<typename TValue> inline
typename CTypedData<TValue>::iterator CTypedData<TValue>::end() throw()
{
  return ( iterator( &dataVec[0]+arraySize, this ) );
}

/** \returns reverse iterator for begin of array */
template<typename TValue> inline
typename CTypedData<TValue>::reverse_iterator CTypedData<TValue>::rbegin() throw()
{
  return ( reverse_iterator( end() ) );
}

/** \returns reverse iterator for end of array */
template<typename TValue> inline
typename CTypedData<TValue>::reverse_iterator CTypedData<TValue>::rend() throw()
{
  return ( reverse_iterator( begin() ) );
}

/** 
 * \returns an iterator to the given position 
 * \param usX position in 1D - dataset
 */
template<typename TValue> inline
typename CTypedData<TValue>::iterator CTypedData<TValue>::moveTo( const ushort usX ) throw()
{
	return iterator( &dataVec[usX], this );
}
	
/** 
 * \returns an iterator to the given position 
 * \param usX position in 1D - dataset
 * \param usY position in 1D - dataset
 */
template<typename TValue> inline
typename CTypedData<TValue>::iterator CTypedData<TValue>::moveTo( const ushort usX,
	const ushort usY ) throw()
{
	return iterator( &dataVec[usX+usY*extentVec[0]], this );
}
	
/** 
 * \returns an iterator to the given position 
 * \param usX position in 1D - dataset
 * \param usY position in 1D - dataset
 * \param usZ position in 1D - dataset
 */
template<typename TValue> inline
typename CTypedData<TValue>::iterator CTypedData<TValue>::moveTo( const ushort usX,
	const ushort usY,	const ushort usZ ) throw()
{
	return iterator( &dataVec[usX+usY*extentVec[0]+usZ*extentVec[0]*extentVec[1]], this );
}
	
/** 
 * \returns an iterator to the given position 
 * \param usX position in 1D - dataset
 * \param usY position in 1D - dataset
 * \param usZ position in 1D - dataset
 * \param usW position in 1D - dataset
 */
template<typename TValue> inline
typename CTypedData<TValue>::iterator CTypedData<TValue>::moveTo( const ushort usX,
	const ushort usY,	const ushort usZ, const ushort usW ) throw()
{
	return iterator( &dataVec[usX+usY*extentVec[0]+usZ*extentVec[0]*extentVec[1]
		+usW*extentVec[0]*extentVec[1]*extentVec[2]], this );
}

