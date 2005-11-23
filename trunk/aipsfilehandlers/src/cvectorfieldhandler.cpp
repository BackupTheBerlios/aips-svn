/************************************************************************
 * File: cvectorfieldhandler                                                       *
 * Project:                                                             *
 * Description:                                                         *
 *                                                                      *
 * Author: Hendrik Belitz ($EMAIL)                                            *
 *                                                                      *
 * Created: Do Jun 2 2005                                                      *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#include "cvectorfieldhandler.h"

using namespace std;
using namespace boost;

/** Constructor */
CVectorFieldHandler::CVectorFieldHandler() throw()
  : CBinaryFileHandler( "CVectorFieldHandler", "0.4", "CFileHandler" )
{
  supportedFileTypesVec.push_back("avf");
  supportedFileTypesVec.push_back("AVF");
}

/** Destructor */
CVectorFieldHandler::~CVectorFieldHandler() throw()
{
}

/**
 * Loads an unsigned short scalar volume dataset from the given file
 * \param sFilename Name of the volume file
 * \exception FileException on any file error
 */
TDataFile CVectorFieldHandler::load( const std::string& sFilename )
  const throw( FileException )
{
FBEGIN;
  ifstream theFile( sFilename.c_str() );
  if ( !theFile.is_open() )
    throw( FileException( SERROR( "File not found" ), CException::RECOVER, ERR_FILENOTFOUND ) );
	
	shared_ptr<CVectorFieldHeader> aHeader ( new CVectorFieldHeader() );
	aHeader->loadHeader( theFile );
DBG2("Header read.");	
	bool bFileEndianess = aHeader->getEndianess();
	vector<size_t> dimensionSize = aHeader->getExtents();
DBG2("Seeking data offset");	
	cerr << dimensionSize[0] << endl;
	cerr << dimensionSize[1] << endl;
	cerr << dimensionSize[2] << endl;
//	size_t dataLength = dimensionSize[0] * dimensionSize[1] * dimensionSize[2];	
	TField3DPtr aDataSet( new TField3D( 3, dimensionSize ) );
  loadData( aDataSet, theFile, DFloat32, bFileEndianess );
  	return make_pair( aDataSet, aHeader );
}

/**
 * Saves an unsigned short scalar volume dataset to the given file
 * \param sFilename Name of the volume file
 * \param theData Pair of dataset and header
 * \exception FileException on any file error
 */
void CVectorFieldHandler::save( const std::string& sFilename, const TDataFile& theData )
  const throw( FileException )
{
FBEGIN;
	if ( theData.first->getType() != typeid( TField3D::TDataType ) )
	{
    alog << LWARN << SERROR( "<on checking input type> Given data set is no 3D field."
			" Floating point fields are not supported by this format" ) << endl;	
		return;
	}
	TField3DPtr aDataSet = static_pointer_cast<TField3D>( theData.first );
  if ( aDataSet.get() == NULL || aDataSet->getDimension() > 3 || aDataSet->getDimension() < 3 )
  {
    alog << LWARN << SERROR( "<on checking input ptr> Given data set is no 3D field."
			" Floating point fields are not supported by this format" ) << endl;
		return;
  }
  ofstream theFile( sFilename.c_str() );
  if ( !theFile.is_open() )
    throw( FileException( SERROR( "Could not create file" ),
    	CException::RECOVER, ERR_FILENOTFOUND ) );
	
	CVectorFieldHeader aHeader;
	aHeader.setExtents( aDataSet->getExtents() );
/*	if ( theData.second->isDefined( "FileEndianess" ) )
		aHeader.setEndianess( theData.second->getEndianess() );
	else*/
		aHeader.setEndianess( false );
	aHeader.saveHeader( theFile );

	DBG("File endianess is " << aHeader.getEndianess() );
	// Do not forget to flip our inferior->superior data to superior->inferior
	saveData( aDataSet, theFile, DFloat32, aHeader.getEndianess() );        
  theFile.close();
FEND;
}
