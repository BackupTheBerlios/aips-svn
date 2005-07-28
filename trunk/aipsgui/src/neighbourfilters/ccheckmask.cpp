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
#include "ccheckmask.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

/**
 * \param ulID unique module ID
 */
CCheckmask::CCheckmask( ulong ulID ) throw()
  : CFilter ( ulID, "Check mask", 2, 1, "CCheckmask", "0.2", "CFilter" )
{
  setModuleID( sLibID );

  sDocumentation = "Extracts local maxima from the given image\n"
                   "** Input ports:\n"
                   "0: A scalar 2D or 3D multichannel data set\n"
                   "**Output ports:\n"
                   "0: A scalar 2D or 3D multichannel data set\n"
                   "**Parameters:\n"
                   "Radius: radius of filter kernel";

  parameters.initUnsignedLong( "Difference", 1UL, 1UL, 65535UL );	
	inputsVec[0].portType = IO2DInteger;
	inputsVec[1].portType = IO2DInteger;
}

CCheckmask::~CCheckmask() throw()
{
}

/*****************
 * Other methods *
 *****************/
 
void CCheckmask::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
	TImagePtr image = static_pointer_cast<TImage>( getInput(1) );
  if ( inputPtr.get() == NULL || inputPtr->getDimension() != 2 
		|| image.get() == NULL || image->getDimension() != 2 )
  {
    alog << LWARN << "Input type is no 2D  image!" << endl;
    return;
  }
	bModuleReady = true;
  deleteOldOutput();

  size_t dimensionSize[2];
  dimensionSize[0] = inputPtr->getExtent(0);
  dimensionSize[1] = inputPtr->getExtent(1);
  
  TImagePtr outputPtr ( new TImage( 2, inputPtr->getExtents() ) );
	
  (*outputPtr) = 0;
  outputPtr->setMaximum( 1 );
  outputPtr->setMinimum( 0 );

  ulong uldiff =  parameters.getUnsignedLong("Difference");
	short diff = static_cast<short>( uldiff );
  PROG_MAX( dimensionSize[1] );
	
  for ( ushort y = 1; y < ( dimensionSize[1] -1); ++y )
	{
  	APP_PROC();
		PROG_VAL( y );
    for ( ushort x = 1; x < ( dimensionSize[0] -1); ++x )
    {
			if ( (*inputPtr)(x,y) > 0 )
			{
				bool isGood = false;
				// Check mask 1
				short val = static_cast<short>((*image)(x,y));
				short mask[5];
				mask[0] = abs( static_cast<short>((*image)(x-1,y-1)) - val );
				mask[1] = abs( static_cast<short>((*image)(x-1,y+1)) - val );
				mask[2] = abs( static_cast<short>((*image)(x,y-1)) - val );
				mask[3] = abs( static_cast<short>((*image)(x,y+1)) - val );
				mask[4] = abs( static_cast<short>((*image)(x-1,y)) - val );				
				isGood = ( mask[0] >= diff && mask[1] >= diff && mask[2] >= diff && mask[3] >= diff				
					&& mask[4] >= diff );
				DS( x << ";" << y << " One: " << isGood );
				if ( !isGood )
				{
				mask[0] = abs( static_cast<short>((*image)(x-1,y-1)) - val );
				mask[1] = abs( static_cast<short>((*image)(x+1,y-1)) - val );
				mask[2] = abs( static_cast<short>((*image)(x-1,y) )- val );
				mask[3] = abs( static_cast<short>((*image)(x+1,y) )- val );
				mask[4] = abs( static_cast<short>((*image)(x,y-1)) - val );
				isGood = ( mask[0] >= diff && mask[1] >= diff && mask[2] >= diff && mask[3] >= diff
				&& mask[4] >= diff );					
				DS( x << ";" << y << " 2: " << isGood );
				}
				if ( !isGood )
				{
				mask[0] = abs( static_cast<short>((*image)(x-1,y+1) )- val );
				mask[1] = abs( static_cast<short>((*image)(x+1,y+1)) - val );
				mask[2] = abs( static_cast<short>((*image)(x-1,y) )- val );
				mask[3] = abs( static_cast<short>((*image)(x+1,y) )- val );
				mask[4] = abs( static_cast<short>((*image)(x,y+1)) - val );
				isGood = ( mask[0] >= diff && mask[1] >= diff && mask[2] >= diff && mask[3] >= diff
					&& mask[4] >= diff );		
					DS( x << ";" << y << " 3: " << isGood );			
				}
				if ( !isGood )
				{
				mask[0] = abs( static_cast<short>((*image)(x+1,y-1) )- val );
				mask[1] = abs( static_cast<short>((*image)(x+1,y+1)) - val );
				mask[2] = abs( static_cast<short>((*image)(x,y-1) )- val );
				mask[3] = abs( static_cast<short>((*image)(x,y+1) )- val );
				mask[4] = abs( static_cast<short>((*image)(x+1,y)) - val );
				isGood = ( mask[0] >= diff && mask[1] >= diff && mask[2] >= diff && mask[3] >= diff
					&& mask[4] >= diff );	
					DS( x << ";" << y << " 4: " << isGood );				
				}				
				if ( isGood )
				{
					(*outputPtr)(x,y) = 1;
				}
			}
		}
	}
	 
PROG_RESET();
  setOutput( outputPtr );
BENCHSTOP;
}

CPipelineItem* CCheckmask::newInstance( ulong ulID ) const throw()
{
  return new CCheckmask( ulID );
}

