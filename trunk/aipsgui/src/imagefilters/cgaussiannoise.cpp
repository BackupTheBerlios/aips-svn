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
#include "cgaussiannoise.h"

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

CGaussianNoise::CGaussianNoise( ulong ulID ) throw()
: CFilter ( ulID, "Add gaussian noise", 1, 1, "CGaussianNoise", "0.1", "CFilter" )
{
  setModuleID( sLibID );

  parameters.initDouble( "Sigma", 1.0, 0.0, 100000.0 );

  inputsVec[0].portType = IOInteger;
	outputsVec[0].portType = IOInteger;
}  

CGaussianNoise::~CGaussianNoise() throw()
{
}

/*****************
 * Other methods *
 *****************/

void CGaussianNoise::apply() throw()
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
  outputPtr->setMaximum( 0 );
  outputPtr->setMinimum( 65535 );
	
	double dSigma = parameters.getDouble( "Sigma" );
  gsl_rng* r = gsl_rng_alloc (gsl_rng_taus);
	
	TImage::iterator iit = inputPtr->begin();
	TImage::iterator oit = outputPtr->begin();
	
	while( oit != outputPtr->end() )
	{		
		long lValue = -1;
		do 
		{
			double der = gsl_ran_gaussian( r, dSigma );
			lValue = static_cast<long>( *iit + der );
		}	while( lValue < 0 || lValue > 65535 );
		*oit = static_cast<ushort>( lValue );
		outputPtr->adjustDataRange( *oit );
		++iit; ++oit;
	}
	
	gsl_rng_free (r);
  setOutput( outputPtr );
	PROG_RESET();	
BENCHSTOP;
}
   
CPipelineItem* CGaussianNoise::newInstance( ulong ulID ) const throw()
{
  return new CGaussianNoise( ulID );
}


