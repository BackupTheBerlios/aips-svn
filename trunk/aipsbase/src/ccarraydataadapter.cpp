/************************************************************************
 * File: ccarraydataadapter.cpp                                         *
 * Project: AIPS base library                                           *
 * Description:   A data adapter to convert aips datasets into C-style  *
 *                arrays and vice versa                                 *
 *                                                                      *
 * Author: HendrikBelitz <hbelitz@users.berlios.de>                          *
 *                                                                      *
 * Created: 2005-01-24                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#include "ccarraydataadapter.h"

using namespace std;
using namespace boost;
using namespace aips;

/*************
 * Structors *
 *************/

CCArrayDataAdapter::CCArrayDataAdapter() throw()
	: CStructuredDataAdapter( "CCArrayDataAdapter", AIPSCCARRAYDATAADAPTER_VERSION, 
		"CStructuredDataAdapter" ), externalShortDataPtr( NULL ),	externalDoubleDataPtr( NULL )
{
	internalDataSPtr.reset();
}	
	
CCArrayDataAdapter::CCArrayDataAdapter( TDataSetPtr internalDataPtr_ ) throw()
	: CStructuredDataAdapter( internalDataPtr_, "CCArrayDataAdapter", AIPSCCARRAYDATAADAPTER_VERSION, 
		"CStructuredDataAdapter" ),	externalShortDataPtr( NULL ),	externalDoubleDataPtr( NULL )
{
}
	
CCArrayDataAdapter::~CCArrayDataAdapter() throw()
{
}

/*****************
 * Other methods *
 *****************/

/**
 * Calling this method without any paramters will reset the external data pointers
 * \param sImagePtr C-style array containing the image data
 * \param dimension Dimension of the dataset (e.g. 2 for an image, 3 for a volume)
 * \param extents   C-style array containing the extents of the dataset 
 */
void CCArrayDataAdapter::setExternalData( short* sImagePtr, size_t dimension, 
	const size_t* const extents ) throw( NullException )
{
	if ( sImagePtr == NULL )
		throw( NullException( "Input short input is NULL", CException::RECOVER, ERR_CALLERNULL ) );
	externalShortDataPtr = sImagePtr;	
	externalDoubleDataPtr = NULL;
	dataExtents.clear();
	for( unsigned short i = 0; i < dimension; i++ )
		dataExtents.push_back( extents[i] );
}

/**
 * Calling this method without any paramters will reset the external data pointers
 * \param dFieldPtr C-style array containing the image data
 * \param dimension Dimension of the dataset (e.g. 2 for an image, 3 for a volume)
 * \param extents   C-style array containing the extents of the dataset 
 */
void CCArrayDataAdapter::setExternalData( double* dFieldPtr, size_t dimension, 
	const size_t* const extents ) throw ( NullException )
{
	if ( dFieldPtr == NULL )
		throw( NullException( "Input field is NULL", CException::RECOVER, ERR_CALLERNULL ) );
	externalDoubleDataPtr = dFieldPtr;
	externalShortDataPtr = NULL;
	dataExtents.clear();
	for( unsigned short i = 0; i < dimension; i++ )
		dataExtents.push_back( extents[i] );
}

/** 
 * \returns CDataSet containing the internal representation of the data
 */
TDataSetPtr CCArrayDataAdapter::convertToInternal() throw( NullException )
{
	return convertToInternal( false );
}

/** 
 * \param bConvertField if true, returns a field of doubles. Otherwise, a field of integers is returned. 
 * \returns CDataSet containing the internal representation of the data
 */
TDataSetPtr CCArrayDataAdapter::convertToInternal( bool bConvertField ) throw( NullException )
{
	if ( externalShortDataPtr )
	{
		if ( bConvertField )
			return actualBackConversion<short, TField>( externalShortDataPtr );
		else
			return actualBackConversion<short, TImage>( externalShortDataPtr );
	}
	else if ( externalDoubleDataPtr )
	{
		if ( bConvertField )
			return actualBackConversion<double, TField>( externalDoubleDataPtr );
		else
			return actualBackConversion<double, TImage>( externalDoubleDataPtr );
	}
	else
		throw( NullException( "No external data present", CException::RECOVER, ERR_REQUESTNULL ) );
}
 
/** \param shortData C-Array to write data into. The array needs to be already allocated! */
void CCArrayDataAdapter::convertToExternal( short* shortData ) throw( NullException )
{
	if ( !shortData )
		throw( NullException( "Target image not allocated", CException::RECOVER, ERR_CALLERNULL ) );
	if ( !internalDataSPtr )
		throw( NullException( "No internal data present", CException::RECOVER, ERR_REQUESTNULL ) );
	actualConversion( shortData );
}

/** \param doubleData C-Array to write data into. The array needs to be already allocated! */
void CCArrayDataAdapter::convertToExternal( double* doubleData ) throw( NullException )
{
	if ( !doubleData )
		throw( NullException( "Target field not allocated", CException::RECOVER, ERR_CALLERNULL ) );
	if ( !internalDataSPtr )
		throw( NullException( "No internal data present", CException::RECOVER, ERR_REQUESTNULL ) );	
	actualConversion( doubleData );
}

/** \param dataPtr C-Array to write data into. The array needs to be already allocated! */
template<typename T> 
void CCArrayDataAdapter::actualConversion( T* dataPtr )
{
FBEGIN;
	if ( checkType<TImage>( *internalDataSPtr ) )
	{
		TImagePtr imagePtr = static_pointer_cast<TImage>( internalDataSPtr );
		if ( imagePtr )
		{
			unsigned int maxZ = 1;
			if ( imagePtr->getDimension() == 3 ) 
				maxZ = imagePtr->getExtent( 2 );
		 	for( unsigned int z = 0; z < maxZ; ++z )
			 	for( unsigned int y = 0; y < imagePtr->getExtent( 1 ); ++y )
					for( unsigned int x = 0; x < imagePtr->getExtent( 0 ); ++x )
						if ( bSwapYAxis )
							dataPtr[ x + y * imagePtr->getExtent( 0 ) 
								+ z * imagePtr->getExtent( 0 ) * imagePtr->getExtent( 1 )]
								= static_cast<T>(	(*imagePtr)( x, imagePtr->getExtent( 1 ) - 1 - y, z ) );
						else
							dataPtr[ x + y * imagePtr->getExtent( 0 ) 
								+ z * imagePtr->getExtent( 0 ) * imagePtr->getExtent( 1 )]
								= static_cast<T>( (*imagePtr)( x, y, z ) );
		}
	}
	else if ( checkType<TField>( *internalDataSPtr ) )
	{
	  TFieldPtr fieldPtr = static_pointer_cast<TField>( internalDataSPtr );
		if ( fieldPtr )
		{
		 	unsigned int maxZ = 1;		 	
			if ( fieldPtr->getDimension() == 3 ) maxZ = fieldPtr->getExtent( 2 );
				for( unsigned int z = 0; z < maxZ; ++z )
			 		for( unsigned int y = 0; y < fieldPtr->getExtent( 1 ); ++y )
						for( unsigned int x = 0; x < fieldPtr->getExtent( 0 ); ++x )
							if ( bSwapYAxis )
								dataPtr[x + y * fieldPtr->getExtent( 0 ) + z * fieldPtr->getExtent( 0 ) 
									* fieldPtr->getExtent( 1 )]
									= static_cast<T>( (*fieldPtr)( x, fieldPtr->getExtent( 1 ) - 1 - y, z ) );
							else
								dataPtr[x + y * fieldPtr->getExtent( 0 ) + z * fieldPtr->getExtent( 0 )
									* fieldPtr->getExtent( 1 )]
									= static_cast<T>( (*fieldPtr)( x, y, z ) );
		}
		else
		{
			FEND;
			alog << LWARN << "Illegal internal data format" << endl;
			return;
		}
	}
	FEND;
}

/** \param inputPtr C-Array to read data from */
template<typename T, typename U> 
TDataSetPtr CCArrayDataAdapter::actualBackConversion( T* inputPtr )
{	
FBEGIN;
	shared_ptr<U> theField( new U( dataExtents.size(), dataExtents ) );
	unsigned int maxZ = 1;
	if ( theField->getDimension() == 3 ) 
		maxZ = theField->getExtent( 2 );
 	for( unsigned int z = 0; z < maxZ; ++z )
	 	for( unsigned int y = 0; y < theField->getExtent( 1 ); ++y )
			for( unsigned int x = 0; x < theField->getExtent( 0 ); ++x )
				if ( bSwapYAxis )
					(*theField)( x, theField->getExtent( 1 ) - 1 - y, z ) = 
						static_cast<typename U::TDataType>(	inputPtr[ x + y * theField->getExtent( 0 ) 
						+ z * theField->getExtent( 0 ) * theField->getExtent( 1 )] );
				else
					(*theField)( x, y, z ) = 
						static_cast<typename U::TDataType>(	inputPtr[ x + y * theField->getExtent( 0 )
						+ z * theField->getExtent( 0 ) * theField->getExtent( 1 )] );
FEND;
	return theField;
}
