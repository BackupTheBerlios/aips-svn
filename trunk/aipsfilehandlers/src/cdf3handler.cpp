/************************************************************************
 * File: cdf3handler.cpp                                                *
 * Project: AIPS                                                        *
 * Description: Handler for the POVRay volume format df3                *
 *                                                                      *
 * Created: 2004-06-03                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#include "cdf3handler.h"

using namespace std;
using namespace boost;

CDF3Handler::CDF3Handler() throw()
  : CBinaryFileHandler( "CDF3Handler", "0.4", "CFileHandler" )
{
  supportedFileTypesVec.push_back("df3");
  supportedFileTypesVec.push_back("DF3");
}

CDF3Handler::~CDF3Handler() throw()
{
}

/**
 * \param sFilename Name of the volume file
 * \exception FileException on any file error
 */
TDataFile CDF3Handler::load( const std::string& sFilename )
  const throw( FileException )
{
  ifstream theFile( sFilename.c_str() );
  if ( !theFile.is_open() )
    throw( FileException( SERROR( "File not found" ), CException::RECOVER, ERR_FILENOTFOUND ) );
	
	shared_ptr<CDF3Header> aHeader ( new CDF3Header() );
	aHeader->loadHeader( theFile );
	vector<size_t> dimensionSize = aHeader->getExtents();
  
	shared_ptr<TImage> aDataSet;   
  if ( dimensionSize[2] > 1 )
    aDataSet.reset( new TImage( 3, dimensionSize ) );
  else
    aDataSet.reset( new TImage( 2, dimensionSize ) );

  loadData( aDataSet, theFile, DUInt8, false );            
  return make_pair( aDataSet, aHeader );
}

/**
 * \param sFilename Name of the volume file
 * \param theData Pair of dataset and header
 * \exception FileException on any file error
 */
void CDF3Handler::save( const std::string& sFilename, const TDataFile& theData )
  const throw( FileException )
{
FBEGIN;
	if ( !( theData.first->getType() == typeid( dataTraits<TImage>::dataType ) ) )
	{
		alog << LWARN << "Illegal input for image writer" << endl;
		return;
	}

	shared_ptr<TImage> aDataSet = static_pointer_cast<TImage>( theData.first );
  if ( aDataSet == NULL || aDataSet->getDimension() > 3 || aDataSet->getDimension() < 2 )
  {
    alog << LWARN << SERROR( "Given data set is no unsigned short 2D/3D image" ) << endl;
  }

  ofstream theFile( sFilename.c_str() );
  if ( !theFile.is_open() )
    throw( FileException( SERROR( "File not found" ), CException::RECOVER, ERR_FILENOTFOUND ) );
	
	CDF3Header aHeader;
	aHeader.setExtents( aDataSet->getExtents() );
	aHeader.setEndianess( false );
	aHeader.saveHeader( theFile );

  saveData( aDataSet, theFile, DUInt8, false );
        
  theFile.close();
FEND;
}
