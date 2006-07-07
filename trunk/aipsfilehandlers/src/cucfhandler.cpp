/************************************************************************
 * File: cucfhandler                                                       *
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
#include "cucfhandler.h"

using namespace std;

CUcfHandler::CUcfHandler() throw()
  : CFileHandler( "CUcfHandler", "0.1", "CFileHandler" )
{
  supportedFileTypesVec.push_back("ucf");
  supportedFileTypesVec.push_back("UCF");
}

CUcfHandler::~CUcfHandler() throw()
{
}

TDataFile CUcfHandler::load( const std::string& sFilename ) const
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
void CUcfHandler::save( const std::string& sFilename, const TDataFile& theData )
  const throw( FileException )
{	
	if ( theData.first.get() == NULL || theData.first->getType() != typeid( TVector2D ) )
		throw( FileException( SERROR( "Illegal input" ),
			CException::RECOVER, ERR_FILEACCESS ) );

	boost::shared_ptr<CImageHeader> h = theData.second;
	TField2DPtr pointList = boost::static_pointer_cast<TField2D>( theData.first );
	
	std::ofstream theFile( sFilename.c_str() );
 	if ( !theFile.is_open() )
    	throw( FileException( SERROR( "Could not create file" ),
    		CException::RECOVER, ERR_FILEACCESS ) );

  theFile.precision( 6 );
  theFile.setf( ios_base::fixed, ios_base::floatfield );
  theFile << "<width=>" << endl;
  theFile << h->getExtents()[0] << endl;
  theFile << "<height=>" << endl;
  theFile << h->getExtents()[1] << endl;
  theFile << "<xrange=>" << endl << "-0.500000 " << static_cast<double>(h->getExtents()[0])-0.5 << endl;
  theFile << "<yrange=>" << endl << "-0.500000 " << static_cast<double>(h->getExtents()[1])-0.5 << endl;
  theFile << "<zrange=>" << endl << "-0.500000 0.500000" << endl;
  theFile << "<levels>" << endl << "1" << endl << "<level number=>" << endl << "0.000000" << endl;
  theFile << "<point_num=>" << endl << pointList->getExtent(0) << endl << "<contour_data=>" << endl;
  for( uint i = 0; i < pointList->getExtent(0); ++i )
  	theFile << (*pointList)(i)[0] << " " << (*pointList)(i)[1] << " 0.000000" << endl;
  theFile << "<end of level>" << endl << "<end>" << endl;
  theFile.setf( ios_base::fmtflags(0), ios_base::floatfield );
	theFile.close();
}

void CUcfHandler::loadData( TDataSetPtr theTargetDataAPtr, std::istream& theFile,
    const EDataType theVoxelType, const bool bFileEndianess ) const throw( FileException, NullException ){}
void CUcfHandler::saveData( TDataSetPtr theSourceDataAPtr, std::ostream& theFile,
    const EDataType theVoxelType, const bool bFileEndianess ) 
		const throw( FileException, NullException ){}


