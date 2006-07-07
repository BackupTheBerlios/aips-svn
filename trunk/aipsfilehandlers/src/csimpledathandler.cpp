/************************************************************************
 * File: csimpledathandler.cpp                                          *
 * Project: AIPS - Basic file handlers plugin                           *
 * Description: A handler for the simple dat format                     *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 11.12.03                                                    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "csimpledathandler.h"

using namespace std;
using namespace boost;

/** Constructor */
CSimpleDatHandler::CSimpleDatHandler() throw()
  : CBinaryFileHandler( "CSimpleDatHandler", "0.4", "CFileHandler" )
{
  supportedFileTypesVec.push_back("DAT");
  supportedFileTypesVec.push_back("dat");
  supportedFileTypesVec.push_back("DATO");
  supportedFileTypesVec.push_back("dato");
}

/** Destructor */
CSimpleDatHandler::~CSimpleDatHandler() throw()
{
}

/**
 * Loads an unsigned short scalar volume dataset from the given file
 * \param sFilename Name of the volume file
 * \exception FileException on any file error
 */
TDataFile CSimpleDatHandler::load( const std::string& sFilename )
  const throw( FileException )
{
FBEGIN;
  ifstream theFile( sFilename.c_str() );
  if ( !theFile.is_open() )
    throw( FileException( SERROR( "File not found" ), CException::RECOVER, ERR_FILENOTFOUND ) );
	
	shared_ptr<CSimpleDatHeader> aHeader ( new CSimpleDatHeader() );
	aHeader->loadHeader( theFile );
DBG2("Header read.");	
	bool bFileEndianess = aHeader->getEndianess();
	vector<size_t> dimensionSize = aHeader->getExtents();
DBG2("Seeking data offset");	
	cerr << dimensionSize[0] << endl;
	cerr << dimensionSize[1] << endl;
	cerr << dimensionSize[2] << endl;
	theFile.seekg( 0, ios::end );
	size_t dataLength = dimensionSize[0] * dimensionSize[1] * dimensionSize[2];
	size_t fileLength = static_cast<size_t>( theFile.tellg() ) - 12;
	theFile.seekg( 12, ios::beg );
DBG2("Determining data type");
	EDataType dataType = DUInt8;
	if ( 2 * dataLength == fileLength )
		dataType = DUInt16;
	else if ( dataLength == fileLength )
		dataType = DUInt8;
	else // This file is too small or too large for the given dimensions
		throw( FileException( SERROR( "File has wrong size for the given dimensions" ), 
			CException::RECOVER, ERR_FILESIZEMISMATCH ) );
DBG2("Resetting memory pool");	  
	TImagePtr aDataSet;   
  if ( dimensionSize[2] > 1 )
    aDataSet.reset( new TImage( 3, dimensionSize ) );
  else
    aDataSet.reset( new TImage( 2, dimensionSize ) );
  loadData( aDataSet, theFile, dataType, bFileEndianess );
DBG("About to flip");
	// We need to flip the z axis since dat files are saved Superior->Inferior
	if ( dimensionSize[2] > 1 )
	{
		TImagePtr flippedImage( new TImage( aDataSet->getDimension(), aDataSet->getExtents() ) );
		flippedImage->setDataRange( aDataSet->getDataRange() );
		for( ushort z = 0; z < dimensionSize[2]; ++z )
			for( ushort y = 0; y < dimensionSize[1]; ++y )
				for( ushort x = 0; x < dimensionSize[0]; ++x )
					(*flippedImage)( dimensionSize[0] - 1 -  x, dimensionSize[1] - 1 - y, dimensionSize[2] - 1 - z ) = (*aDataSet)( x, y, z );
DBG("Flipped");
FEND;
		return make_pair( flippedImage, aHeader );
	}
	else
	{
FEND;	
  	return make_pair( aDataSet, aHeader );
	}
}

/**
 * Saves an unsigned short scalar volume dataset to the given file
 * \param sFilename Name of the volume file
 * \param theData Pair of dataset and header
 * \exception FileException on any file error
 */
void CSimpleDatHandler::save( const std::string& sFilename, const TDataFile& theData )
  const throw( FileException )
{
FBEGIN;
cerr << "A" << endl;
	if ( !theData.first || theData.first->getType() != typeid( TImage::TDataType ) )
	{
    alog << LWARN << SERROR( "Given data set is no unsigned short 2D/3D image." 
			" Floating point fields are not supported by this format" ) << endl;	
		return;
	}
cerr << "B" << endl;
	TImagePtr aDataSet = static_pointer_cast<TImage>( theData.first );
  if ( !aDataSet || aDataSet->getDimension() > 3 || aDataSet->getDimension() < 2 )
  {
    alog << LWARN << SERROR( "Given data set is no unsigned short 2D/3D image." 
			" Floating point fields are not supported by this format" ) << endl;
		return;
  }
cerr << "C" << endl;
  ofstream theFile( sFilename.c_str() );
  if ( !theFile.is_open() )
    throw( FileException( SERROR( "Could not create file" ),
    	CException::RECOVER, ERR_FILENOTFOUND ) );
cerr << "D" << endl;
	CSimpleDatHeader aHeader;
	aHeader.setExtents( aDataSet->getExtents() );
	if ( theData.second && theData.second->isDefined( "FileEndianess" ) )
		aHeader.setEndianess( theData.second->getEndianess() );
	else
		aHeader.setEndianess( false );
	aHeader.saveHeader( theFile );
cerr << "E" << endl;
	DBG("File endianess is " << aHeader.getEndianess() );
	EDataType dataType = DUInt8;
	//if ( aDataSet->getMaximum() > 255 )
		dataType = DUInt16;
	// Do not forget to flip our inferior->superior data to superior->inferior
cerr << "F" << endl;
	if ( aDataSet->getDimension() == 3 )
	{
		TImagePtr flippedImage( new TImage( aDataSet->getDimension(), aDataSet->getExtents() ) );
		for( ushort z = 0; z < aDataSet->getExtent(2); ++z )
			for( ushort y = 0; y < aDataSet->getExtent(1); ++y )
				for( ushort x = 0; x < aDataSet->getExtent(0); ++x )
					(*flippedImage)( aDataSet->getExtent(1) - 1 - x, aDataSet->getExtent(1) - 1 - y, aDataSet->getExtent(2) - 1 - z ) = (*aDataSet)( x, y, z );
		saveData( flippedImage, theFile, dataType, aHeader.getEndianess() );
	}
 	else
		saveData( aDataSet, theFile, dataType, aHeader.getEndianess() );
cerr << "G" << endl;
  theFile.close();
FEND;
}
