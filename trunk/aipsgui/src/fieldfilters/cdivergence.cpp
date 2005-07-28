/************************************************************************
 * File: cdivergence.cpp                                                *
 * Project: AIPS                                                        *
 * Description: Generates a scalar divergence field from a vector field *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 03.02.04                                                    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "cdivergence.h"

using namespace std;
using namespace boost;

CDivergence::CDivergence( ulong ulID ) throw()
  : CFilter ( ulID, "Divergence operator", 1, 1, "CDivergence", "0.1", "CFilter" )
{
  setModuleID( sLibID );
  sDocumentation = "Creates a scalar divergence field from the input vector field\n"
                   "** Input ports:\n"
                   "0: A 2D vector field\n"
                   "** Output ports:\n"
                   "1: A 2D single channel image"
                   "** Parameters:\n"
                   "Intensity Range: Intensity range of resulting scalar image";

  parameters.initUnsignedLong( "Intensity Range", 255, 1, 65535 );
                   
  inputsVec[0].portType = CPipelineItem::IOVector;
  outputsVec[0].portType = CPipelineItem::IOInteger;

}

CDivergence::~CDivergence() throw()
{
}

/** Reimplemented from CPipelineItem */
void CDivergence::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
  shared_ptr<TField2D> inputPtr = static_pointer_cast<TField2D>( getInput() );
  if ( inputPtr == NULL || inputPtr->getDimension() != 2 )
  {
    alog << LWARN << "Input type is no 2D field!" << endl;
    return;
  }
	bModuleReady = true;
  deleteOldOutput();
  shared_ptr<TImage> outputPtr ( new TImage( 2, inputPtr->getExtents() ) );
  outputPtr->setMinimum( 0 );
  outputPtr->setMaximum( parameters.getUnsignedLong( "Intensity Range" ) );
  
  ushort usWidth = inputPtr->getExtent(0);
  ushort usHeight = inputPtr->getExtent(1);
	double dIntensityRange = parameters.getDouble("Intensity Range");
  
	for ( ushort x = 1; x < usWidth - 1; x++ )
    for ( ushort y = 1; y < usHeight - 1; y++ )
    {
      (*outputPtr)( x, y ) = static_cast<ushort>( 128 + round(
          ( (*inputPtr)( x + 1, y )[0] + (*inputPtr)( x - 1, y )[0] )
        * ( ( dIntensityRange - 1.0 ) * 0.25 )
        + ( (*inputPtr)( x ,y + 1 )[1] + (*inputPtr)( x, y - 1 )[1] )
        * ( ( dIntensityRange - 1.0 ) * 0.25 ) ) );
    }

  setOutput( outputPtr );
BENCHSTOP;	
}

/** Reimplemented from CPipelineItem */
CPipelineItem* CDivergence::newInstance( ulong ulID ) const throw()
{
  return new CDivergence( ulID );
}
