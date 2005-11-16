/************************************************************************
 * File: $FILE                                                          *
 * Project: AIPS                                                        *
 * Description:                                                         *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: $DATE                                                       *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#include "ccropimage.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

CCropImage::CCropImage( ulong ulID ) throw()
   : CFilter ( ulID, "Crop image", 1, 1, "CCropImage","0.1","CFilter" )
{
	parameters.initUnsignedLong( "MinX", 0, 0, 65535 );
	parameters.initUnsignedLong( "MaxX", 65535, 0, 65535 );
	parameters.initUnsignedLong( "MinY", 0, 0, 65535 );
	parameters.initUnsignedLong( "MaxY", 65535, 0, 65535 );
	parameters.initUnsignedLong( "MinZ", 0, 0, 65535 );
	parameters.initUnsignedLong( "MaxZ", 65535, 0, 65535 );
  setModuleID( sLibID );
}

CCropImage::~CCropImage() throw()
{
}

/***************** 
 * Other methods *
 *****************/

void CCropImage::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
  if ( !checkInput<TImage>( inputPtr ) )
    return;
	
	bModuleReady = true;
  deleteOldOutput();

	size_t extents[3];
	size_t coords[6];
	coords[0] = parameters.getUnsignedLong( "MinX" );
	coords[0] = std::min( coords[0], inputPtr->getExtent(0) );
	coords[1] = parameters.getUnsignedLong( "MaxX" );
	coords[1] = std::min( coords[1], inputPtr->getExtent(0) );
	coords[2] = parameters.getUnsignedLong( "MinY" );
	coords[2] = std::min( coords[2], inputPtr->getExtent(1) );
	coords[3] = parameters.getUnsignedLong( "MaxY" );
	coords[3] = std::min( coords[3], inputPtr->getExtent(1) );
	
	if ( inputPtr->getDimension() == 3 )
	{
		coords[4] = parameters.getUnsignedLong( "MinZ" );
		coords[4] = std::min( coords[4], inputPtr->getExtent(2) );
		coords[5] = parameters.getUnsignedLong( "MaxZ" );
		coords[5] = std::min( coords[5], inputPtr->getExtent(2) );
		extents[2] = coords[5] - coords[4];
	}
	else
		extents[2] = 1;
	extents[0] = coords[1] - coords[0];
	extents[1] = coords[3] - coords[2];	
  TImagePtr outputPtr ( new TImage( inputPtr->getDimension(), extents, 
		inputPtr->getDataDimension() ) );
	for( uint z = 0; z < extents[2]; ++z )
		for( uint y = 0; y < extents[1]; ++y )
			for( uint x = 0; x < extents[0]; ++x )
				(*outputPtr)(x,y,z) = (*inputPtr)( x + coords[0], y + coords[2], z + coords[4] );	
  outputPtr->setDataRange( inputPtr->getDataRange() );				
  setOutput( outputPtr );
BENCHSTOP;
}

CPipelineItem* CCropImage::newInstance( ulong ulID ) const throw()
{
  return new CCropImage( ulID );
}
