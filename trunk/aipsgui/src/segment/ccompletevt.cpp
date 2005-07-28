
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
#include "ccompletevt.h"

using namespace std;
using namespace boost;

CCompleteVT::CCompleteVT( ulong ulID ) throw()
  : CFilter( ulID, "Complete VT", 1, 1, "CCompleteVT", "0.1", "CFilter" )
{
  sModuleID = getClassName() + "/" + getClassVersion() + "/" + sLibID;
  
  inputsVec[0].portType = IOInteger;
  outputsVec[0].portType = IOInteger;
}

CCompleteVT::~CCompleteVT() throw()
{
}

CPipelineItem* CCompleteVT::newInstance( ulong ulID ) const throw()
{
  return new CCompleteVT( ulID );
}

void CCompleteVT::apply() throw()
{
FBEGIN;
cerr << "Begin CCompleteVT" << endl;
	bModuleReady = false;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
  if ( !checkInput(inputPtr, 2, 2, 1, 1 ) )
	{
    return;
	}
	bModuleReady = true;
	TImagePtr outputPtr( new TImage( *inputPtr ) );
	*outputPtr = *inputPtr;
	// Determine minimum and maximum row of VT
	uint minrow = inputPtr->getExtent(1);
	uint maxrow = 0;
	for( uint y = 0; y < inputPtr->getExtent(1); ++y )
		for( uint x = 0; x < inputPtr->getExtent(0); ++x )
		{
			if ( (*inputPtr)(x,y) == 2 && minrow > y )
				minrow = y;
			if ( (*inputPtr)(x,y) == 2 && maxrow < y )
				maxrow = y;
		}
	// Mark all background inside brain within the row interval
	for( uint y = minrow; y <= maxrow; ++y )
		for( uint x = 0; x < inputPtr->getExtent(0); ++x )
		{
			if ( (*outputPtr)(x,y) == 0 )
				(*outputPtr)(x,y) = 2;
		}
	// and mark all bg pixels outside as unneccessary
	for( uint y = 0; y < inputPtr->getExtent(1); ++y )
		for( uint x = 0; x < inputPtr->getExtent(0); ++x )
		{
			if ( (*outputPtr)(x,y) > 2 )
				(*outputPtr)(x,y) = 0;
		}
	outputPtr->setMinimum( 0 );
  outputPtr->setMaximum( 2 );
  setOutput( outputPtr );
cerr << "End CCompleteVT" << endl;
FEND;
}


