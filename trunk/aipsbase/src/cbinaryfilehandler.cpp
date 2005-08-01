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
 * \param theTargetDataSPtr pointer to target dataset
 * \param theFile input stream (file needs to be already opened)
 * \param bFileEndianess true == data is big endian, false == little endian (intel)
 * \param theVoxelSize size of one voxel (in bytes)
 * \throws FileException on any file error 
 */
template<typename SetType, typename DataType> 
void CBinaryFileHandler::loadSpecificType( boost::shared_ptr<SetType> theTargetDataSPtr, istream& theFile,
	const bool bFileEndianess, const size_t theVoxelSize ) const throw( FileException )
{
FBEGIN;
	std::vector<unsigned char> scanline;
  size_t scanlineSize = theTargetDataSPtr->getExtent(0) * theVoxelSize;
  scanline.resize( scanlineSize );
	typename SetType::iterator it = theTargetDataSPtr->begin();
	typename SetType::iterator end = theTargetDataSPtr->end();
	typedef typename dataTraits<SetType>::dataType SetDataType;
	theTargetDataSPtr->setMaximum( numeric_limits<SetDataType>::min() );
	theTargetDataSPtr->setMinimum( numeric_limits<SetDataType>::max() );
DBG("Loading into dataset with dimensions " << theTargetDataSPtr->getExtent(0) << " x " << theTargetDataSPtr->getExtent(1)
	<< " x " << theTargetDataSPtr->getExtent(2) );
	
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
			if ( *it > theTargetDataSPtr->getMaximum() )
				theTargetDataSPtr->setMaximum( *it );
			if ( *it < theTargetDataSPtr->getMinimum() )
				theTargetDataSPtr->setMinimum( *it );
			++it;
		}
	}
FEND;	
}

/**
 * \param theSourceDataSPtr pointer to source dataset
 * \param theFile output stream (file needs to be already opened)
 * \param bFileEndianess true == data is big endian, false == little endian (intel)
 * \param theVoxelSize size of one voxel (in bytes)
 * \throws FileException on any file error 
 */
template<typename SetType, typename DataType> 
void CBinaryFileHandler::saveSpecificType( shared_ptr<SetType> theSourceDataSPtr, ostream& theFile,
	const bool bFileEndianess, const size_t theVoxelSize ) const throw( FileException )
{
FBEGIN;
	typedef typename dataTraits<SetType>::dataType SetDataType;
	std::vector<unsigned char> scanline;	
  size_t scanlineSize = theSourceDataSPtr->getExtent(0) * theVoxelSize;
  scanline.resize( scanlineSize );
	typename SetType::iterator it = theSourceDataSPtr->begin();
	typename SetType::iterator end = theSourceDataSPtr->end();
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
 * \param theSourceDataSPtr pointer to source dataset
 * \param theFile output stream (file needs to be already opened)
 * \param bFileEndianess true == data is big endian, false == little endian (intel)
 * \param theVoxelSize size of one voxel (in bytes)
 * \throws FileException on any file error 
 */
template<>
void CBinaryFileHandler::loadSpecificType<TField3D, TFloatType>( TField3DPtr theTargetDataSPtr, 		istream& theFile,	const bool bFileEndianess, const size_t theVoxelSize ) 
	const throw( FileException )
{
FBEGIN;
	std::vector<unsigned char> scanline;
  size_t scanlineSize = theTargetDataSPtr->getExtent(0) * sizeof( TVector3D );
  scanline.resize( scanlineSize );
	TField3D::iterator it = theTargetDataSPtr->begin();
	TField3D::iterator end = theTargetDataSPtr->end();
	TVector3D One( 1.0, 1.0, 1.0 );
	theTargetDataSPtr->setMaximum( VEC_ZERO3D );
	theTargetDataSPtr->setMinimum( One );
DBG("Loading into dataset with dimensions " << theTargetDataSPtr->getExtent(0) << " x " << theTargetDataSPtr->getExtent(1)
	<< " x " << theTargetDataSPtr->getExtent(2) );
	
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

template<>
void CBinaryFileHandler::saveSpecificType<TField3D, TFloatType>
	( TField3DPtr theSourceDataSPtr, ostream& theFile, const bool bFileEndianess, 
	const size_t theVoxelSize ) const throw( FileException )
{
FBEGIN;
	std::vector<unsigned char> scanline;
  size_t scanlineSize = theSourceDataSPtr->getExtent(0) * sizeof( TVector3D );
  scanline.resize( scanlineSize );
	TField3D::iterator it = theSourceDataSPtr->begin();
	TField3D::iterator end = theSourceDataSPtr->end();
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
