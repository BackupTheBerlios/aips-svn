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
#include "crescalehistogram.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

CRescaleHistogram::CRescaleHistogram( ulong ulID ) throw()
: CFilter ( ulID, "Rescale histogram", 1, 1, "CRescaleHistogram", "0.1", "CFilter" )
{
  setModuleID( sLibID );
  setType(2);
  sDocumentation = "Adds or subtracts two images. Through free choice of the "
	                 "weighting parameters also subraction is possible\n"
                   "** Input ports:\n"
                   "0: A scalar 2D or 3D multichannel data set\n"
                   "**Output ports:\n"
                   "1: A scalar 2D or 3D multichannel data set\n"
                   "**Parameters:\n"
									 "First factor: factor applied to the first image before operation\n"
                   "Second factor: factor applied to the second image before operation";

  parameters.initLong( "New maximum", 255, 0, 65535 );
	parameters.initLong( "New minimum", 0, 0, 65535 );

  inputsVec[0].portType = IOInteger;
	outputsVec[0].portType = IOInteger;
}  

CRescaleHistogram::~CRescaleHistogram() throw()
{
}

/*****************
 * Other methods *
 *****************/

void CRescaleHistogram::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput( 0 ) );
  if ( inputPtr.get() == NULL || inputPtr->getDimension() < 2 || inputPtr->getDimension() > 3 )
  {
    alog << LWARN << SERROR("Input type is no 2D or 3D image!") << endl;
    return;
  }
	bModuleReady = true;
  deleteOldOutput();

	// Determine dimensions of the resulting image
  size_t dimensionSize[3];
  dimensionSize[0] = inputPtr->getExtent(0);
  dimensionSize[1] = inputPtr->getExtent(1);
  if ( inputPtr->getDimension() == 3 )
    dimensionSize[2] = inputPtr->getExtent(2);
  else
    dimensionSize[2] = 1;		
	ushort usOutputDataDimension = inputPtr->getDataDimension();
		
  TImagePtr outputPtr ( new TImage( (dimensionSize[2] > 1)?3:2,
    dimensionSize, usOutputDataDimension ) );
		
	long loMax = inputPtr->getDataRange().getMaximum();
	long loMin = inputPtr->getDataRange().getMinimum();
	double d = loMax-loMin;
	long lMax = parameters.getLong( "New maximum" );
	long lMin = parameters.getLong( "New minimum" );
	double e = lMax-lMin;
  outputPtr->setMaximum( lMax );
  outputPtr->setMinimum( lMin );

	TImage::iterator iit = inputPtr->begin();
	TImage::iterator oit = outputPtr->begin();
	
	while( oit != outputPtr->end() )
	{
		double i = d / static_cast<double>( *iit - loMin );
		long lValue = static_cast<long>( lMin + (e / i) );
		if ( lValue < 0 ) 
			lValue = 0;
		else if ( lValue > 65535 ) 
			lValue = 65535;
		*oit = static_cast<ushort>( lValue );
		++iit; ++oit;
	}

  setOutput( outputPtr );
BENCHSTOP;
}
   
CPipelineItem* CRescaleHistogram::newInstance( ulong ulID ) const throw()
{
  return new CRescaleHistogram( ulID );
}
