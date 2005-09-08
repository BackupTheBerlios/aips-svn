/************************************************************************
 * File: cdicecoefficient.cpp                                           *
 * Project: AIPS                                                        *
 * Description: Dice coefficient of two images                          *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 2004-06-07                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "cdicecoefficient.h"

using namespace std;
using namespace boost;

/* Structors */

CDiceCoefficient::CDiceCoefficient( ulong ulID ) throw()
: CFilter ( ulID, "Dice Coefficient", 2, 1 )
{
  setModuleID( sLibID );

  sDocumentation = "Computes the dice coefficient of two images. The images must "
									 "be of the same size and dimensionality. They also must have the "
									 "same number of channels.\n"
                   "** Input ports:\n"
                   "0: A scalar 2D or 3D multichannel data set\n"
									 "1: A scalar 2D or 3D multichannel data set\n"
                   "**Output ports:\n"
                   "0: A single scalar floating point value";

	parameters.initDouble("Result",0,0,1000);

  inputsVec[0].portType = IOInteger;
  inputsVec[1].portType = IOInteger;
	outputsVec[0].portType = IOPoint;
}  

CDiceCoefficient::~CDiceCoefficient() throw()
{
}

/* Other methods */

void CDiceCoefficient::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
  TImagePtr inputPtr1 = static_pointer_cast<TImage>( getInput( 0 ) );
  if ( !inputPtr1  || inputPtr1->getDimension() < 2 || inputPtr1->getDimension() > 3 )
  {
    alog << LWARN << "Input type is no 2D or 3D image!" << endl;
    return;
  }

  TImagePtr inputPtr2 = static_pointer_cast<TImage>( getInput( 1 ) );
  if ( !inputPtr2  || inputPtr2->getDimension() < 2 || inputPtr2->getDimension() > 3 )
  {
    alog << LWARN << "Input type is no 2D or 3D image!" << endl;
    return;
  }
	
	deleteOldOutput();
	
	double dDiceCoefficient;
	ulong ulAreaOfImage1 = 0;
	ulong ulAreaOfImage2 = 0;
	ulong ulSharedArea = 0;
	TImage::iterator it1 = inputPtr1->begin(); 
	TImage::iterator it2 = inputPtr2->begin();
	while( it1 != inputPtr1->end() )
	{
		if ( *it1 > 0 ) ulAreaOfImage1++;
		if ( *it2 > 0 ) ulAreaOfImage2++;
		if ( *it1 > 0 && *it2 > 0 ) ulSharedArea++;
		++it1; ++it2;
	}
	dDiceCoefficient = 2.0 * static_cast<double>( ulSharedArea ) / 
		( static_cast<double>( ulAreaOfImage1 ) + static_cast<double>( ulAreaOfImage1 ) );
DS( "*** Dice coefficient is: " << dDiceCoefficient );
	parameters.setDouble( "Result", dDiceCoefficient );
BENCHSTOP;
}
   
CPipelineItem* CDiceCoefficient::newInstance( ulong ulID ) const throw()
{
  return new CDiceCoefficient( ulID );
}
