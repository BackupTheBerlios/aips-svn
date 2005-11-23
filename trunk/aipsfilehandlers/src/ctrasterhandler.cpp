/***************************************************************************
 *   Copyright (C) 2004 by Patrick Musmann                                 *
 *   p.musmann@fz-juelich.de                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "ctrasterhandler.h"

using namespace aips;
using namespace std;

/** Constructor */
CTRasterHandler::CTRasterHandler() throw()
  : CBinaryFileHandler("CTRasterHandler", "0.1", "CBinaryFileHandler")
{
  supportedFileTypesVec.push_back("BIN");
  supportedFileTypesVec.push_back("bin");
}

/** Destructor */
CTRasterHandler::~CTRasterHandler() throw()
{
}

/**
 * Loads an TRaster scalar volume dataset from the given file
 * \param sFilename Name of the volume file
 * \exception FileException on any file error
 */
TDataFile CTRasterHandler::load( const string& sFilename )
  const throw( FileException )
{
    ifstream theFile( sFilename.c_str() );
    if ( !theFile.is_open() )
        throw( FileException( SERROR( "File not found" ), CException::RECOVER, ERR_FILENOTFOUND ) );

    CTRasterHeader* aHeader = new CTRasterHeader();
    aHeader->loadHeader( theFile );

    const bool bFileEndianess = aHeader->getEndianess();
    const unsigned short usDataType = aHeader->getUnsignedLong( "data_type" );
/* FIXME */		
    const unsigned short usVoxelSize = 2;// = aHeader->getVoxelSize();
    // DataSet Dimensions
    const vector<size_t> extentVec = aHeader->getExtents();
    const ushort usDimension = extentVec[2] > 1 ? 3 : 2;

    // Initialization of Data (2D or 3D), allocation of memory
    CDataSet *aDataSet = NULL;
    if( ( usDataType == 0 ) && ( usVoxelSize == sizeof(uint) ) )
    {
        aDataSet = new CTypedData<int32_t>( usDimension, extentVec );
    }
    else if( ( usDataType == 1 ) && ( usVoxelSize == sizeof(int) ) )
    {
        aDataSet = new CTypedData<int32_t>( usDimension, extentVec );
    }
    else if( ( usDataType == 2 ) && ( usVoxelSize == sizeof(float) ) )
    {
        aDataSet = new CTypedData<float>( usDimension, extentVec );
    }
    else
    {
        throw( FileException( SERROR( "Bad data_type and/or data_type_size" ),
         	CException::RECOVER, ERR_FILEFORMATUNSUPPORTED ) );
    }

//     loadData( aDataSet, theFile, usVoxelSize, bFileEndianess );
    // raw load of float dataset
    theFile.read( reinterpret_cast<char*>(aDataSet->getVoidArray()), extentVec[0]*extentVec[1]*extentVec[2]*usVoxelSize );

    return make_pair( aDataSet, aHeader );
}


/**
 * Saves an TRasterFile float image to the given file
 * \param sFilename Name of the volume file
 * \param theData Pair of data set and header (will be ignored for this file type) to be saved
 * \exception FileException on any file error
 */
void CTRasterHandler::save( const string& sFilename, const TDataFile& theData )
  const throw( FileException )
{
    if( theData.first == NULL )
        throw( NullException( SERROR( "No DataSet" ),
        	CException::RECOVER, ERR_REQUESTNULL ) );

    ofstream theFile( sFilename.c_str() );
    if ( !theFile.is_open() )
        throw( FileException( SERROR( "File not found" ),
        	CException::RECOVER, ERR_FILENOTFOUND ) );

    // save header
    CTRasterHeader aHeader;
    unsigned short usDataType;
    unsigned short usVoxelSize;
    if( theData.first->getType() == typeid(uint) )
    {
        usDataType = 0;
        usVoxelSize = sizeof(uint);
    }
    else if( theData.first->getType() == typeid(int) )
    {
        usDataType = 1;
        usVoxelSize = sizeof(int);
    }
    else if( theData.first->getType() == typeid(float) )
    {
        usDataType = 2;
        usVoxelSize = sizeof(float);
    }
    else
    {
        throw( FileException( SERROR( "Bad data_type" ),
         	CException::RECOVER, ERR_FILEFORMATUNSUPPORTED ) );
    }
    aHeader.setUnsignedLong( "data_type", usDataType );
    aHeader.setUnsignedLong( "data_type_size", usVoxelSize );

    //"byte_order"
    bool bFileEndianess = theData.second->isDefined( "BigEndian" );
    aHeader.setEndianess( bFileEndianess );

    //"width", "height" and "depth"
    vector<size_t> dimensionSize = theData.second->getExtents();
    while( dimensionSize.size() < 3 )
        dimensionSize.push_back( 1 );
    aHeader.setExtents( dimensionSize );

    aHeader.setUnsignedLong( "crc32", 0 );  //TODO: implement checksum calculation
    aHeader.setString( "description", theData.second->getString( "Description" ) );

    aHeader.saveHeader( theFile );

    //save data
    //saveData( aDataSet, theFile, usVoxelSize, bFileEndianess );
    // write data
    theFile.write( reinterpret_cast<const char *>(theData.first->getVoidArray()), dimensionSize.at(0)*dimensionSize.at(1)*dimensionSize.at(2)*usVoxelSize );

    theFile.close();
}

