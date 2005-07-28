/************************************************************************
 * File: cdf3header.cpp                                                 *
 * Project: AIPS                                                        *
 * Description: Header for the POVRay volume format df3                 *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 2004-06-03                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#include "cdf3header.h"

using namespace std;

CDF3Header::CDF3Header() throw()
{
}


CDF3Header::~CDF3Header() throw()
{
}

void CDF3Header::loadHeader( istream& theFile ) 
	throw( FileException )
{
  unsigned char ucBuffer[6];
  vector<size_t> dimensionSize(3);
	vector<size_t> swapSize(3);
  theFile.read( (char*)ucBuffer, 6 );
	// His header should always be little endian
  dimensionSize[0] = static_cast<size_t>(ucBuffer[1])
    + (static_cast<size_t>(ucBuffer[0]) << 8);
  dimensionSize[1] = static_cast<size_t>(ucBuffer[3])
    + (static_cast<size_t>(ucBuffer[2]) << 8);
  dimensionSize[2] = static_cast<size_t>(ucBuffer[5])
    + (static_cast<size_t>(ucBuffer[4]) << 8);
		
  // Check if something went wrong. 
  if ( dimensionSize[0] == 0 || dimensionSize[1] == 0 || dimensionSize[2] == 0 )
  {
		throw( FileException( 
			SERROR( "Errornous file header or volume dimensions > 65535" ), 
			CException::RECOVER, ERR_FILEHEADER ) );
  }
	
	setEndianess( false );
	setExtents( dimensionSize );
	setVoxelType( "UInt16" );   
}

void CDF3Header::saveHeader( ostream& theFile ) 
	throw( FileException )    		
{
	vector<size_t> dimensionSize = getExtents();
  unsigned char ucBuffer[6];
  for ( int i = 0; i < 12; i++ )
    ucBuffer[i] = 0;
	if ( getEndianess() )
		alog << LWARN << "Cannot write big endian df3 files. Forcing little endian order." << endl;
  ucBuffer[1] = dimensionSize[0] % 256;
 	ucBuffer[0] = dimensionSize[0] / 256;
  ucBuffer[3] = dimensionSize[1] % 256;
  ucBuffer[2] = dimensionSize[1] / 256;
 	if ( dimensionSize.size() == 2 )
 	{
   	ucBuffer[5] = 1; 
		ucBuffer[4] = 0;
  }
  else
 	{
   	ucBuffer[5] = dimensionSize[2] % 256;
    ucBuffer[4] = dimensionSize[2] / 256;
 	}
  theFile.write( (char*)ucBuffer, 6 );
}

