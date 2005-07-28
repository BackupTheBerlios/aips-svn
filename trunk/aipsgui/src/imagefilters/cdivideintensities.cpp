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
#include "cdivideintensities.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

CDivideIntensities::CDivideIntensities( ulong ulID ) throw()
   : CFilter ( ulID, "Divide intensities", 1, 1, "CDivideIntensities","0.1","CFilter" )
{
	parameters.initDouble( "Divisor", 1.0, 0.0, 65535.0 );
  setModuleID( sLibID );
}

CDivideIntensities::~CDivideIntensities() throw()
{
}

/***************** 
 * Other methods *
 *****************/

void CDivideIntensities::apply() throw()
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
	double dDiv = parameters.getDouble("Divisor");
  deleteOldOutput();	
  TImagePtr outputPtr ( new TImage( inputPtr->getDimension(), inputPtr->getExtents(), 
		inputPtr->getDataDimension() ) );
	for( uint z = 0; z < inputPtr->getExtent(2); ++z )
		for( uint y = 0; y < inputPtr->getExtent(1); ++y )
			for( uint x = 0; x < inputPtr->getExtent(0); ++x )
				{
					(*outputPtr)(x,y,z) = static_cast<ushort>( round( (*inputPtr)(x,y,z) / dDiv ) );	
				}
  outputPtr->setMaximum( static_cast<ushort>( round( inputPtr->getMaximum() / dDiv ) ) );
  outputPtr->setMinimum( static_cast<ushort>( round( inputPtr->getMinimum() / dDiv ) ) );    
  setOutput( outputPtr );
BENCHSTOP;
}

CPipelineItem* CDivideIntensities::newInstance( ulong ulID ) const throw()
{
  return new CDivideIntensities( ulID );
}
