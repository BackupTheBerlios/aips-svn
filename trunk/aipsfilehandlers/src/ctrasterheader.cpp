//
// C++ Implementation: %{MODULE}
//
// Description: A file header for the bin format
//
//
// Author: P. Musmann <p.musmann@fz-juelich.de>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ctrasterheader.h"
//header structs
#include "trasterheader.h"


using namespace std;

CTRasterHeader::CTRasterHeader() throw()
{
}

CTRasterHeader::~CTRasterHeader() throw()
{
}

// Loads the header from an open stream
void CTRasterHeader::loadHeader( istream& theFile )
    throw( FileException )
{
    // raw load of header
    TRasterFileHeader header;
    theFile.read( reinterpret_cast<char*>(&header), sizeof(header) );

    // Check if something went wrong
    if( (header.magic != RASTER_FILE_MAGIC) || (header.file_version != RASTER_FILE_VERSION) )
    {
        //TODO: check endianess
        throw( FileException(
            SERROR( "Errornous file header" ),
            CException::RECOVER, ERR_FILEHEADER ) );
    }
    else    //everything seams ok
    {
        // data type
        setUnsignedLong( "data_type", header.data_type );
        // little endian
        setEndianess( false );
        // sets ushort VoxelSize
/* FIXME */				
        //setVoxelSize( header.data_type_size );
        // dimension
        vector<size_t> dimensionSize(3);
        dimensionSize[0] = header.width;
        dimensionSize[1] = header.height;
        dimensionSize[2] = header.depth;
        setExtents( dimensionSize );
        // description
        setString( "description", header.description );
    }

}

// Saves the header to an open stream
void CTRasterHeader::saveHeader( ostream& theFile )
    throw( FileException )
{
    TRasterFileHeader header;

    header.magic = RASTER_FILE_MAGIC;
    header.file_version = RASTER_FILE_VERSION;
    header.data_type = getUnsignedLong( "data_type" );
    header.byte_order = getEndianess();
/* FIXME */		
    //header.data_type_size = getVoxelSize();

    vector<size_t> dimensionSize = getExtents();
    header.width = dimensionSize.at(0);
    header.height = dimensionSize.at(1);
    header.depth = dimensionSize.at(2);
    header.crc32 = getUnsignedLong( "crc32" );
    // description field
    strncpy( header.description, getString( "description" ).c_str(), sizeof(header.description) );

    theFile.write( reinterpret_cast<const char*>(&header), sizeof(header) );
}
