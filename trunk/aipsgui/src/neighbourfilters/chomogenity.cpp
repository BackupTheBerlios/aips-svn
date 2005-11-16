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
#include "chomogenity.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

/**
 * \param ulID unique module ID
 */
CHomogenity::CHomogenity( ulong ulID ) throw()
  : CFilter ( ulID, "Homogenity filter", 2, 1, "CHomogenity", "0.4", "CFilter" )
{
  setModuleID( sLibID );

  sDocumentation = "Filters the input image\n"
                   "** Input ports:\n"
                   "0: A scalar 2D or 3D multichannel data set\n"
                   "**Output ports:\n"
                   "1: A scalar 2D or 3D multichannel data set\n"
                   "**Parameters:\n"
                   "Radius: radius of filter kernel";
									 
	inputsVec[0].portType = IO2DInteger;
	inputsVec[1].portType = IO2DInteger;
}

CHomogenity::~CHomogenity() throw()
{
}

/*****************
 * Other methods *
 *****************/
 
void CHomogenity::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
  if ( inputPtr.get() == NULL || inputPtr->getDimension() < 2 || inputPtr->getDimension() > 3 )
  {
    alog << LWARN << "Input type is no 2D or 3D image!" << endl;
    return;
  }
  TImagePtr lowPtr = static_pointer_cast<TImage>( getInput(1) );
  if ( lowPtr.get() == NULL || lowPtr->getDimension() < 2 || lowPtr->getDimension() > 3 )
  {
    alog << LWARN << "Input type is no 2D or 3D image!" << endl;
    return;
  }
	bModuleReady = true;
  deleteOldOutput();

  size_t dimensionSize[3];
  dimensionSize[0] = inputPtr->getExtent(0);
  dimensionSize[1] = inputPtr->getExtent(1);
  dimensionSize[2] = ( inputPtr->getDimension() == 3 ) ? inputPtr->getExtent(2) : 1;
  
  TImagePtr outputPtr ( new TImage( inputPtr->getDimension(),
    inputPtr->getExtents(), inputPtr->getDataDimension() ) );
  (*outputPtr) = (*inputPtr);
  outputPtr->setMaximum( 0 );
  outputPtr->setMinimum( 0 );

	TImage::iterator iit = inputPtr->begin();
	TImage::iterator lit = lowPtr->begin();
	TImage::iterator oit = outputPtr->begin();
	while( iit != inputPtr->end() )
	{
		*oit = static_cast<ushort>( std::abs( static_cast<short>(*iit) - static_cast<short>(*lit) ));
		outputPtr->adjustDataRange( *oit );
		++iit; ++lit; ++oit;
	}	
  setOutput( outputPtr );
BENCHSTOP;
}

CPipelineItem* CHomogenity::newInstance( ulong ulID ) const throw()
{
  return new CHomogenity( ulID );
}
