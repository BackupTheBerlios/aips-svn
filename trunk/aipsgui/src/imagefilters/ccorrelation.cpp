/************************************************************************
 * File: ccorrelation.h                                                 *
 * Project: AIPS image filters plugin                                   *
 * Description: Computes the voxelwise correlation of two images        *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 2004-05-07                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#include "ccorrelation.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

CCorrelation::CCorrelation( ulong ulID ) throw()
: CFilter ( ulID, "Image correlation", 2, 1, "CCorrelation", "0.2", "CFilter" )
{
  setModuleID( sLibID );

  sDocumentation = "Adds or subtracts two images.\n"
                   "** Input ports:\n"
                   "0: A scalar 2D or 3D multichannel data set\n"
									 "1: A scalar 2D or 3D multichannel data set\n"
                   "**Output ports:\n"
                   "0: A scalar 2D or 3D multichannel data set";


  inputsVec[0].portType = IOInteger;
  inputsVec[1].portType = IOInteger;
	outputsVec[0].portType = IOInteger;
}  

CCorrelation::~CCorrelation() throw()
{
}

/*****************
 * Other methods *
 *****************/

void CCorrelation::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
  TImagePtr inputPtr1 = static_pointer_cast<TImage>( getInput( 0 ) );
  if ( inputPtr1.get() == NULL || inputPtr1->getDimension() < 2 || inputPtr1->getDimension() > 3 )
  {
    alog << LWARN << SERROR("Input type is no 2D or 3D image!") << endl;
    return;
  }

  TImagePtr inputPtr2 = static_pointer_cast<TImage>( getInput( 1 ) );
  if ( inputPtr2.get() == NULL || inputPtr2->getDimension() < 2 || inputPtr2->getDimension() > 3 )
  {
    alog << LWARN << SERROR("Input type is no 2D or 3D image!") << endl;
    return;
  }
	bModuleReady = true;
  deleteOldOutput();

  size_t dimensionSize[3];
  dimensionSize[0] = std::max( inputPtr1->getExtent(0), inputPtr2->getExtent(0) );
  dimensionSize[1] = std::max( inputPtr1->getExtent(1), inputPtr2->getExtent(1) );
  if ( inputPtr1->getDimension() == 3 )
    dimensionSize[2] = inputPtr1->getExtent(2);
  else
    dimensionSize[2] = 1;		
	if ( inputPtr2->getDimension() == 3 && inputPtr2->getExtent( 2 ) > dimensionSize[2] )
		dimensionSize[2] = inputPtr2->getExtent( 2 );
	ushort usOutputDataDimension = std::max( inputPtr1->getDataDimension(),
		inputPtr2->getDataDimension() );
		
  TImagePtr outputPtr ( new TImage( (dimensionSize[2] > 1)?3:2,
    dimensionSize, usOutputDataDimension ) );
  outputPtr->setMaximum( 0 );
  outputPtr->setMinimum( 65535 );
	
	PROG_MAX( usOutputDataDimension * dimensionSize[2] );
	for ( ushort usChannel = 0; usChannel < usOutputDataDimension; ++usChannel )
		for ( ushort z = 0; z < dimensionSize[2]; ++z )
		{
			PROG_VAL ( ( usChannel + 1 ) * z );
			APP_PROC();
			for ( ushort y = 0; y < dimensionSize[1]; ++y )
				for ( ushort x = 0; x < dimensionSize[0]; ++x )
				{
					long lValue = 0;
					if ( inputPtr1->getDataDimension() > usChannel && x < inputPtr1->getExtent( 0 ) 
						&& y < inputPtr1->getExtent( 1 ) 
						&& (inputPtr1->getDimension()==3?z < inputPtr1->getExtent( 2 ):z==0) )
						lValue = ( (*inputPtr1)( x, y, z, usChannel ) );
					if ( inputPtr2->getDataDimension() > usChannel && x < inputPtr2->getExtent( 0 ) 
						&& y < inputPtr2->getExtent( 1 ) 
						&& (inputPtr2->getDimension()==3?z < inputPtr2->getExtent( 2 ):z==0) )
					{
						lValue -= ( (*inputPtr2)( x, y, z, usChannel ) );
					}
					lValue = std::abs( lValue );					
    			if ( lValue > 65535 ) 
						lValue = 65535;
		    	(*outputPtr)( x, y, z, usChannel ) = static_cast<ushort>( lValue );
		    	outputPtr->adjustDataRange( (*outputPtr)( x, y, z, usChannel ) );						
				}
			}
  setOutput( outputPtr );
	PROG_RESET();	
BENCHSTOP;
}
   
CPipelineItem* CCorrelation::newInstance( ulong ulID ) const throw()
{
  return new CCorrelation( ulID );
}


