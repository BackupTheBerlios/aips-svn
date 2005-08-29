
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
#include "cextractmask.h"
#include <list>

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

struct Point
{
	int x,y;
};
 
/**
 * \param ulID unique module ID
 */
CExtractMask::CExtractMask( ulong ulID ) throw()
  : CFilter ( ulID, "Mask extraction", 1, 1, "CExtractMask", "0.1", "CFilter" )
{
  setModuleID( sLibID );

  sDocumentation = "Extracts local maxima from the given image\n"
                   "** Input ports:\n"
                   "0: A scalar 2D or 3D multichannel data set\n"
                   "**Output ports:\n"
                   "0: A scalar 2D or 3D multichannel data set\n"
                   "**Parameters:\n"
                   "Radius: radius of filter kernel"
									 "Borderwidth: width of grey matter border which should be eroded";
	parameters.initUnsignedLong( "Radius", 2, 1, 1000 );
	parameters.initUnsignedLong( "Borderwidth", 2, 0, 1000 );
	inputsVec[0].portType = IO2DInteger;
}

CExtractMask::~CExtractMask() throw()
{
}

/*****************
 * Other methods *
 *****************/
 
void CExtractMask::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
	
  if ( !checkInput<TImage>( inputPtr, 2, 2 ) )
    return;
	
	bModuleReady = true;
  deleteOldOutput();

  size_t dimensionSize[2];
  dimensionSize[0] = inputPtr->getExtent(0);
  dimensionSize[1] = inputPtr->getExtent(1);
  
  TImagePtr outputPtr ( new TImage( 2, inputPtr->getExtents() ) );
	TImagePtr tmpPtr ( new TImage( 2, inputPtr->getExtents() ) );
	
  
	(*tmpPtr) = (*inputPtr);

	ulong radius = parameters.getUnsignedLong("Radius");
	list<Point> pL;
	// Erode around all WM voxels
	for( ushort y = radius; y < dimensionSize[1]-radius; ++y )
		for( ushort x = radius; x < dimensionSize[0]-radius; ++x )
		{
			if ( (*inputPtr)(x,y) == 4 ) 
			{
				for( ushort yy = y - radius; yy < y + radius; ++yy )
					for( ushort xx = x - radius; xx < x + radius; ++xx )
						(*tmpPtr)(xx,yy) = 4;
			}
		}
	(*outputPtr) = (*tmpPtr);
	// Dilate around all GM voxels
	for( ushort y = radius; y < dimensionSize[1]-radius; ++y )
		for( ushort x = radius; x < dimensionSize[0]-radius; ++x )
		{
			if ( (*tmpPtr)(x,y) == 3 ) 
			{
				for( ushort yy = y - radius; yy < y + radius; ++yy )
					for( ushort xx = x - radius; xx < x + radius; ++xx )
						(*outputPtr)(xx,yy) = 3;
			}
		}		
	TImage::iterator outputIt = outputPtr->begin();
	TImage::iterator outputEnd = outputPtr->end();
	while( outputIt != outputEnd )
	{
		if (*outputIt < 3 )
			*outputIt = 0;
		++outputIt;
	}
	// Mark possible holes
	ushort target = 4;
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
			//alog << LINFO << "Creating boundary at (" << xmin << ","<<y<<") - (" << xmax << "," << y << ")" << endl;
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
			//alog << LINFO << "Creating boundary at (" << xmin << ","<<y<<") - (" << xmax << "," << y << ")" << endl;
			for( ushort z = xmin+1; z < xmax; ++z )
				(*outputPtr)(z,y) = target;
			break;
		}
	}
	// Fill outer rim
	queue<TPoint2D> pointQueue;
	TPoint2D p(0,0);	
	pointQueue.push( p );
	//long i = 0;
	while( !pointQueue.empty() )//&& i < 10000 )
	{
		TPoint2D q = pointQueue.front(); pointQueue.pop();
		if ( (*outputPtr)(q[0],q[1]) != 5 )
		{
			(*outputPtr)(q[0],q[1]) = 5;
			TPoint2D r (q);
			if ( q[0] < static_cast<long>(dimensionSize[0]-1) && (*outputPtr)(q[0]+1,q[1]) < 4 )
				{ r[0] = q[0]+1; r[1] = q[1]; pointQueue.push( r ); }
			if ( q[0] > 0 && (*outputPtr)(q[0]-1,q[1]) < 4 ) 
				{ r[0] = q[0]-1; r[1] = q[1]; pointQueue.push( r ); }
			if ( q[1] < static_cast<long>(dimensionSize[1]-1) && (*outputPtr)(q[0],q[1]+1) < 4 ) 
				{ r[0] = q[0]; r[1] = q[1]+1; pointQueue.push( r ); }
			if ( q[1] > 0 && (*outputPtr)(q[0],q[1]-1) < 4 ) 
				{ r[0] = q[0]; r[1] = q[1]-1; pointQueue.push( r ); }			
			//++i;
		}
	}
	
	
	// Delete outlying GM voxels
	long border = parameters.getUnsignedLong("Borderwidth");
//	const long maxborder = border;
	while ( border > 0 )
	{
		for( ushort y = radius; y < dimensionSize[1]-radius; ++y )
			for( ushort x = radius; x < dimensionSize[0]-radius; ++x )
			{
				if ( (*inputPtr)(x,y) == 3 && 
					( (*outputPtr)(x+1,y) == 5 || (*outputPtr)(x-1,y) == 5 
					|| (*outputPtr)(x,y+1) == 5 || (*outputPtr)(x,y-1) == 5 
					) )
					(*outputPtr)(x,y) = border+5;
			}
		--border;
		outputIt = outputPtr->begin();
		outputEnd = outputPtr->end();
		while( outputIt != outputEnd )
		{
			if (*outputIt == border+6 )
				*outputIt = 2;
			++outputIt;
		}
	}
	outputIt = outputPtr->begin();
	TImage::iterator inputIt = inputPtr->begin();
	outputEnd = outputPtr->end();
	while( outputIt != outputEnd )
	{
		if (*outputIt == 3 || *outputIt == 4 )
			*outputIt = 1;
		else
			*outputIt = 0;
		if ( *inputIt == 2 )
			*outputIt = 0;
		++inputIt;
		++outputIt;
	}
	outputPtr->setMinimum( 0 );
	outputPtr->setMaximum( 1 );
PROG_RESET();
  setOutput( outputPtr );
BENCHSTOP;
}

CPipelineItem* CExtractMask::newInstance( ulong ulID ) const throw()
{
  return new CExtractMask( ulID );
}
