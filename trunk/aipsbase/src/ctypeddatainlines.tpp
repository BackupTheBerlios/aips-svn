/************************************************************************
 * File: ctypeddatainlines.cpp                                          *
 * Project: AIPS                                                        *
 * Description: A dataset representing an array of a specific type      *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
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
template<typename valueType>
CTypedData<valueType>::CTypedData( const ushort usDimension_, const size_t* extentArr_,
  const size_t dataDimensionSize_ ) throw() 
	: CDataSet( usDimension_, extentArr_, dataDimensionSize_, "CTypedData", CTYPEDDATA_VERSION, 
	"CDataSet" )
{
	// Compute array size and resize internal vector
  arraySize = 1;
  for ( ushort i = 0; i < usDimension; i++ )
    arraySize *= extentVec[i];
  arraySize *= dataDimensionSize;
  dataVec.resize( arraySize );
  memset( &dataVec[0], 0, arraySize * sizeof( valueType ) );
  setDataRange( 0, 1 );
}

/**
 * \param usDimension_ Dimension of data field
 * \param extentVec_ Extends of field dimensions
 * \param dataDimensionSize_ Dimension of each field entry (for nonscalar fields)
 */
template<typename valueType> 
CTypedData<valueType>::CTypedData( const ushort usDimension_,
  const std::vector<size_t> extentVec_, const size_t dataDimensionSize_ ) throw()
  : CDataSet( usDimension_, extentVec_, dataDimensionSize_, "CTypedData", CTYPEDDATA_VERSION,
  "CDataSet" )
{
	// Compute array size and resize internal vector
  arraySize = 1;
  for ( ushort i = 0; i < usDimension; i++ )
    arraySize *= extentVec[i];
  arraySize *= dataDimensionSize;
  dataVec.resize( arraySize );
  memset( &dataVec[0], 0, arraySize * sizeof( valueType ) );
  setDataRange( 0, 1 );
}

/**
 * Convienience constructor for one-dimensional data fields
 * \param extent_ extent of field dimension
 * \param dataDimensionSize_ Dimension of each field entry (for nonscalar fields)
 */
template<typename valueType> 
CTypedData<valueType>::CTypedData( const size_t extent_, const size_t dataDimensionSize_ ) throw()
	: CDataSet( extent_, dataDimensionSize_, "CTypedData", CTYPEDDATA_VERSION,
	"CDataSet" )
{
	arraySize = extent_;
	arraySize *= dataDimensionSize;
  dataVec.resize( arraySize );
  memset( &dataVec[0], 0, arraySize * sizeof( valueType ) );
  setDataRange( 0, 1 );
}

/**
 * \param aDataSet Object to copy
 */
template<typename valueType> 
CTypedData<valueType>::CTypedData ( const CTypedData<valueType>& aDataSet ) throw()
  : CDataSet( aDataSet.usDimension, aDataSet.extentVec, aDataSet.dataDimensionSize, 
		"CTypedData", CTYPEDDATA_VERSION, "CDataSet" )
{
  arraySize = aDataSet.arraySize;
  dataVec.resize( arraySize );
  dataVec = aDataSet.dataVec;
  setMaximum( aDataSet.getMaximum() );
  setMinimum( aDataSet.getMinimum() );  
}

template<typename valueType> 
CTypedData<valueType>::~CTypedData() throw()
{
  dataVec.clear();
}

/*************
 * Operators *
 *************/

/**
 * \param aDataSet new value set
 */
template<typename valueType> inline 
CTypedData<valueType>& CTypedData<valueType>::operator=
  ( const CTypedData<valueType> &aDataSet ) throw()
{
  if ( &aDataSet == this ) return *this;

  usDimension = aDataSet.usDimension;
  dataDimensionSize = aDataSet.dataDimensionSize;
  extentVec = aDataSet.extentVec;
  setMaximum( aDataSet.getMaximum() );
  setMinimum( aDataSet.getMinimum() );
  arraySize = aDataSet.arraySize;
  dataVec.resize( arraySize );
  dataVec.assign( aDataSet.dataVec.begin(), aDataSet.dataVec.end() );  
  return *this;  
}

/**
 * All field elements will be set to newDefault.
 * \param newDefault The new value for all data set elements
 */
template<typename valueType> inline 
CTypedData<valueType>& CTypedData<valueType>::operator=
  ( const valueType newDefault ) throw()
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
template<typename valueType> inline
const std::type_info& CTypedData<valueType>::getType() const throw()
{
  return typeid( valueType );
}

/**
 * Get operator. This operator is slow but does range checking.
 * \param usX x-coordinate of element
 * \param usY y-coordinate of element
 * \param usZ z-coordinate of element
 * \param usW w-coordinate of element
 * \returns value of the indexed element
 */
template<typename valueType> inline 
const valueType& CTypedData<valueType>::get( const ushort usX, const ushort usY, const ushort usZ,
	const ushort usW ) const throw(OutOfRangeException)
{
  if ( usX > extentVec[0] || usY > extentVec[1] || usZ > extentVec[2] || usW > extentVec[3] )
    throw( OutOfRangeException( SERROR( "Index out of range"), CException::RECOVER, ERR_BADCOORDS ) );
  return dataVec[usX + usY * extentVec[0] + usZ * extentVec[0] * extentVec[1]
    + usW * extentVec[0] * extentVec[1] * extentVec[2]];
}

/**
 * \param usChannel data channel to be retrieved ( ignore for scalar data )
 * \returns a typed handle to the data array
 */
template<typename valueType> inline 
valueType* CTypedData<valueType>::getArray( ushort usChannel ) throw( OutOfRangeException )
{
  if ( usChannel >= dataDimensionSize )
    throw( OutOfRangeException( SERROR("Data dimension out of range"),
      CException::RECOVER, ERR_BADDIMENSION ) );
  return &(dataVec[arraySize / dataDimensionSize * usChannel]);
}

/** \returns a void handle to the data array */
template<typename valueType> inline
void* CTypedData<valueType>::getVoidArray() throw()
{
  return static_cast<void*>( &( dataVec[0] ) );
}

/** \returns the size of the internal Array (no. of elements) */
template<typename valueType> inline 
ulong CTypedData<valueType>::getArraySize() const throw()
{
  return arraySize;
}

/** \returns the size of the data block (in bytes) */
template<typename valueType> inline 
ulong CTypedData<valueType>::getDataSize() const throw()
{
	return arraySize * sizeof( valueType );
}

/**
 * Only works if minimum was set with setMinimum() before, otherwise the return value is undefined.
 * \returns the minimum value of the dataset.
 */
template<typename valueType> inline 
const valueType& CTypedData<valueType>::getMinimum() const throw()
{
  return theMinimum;
}

/**
 * Only works if maximum was set with setMaximum() before, otherwise the return value is undefined.
 * \returns the maximum value of the dataset.
 */
template<typename valueType> inline 
const valueType& CTypedData<valueType>::getMaximum() const throw()
{
  return theMaximum;
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
 * \returns value of the indexed element
 */
template<typename valueType> inline 
valueType& CTypedData<valueType>::set( const ushort usX, const ushort usY, const ushort usZ, 
	const ushort usW, const valueType newValue ) throw(OutOfRangeException)
{
  if ( usX > extentVec[0] || usY > extentVec[1] || usZ > extentVec[2] || usW > extentVec[3] )
    throw( OutOfRangeException( SERROR( "Index out of range"), CException::RECOVER, ERR_BADCOORDS ) );
  dataVec[usX + usY * extentVec[0] + usZ * extentVec[0] * extentVec[1]
    + usW * extentVec[0] * extentVec[1] * extentVec[2]] = newValue;
  if ( newValue < theMinimum ) theMinimum = newValue;
  if ( newValue > theMaximum ) theMaximum = newValue;
}

/**
 * \param newMinimum the new Minimum
 */
template<typename valueType> inline 
void CTypedData<valueType>::setMinimum( const valueType newMinimum ) throw()
{
  theMinimum = newMinimum;
}

/**
 * \param newMaximum the new Maximum
 */
template<typename valueType> inline 
void CTypedData<valueType>::setMaximum( const valueType newMaximum ) throw()
{
  theMaximum = newMaximum;
}

	/// Set a new minimum and maximum at once
template<typename valueType> inline 
void CTypedData<valueType>::setDataRange( const valueType newMinimum, const valueType newMaximum ) throw()
{
	theMaximum = newMaximum;
	theMinimum = newMinimum;
}

/// Adjust range so that the given value lies definitely in data range
template<typename valueType> inline 
void CTypedData<valueType>::adjustDataRange( const valueType theValue ) throw()
{
	if ( theValue > theMaximum )
		theMaximum = theValue;
	if ( theValue < theMinimum )
		theMinimum = theValue;		
}

/**
 * \param extentArr_ Array of new dataset extents
 * \param alignment Alignment of old data in the new dataset
 */
template<typename valueType> inline 
void CTypedData<valueType>::resize( const size_t* extendArr_, const EDataAlign 
	alignment ) throw()
{
	newArraySize = 1;
  for ( ushort i = 0; i < usDimension; i++ )
    newArraySize *= extentArr_[i];
  newArraySize *= dataDimensionSize;	
  std::vector<valueType> newDataVec( arraySize );
	size_t maxExtent[usDimension];
	for( size_t dims = 0; dims < dataDimensionSize; ++dims )
	{
		for ( ushort d = 0; d < usDimension; d++ )	
		{
			maxExtent[d] = std::min( extendArr_[d], extendVec[d] );
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
template<typename valueType> inline 
void CTypedData<valueType>::resize( const std::vector<size_t> extendVec_,
	const EDataAlign alignment )	throw()
{
	newArraySize = 1;
  for ( ushort i = 0; i < usDimension; i++ )
    newArraySize *= extentVec_[i];
  newArraySize *= dataDimensionSize;	
  std::vector<valueType> newDataVec( arraySize );
	size_t maxExtent[usDimension];
	for( size_t dims = 0; dims < dataDimensionSize; ++dims )
	{
		for ( ushort d = 0; d < usDimension; d++ )	
		{
			maxExtent[d] = std::min( extendVec_[d], extendVec[d] );
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
template<typename valueType> inline 
void CTypedData<valueType>::increaseDataDimension( const size_t addToDataDimension ) throw()
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
template<typename valueType> inline 
void CTypedData<valueType>::decreaseDataDimension( const size_t subFromDataDimension ) throw()
{
	typedef typename std::vector<valueType>::iterator localIterator;
	
	if ( addToDataDimension == 0 ) 
		return;
	ulong dimensionSize = 1;
	for( uint i = 0; i < usDimension; ++i )
		dimensionSize *= getExtent( i );
	dataDimensionSize -= addToDataDimension;	
	std::vector<valueType> newDataVec( dimensionSize * dataDimensionSize );
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
template<typename valueType> inline 
valueType& CTypedData<valueType>::operator()( const ushort usX ) throw()
{
  return dataVec[usX];
}

/**
 * Access operator for 2D-fields. No range checking is done.
 * \param usX x-coordinate of element
 * \param usY y-coordinate of element
 * \returns a reference to the indexed element
 */
template<typename valueType> inline 
valueType& CTypedData<valueType>::operator()( const ushort usX, const ushort usY ) throw()
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
template<typename valueType> inline 
valueType& CTypedData<valueType>::operator()( const ushort usX, const ushort usY, 
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
template<typename valueType> inline 
valueType& CTypedData<valueType>::operator()( const ushort usX, const ushort usY, 
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
template<typename valueType> inline 
const valueType& CTypedData<valueType>::operator()( const ushort usX ) const throw()
{
  return dataVec[usX];
}

/**
 * Access operator for 2D-fields. No range checking is done. Constant version.
 * \param usX x-coordinate of element
 * \param usY y-coordinate of element
 * \returns a reference to the indexed element
 */
template<typename valueType> inline 
const valueType& CTypedData<valueType>::operator()( const ushort usX, const ushort usY ) 
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
template<typename valueType> inline 
const valueType& CTypedData<valueType>::operator()( const ushort usX, const ushort usY, 
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
template<typename valueType> inline 
const valueType& CTypedData<valueType>::operator()( const ushort usX, const ushort usY, 
	const ushort usZ, const ushort usW ) const throw()
{
  return dataVec[usX + usY * extentVec[0] + usZ * extentVec[0] * extentVec[1]
    + usW * extentVec[0] * extentVec[1] * extentVec[2]];
}

/** \param ulIndex index to retrieve */
template<typename valueType> inline 
valueType& CTypedData<valueType>::operator[]( const ulong ulIndex ) throw()
{
	return dataVec[ ulIndex ];
}

/** \param ulIndex index to retrieve */
template<typename valueType> inline 
const valueType& CTypedData<valueType>::operator[]( const ulong ulIndex ) const throw()
{
	return dataVec[ ulIndex ];
}		

/*****************
 * Other methods *
 *****************/

template<typename valueType> const std::string CTypedData<valueType>::dump() const throw()
{
  std::ostringstream os;
  os << "ulArraySize: " << arraySize << " dataVec ptr : " << &dataVec << "\n"
    << "theMinimum ptr: " << &theMinimum << " theMaximum ptr: " << &theMaximum << std::endl;
  return CDataSet::dump() + os.str();
}

/**
 * \param aDataSet the other data set
 */
template<typename valueType> inline 
void CTypedData<valueType>::swap( CTypedData<valueType>& aDataSet ) throw()
{
	std::swap( theMinimum, aDataSet.theMinimum );
	std::swap( theMaximum, aDataSet.theMaximum );
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
template<typename valueType> inline
typename CTypedData<valueType>::iterator CTypedData<valueType>::begin() throw()
{
  return ( iterator( &dataVec[0], this ) );
}

/** \returns iterator for end of array */
template<typename valueType> inline
typename CTypedData<valueType>::iterator CTypedData<valueType>::end() throw()
{
  return ( iterator( &dataVec[0]+arraySize, this ) );
}

/** \returns reverse iterator for begin of array */
template<typename valueType> inline
typename CTypedData<valueType>::reverse_iterator CTypedData<valueType>::rbegin() throw()
{
  return ( reverse_iterator( end() ) );
}

/** \returns reverse iterator for end of array */
template<typename valueType> inline
typename CTypedData<valueType>::reverse_iterator CTypedData<valueType>::rend() throw()
{
  return ( reverse_iterator( begin() ) );
}

/** 
 * \returns an iterator to the given position 
 * \param usX position in 1D - dataset
 */
template<typename valueType> inline
typename CTypedData<valueType>::iterator CTypedData<valueType>::moveTo( const ushort usX ) throw()
{
	return iterator( &dataVec[usX], this );
}
	
/** 
 * \returns an iterator to the given position 
 * \param usX position in 1D - dataset
 * \param usY position in 1D - dataset
 */
template<typename valueType> inline
typename CTypedData<valueType>::iterator CTypedData<valueType>::moveTo( const ushort usX, 
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
template<typename valueType> inline
typename CTypedData<valueType>::iterator CTypedData<valueType>::moveTo( const ushort usX, 
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
template<typename valueType> inline
typename CTypedData<valueType>::iterator CTypedData<valueType>::moveTo( const ushort usX, 
	const ushort usY,	const ushort usZ, const ushort usW ) throw()
{
	return iterator( &dataVec[usX+usY*extentVec[0]+usZ*extentVec[0]*extentVec[1]
		+usW*extendVec[0]*extentVec[1]*extentVec[2]], this );
}

/*********************************
 * CTypedData::TypedDataIterator *
 *********************************/

/**
 * \param iterator iterator to move
 * \param amount amount to move iterator by (gets added to iterator position)
 */
template<typename T, typename U>
CTypedData<T>::TypedDataIterator<T,U> operator+( CTypedData<T>::TypedDataIterator<T,U> anIterator,
	ptrdiff_t amount )
{
	return CTypedData<T>::iterator( &(*iterator) + amount, iterator.getParent() );
}

/**
 * \param iterator iterator to move
 * \param amount amount to move iterator by (gets subtracted from iterator position)
 */
template<typename T, typename U>
CTypedData<T>::TypedDataIterator<T,U> operator-( CTypedData<T>::TypedDataIterator<T,U> anIterator, 
	ptrdiff_t amount )
{
	return CTypedData<T>::iterator( &(*iterator) - amount, iterator.getParent() );
}
