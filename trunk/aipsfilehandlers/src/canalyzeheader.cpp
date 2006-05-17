/************************************************************************
 * File: canalyzeheader.cpp                                             *
 * Project: AIPS                                                        *
 * Description: Header for the analyze file format                      *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 2004-05-11                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#include "canalyzeheader.h"

using namespace std;

/// Struct defining the key header of an analyze file
/* These structs are based on the official analyze documentation and look quite ugly,
 so we better hide them here in the cpp file :-) */
struct SAnalyzeHeaderKey
{
  uint32_t sizeof_hdr;
  char dataType[10];
  char db_name[18];
  uint32_t dimensionSizes;
  int16_t session_error;
  char regular;
  int8_t hkey_un0;
}; /* this should be forty bytes long */

/// Struct defining the dimension header of an analyze file
struct SAnalyzeImageDimension
{
  uint16_t dim[8];
	char vox_units[4];
	char cal_units[8];
  int16_t unused;
  uint16_t datatype;
  int16_t bitpix;
  int16_t dim_un0;
  float pixdim[8];
  float vox_offset;
	float scale; // Scale factor in SPM99/SPM2
	float dcoff; // Intensity zero interception in SPM2
  float unused2[1];
  float cal_max;
  float cal_min;
  float compressed;
  float verified;
  int32_t glmax,glmin;
}; /* this should be 108 bytes long */

/// Struct defining the history header of an analyze file
struct SAnalyzeDataHistory
{
  char descrip[80];
  char aux_file[24];
  char orient;
  int16_t originator[5]; // Origin in SPM99/SPM2
  char generated[10];
  char scannum[10];
  char patient_id[10];
  char exp_date[10];
  char exp_time[10];
  char hist_un0[3];
  int32_t views;
  int32_t vols_added;
  int32_t start_field;
  int32_t field_skip;
  int32_t omax,omin;
  int32_t smax,smin;
}; /* this should be 200 bytes long */

/// Struct defining the complete header of an analyze file
struct SAnalyzeHeader
{
  SAnalyzeHeaderKey hk;
  SAnalyzeImageDimension dime;
  SAnalyzeDataHistory hist;
}; /* this should be 348 bytes long */

/*************
 * Structors *
 *************/
CAnalyzeHeader::CAnalyzeHeader() throw()
 : CImageHeader()
{
}

CAnalyzeHeader::~CAnalyzeHeader() throw()
{
}

/*****************
 * Other methods *
 *****************/
 
void CAnalyzeHeader::loadHeader( istream& theFile ) throw( FileException )
{
	SAnalyzeHeader header;
	// Basically we don't swap the endianess
	setEndianess( false );
  // Read the header from file
  theFile.read( (char*) &header, sizeof(header) );
  // Header has not 348 bytes. Maybe we have the wrong endianess
  if ( header.hk.sizeof_hdr != 348 )
    {
			setEndianess( true );
			uint32_t headSize = header.hk.sizeof_hdr;
      swapEndianess( headSize );
      if ( headSize != 348 )
      {
        // Header size is still not correct. This cannot be valid ANALYZE 7.5 file
        throw ( FileException( SERROR(
              "This file is not a valid ANALYZE 7.5 file. We do not support SPM2 extended headers." ),
              CException::RECOVER, ERR_FILEACCESS ) );
      }
			for( ushort i = 0; i < 8; ++i )
			{
      	swapEndianess(header.dime.dim[i]);
				swapEndianess(header.dime.pixdim[i]);
			}
      swapEndianess(header.dime.datatype);
      swapEndianess(header.dime.bitpix);
			swapEndianess(header.dime.vox_offset);
			swapEndianess(header.dime.cal_max);
			swapEndianess(header.dime.cal_min);
			swapEndianess(header.dime.compressed);
			swapEndianess(header.dime.verified);
			swapEndianess(header.dime.glmax);
			swapEndianess(header.dime.glmin);
      swapEndianess(header.hk.dataType);
      swapEndianess(header.hk.dimensionSizes);
      swapEndianess(header.hk.session_error);
			swapEndianess(header.hist.views);
			swapEndianess(header.hist.vols_added);
			swapEndianess(header.hist.start_field);
			swapEndianess(header.hist.field_skip);
			swapEndianess(header.hist.omax);
			swapEndianess(header.hist.omin);
			swapEndianess(header.hist.smax);
			swapEndianess(header.hist.smin);
    }
	
	// File format specs sez' that dimensionSizes must be 16384 and hold an 'r' to charactirize a regular file
 	if ( header.hk.regular != 'r' )
 		alog << LWARN << SERROR( "File is not a regular ANALYZE 7.5 file. Further operations may fail." ) << endl;
	if ( header.hk.dimensionSizes != 16384 )
		alog << LWARN
			<< SERROR( "dimensionSizes != 16384. This file is not conforming to the analyze standard." ) << endl;

	vector<size_t> imageExtents;
	if ( header.dime.dim[0] > 4 && header.dime.dim[3] > 1 )
	{
		alog << LFIXME 
			<< SERROR("Analyze file contains more than one volume. Only reading the first volume")
			<< endl;
		header.dime.dim[0] = 4;
	}
	for( ushort i = 1; i < header.dime.dim[0]; ++i )
		imageExtents.push_back( header.dime.dim[i] );

DBG3(	"Image dimensions are " << header.dime.dim[1] << " x " << header.dime.dim[2]
	<< " x " << header.dime.dim[3] );
DBG3( " Dataset orientation is " << (int)header.hist.orient );
		
	switch( header.dime.datatype )
	{
		case 2: // uchar
			setVoxelType( "UInt8" ); 
			break;
		case 4:
			setVoxelType( "Int16" );
			break;
		case 8:
			setVoxelType( "Int32" );
			break;		
		case 16:
			setVoxelType( "Float16" );
			break;
		case 32:
			setVoxelType( "Float32" );
			break;
		case 64:
			setVoxelType( "Float32" );
			break;
		default:
		setVoxelType( "Int16" );//	setVoxelType( "Unknown" );	
	}
	setDouble( "VoxelOffset", header.dime.vox_offset );
	setUnsignedLong( "Orientation", header.hist.orient );
	setExtents( imageExtents );
  setLong( "DataType", header.dime.datatype ); 
	setLong( "BitsPerPixel", header.dime.bitpix );
	setString( "DBName", header.hk.db_name );
	setString( "VoxUnits", header.dime.vox_units );
	setString( "CalUnits", header.dime.cal_units );
	setVoxelDimensions( header.dime.pixdim[1], header.dime.pixdim[2], header.dime.pixdim[3] );
	setDouble( "Scale", header.dime.scale );
	setDouble( "DCoff", header.dime.dcoff );
	setDouble( "CalMax", header.dime.cal_max );
	setDouble( "CalMin", header.dime.cal_min );
	setDouble( "Compressed", header.dime.compressed );
	setDouble( "Verified", header.dime.verified );
	setString( "Description", header.hist.descrip );
	setString( "AuxFile", header.hist.aux_file );
	setOrigin( header.hist.originator[0], header.hist.originator[1], header.hist.originator[2] );
	setString( "Generated", header.hist.generated );
	setString( "ScanNum", header.hist.scannum );
	setString( "PatientID", header.hist.patient_id );
	setString( "ExpDate", header.hist.exp_date );
	setString( "ExpTime", header.hist.exp_time );
	setLong( "Views", header.hist.views );
	setLong( "VolsAdded", header.hist.vols_added );
	setLong( "StartField", header.hist.start_field );
	setLong( "FieldSkip", header.hist.field_skip );
	setLong( "OMax", header.hist.omax );
	setLong( "OMin", header.hist.omin );
	setLong( "SMax", header.hist.smax );
	setLong( "SMin", header.hist.smin );
}

void CAnalyzeHeader::saveHeader( ostream& theFile ) throw( FileException )    		
{
	SAnalyzeHeader header;
	header.hk.sizeof_hdr = 348;
	header.hk.dimensionSizes = 16384;
	header.hk.session_error = 0;
	header.hk.regular = 'r';	
	vector<size_t> imageExtents = getExtents();
	header.dime.dim[0] = imageExtents.size() + 1;
	// Support for multidimensional files (e.g. 4D volumes)
	if ( isDefined( "DataDimension" ) )
	{
		header.dime.dim[header.dime.dim[0]] = getUnsignedLong( "DataDimension" );
		header.dime.dim[0]++;			
	}
	for( uint i = 0; i < imageExtents.size(); ++i )
		header.dime.dim[i + 1] = imageExtents[i];
	header.dime.dim[header.dime.dim[0]] = 1;
	strncpy( header.hk.db_name, "AIPS Filehandler  ", 18 );
	// Determine data type and bits per pixel
	if ( getVoxelType() == "UInt8" )
	{
		header.dime.datatype = 2; 
		header.dime.bitpix = 8;
		strncpy( header.hk.dataType, "UCHAR     ", 10 );
	}
	else if ( getVoxelType() == "Int16" )
	{
		header.dime.datatype = 4; 
		header.dime.bitpix = 16;
		strncpy( header.hk.dataType, "SHORT     ", 10 );
	}
	else
	if ( getVoxelType() == "Int32" )
	{
		header.dime.datatype = 8; 
		header.dime.bitpix = 32;
		strncpy( header.hk.dataType, "INT       ", 10 );
	}
	else
	if ( getVoxelType() == "Float16" )
	{
		header.dime.datatype = 16; 
		header.dime.bitpix = sizeof(float)*8;
		strncpy( header.hk.dataType, "FLOAT     ", 10 );
	}
	else
	if ( getVoxelType() == "Float32" )
	{
		header.dime.datatype = 32; 
		header.dime.bitpix = sizeof(double)*8;
		strncpy( header.hk.dataType, "DOUBLE    ", 10 );
	}
	strncpy(header.dime.vox_units,"mm  ", 4);
	strncpy(header.dime.cal_units,"mm      ", 8);
	header.dime.pixdim[0] = 3.0;
	std::vector<double> voxelDimensions = getVoxelDimensions();
	header.dime.pixdim[1] = voxelDimensions[0];
	if ( voxelDimensions.size() > 1 )
		header.dime.pixdim[2] = voxelDimensions[1];
	else
		header.dime.pixdim[2] = 1.0;
	if ( voxelDimensions.size() > 2 )
		header.dime.pixdim[3] = voxelDimensions[2];
	else
		header.dime.pixdim[3] = 1.0;
	header.dime.vox_offset = 0.0;
	header.dime.scale = 1.0;    // SPM Scale Factor
	header.dime.dcoff = 0.0;		// SPM2 zero intesity (Dunno what it's good for)???
	header.dime.cal_min = 0.0; // ???
	header.dime.cal_max = 0.0; // ???
	header.dime.compressed = 0.0; // ???
	header.dime.verified = 0.0; // ???
	header.dime.glmax = getLong( "MaxIntensity" );
	header.dime.glmin = getLong( "MinIntensity" );
	strncpy( header.hist.descrip, "Created by the AIPS image filehandler library                         ", 80 );	
	strncpy( header.hist.aux_file, "                           ", 24 ); // ???
	std::vector<double> origin = getOrigin();
	header.hist.originator[0] = static_cast<int16_t>( round( origin[0] ) ); // SPM Origin X
	if ( origin.size() > 1 )
		header.hist.originator[1] = static_cast<int16_t>( round( origin[1] ) ); // SPM Origin Y
	else
		header.hist.originator[1] = 1; // SPM Origin Y
	if ( origin.size() > 2 )
		header.hist.originator[2] = static_cast<int16_t>( round( origin[2] ) ); // SPM Origin Z
	else
		header.hist.originator[2] = 1; // SPM Origin Z
	header.hist.orient = 0;
	strncpy( header.hist.generated, "          ", 10 );
	strncpy( header.hist.scannum, "          ", 10 );
	strncpy( header.hist.patient_id, "          ", 10 );
	strncpy( header.hist.exp_date, "          ", 10 );
	strncpy( header.hist.exp_time, "          ", 10 );
	header.hist.views = 1; // ???
	header.hist.vols_added = 0; // ???
	header.hist.start_field = 0; // ???
	header.hist.field_skip = 0; // ???
	header.hist.omax = 0; // ???
	header.hist.omin = 0; // ???
	header.hist.smax = 0; // ???
	header.hist.smin = 0; // ???
	theFile.write( (char*) &header, 348 );
	if ( !theFile.good() )
		throw( FileException( "Error on writing ANALYZE header", CException::RECOVER, ERR_FILEACCESS ) );
}

