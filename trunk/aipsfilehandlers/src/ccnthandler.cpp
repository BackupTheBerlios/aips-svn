/************************************************************************
 * File: ccnthandler                                                       *
 * Project:                                                             *
 * Description:                                                         *
 *                                                                      *
 * Author: Hendrik Belitz ($EMAIL)                                            *
 *                                                                      *
 * Created: Di Mai 17 2005                                                      *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#include "ccnthandler.h"

using namespace std;

CCntHandler::CCntHandler() throw()
  : CFileHandler( "CCntHandler", "0.1", "CFileHandler" )
{
  supportedFileTypesVec.push_back("cnt");
  supportedFileTypesVec.push_back("CNT");
}

CCntHandler::~CCntHandler() throw()
{
}

TDataFile CCntHandler::load( const std::string& sFilename ) const
    throw( FileException )
{
	
/*  std::ifstream theFile;
	theFile.open( sFilename.c_str() );
 	if ( !theFile.is_open() )
	  throw( FileException( SERROR( "File not found" ), CException::RECOVER, ERR_FILENOTFOUND ) );

	// Header loading code goes here 
	

	// Data loading code goes here 
	
	theFile.close();	  
  return make_pair( anImageSet, aHeader );*/
}

/**
 * \param sFilename Filename of file to save to.
 * \param theData Pair of data and (in most cases optional) header information
 */
void CCntHandler::save( const std::string& sFilename, const TDataFile& theData )
  const throw( FileException )
{	
	if ( theData.first.get() == NULL || theData.first->getType() != typeid( TVector2D ) )
		throw( FileException( SERROR( "Illegal input" ),
			CException::RECOVER, ERR_FILEACCESS ) );

	TField2DPtr pointList = boost::static_pointer_cast<TField2D>( theData.first );
	
	std::ofstream theFile( sFilename.c_str() );
 	if ( !theFile.is_open() )
    	throw( FileException( SERROR( "Could not create file" ),
    		CException::RECOVER, ERR_FILEACCESS ) );

  theFile << "S "	<< 1 << endl;
  theFile.precision( 6 );
  theFile.setf( ios_base::fixed, ios_base::floatfield );
  theFile << "v " << pointList->getExtent(0) << " z 0.000000" << endl << "{" << endl;
  for( uint i = 0; i < pointList->getExtent(0); ++i )
  	theFile << (*pointList)(i)[0] << " " << (*pointList)(i)[1] << endl;
  theFile << "}" << endl;
  theFile.setf( ios_base::fmtflags(0), ios_base::floatfield );
	theFile.close();
}

void CCntHandler::loadData( TDataSetPtr theTargetDataAPtr, std::istream& theFile,
    const EDataType theVoxelType, const bool bFileEndianess )
    const throw( FileException, NullException ){}
void CCntHandler::saveData( TDataSetPtr theSourceDataAPtr, std::ostream& theFile,
    const EDataType theVoxelType, const bool bFileEndianess ) 
		const throw( FileException, NullException ){}
