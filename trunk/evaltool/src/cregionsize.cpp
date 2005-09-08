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
#include "cregionsize.h"

using namespace std;
using namespace boost;

/* Structors */

CRegionSize::CRegionSize( ulong ulID ) throw()
: CFilter ( ulID, "Region Size", 2, 1 )
{
  setModuleID( sLibID );

	sDocumentation = "Computes the mean distance of two shapes. The images must "
									 "be of the same size and dimensionality. They also must have "
									 "only one channel.\n"
                   "** Input ports:\n"
                   "0: A scalar 2D or 3D singlechannel data set\n"
                   "**Output ports:\n"
                   "0: A single scalar floating point value";

	parameters.initDouble("Result",0,0,1000);
	
  inputsVec[0].portType = IOInteger;
  inputsVec[1].portType = IOInteger;
	outputsVec[0].portType = IOPoint;
}  

CRegionSize::~CRegionSize() throw()
{
}

/* Other methods */

void CRegionSize::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput( 0 ) );
  if ( !inputPtr || inputPtr->getDimension() < 2 || inputPtr->getDimension() > 3 
		|| inputPtr->getDataDimension() > 1 )
  {
    alog << LWARN << SERROR("Input type is no single-channel 2D or 3D image!") << endl;
    return;
  }
	
	deleteOldOutput();
	
	ulong ulRegionSize = 0;
	for( TImage::iterator it = inputPtr->begin(); it != inputPtr->end(); ++it )
		if ( *it > 0 ) ulRegionSize++;
	parameters.setDouble( "Result", static_cast<double>(ulRegionSize) );	
	
alog << LINFO << "*** Region size is: " << ulRegionSize << endl;
BENCHSTOP;
}
   
CPipelineItem* CRegionSize::newInstance( ulong ulID ) const throw()
{
  return new CRegionSize( ulID );
}
