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
#include "cclosewm.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

/**
 * \param ulID unique module ID
 */
CCloseWM::CCloseWM( ulong ulID ) throw()
  : CFilter ( ulID, "WM boundary closing", 1, 1, "CCloseWM", "0.1", "CFilter" )
{
  setModuleID( sLibID );

  sDocumentation = "Extracts local maxima from the given image\n"
                   "** Input ports:\n"
                   "0: A scalar 2D or 3D multichannel data set\n"
                   "**Output ports:\n"
                   "0: A scalar 2D or 3D multichannel data set\n"
                   "**Parameters:\n"
                   "Radius: radius of filter kernel";

	parameters.initUnsignedLong("Intensity",4,0,4096);
	inputsVec[0].portType = IO2DInteger;
}

CCloseWM::~CCloseWM() throw()
{
}

/*****************
 * Other methods *
 *****************/
 
void CCloseWM::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
	
  if ( !checkInput( inputPtr, 2, 2 ) )
    return;
	
	bModuleReady = true;
  deleteOldOutput();

  size_t dimensionSize[2];
  dimensionSize[0] = inputPtr->getExtent(0);
  dimensionSize[1] = inputPtr->getExtent(1);
  
  TImagePtr outputPtr ( new TImage( 2, inputPtr->getExtents() ) );
	
  (*outputPtr) = (*inputPtr);

	ushort target = parameters.getUnsignedLong("Intensity");
	ushort xm = dimensionSize[0]/2;
  for ( ushort y = dimensionSize[1]-1; y > 0; --y )
	{
		ushort xmin = xm;
		while( xmin > 0 )
		{
			if ( (*outputPtr)(xmin,y) == target )
				break;
			xmin--;
		}
		ushort xmax = xm;
		while( xmax < dimensionSize[0] )
		{
			if ( (*outputPtr)(xmax,y) == target )
				break;
			xmax++;
		}
		if ( xmin != 0 && xmax != dimensionSize[0] )
		{
			alog << LINFO << "Creating boundary at (" << xmin << ","<<y<<") - (" << xmax << "," << y << ")" << endl;
			for( ushort z = xmin+1; z < xmax; ++z )
				(*outputPtr)(z,y) = target;
			break;
		}
	}
	for ( ushort y = 0; y < dimensionSize[1]-1; ++y )
	{
		ushort xmin = xm;
		while( xmin > 0 )
		{
			if ( (*outputPtr)(xmin,y) == target )
				break;
			xmin--;
		}
		ushort xmax = xm;
		while( xmax < dimensionSize[0] )
		{
			if ( (*outputPtr)(xmax,y) == target )
				break;
			xmax++;
		}
		if ( xmin != 0 && xmax != dimensionSize[0] )
		{
			alog << LINFO << "Creating boundary at (" << xmin << ","<<y<<") - (" << xmax << "," << y << ")" << endl;
			for( ushort z = xmin+1; z < xmax; ++z )
				(*outputPtr)(z,y) = target;
			break;
		}
	}	 
PROG_RESET();
  setOutput( outputPtr );
BENCHSTOP;
}

CPipelineItem* CCloseWM::newInstance( ulong ulID ) const throw()
{
  return new CCloseWM( ulID );
}



