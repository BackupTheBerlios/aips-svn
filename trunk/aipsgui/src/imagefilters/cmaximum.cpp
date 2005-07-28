/***************************************************************************
 *   Copyright (C) 2004 by Hendrik Belitz                                  *
 *   h.belitz@fz-juelich.de                                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "cmaximum.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

CMaximum::CMaximum( ulong ulID ) throw()
: CFilter ( ulID, "Pixelwise maximum", 2, 1, "CMaximum", "0.1", "CFilter" )
{
  setModuleID( sLibID );

  sDocumentation = "Combines two images. For each pixel the maximum value of the two images at the\n"
									 "corresponding position is used.\n"
                   "** Input ports:\n"
                   "0: A scalar 2D or 3D multichannel data set\n"
                   "**Output ports:\n"
                   "1: A scalar 2D or 3D multichannel data set";

  inputsVec[0].portType = IOInteger;
  inputsVec[1].portType = IOInteger;
	outputsVec[0].portType = IOInteger;
}  

CMaximum::~CMaximum() throw()
{
}

/*****************
 * Other methods *
 *****************/

void CMaximum::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
  TImagePtr inputPtr1 = static_pointer_cast<TImage>( getInput( 0 ) );
  if ( !checkInput( inputPtr1, 2, 3 ) )
    return;

  TImagePtr inputPtr2 = static_pointer_cast<TImage>( getInput( 1 ) );
  if ( !checkInput( inputPtr2, 2, 3 ) )
    return;
		
	bModuleReady = true;
  deleteOldOutput();

	// Determine dimensions of the resulting image
  size_t dimensionSize[3];
  dimensionSize[0] = std::max( inputPtr1->getExtent(0), inputPtr2->getExtent(0) );
  dimensionSize[1] = std::max( inputPtr1->getExtent(1), inputPtr2->getExtent(1) );
  if ( inputPtr1->getDimension() == 3 )
    dimensionSize[2] = inputPtr1->getExtent(2);
  else
    dimensionSize[2] = 1;		
	if ( inputPtr2->getDimension() == 3 && inputPtr2->getExtent( 2 ) > dimensionSize[2] )
		dimensionSize[2] = inputPtr2->getExtent( 2 );
	ushort usOutputDataDimension = std::max( inputPtr1->getDataDimension(),
		inputPtr2->getDataDimension() );
		
  TImagePtr outputPtr ( new TImage( (dimensionSize[2] > 1)?3:2,
    dimensionSize, usOutputDataDimension ) );
  outputPtr->setMaximum( 0 );
  outputPtr->setMinimum( 32767 );
		
	PROG_MAX( usOutputDataDimension * dimensionSize[2] );
	for ( ushort usChannel = 0; usChannel < usOutputDataDimension; ++usChannel )
		for ( ushort z = 0; z < dimensionSize[2]; ++z )
		{
			PROG_VAL ( ( usChannel + 1 ) * z );
			APP_PROC();
			for ( ushort y = 0; y < dimensionSize[1]; ++y )
				for ( ushort x = 0; x < dimensionSize[0]; ++x )
				{
					short firstValue = 0;
					if ( inputPtr1->getDataDimension() > usChannel 
						&& x < inputPtr1->getExtent( 0 ) 
						&& y < inputPtr1->getExtent( 1 ) 
						&& (inputPtr1->getDimension()==3?z < inputPtr1->getExtent( 2 ):z==0) )
					{
						firstValue = (*inputPtr1)( x, y, z, usChannel );
					}
					if ( inputPtr2->getDataDimension() > usChannel 
						&& x < inputPtr2->getExtent( 0 ) 
						&& y < inputPtr2->getExtent( 1 ) 
						&& (inputPtr2->getDimension()==3?z < inputPtr2->getExtent( 2 ):z==0) )
					{
						firstValue = std::max( firstValue, (*inputPtr2)( x, y, z, usChannel ) );
					}
		    	(*outputPtr)( x, y, z, usChannel ) = firstValue;
			    if ( firstValue > outputPtr->getMaximum() )
  	    		outputPtr->setMaximum( firstValue );
				  if ( firstValue < outputPtr->getMinimum() )
		  	    outputPtr->setMinimum( firstValue );
				}
			}
  setOutput( outputPtr );
	PROG_RESET();	
BENCHSTOP;
}
   
CPipelineItem* CMaximum::newInstance( ulong ulID ) const throw()
{
  return new CMaximum( ulID );
}



