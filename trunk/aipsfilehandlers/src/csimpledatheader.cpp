/************************************************************************
 * File: csimpledatheader.cpp                                           *
 * Project: AIPS - Basic file handlers plugin                           *
 * Description: A file header for the dat file format                   *
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
#include "csimpledatheader.h"

using namespace std;

CSimpleDatHeader::CSimpleDatHeader() throw()
{
}


CSimpleDatHeader::~CSimpleDatHeader() throw()
{
}

// Loads the header from an open stream
void CSimpleDatHeader::loadHeader( istream& theFile ) 
	throw( FileException )
{
FBEGIN;
  unsigned char ucBuffer[12];
  vector<size_t> dimensionSize(3);
	vector<size_t> swapSize(3);
  theFile.read( (char*)ucBuffer, 12 );
  dimensionSize[0] = static_cast<size_t>(ucBuffer[0])
    + (static_cast<size_t>(ucBuffer[1]) << 8);
  dimensionSize[1] = static_cast<size_t>(ucBuffer[4])
    + (static_cast<size_t>(ucBuffer[5]) << 8);
  dimensionSize[2] = static_cast<size_t>(ucBuffer[8])
    + (static_cast<size_t>(ucBuffer[9]) << 8);
	swapSize[0] = static_cast<size_t>(ucBuffer[3])
    + (static_cast<size_t>(ucBuffer[2]) << 8);
  swapSize[1] = static_cast<size_t>(ucBuffer[7])
    + (static_cast<size_t>(ucBuffer[6]) << 8);
  swapSize[2] = static_cast<size_t>(ucBuffer[11])
    + (static_cast<size_t>(ucBuffer[10]) << 8);
  // Check if something went wrong. In this case, the file may have the wrong endianess,
  // so we need to swap the bytes.
  if ( dimensionSize[0] == 0 || dimensionSize[1] == 0 || dimensionSize[2] == 0 )
  {
		if ( swapSize[0] != 0 && swapSize[1] != 0 && swapSize[2] != 0 )
		{
DBG2("This file is big endian");			
			setEndianess( true );
	    dimensionSize = swapSize;
		}
		else
		{
			throw( FileException( 
				SERROR( "Errornous file header or volume dimensions > 65535" ), 
				CException::RECOVER, ERR_FILEHEADER ) );
		}
  }
	else
	{
DBG2("This file is little endian");
		setEndianess( false );
	}
	setExtents( dimensionSize );
	setVoxelType( "UInt16" );   
FEND;	
}

// Saves the header to an open stream
void CSimpleDatHeader::saveHeader( ostream& theFile ) 
	throw( FileException )    		
{
	vector<size_t> dimensionSize = getExtents();
  unsigned char ucBuffer[12];
  for ( int i = 0; i < 12; i++ )
    ucBuffer[i] = 0;
	if ( getEndianess() )
	{
	  ucBuffer[3] = dimensionSize[0] % 256;
  	ucBuffer[2] = dimensionSize[0] / 256;
    ucBuffer[7] = dimensionSize[1] % 256;
	  ucBuffer[6] = dimensionSize[1] / 256;
  	if ( dimensionSize.size() == 2 )
   	{
     	ucBuffer[11] = 1; dimensionSize[2] = 1;
    }
 	  else
   	{
     	ucBuffer[11] = dimensionSize[2] % 256;
      ucBuffer[10] = dimensionSize[2] / 256;
 	  }
   }
	else
	{
		ucBuffer[0] = dimensionSize[0] % 256;
 	  ucBuffer[1] = dimensionSize[0] / 256;
   	ucBuffer[4] = dimensionSize[1] % 256;
    ucBuffer[5] = dimensionSize[1] / 256;
 	  if ( dimensionSize.size() == 2 )
   	{
     	ucBuffer[8] = 1; dimensionSize[2] = 1;
    }
 	  else
   	{
     	ucBuffer[8] = dimensionSize[2] % 256;
      ucBuffer[9] = dimensionSize[2] / 256;
 	  }		
	}
DBG3("Writing header");
  theFile.write( (char*)ucBuffer, 12 );
}
