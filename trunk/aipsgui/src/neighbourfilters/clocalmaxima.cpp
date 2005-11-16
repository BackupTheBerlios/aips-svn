/************************************************************************
 * File: clocalmaxima.cpp                                               *
 * Project: AIPS                                                        *
 * Description: Extracts local maxima from the given image              *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 05-07-2004                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#include "clocalmaxima.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

/**
 * \param ulID unique module ID
 */
CLocalMaxima::CLocalMaxima( ulong ulID ) throw()
  : CFilter ( ulID, "Extract local maxima", 1, 1, "CLocalMaxima", "0.2", "CFilter" )
{
  setModuleID( sLibID );

  sDocumentation = "Extracts local maxima from the given image\n"
                   "** Input ports:\n"
                   "0: A scalar 2D or 3D multichannel data set\n"
                   "**Output ports:\n"
                   "0: A scalar 2D or 3D multichannel data set\n"
                   "**Parameters:\n"
                   "Radius: radius of filter kernel";

  parameters.initUnsignedLong( "Radius", 1UL, 1UL, 1000UL );	
}

CLocalMaxima::~CLocalMaxima() throw()
{
}

/*****************
 * Other methods *
 *****************/
 
void CLocalMaxima::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
  if ( inputPtr.get() == NULL || inputPtr->getDimension() < 2 || inputPtr->getDimension() > 3 )
  {
    alog << LWARN << "Input type is no 2D or 3D image!" << endl;
    return;
  }
	bModuleReady = true;
  deleteOldOutput();

  size_t dimensionSize[3];
  dimensionSize[0] = inputPtr->getExtent(0);
  dimensionSize[1] = inputPtr->getExtent(1);
  dimensionSize[2] = ( inputPtr->getDimension() == 3 ) ? inputPtr->getExtent(2) : 1;
  
  TImagePtr outputPtr ( new TImage( inputPtr->getDimension(),
    inputPtr->getExtents(), inputPtr->getDataDimension() ) );
  (*outputPtr) = (*inputPtr);
  outputPtr->setDataRange( inputPtr->getDataRange() );

  ulong ulRadius =  parameters.getUnsignedLong("Radius");
  PROG_MAX( inputPtr->getDataDimension() * dimensionSize[2] );
	
  for ( ushort usChannel = 0; usChannel < inputPtr->getDataDimension(); ++usChannel )
  if ( dimensionSize[2] > 1 )
  {	
    for ( ushort z = ulRadius; z < ( dimensionSize[2] - ulRadius ); ++z )
    {
      APP_PROC();
			PROG_VAL( z * ( usChannel + 1 ) );
      for ( ushort y = ulRadius; y < ( dimensionSize[1] - ulRadius ); ++y )
        for ( ushort x = ulRadius; x < ( dimensionSize[0] - ulRadius ); ++x )
        {
          ushort usLocalMaximum = 0;
					ushort usMaxPos[3];
          for ( ushort k =  x - ulRadius; k < ( x + ulRadius + 1 ); ++k )
            for ( ushort l =  y - ulRadius; l < ( y + ulRadius + 1 ); ++l )
              for ( ushort m =  z - ulRadius; m < ( z + ulRadius + 1 ); ++m )
							{
								if ( usLocalMaximum < (*outputPtr)( k, l, m, usChannel ) )
								{
									usMaxPos[0] = k; usMaxPos[1] = l; usMaxPos[2] = m;
									usLocalMaximum = (*outputPtr)( k, l, m, usChannel );
								}
							}
          for ( ushort k =  x - ulRadius; k < ( x + ulRadius + 1 ); ++k )
            for ( ushort l =  y - ulRadius; l < ( y + ulRadius + 1 ); ++l )
              for ( ushort m =  z - ulRadius; m < ( z + ulRadius + 1 ); ++m )
							{
			          (*outputPtr)( k, l, m, usChannel ) = 0;
							}
					(*outputPtr)( usMaxPos[0], usMaxPos[1], usMaxPos[2], usChannel ) = usLocalMaximum;
        }
    }
  }
  else
  {
    for ( ushort usChannel = 0; usChannel < inputPtr->getDataDimension(); ++usChannel )
    {
      APP_PROC();
			PROG_VAL( usChannel + 1 );
      for ( ushort y = ulRadius; y < ( dimensionSize[1] - ulRadius ); ++y )
        for ( ushort x = ulRadius; x < ( dimensionSize[0] - ulRadius ); ++x )
          {
	          ushort usLocalMaximum = 0;
						ushort usMaxPos[2];
    	      for ( ushort k =  x - ulRadius; k < ( x + ulRadius + 1 ); ++k )
      	      for ( ushort l =  y - ulRadius; l < ( y + ulRadius + 1 ); ++l )
								{
									if ( usLocalMaximum < (*outputPtr)( k, l, usChannel ) )
									{
										usMaxPos[0] = k; usMaxPos[1] = l; 
										usLocalMaximum = (*outputPtr)( k, l, usChannel );
										DS( usLocalMaximum << " at " << k << ";" << l );
									}
								}
	          for ( ushort k =  x - ulRadius; k < ( x + ulRadius + 1 ); ++k )
  	          for ( ushort l =  y - ulRadius; l < ( y + ulRadius + 1 ); ++l )
								{
									if ( k != usMaxPos[0] || l != usMaxPos[1] )
					          (*outputPtr)( k, l, usChannel ) = 0;
								}
          }
    }
  }
PROG_RESET();
  setOutput( outputPtr );
BENCHSTOP;
}

CPipelineItem* CLocalMaxima::newInstance( ulong ulID ) const throw()
{
  return new CLocalMaxima( ulID );
}
