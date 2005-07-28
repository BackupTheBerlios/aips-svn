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
#include "cextractintensity.h"

using namespace std;
using namespace boost;

/* Structors */

CExtractIntensity::CExtractIntensity( ulong ulID ) throw()
: CFilter ( ulID, "Extract intensity range", 1, 1, "CExtractIntensity", "0.1", "CFilter" )
{
  setModuleID( sLibID );

	parameters.initLong( "Low", 0, 0, 65535 );
	parameters.initLong( "High", 255, 0, 65535 );
  inputsVec[0].portType = IOInteger;
	outputsVec[0].portType = IOInteger;
}  

CExtractIntensity::~CExtractIntensity() throw()
{
}

/* Other methods */

void CExtractIntensity::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput( 0 ) );
  if ( inputPtr.get() == NULL || inputPtr->getDimension() < 2 || inputPtr->getDimension() > 3 )
  {
    alog << LWARN << "Input type is no 2D or 3D image!" << endl;
    return;
  }

	bModuleReady = true;
  deleteOldOutput();

	long low = parameters.getLong( "Low" );
	long high = parameters.getLong( "High" );
	
  TImagePtr outputPtr ( new TImage( inputPtr->getDimension(), inputPtr->getExtents(), 
		inputPtr->getDataDimension() ) );
  outputPtr->setMaximum( high );
  outputPtr->setMinimum( 0 );
	
	TImage::iterator inputIt = inputPtr->begin();
	TImage::iterator outputIt = outputPtr->begin();
	while( inputIt != inputPtr->end() )
	{
  	if ( (*inputIt) >= low && (*inputIt) <= high )
			(*outputIt) = (*inputIt);
		else
			(*outputIt) = 0;
		++outputIt;
		++inputIt;
	}
  setOutput( outputPtr );
PROG_RESET();	
BENCHSTOP;
}
   
CPipelineItem* CExtractIntensity::newInstance( ulong ulID ) const throw()
{
  return new CExtractIntensity( ulID );
}


