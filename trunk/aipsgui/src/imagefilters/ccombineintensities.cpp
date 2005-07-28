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
#include "ccombineintensities.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

CCombineIntensities::CCombineIntensities( ulong ulID ) throw()
   : CFilter ( ulID, "Combine intensities", 1, 1, "CCombineIntensities","0.1","CFilter" )
{
  setModuleID( sLibID );
}

CCombineIntensities::~CCombineIntensities() throw()
{
}

/***************** 
 * Other methods *
 *****************/

void CCombineIntensities::apply() throw()
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
  deleteOldOutput();	
  TImagePtr outputPtr ( new TImage( inputPtr->getDimension(), inputPtr->getExtents(), 
		inputPtr->getDataDimension() ) );
	for( uint z = 0; z < inputPtr->getExtent(2); ++z )
		for( uint y = 0; y < inputPtr->getExtent(1); ++y )
			for( uint x = 0; x < inputPtr->getExtent(0); ++x )
				{
					ushort val = (*inputPtr)(x,y,z);	
					if ( val == 6 || val == 9 || val == 17 || val == 3 || val == 45 || val == 79
						|| val == 5 || val == 7 || val == 19 || val == 15 || val == 39 ) 
						(*outputPtr)(x,y,z) = 1;
					else
						(*outputPtr)(x,y,z) = 0;
				}
  outputPtr->setMaximum( 1 );
  outputPtr->setMinimum( 0 );    
  setOutput( outputPtr );
BENCHSTOP;
}

CPipelineItem* CCombineIntensities::newInstance( ulong ulID ) const throw()
{
  return new CCombineIntensities( ulID );
}



