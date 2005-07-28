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
#include "cmultiply.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

CMultiply::CMultiply( ulong ulID ) throw()
: CFilter ( ulID, "Image multiplication", 2, 1, "CMultiply", "0.1", "CFilter" )
{
  setModuleID( sLibID );

  parameters.initDouble( "FirstFactor", 1.0, -10000.0, 10000.0 );
	parameters.initDouble( "SecondFactor", 1.0, -10000.0, 10000.0 );

  inputsVec[0].portType = IOInteger;
  inputsVec[1].portType = IOInteger;
	outputsVec[0].portType = IOInteger;
}  

CMultiply::~CMultiply() throw()
{
}

/*****************
 * Other methods *
 *****************/

void CMultiply::apply() throw()
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

	// Determine dimensions of the resulting image
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
  outputPtr->setMinimum( 32767 );
	
	double dFirstFactor = parameters.getDouble( "FirstFactor" );
	double dSecondFactor = parameters.getDouble( "SecondFactor" );
	
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
					if ( inputPtr1->getDataDimension() > usChannel 
						&& x < inputPtr1->getExtent( 0 ) 
						&& y < inputPtr1->getExtent( 1 ) 
						&& (inputPtr1->getDimension()==3?z < inputPtr1->getExtent( 2 ):z==0) )
					{
						lValue = static_cast<long>( dFirstFactor 
							*	(*inputPtr1)( x, y, z, usChannel ) );
					}
					if ( inputPtr2->getDataDimension() > usChannel 
						&& x < inputPtr2->getExtent( 0 ) 
						&& y < inputPtr2->getExtent( 1 ) 
						&& (inputPtr2->getDimension()==3?z < inputPtr2->getExtent( 2 ):z==0) )
					{
						lValue *= static_cast<long>( dSecondFactor
							* (*inputPtr2)( x, y, z, usChannel ) );
					}
					// Crop illegal values
			    if ( lValue < 0 ) 
						lValue = 0;
    			else if ( lValue > 32767 ) 
						lValue = 32767;
		    	(*outputPtr)( x, y, z, usChannel ) = static_cast<short>( lValue );
			    if ( (*outputPtr)( x, y, z, usChannel ) > outputPtr->getMaximum() )
  	    		outputPtr->setMaximum( (*outputPtr)( x, y, z, usChannel ) );
			    if ( (*outputPtr)( x, y, z, usChannel ) < outputPtr->getMinimum() )
		  	    outputPtr->setMinimum( (*outputPtr)( x, y, z, usChannel ) );								
				}
			}
  setOutput( outputPtr );
	PROG_RESET();	
BENCHSTOP;
}
   
CPipelineItem* CMultiply::newInstance( ulong ulID ) const throw()
{
  return new CMultiply( ulID );
}


