/************************************************************************
 * File: cdataheader.cpp                                                *
 * Project: AIPS - Basic file handlers plugin                           *
 * Description: File header for the data file format                    *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 2004-04-19                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "cdataheader.h"

using namespace std;

CDataHeader::CDataHeader() throw()
 : CImageHeader()
{
}

CDataHeader::~CDataHeader() throw()
{
}

/** \param theFile input stream to load from */
void CDataHeader::loadHeader( istream& theFile ) throw( FileException )
{
	char cBuffer[11];  
	ushort usVoxelSize;
	vector<size_t> dimensionSize( 3 );
	try
	{
	  theFile.getline( cBuffer, 10 );
	
		// Determine data type
	  usVoxelSize = atoi( cBuffer );
		if ( usVoxelSize == 1 )
			setVoxelType( "UInt8" );
		else if ( usVoxelSize == 2 )
			setVoxelType( "UInt16" );
		
		// Determine dimensions
	  for ( ushort i = 0; i < 3; i++ )
  	{
    	theFile.getline( cBuffer, 10 );
	    dimensionSize[i] = atoi( cBuffer );		
  	}
		setExtents( dimensionSize );
	
		// Determine endianess
		if ( usVoxelSize > 1 )
		{
			theFile.getline( cBuffer, 10 );
			// We also got some endianess information here
			if ( theFile.good() )
				setEndianess( atoi( cBuffer ) == 1 );
			else
			{
				theFile.clear();
				setEndianess( false );
			}
		}	
		else
			setEndianess( false );
	}
	catch( std::exception& e )
	{	
		throw( FileException( e.what() ) );
	}
}

/** \param theFile output stream to save to */
void CDataHeader::saveHeader( ostream& theFile ) throw( FileException )    		
{  
	std::string dataType = getVoxelType();
	try
	{
		if ( dataType == "UInt8" )
  		theFile << 1 << endl;    
		else if ( dataType == "UInt16" )
  		theFile << 2 << endl;    
	  theFile << getUnsignedLong( "ExtendX" ) << endl;
	  theFile << getUnsignedLong( "ExtendY" ) << endl;
  	theFile << getUnsignedLong( "ExtendZ" ) << endl;
		if ( dataType == "UInt16" )
		{
			if ( getEndianess() )
				theFile << "1" << endl;
			else
				theFile << "0" << endl;
		}	
	}
	catch( std::exception& e )
	{	
		throw( FileException( e.what() ) );
	}
}
