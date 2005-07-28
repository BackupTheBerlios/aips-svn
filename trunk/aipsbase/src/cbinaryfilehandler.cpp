/************************************************************************
 * File: cbinaryfilehandler.cpp                                         *
 * Project: AIPS                                                        *
 * Description: A class to handle binary file types                     *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 22.01.04                                                    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "cbinaryfilehandler.h"

using namespace std;
using namespace boost;
using namespace aips;

/*************
 * Structors *
 *************/

/**
 * \param sClassName_ Class name
 * \param sClassVersion_ Class version
 * \param sDerivedFrom_ Parent classes
 */
CBinaryFileHandler::CBinaryFileHandler( const std::string &sClassName_, const std::string &sClassVersion_,
    const std::string &sDerivedFrom_ ) throw()
  : CFileHandler( sClassName_, sClassVersion_, sDerivedFrom_ )
{
}

CBinaryFileHandler::~CBinaryFileHandler() throw()
{
}

/*****************
 * Other methods *
 *****************/

/**
 * \param theTargetDataAPtr pointer to target dataset (needs to be already allocated)
 * \param theFile input stream (file needs to be already open)
 * \param theVoxelType type of a voxel 
 * \param bFileEndianess true data is big endian (e.g. SUN), false if it is little endian (e.g. intel)
 * \throws FileException on any file error
 * \throws NullException if the type of theTargetDataAPtr isn't supported or cannot be determined
 */
void CBinaryFileHandler::loadData( TDataSetPtr theTargetDataAPtr, istream& theFile,
    const EDataType theVoxelType, const bool bFileEndianess ) const throw( FileException, NullException )
{
FBEGIN;
	if ( theTargetDataAPtr.get() == NULL )
		throw ( NullException( SERROR( "Target data pointer is not allocated" ), CException::RECOVER, ERR_CALLERNULL ) );
		
DBG("Reading into dataset with dimensions " << theTargetDataAPtr->getExtent(0) << " x " << theTargetDataAPtr->getExtent(1)
	<< " x " << theTargetDataAPtr->getExtent(2) );

	size_t theVoxelSize = theVoxelType % 10;
	if ( checkType<TImage>( *theTargetDataAPtr ) )
	{
		TImagePtr imageAPtr = static_pointer_cast<TImage>( theTargetDataAPtr );
		if ( imageAPtr.get() == NULL )
			throw ( NullException( SERROR( "Could not determine dataset type" ), CException::RECOVER, ERR_UNKNOWNTYPE ) );
		
		switch( theVoxelType )
		{
			case DInt8: 
				loadSpecificType<TImage, int8_t>( imageAPtr, theFile, bFileEndianess, theVoxelSize ); 
				break;
			case DInt16: 
				loadSpecificType<TImage, int16_t>( imageAPtr, theFile, bFileEndianess, theVoxelSize ); 
				break;
			case DInt32: 
				loadSpecificType<TImage, int32_t>( imageAPtr, theFile, bFileEndianess, theVoxelSize ); 
				break;
			case DUInt8: 
				loadSpecificType<TImage, uint8_t>( imageAPtr, theFile, bFileEndianess, theVoxelSize ); 
				break;
			case DUInt16: 
				loadSpecificType<TImage, uint16_t>( imageAPtr, theFile, bFileEndianess, theVoxelSize ); 
				break;
			case DUInt32: 
				loadSpecificType<TImage, uint32_t>( imageAPtr, theFile, bFileEndianess, theVoxelSize ); 
				break;
			case DFloat16: 
				loadSpecificType<TImage, float>( imageAPtr, theFile, bFileEndianess, theVoxelSize ); 
				break;
			case DFloat32: 
				loadSpecificType<TImage, double>( imageAPtr, theFile, bFileEndianess, theVoxelSize ); 
				break;
			case DFloat64: 
				loadSpecificType<TImage, long double>( imageAPtr, theFile, bFileEndianess, theVoxelSize ); 
				break;
		}
	}
	else if ( checkType<TField>( *theTargetDataAPtr ) )
	{
		TFieldPtr fieldAPtr = static_pointer_cast<TField>( theTargetDataAPtr );
		if ( fieldAPtr == NULL )
			throw ( NullException( SERROR( "Could not determine dataset type" ), CException::RECOVER, ERR_UNKNOWNTYPE ) );
		
		switch( theVoxelType )
		{
			case DInt8: 
				loadSpecificType<TField, int8_t>( fieldAPtr, theFile, bFileEndianess, theVoxelSize ); 
				break;
			case DInt16: 
				loadSpecificType<TField, int16_t>( fieldAPtr, theFile, bFileEndianess, theVoxelSize ); 
				break;
			case DInt32: 
				loadSpecificType<TField, int32_t>( fieldAPtr, theFile, bFileEndianess, theVoxelSize ); 
				break;
			case DUInt8: 
				loadSpecificType<TField, uint8_t>( fieldAPtr, theFile, bFileEndianess, theVoxelSize ); 
				break;
			case DUInt16: 
				loadSpecificType<TField, uint16_t>( fieldAPtr, theFile, bFileEndianess, theVoxelSize ); 
				break;
			case DUInt32: 
				loadSpecificType<TField, uint32_t>( fieldAPtr, theFile, bFileEndianess, theVoxelSize ); 
				break;
			case DFloat16: 
				loadSpecificType<TField, float>( fieldAPtr, theFile, bFileEndianess, theVoxelSize ); 
				break;
			case DFloat32: 
				loadSpecificType<TField, double>( fieldAPtr, theFile, bFileEndianess, theVoxelSize ); 
				break;
			case DFloat64: 
				loadSpecificType<TField, long double>( fieldAPtr, theFile, bFileEndianess, theVoxelSize ); 
				break;
		}					
	}
	else if ( checkType<TField3D>( *theTargetDataAPtr ) )
	{
		TField3DPtr fieldAPtr = static_pointer_cast<TField3D>( theTargetDataAPtr );
		if ( fieldAPtr == NULL )
			throw ( NullException( SERROR( "Could not determine dataset type" ), CException::RECOVER, ERR_UNKNOWNTYPE ) );
		loadVectorField( fieldAPtr, theFile, bFileEndianess );
	}
	else
		throw ( NullException( SERROR( "Could not determine dataset type" ), CException::RECOVER, ERR_UNKNOWNTYPE ) );
FEND;		
}

/**
 * \param theSourceDataAPtr pointer to source dataset
 * \param theFile output stream (file needs to be already open)
 * \param theVoxelType type of a voxel 
 * \param bFileEndianess true data is big endian, false if it is little endian (intel)
 * \throws FileException on any file error
 * \throws NullException if the type of theSourceDataAPtr isn't supported or cannot be determined
 */
void CBinaryFileHandler::saveData( TDataSetPtr theSourceDataAPtr, ostream& theFile,
    const EDataType theVoxelType, const bool bFileEndianess ) const throw( FileException, NullException )
{
	if ( theSourceDataAPtr.get() == NULL )
		throw ( NullException( SERROR( "Source data pointer is not allocated" ), CException::RECOVER, ERR_CALLERNULL ) );
	
	size_t theVoxelSize = theVoxelType % 10;
	std::vector<unsigned char> scanline;	
  size_t scanlineSize = theSourceDataAPtr->getExtent(0) * theVoxelSize;
  scanline.resize( scanlineSize );
  if ( checkType<TField>( *theSourceDataAPtr ) )
  {
		TFieldPtr fieldAPtr = static_pointer_cast<TField>( theSourceDataAPtr );
		if ( !fieldAPtr )
			throw ( NullException( SERROR( "Could not determine dataset type" ), CException::RECOVER, ERR_UNKNOWNTYPE ) );
			
		switch( theVoxelType )
		{
			case DFloat16: 
				saveSpecificType<TField, float>( fieldAPtr, theFile, bFileEndianess, theVoxelSize );
				break;
			case DFloat32: 
				saveSpecificType<TField, double>( fieldAPtr, theFile, bFileEndianess, theVoxelSize );
				break;
			case DFloat64: 
				saveSpecificType<TField, long double>( fieldAPtr, theFile, bFileEndianess, theVoxelSize );
				break;
			case DInt8: 
				saveSpecificType<TField, int8_t>( fieldAPtr, theFile, bFileEndianess, theVoxelSize );
				break;
			case DInt16: 
				saveSpecificType<TField, int16_t>( fieldAPtr, theFile, bFileEndianess, theVoxelSize );
				break;
			case DInt32: 
				saveSpecificType<TField, int32_t>( fieldAPtr, theFile, bFileEndianess, theVoxelSize );
				break;
			case DUInt8: 
				saveSpecificType<TField, uint8_t>( fieldAPtr, theFile, bFileEndianess, theVoxelSize );
				break;
			case DUInt16: 
				saveSpecificType<TField, uint16_t>( fieldAPtr, theFile, bFileEndianess, theVoxelSize );
				break;
			case DUInt32: 
				saveSpecificType<TField, uint32_t>( fieldAPtr, theFile, bFileEndianess, theVoxelSize );
				break;
		}
	}
	else if ( checkType<TImage>( *theSourceDataAPtr ) )
  {
		TImagePtr imageAPtr = static_pointer_cast<TImage>( theSourceDataAPtr );
		if ( !imageAPtr )
			throw ( NullException( SERROR( "Could not determine dataset type" ), CException::RECOVER, ERR_UNKNOWNTYPE ) );

		switch( theVoxelType )
		{
			case DFloat16: 
				saveSpecificType<TImage, float>( imageAPtr, theFile, bFileEndianess, theVoxelSize );
				break;
			case DFloat32: 
				saveSpecificType<TImage, double>( imageAPtr, theFile, bFileEndianess, theVoxelSize );
				break;
			case DFloat64: 
				saveSpecificType<TImage, long double>( imageAPtr, theFile, bFileEndianess, theVoxelSize );
				break;
			case DInt8: 
				saveSpecificType<TImage, int8_t>( imageAPtr, theFile, bFileEndianess, theVoxelSize );
				break;
			case DInt16: 
				saveSpecificType<TImage, int16_t>( imageAPtr, theFile, bFileEndianess, theVoxelSize );
				break;
			case DInt32: 
				saveSpecificType<TImage, int32_t>( imageAPtr, theFile, bFileEndianess, theVoxelSize );
				break;
			case DUInt8: 
				saveSpecificType<TImage, uint8_t>( imageAPtr, theFile, bFileEndianess, theVoxelSize );
				break;
			case DUInt16: 
				saveSpecificType<TImage, uint16_t>( imageAPtr, theFile, bFileEndianess, theVoxelSize );
				break;
			case DUInt32: 
				saveSpecificType<TImage, uint32_t>( imageAPtr, theFile, bFileEndianess, theVoxelSize );
				break;
		}
	}
	else if ( checkType<TField3D>( *theSourceDataAPtr ) )
  {
		TField3DPtr imageAPtr = static_pointer_cast<TField3D>( theSourceDataAPtr );
		if ( !imageAPtr )
			throw ( NullException( SERROR( "Could not determine dataset type" ), CException::RECOVER, ERR_UNKNOWNTYPE ) );
		saveVectorField( imageAPtr, theFile, bFileEndianess );
	}
	else
		throw ( NullException( SERROR( "Could not determine dataset type" ), CException::RECOVER, ERR_UNKNOWNTYPE ) );
}

/**
 * \param theTargetDataAPtr pointer to target dataset
 * \param theFile input stream (file needs to be already opened)
 * \param bFileEndianess true == data is big endian, false == little endian (intel)
 * \param theVoxelSize size of one voxel (in bytes)
 * \throws FileException on any file error 
 */
template<typename SetType, typename DataType> 
void CBinaryFileHandler::loadSpecificType( boost::shared_ptr<SetType> theTargetDataAPtr, istream& theFile,
	const bool bFileEndianess, const size_t theVoxelSize ) const throw( FileException )
{
FBEGIN;
	std::vector<unsigned char> scanline;
  size_t scanlineSize = theTargetDataAPtr->getExtent(0) * theVoxelSize;
  scanline.resize( scanlineSize );
	typename SetType::iterator it = theTargetDataAPtr->begin();
	typename SetType::iterator end = theTargetDataAPtr->end();
	typedef typename dataTraits<SetType>::dataType SetDataType;
	theTargetDataAPtr->setMaximum( numeric_limits<SetDataType>::min() );
	theTargetDataAPtr->setMinimum( numeric_limits<SetDataType>::max() );
DBG("Loading into dataset with dimensions " << theTargetDataAPtr->getExtent(0) << " x " << theTargetDataAPtr->getExtent(1)
	<< " x " << theTargetDataAPtr->getExtent(2) );
	
	while( it != end )
	{
		theFile.read( (char*)( &( scanline[0] ) ), scanlineSize );
    if ( theFile.gcount() != static_cast<long>( scanlineSize ) )
    {
			ostringstream os;
     	os << "Image data seems to be corrupted. Image not loaded." << " Loaded "
				<< theFile.gcount() << " bytes, should be " << scanlineSize;
      throw ( FileException( SERROR( os.str().c_str() ), CException::RECOVER, ERR_FILEACCESS ) );
    }
		size_t scanlineIndex = 0;
		while( scanlineIndex < scanline.size() )
		{
			if ( it == end )
			{
				ostringstream os;
   	  	os << "Image data seems to be corrupted. Image not loaded." << " Loaded "
					<< theFile.gcount() << " bytes, should be " << scanlineSize;
	      throw ( FileException( SERROR( os.str().c_str() ), CException::RECOVER, ERR_FILEACCESS ) );
			}
			DataType value;
			memcpy( &value, &scanline[scanlineIndex], sizeof( DataType ) );
			if ( bFileEndianess ) 
				swapEndianess( value );
			*it = static_cast<SetDataType>( value ); 
			scanlineIndex += sizeof( DataType );
			if ( *it > theTargetDataAPtr->getMaximum() )
				theTargetDataAPtr->setMaximum( *it );
			if ( *it < theTargetDataAPtr->getMinimum() )
				theTargetDataAPtr->setMinimum( *it );
			++it;
		}
	}
FEND;	
}

/**
 * \param theSourceDataAPtr pointer to source dataset
 * \param theFile output stream (file needs to be already opened)
 * \param bFileEndianess true == data is big endian, false == little endian (intel)
 * \param theVoxelSize size of one voxel (in bytes)
 * \throws FileException on any file error 
 */
template<typename SetType, typename DataType> 
void CBinaryFileHandler::saveSpecificType( shared_ptr<SetType> theSourceDataAPtr, ostream& theFile,
	const bool bFileEndianess, const size_t theVoxelSize ) const throw( FileException )
{
FBEGIN;
	typedef typename dataTraits<SetType>::dataType SetDataType;
	std::vector<unsigned char> scanline;	
  size_t scanlineSize = theSourceDataAPtr->getExtent(0) * theVoxelSize;
  scanline.resize( scanlineSize );
	typename SetType::iterator it = theSourceDataAPtr->begin();
	typename SetType::iterator end = theSourceDataAPtr->end();
	while( it != end )
	{					
		size_t scanlineIndex = 0;
		while( scanlineIndex < scanline.size() && it != end )
		{
			DataType value;
			value = static_cast<DataType>( *it );
			if ( bFileEndianess ) 
				swapEndianess( value );
			memcpy( &scanline[scanlineIndex], &value, sizeof( DataType ) );
			scanlineIndex += sizeof( DataType );
			++it;		
		}
		theFile.write( (char*)( &( scanline[0] ) ), scanlineSize );
    if ( !theFile.good() ) // something went wrong
    {
      throw ( FileException( SERROR(
        "Error occured on saving file. Image not saved..." ), CException::RECOVER, ERR_FILEACCESS ) );
    }		
	}
FEND;	
}


/**
 * \param theSourceDataAPtr pointer to source dataset
 * \param theFile output stream (file needs to be already opened)
 * \param bFileEndianess true == data is big endian, false == little endian (intel)
 * \param theVoxelSize size of one voxel (in bytes)
 * \throws FileException on any file error 
 */
void CBinaryFileHandler::saveVectorField( TField3DPtr theSourceDataAPtr, ostream& theFile,
	const bool bFileEndianess ) const throw( FileException )
{
FBEGIN;
	std::vector<unsigned char> scanline;
  size_t scanlineSize = theSourceDataAPtr->getExtent(0) * sizeof( TVector3D );
  scanline.resize( scanlineSize );
	TField3D::iterator it = theSourceDataAPtr->begin();
	TField3D::iterator end = theSourceDataAPtr->end();
	while( it != end )
	{					
		size_t scanlineIndex = 0;
		while( scanlineIndex < scanline.size() && it != end )
		{
			TVector3D& value =  *it;
			if ( bFileEndianess ) 
				swapEndianess( value );
			memcpy( &scanline[scanlineIndex], &value, sizeof( TVector3D ) );
			scanlineIndex += sizeof( TVector3D );
			++it;		
		}
		theFile.write( (char*)( &( scanline[0] ) ), scanlineSize );
    if ( !theFile.good() ) // something went wrong
    {
      throw ( FileException( SERROR(
        "Error occured on saving file. Image not saved..." ), CException::RECOVER, ERR_FILEACCESS ) );
    }		
	}
FEND;	
}

void CBinaryFileHandler::loadVectorField( TField3DPtr theTargetDataAPtr, istream& theFile,
	const bool bFileEndianess ) const throw( FileException )
{
FBEGIN;
	std::vector<unsigned char> scanline;
  size_t scanlineSize = theTargetDataAPtr->getExtent(0) * sizeof( TVector3D );
  scanline.resize( scanlineSize );
	TField3D::iterator it = theTargetDataAPtr->begin();
	TField3D::iterator end = theTargetDataAPtr->end();
	TVector3D One( 1.0, 1.0, 1.0 );
	theTargetDataAPtr->setMaximum( VEC_ZERO3D );
	theTargetDataAPtr->setMinimum( One );
DBG("Loading into dataset with dimensions " << theTargetDataAPtr->getExtent(0) << " x " << theTargetDataAPtr->getExtent(1)
	<< " x " << theTargetDataAPtr->getExtent(2) );
	
	while( it != end )
	{
		theFile.read( (char*)( &( scanline[0] ) ), scanlineSize );
    if ( theFile.gcount() != static_cast<long>( scanlineSize ) )
    {
			ostringstream os;
     	os << "Image data seems to be corrupted. Image not loaded." << " Loaded "
				<< theFile.gcount() << " bytes, should be " << scanlineSize;
      throw ( FileException( SERROR( os.str().c_str() ), CException::RECOVER, ERR_FILEACCESS ) );
    }
		size_t scanlineIndex = 0;
		while( scanlineIndex < scanline.size() )
		{
			if ( it == end )
			{
				ostringstream os;
   	  	os << "Image data seems to be corrupted. Image not loaded." << " Loaded "
					<< theFile.gcount() << " bytes, should be " << scanlineSize;
	      throw ( FileException( SERROR( os.str().c_str() ), CException::RECOVER, ERR_FILEACCESS ) );
			}
			TVector3D value;
			memcpy( &(*it), &scanline[scanlineIndex], sizeof( TVector3D ) );
			if ( bFileEndianess ) 
				swapEndianess( *it );
			scanlineIndex += sizeof( TVector3D );
			++it;
		}
	}
FEND;	
}
