/************************************************************************
 * File: canalyzehandler.cpp                                            *
 * Project: AIPS                                                        *
 * Description: File handler for the ANALYZE 7.5 format                 *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 16.12.03                                                    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "canalyzehandler.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

CAnalyzeHandler::CAnalyzeHandler() throw()
  : CBinaryFileHandler( "CAnalyzeHandler", "0.3", "CFileHandler" )
{
  supportedFileTypesVec.push_back("hdr");
  supportedFileTypesVec.push_back("HDR");
}

CAnalyzeHandler::~CAnalyzeHandler() throw()
{
}

/*****************
 * Other methods *
 *****************/
 
/**
 * \param sFilename Filename of file to load
 * \returns Pair of data and header information
 */
TDataFile CAnalyzeHandler::load( const std::string& sFilename ) const
    throw( FileException )
{
	
  std::ifstream theFile;
	// The header will never be saved compressed
	theFile.open( sFilename.c_str() );
 	if ( !theFile.is_open() )
	  throw( FileException( SERROR( "File not found" ), CException::RECOVER, ERR_FILENOTFOUND ) );

  boost::shared_ptr<CAnalyzeHeader> aHeader ( new CAnalyzeHeader() );
	aHeader->loadHeader( theFile );
	theFile.close();
	vector<size_t> extentSize = aHeader->getExtents();
	  
	std::string sDataFilename( sFilename, 0, sFilename.size() - 3 );
  sDataFilename += "img";
	
	bool bCompressed = false;
	gz::igzstream theGzFile;
	theFile.clear();
  theFile.open( sDataFilename.c_str() );
  if ( !theFile.is_open() )
  {
		theFile.clear();
		alog << LINFO << "Uncompressed data file not found. Looking for compressed data file..." << endl;
		bCompressed = true;
		sDataFilename += ".gz";
		theGzFile.open( sDataFilename.c_str() );
  	if ( !theGzFile.good() )
    	throw ( FileException( SERROR( "Could not open ANALYZE img file!" ),
      	CException::RECOVER, ERR_FILEACCESS ) );
  }
	EDataType dataType = this->getDataType( aHeader->getVoxelType() );
		
	if ( dataType == DFloat32 || dataType == DFloat16 )
	{
		TFieldPtr aFieldSet( new TField( extentSize.size(), extentSize, 1 ) );
		if ( bCompressed )
  		loadData( aFieldSet, theGzFile, dataType, aHeader->getEndianess() );
		else
			loadData( aFieldSet, theFile, dataType, aHeader->getEndianess() );
		switch( aHeader->getUnsignedLong( "Orientation" ) )
		{
			case 0:
				break;				
			case 1:
				aFieldSet = flip( aFieldSet, false, false, true ); 
				aFieldSet = rotate( aFieldSet, 0, 2, 1 ); 
				break;
			case 2:				
				aFieldSet = rotate( aFieldSet, 1, 2, 0 ); 
				break;
			case 3: 
				aFieldSet = flip( aFieldSet, false, true, false );
				break;
			case 4: 
				aFieldSet = flip( aFieldSet, false, false, true );
				aFieldSet = rotate( aFieldSet, 0, 2, 1 ); 
				break;
			case 5:
				aFieldSet = flip( aFieldSet, false, false, true ); 
				aFieldSet = rotate( aFieldSet, 1, 2, 0 ); 
				break;
		}
		return make_pair( aFieldSet, aHeader );
	}
	else
	{
		TImagePtr anImageSet( new TImage( extentSize.size(), extentSize, 1 ) );
		if ( bCompressed )
		{
  		loadData( anImageSet, theGzFile, dataType, aHeader->getEndianess() );
			theGzFile.close();
		}
		else
		{
			loadData( anImageSet, theFile, dataType, aHeader->getEndianess() );
			theFile.close();
		}
		switch( aHeader->getUnsignedLong( "Orientation" ) )
		{
			case 0:
				// Do not swap or rotate anything
				anImageSet = flip( anImageSet, false, true, false ); 
				break;
			case 1:
				anImageSet = flip( anImageSet, false, true, true ); 
				anImageSet = rotate( anImageSet, 0, 2, 1 ); break;
			case 2:				
				anImageSet = flip( anImageSet, false, true, false ); 
				anImageSet = rotate( anImageSet, 1, 2, 0 ); break;
			case 4: 
				anImageSet = flip( anImageSet, false, true, true );
				anImageSet = rotate( anImageSet, 0, 2, 1 ); break;
			case 5:
				anImageSet = flip( anImageSet, false, true, true ); 
				anImageSet = rotate( anImageSet, 1, 2, 0 ); break;
			case 3: 
				//anImageSet = flip( anImageSet, false, true, false ); 
				break; 
		}		
	  return make_pair( anImageSet, aHeader );
	}
}

/**
 * \param sFilename Filename of file to save to.
 * \param theData Pair of data and (in most cases optional) header information
 */
void CAnalyzeHandler::save( const std::string& sFilename, const TDataFile& theData )
  const throw( FileException )
{	
	if ( !theData.first )
		throw( FileException( SERROR( "No image" ),	CException::RECOVER, ERR_FILEACCESS ) );
		
	// Flip Y orientation so we can write in medical standard orientation
	TDataSetPtr tmpPtr;
	if ( checkType<TField>( *theData.first ) )
	{
		TFieldPtr fieldPtr = static_pointer_cast<TField>( theData.first );
		if ( !fieldPtr )
		{
			fieldPtr = flip( fieldPtr, false, true, false );
			tmpPtr = fieldPtr;
		}
	}
	else if ( checkType<TImage>( *theData.first ) )
	{
		TImagePtr imagePtr = static_pointer_cast<TImage>( theData.first );
		if ( !imagePtr )
		{
			alog << LWARN << "Illegal image format. Nothing saved..." << endl;
			return;
		}
		imagePtr = flip( imagePtr, false, true, false );
		tmpPtr = imagePtr;
	}
	else
	{
		alog << LWARN << "Illegal input type for CommonImage writer" << endl;
		return;
	}
	
	bool bCompressed = true;
	
	std::ofstream theFile;
	theFile.open( sFilename.c_str() );
 	if ( !theFile.is_open() )
    	throw( FileException( SERROR( "Could not create file" ),
    		CException::RECOVER, ERR_FILEACCESS ) );
		
  shared_ptr<CAnalyzeHeader> aHeader ( new CAnalyzeHeader() );
	if ( theData.second.get() != NULL )
		aHeader->append( *(theData.second) );
	aHeader->setExtents( theData.first->getExtents() );
	if ( aHeader->isDefined( "Compress" ) )
	{
		if ( aHeader->getBool( "Compress" ) == false )
			bCompressed = false;
	}
	EDataType dataType = determineDataType( theData.first.get(), aHeader.get() );
	if ( aHeader->isDefined( "ForceDataType" ) )
	{
		dataType = getDataType( aHeader->getString( "ForceDataType" ) );
		aHeader->setVoxelType( aHeader->getString( "ForceDataType" ) );
	}
	aHeader->saveHeader( theFile );
	theFile.close();
  
	string sDataFilename( sFilename, 0, sFilename.size() - 3 );
  sDataFilename += "img";
	if ( bCompressed ) 
	{
		gz::ogzstream theGzFile;
		sDataFilename += ".gz";
  	theGzFile.open( sDataFilename.c_str() );
  	if ( !theGzFile.rdbuf()->is_open() )
	  {
    	throw ( FileException( SERROR( "Could not create ANALYZE img file!" ),
      	CException::RECOVER, ERR_FILEACCESS ) );
  	}			
	  saveData( tmpPtr, theGzFile, dataType, false );
		theGzFile.close();
	}
	else
	{
  	theFile.open( sDataFilename.c_str() );
  	if ( !theFile.is_open() )
	  {
    	throw ( FileException( SERROR( "Could not create ANALYZE img file!" ),
      	CException::RECOVER, ERR_FILEACCESS ) );
  	}			
	  saveData( tmpPtr, theFile, dataType, false );
		theFile.close();
	}
}

CAnalyzeHandler::EDataType CAnalyzeHandler::determineDataType( CDataSet* theDataSet, CImageHeader* theHeader ) const
{
	if ( theDataSet->getType() == typeid( dataTraits<TImage>::dataType ) )
	{
		TImage* image = static_cast<TImage*>( theDataSet );
		if ( image != NULL )
		{
			theHeader->setLong("MaxIntensity", image->getMaximum() );
			theHeader->setLong("MinIntensity", image->getMinimum() );
			if ( image->getMaximum() < numeric_limits<uint8_t>::max() ) 
			{ 
				theHeader->setVoxelType( "UInt8" );
				return DUInt8; 
			}
			if ( image->getMaximum() < numeric_limits<int16_t>::max() ) 
			{ 
				theHeader->setVoxelType( "Int16" );
				return DInt16; 
			}
			else 
			{
				theHeader->setVoxelType( "Int32" );
				return DInt32;
			}
		}
	}
	else if ( theDataSet->getType() == typeid( dataTraits<TField>::dataType ) )
	{
		TField* field = static_cast<TField*>( theDataSet );
		if ( field != NULL )
		{
			theHeader->setLong("MaxIntensity", static_cast<long>( ceil( field->getMaximum()) ) );
			theHeader->setLong("MinIntensity", static_cast<long>( floor( field->getMinimum()) ) );
			if ( field->getMaximum() < numeric_limits<float>::max() )
			{ 
				theHeader->setVoxelType( "Float16" );
				return DFloat16; 
			}
			else 
			{ 
				theHeader->setVoxelType( "Float32" );
				return DFloat32; 
			}
		}
		else
		{
			alog << LFATAL << SERROR("Reached some code we should never reach.") << endl;
		}
	}
}

template<typename T> 
shared_ptr<T> CAnalyzeHandler::flip( shared_ptr<T> original, bool bSwapX, bool bSwapY, bool bSwapZ ) const
{
	shared_ptr<T> copy ( new T( original->getDimension(), original->getExtents(), original->getDataDimension() ) );
	copy->setMaximum( original->getMaximum() );
	copy->setMinimum( original->getMinimum() );
	std::vector<size_t> dimensionSize = copy->getExtents();
	ushort ay = 0; ushort ax = 0; ushort az = 0;
	for ( uint z = 0; z < dimensionSize[2]; ++z )
	{
    for ( uint y = 0; y < dimensionSize[1]; ++y )
		  for ( uint x = 0; x < dimensionSize[0]; ++x )
	    {
     	  if ( bSwapX ) 
					ax = dimensionSize[0] - ( x + 1 ); 
				else 
					ax = x;
     	  if ( bSwapY ) 
					ay = dimensionSize[1] - ( y + 1 ); 
				else 
					ay = y;
     	  if ( bSwapZ ) 
					az = dimensionSize[2] - ( z + 1 ); 
				else 
					az = z;					
	      for ( uint i = 0; i < original->getDataDimension(); ++i )
  	     	(*copy)( ax, ay, az, i ) = (*original)( x, y, z, i );
    	}
	}
	return copy;
}

template<typename T> 
boost::shared_ptr<T> CAnalyzeHandler::rotate( boost::shared_ptr<T> original, 
		ushort usNewX, ushort usNewY, ushort usNewZ ) const
{
	std::vector<size_t> dimensionSize = original->getExtents();
	if ( usNewX == 1 && usNewY == 2 && usNewZ == 0 )
	{
		swap( dimensionSize[0], dimensionSize[1] );
		swap( dimensionSize[1], dimensionSize[2] );
	}
	else if ( usNewX == 0 && usNewY == 2 && usNewZ == 1 )
		swap( dimensionSize[1], dimensionSize[2] );
	else
	{
		alog << LWARN << "Axis swapping not implemented !!!!!!!!" << endl;
		return original;
	}	
	shared_ptr<T> copy ( new T( original->getDimension(), dimensionSize, original->getDataDimension() ) );
	copy->setMaximum( original->getMaximum() );
	copy->setMinimum( original->getMinimum() );
	
	for ( uint z = 0; z < dimensionSize[2]; ++z )
	{
    for ( uint y = 0; y < dimensionSize[1]; ++y )
		  for ( uint x = 0; x < dimensionSize[0]; ++x )
	    {
 	      for ( uint i = 0; i < original->getDataDimension(); ++i )
					if ( usNewX == 1 && usNewY == 2 && usNewZ == 0 )
  	     		(*copy)( x, y, z, i ) = (*original)( y, z, x, i );
					else if ( usNewX == 0 && usNewY == 2 && usNewZ == 1 )
						(*copy)( x, y, z, i ) = (*original)( x, z, y, i );
    	}
	}
	return copy;
}
// 