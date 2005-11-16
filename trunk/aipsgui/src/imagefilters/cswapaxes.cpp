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
#include "cswapaxes.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

CSwapAxes::CSwapAxes( ulong ulID ) throw()
   : CFilter ( ulID, "Swap axes", 1, 1, "CSwapAxes","0.1","CFilter" )
{
	parameters.initUnsignedLong( "Swap", 0, 1, 3 );
	// 0 - don't swap, 1 - swap xz, 2 - swap xy, 3 - swap yz
  setModuleID( sLibID );
}

CSwapAxes::~CSwapAxes() throw()
{
}

/***************** 
 * Other methods *
 *****************/

void CSwapAxes::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
  if ( inputPtr.get() == NULL )
  {
    alog << LWARN << "Input type is no 2D/3D image!" << endl;
    return;
  }
	bModuleReady = true;
	size_t extents[3];
	ulong ulSwap = parameters.getUnsignedLong("Swap");
	switch( ulSwap )
	{
		case 0:
			extents[0] = inputPtr->getExtent(0);
			extents[1] = inputPtr->getExtent(1);
			extents[2] = inputPtr->getExtent(2);
			break;
		case 1:
			extents[2] = inputPtr->getExtent(0);
			extents[1] = inputPtr->getExtent(1);
			extents[0] = inputPtr->getExtent(2);
			break;
		case 2:
			extents[1] = inputPtr->getExtent(0);
			extents[0] = inputPtr->getExtent(1);
			extents[2] = inputPtr->getExtent(2);
			break;
		case 3:
			extents[0] = inputPtr->getExtent(0);
			extents[2] = inputPtr->getExtent(1);
			extents[1] = inputPtr->getExtent(2);
			break;
	}
	TImagePtr outputPtr ( new TImage( inputPtr->getDimension(), extents, 
		inputPtr->getDataDimension() ) );
	
	for( uint z = 0; z < inputPtr->getExtent(2); ++z )
		for( uint y = 0; y < inputPtr->getExtent(1); ++y )
			for( uint x = 0; x < inputPtr->getExtent(0); ++x )
				{
					switch( ulSwap )
					{
						case 0:
							(*outputPtr)(x,y,z) = (*inputPtr)(x,y,z);	
							break;
						case 1:
							(*outputPtr)(z,y,x) = (*inputPtr)(x,y,z);	
							break;
						case 2:
							(*outputPtr)(y,x,z) = (*inputPtr)(x,y,z);	
							break;
						case 3:
							(*outputPtr)(x,z,y) = (*inputPtr)(x,y,z);	
							break;
					}					
				}
	outputPtr->setDataRange( inputPtr->getDataRange() );
  setOutput( outputPtr );
BENCHSTOP;
}

CPipelineItem* CSwapAxes::newInstance( ulong ulID ) const throw()
{
  return new CSwapAxes( ulID );
}

