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
#include "cimagescaler.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

CImageScaler::CImageScaler( ulong ulID ) throw()
   : CFilter ( ulID, "Scale image", 1, 1, "CImageScaler","0.1","CFilter" )
{
	parameters.initUnsignedLong( "ScaleX", 1, 1, 10 );
	parameters.initUnsignedLong( "ScaleY", 1, 1, 10 );
	parameters.initUnsignedLong( "ScaleZ", 1, 1, 10 );
  setModuleID( sLibID );
}

CImageScaler::~CImageScaler() throw()
{
}

/***************** 
 * Other methods *
 *****************/

void CImageScaler::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
  if ( !checkInput( inputPtr ) )
    return;
	
	bModuleReady = true;
  deleteOldOutput();

	size_t extents[3];
	size_t scale[3];
	scale[0] = parameters.getUnsignedLong( "ScaleX" );
	scale[1] = parameters.getUnsignedLong( "ScaleY" );
	scale[2] = parameters.getUnsignedLong( "ScaleZ" );
	extents[0] = inputPtr->getExtent( 0 ) * scale[0];
	extents[1] = inputPtr->getExtent( 1 ) * scale[1];
	if ( inputPtr->getDimension() == 3 ) 
		extents[2] = inputPtr->getExtent( 2 ) * scale[2];
	else
		extents[2] = 1;
	
  TImagePtr outputPtr ( new TImage( inputPtr->getDimension(), extents, 
		inputPtr->getDataDimension() ) );
	for( uint z = 0; z < extents[2]; ++z )
		for( uint y = 0; y < extents[1]; ++y )
			for( uint x = 0; x < extents[0]; ++x )
				(*outputPtr)(x,y,z) = (*inputPtr)(x / scale[0],y / scale[1],z / scale[2]);	
  outputPtr->setMaximum( inputPtr->getMaximum() );
  outputPtr->setMinimum( inputPtr->getMinimum() );    
  setOutput( outputPtr );
BENCHSTOP;
}

CPipelineItem* CImageScaler::newInstance( ulong ulID ) const throw()
{
  return new CImageScaler( ulID );
}



