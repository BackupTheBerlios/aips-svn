

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
#include "cdeletenarrows.h"

using namespace std;
using namespace boost;

CDeleteNarrows::CDeleteNarrows( ulong ulID ) throw()
  : CFilter( ulID, "Extract borders", 2, 1, "CDeleteNarrows", "0.1", "CFilter" )
{
  sModuleID = getClassName() + "/" + getClassVersion() + "/" + sLibID;
  
  inputsVec[0].portType = IOInteger;
  inputsVec[1].portType = IOOther;
  outputsVec[0].portType = IOInteger;
}

CDeleteNarrows::~CDeleteNarrows() throw()
{
}

CPipelineItem* CDeleteNarrows::newInstance( ulong ulID ) const throw()
{
  return new CDeleteNarrows( ulID );
}

void CDeleteNarrows::apply() throw()
{
FBEGIN;
cerr << "Begin CDeleteNarrows" << endl;
	bModuleReady = false;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
  if ( !checkInput<TImage>(inputPtr, 2, 2, 1, 1 ) )
	{
    return;
	}
	uint uiCenter;
	shared_ptr<TInteger> centerLine = static_pointer_cast<TInteger>( getInput(1) );
	if ( centerLine.get() == NULL )
		uiCenter = inputPtr->getExtent(0)/2;
	else
		uiCenter = (*centerLine)();
	bModuleReady=true;
	TImagePtr outputPtr( new TImage( inputPtr->getDimension(), inputPtr->getExtents() ) );
	(*outputPtr)=0;
	// Find all points that connect 2 regions
	uint lastmin = 0; uint lastmax = 0;
		for( uint y = 1; y < inputPtr->getExtent(1)-1; ++y )
		{
			uint xmax = 0;
			uint xmin = inputPtr->getExtent(0)-1;
			for( uint x = 1; x < inputPtr->getExtent(0)-1; ++x )
				if ( (*inputPtr)(x+1,y) == 2 )	
				{
					xmin = x; break;
				}
			if ( xmin != inputPtr->getExtent(0)-1 )
			{
				cerr << xmin << endl;
				for( uint x = inputPtr->getExtent(0)-1; x >= xmin; --x )
					if ( (*inputPtr)(x-1,y) == 2 )
					{
						xmax = x; break;
					}
			bool maybe = false;
			bool maybe2 = false;
			if ( xmax < (uiCenter+1) )
			{
				xmax = uiCenter+1; maybe = true;
			}
			if ( xmin > (uiCenter-1) )
			{
				xmin = uiCenter-1; maybe2 = true;
				if ( maybe ) // We don't have any information here. This might be a VT sepreation
				{
					(*outputPtr)(uiCenter,y) = 3;
				}
			}
			if (!maybe) (*outputPtr)(xmax,y) = 1;
			if (!maybe2) (*outputPtr)(xmin,y) = 2;
			if ( abs(static_cast<long>(xmax)-static_cast<long>(lastmax)) > 3 )
			{
				cerr << "L " << y << " ( " << abs(static_cast<long>(xmax)-static_cast<long>(lastmax)) << " ) " << endl;
			}
			if ( abs(static_cast<long>(xmin)-static_cast<long>(lastmin)) > 3 )
			{
				cerr << "R " << y << " ( " << abs(static_cast<long>(xmin)-static_cast<long>(lastmin)) << " ) " << endl;
			}
			lastmin = xmin;
			lastmax = xmax;
			}
			else
			{
				xmin = 0;
				xmax = 0;
			}
		}
	// K-Means Clustering to find a certain ridge
	// Suche danach macht nur Sinn, wenn überhaupt eine Engstelle vorhanden ist
	
	outputPtr->setMinimum( 0 );
  outputPtr->setMaximum( 3 );
  setOutput( outputPtr );
cerr << "End CDeleteNarrows" << endl;
FEND;
}



