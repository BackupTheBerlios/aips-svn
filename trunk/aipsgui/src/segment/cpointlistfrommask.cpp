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
#include "cpointlistfrommask.h"


using namespace std;
using namespace boost;

CPointListFromMask::CPointListFromMask( ulong ulID ) throw()
  : CFilter( ulID, "Mask to pointlist", 1, 1, "CPointListFromMask", "0.1", "CFilter" )
{
  sModuleID = getClassName() + "/" + getClassVersion() + "/" + sLibID;
  
  parameters.initUnsignedLong( "ZSlice", 0, 0, 65535 );

  inputsVec[0].portType = IO2DInteger;
  outputsVec[0].portType = IO1DVector;
}

CPointListFromMask::~CPointListFromMask() throw()
{
}

CPipelineItem* CPointListFromMask::newInstance( ulong ulID ) const throw()
{
  return new CPointListFromMask( ulID );
}

void CPointListFromMask::apply() throw()
{
FBEGIN;
	bModuleReady = false;
	if ( getInput()->getType() != typeid( dataTraits<TImage>::dataType ) )
		return;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
  if ( !checkInput<TImage>(inputPtr, 2, 2, 1, 1 ) )
	{
    return;
	}
	bModuleReady = true;
	for( uint y = 0; y < inputPtr->getExtent(1); ++y )
		for( uint x = 0; x < inputPtr->getExtent(0); ++x )
			if ( (*inputPtr)(x,y) > 0 )
				work.push_back( TPoint2D( x, y ) );

	size_t dims[1];
	dims[0] = work.size();
	ulong zSlice = parameters.getUnsignedLong("ZSlice");
	if ( zSlice )
	{
  	TField3DPtr outputPtr ( new TField3D( 1, dims ) );
	  vector<TPoint2D>::iterator vit = work.begin();
  	for ( TField3D::iterator it = outputPtr->begin(); it != outputPtr->end(); ++it, ++vit )
		{
			(*it)[0] = (*vit)[0];
			(*it)[1] = (*vit)[1];
			(*it)[2] = zSlice;
			cerr << (*it)[0] << " " << (*it)[1] << " " << (*it)[2] << endl;
		}
	  setOutput( outputPtr );
	}  
	else
	{
  	TField2DPtr outputPtr ( new TField2D( 1, dims ) );
	  vector<TPoint2D>::iterator vit = work.begin();
  	for ( TField2D::iterator it = outputPtr->begin(); it != outputPtr->end(); ++it, ++vit )
		{
			(*it)[0] = (*vit)[0];
			(*it)[1] = (*vit)[1];
		}
		cerr << "Setting 2D data" << endl;
	  setOutput( outputPtr );	  
	}  
  work.clear();
FEND;
}





