/************************************************************************
 * File: ccropintensityrange.cpp                                        *
 * Project: AIPS                                                        *
 * Description: Module to delete unwanted intensity values from an image*
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 08-07-2004                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#include "ccropintensityrange.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

/**
 * \param ulID unique module ID
 */
CCropIntensityRange::CCropIntensityRange( ulong ulID ) throw()
  : CFilter ( ulID, "Crop intensity range", 1, 1, "CCropIntensityRange", "0.1", "CFilter" )
{
  setModuleID( sLibID );
  
  sDocumentation = "Deletes all intensities ( = setting them to zero ) in the"
									 " image which are not in the given range\n"
                   "** Input ports:\n"
                   "0: A scalar multidimensional, multichannel data set\n"
                   "**Output ports:\n"
                   "1: A scalar multidimensional, multichannel data set";  
									 
	parameters.initUnsignedLong( "LowerIntensity", 0, 0, 65535 );
	parameters.initUnsignedLong( "UpperIntensity", 65535, 0, 65535 );
}


CCropIntensityRange::~CCropIntensityRange() throw()
{
}

/*****************
 * Other methods *
 *****************/
 
void CCropIntensityRange::apply () throw()
{
BENCHSTART;
	bModuleReady = false;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
  if ( inputPtr.get() == NULL )
  {
    alog << LWARN << "Input type is no image!" << endl;
    return;
  }
	bModuleReady = true;
  deleteOldOutput();

  TImagePtr outputPtr ( new TImage( inputPtr->getDimension(),
		inputPtr->getExtents(), inputPtr->getDataDimension() ) );
	
	alog << "Before" << endl;
	ushort usMinIntensity = static_cast<ushort> 
		( parameters.getUnsignedLong( "LowerIntensity" ) );
	ushort usMaxIntensity = static_cast<ushort> 
		( parameters.getUnsignedLong( "UpperIntensity" ) );
  alog << "After" << endl;
	
	outputPtr->setMinimum( 0 );
  outputPtr->setMaximum( usMaxIntensity );
		
	TImage::iterator inputIt = inputPtr->begin();
	TImage::iterator outputIt = outputPtr->begin();
	while( inputIt != inputPtr->end() )
	{
		if ( (*inputIt) >= usMinIntensity && (*inputIt) <= usMaxIntensity )
			(*outputIt) = (*inputIt);
		else
			(*outputIt) = 0;
		++inputIt; ++outputIt; 
	}
	
  setOutput( outputPtr );
BENCHSTOP;	
}

CPipelineItem* CCropIntensityRange::newInstance( ulong ulID ) const throw()
{
  return new CCropIntensityRange( ulID );
}
