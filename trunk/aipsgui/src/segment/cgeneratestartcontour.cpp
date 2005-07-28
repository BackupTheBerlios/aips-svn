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
#include "cgeneratestartcontour.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/
 
/**
 * \param ulID unique module ID
 */
CGenerateStartContour::CGenerateStartContour( ulong ulID ) throw()
  : CFilter ( ulID, "Start contour", 4, 2, "CGenerateStartContour", "0.1", "CFilter" )
{
  setModuleID( sLibID );

  sDocumentation = "None\n"
                   "** Input ports:\n"
                   "0: Mask image\n"
                   "1: Classified image\n"
                   "2: Refined output of the foerstner operator\n"
                   "3: Position of center line\n"
                   "**Output ports:\n"
                   "0: A scalar 2D or 3D multichannel data set\n"
									 "1: A polygon\n"
                   "**Parameters:\n"
                   "Extractmesh: Mesh to produce (0-5)";
	parameters.initUnsignedLong( "Extractmesh", 0, 0, 5 );
	inputsVec[0].portType = IO2DInteger;
	inputsVec[1].portType = IO2DInteger;
	inputsVec[2].portType = IO2DInteger;
	inputsVec[3].portType = IOOther;
	outputsVec[1].portType = IO1DVector;
}

CGenerateStartContour::~CGenerateStartContour() throw()
{
}

/*****************
 * Other methods *
 *****************/
 
void CGenerateStartContour::apply() throw()
{
BENCHSTART;
	// Test inputs
	bModuleReady = false;	
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
	TImagePtr classPtr = static_pointer_cast<TImage>( getInput(1) );
	TImagePtr foerstPtr = static_pointer_cast<TImage>( getInput(2) );
	shared_ptr<TInteger> cLine = static_pointer_cast<TInteger>( getInput(3) );
  if ( !cLine || !checkInput( inputPtr, 2, 2 ) || !checkInput( classPtr, 2, 2 ) || !checkInput( foerstPtr, 2, 2 ))
    return;
	
	bModuleReady = true;
  deleteOldOutput();

  ulong meshtype = 0; //parameters.getUnsignedLong( "Extractmesh" );
  
	// Arrays that store our start contour points
	TPoint2D ncl[4];
// 	TPoint2D ncr[4];
// 	TPoint2D thl[4];
// 	TPoint2D thr[4];
// 	TPoint2D ptl[4];
// 	TPoint2D ptr[4];

	ushort centerLine = (*cLine)();
	
  size_t dimensionSize[2];
  dimensionSize[0] = inputPtr->getExtent(0);
  dimensionSize[1] = inputPtr->getExtent(1);
  
  TImagePtr outputPtr ( new TImage( 2, inputPtr->getExtents() ) );
	cerr << "Computing probable nucleus caudatus anchor points" << endl;
	for( ushort y = ( dimensionSize[1] - 2 ); y > ( dimensionSize[1] - 1 ) / 2; --y )
		for ( ushort x = 3; x < ( dimensionSize[0] - 3 ); ++x )
		{
/*			if ( x == centerLine - 3 )
				x = centerLine + 4;*/
			if ( (*foerstPtr)(x,y) > 0 )
			{
				bool water = false;
				for ( int yy = y + 2; yy > y - 3; --yy )
					for ( int xx = x + 2; xx > x - 3; --xx )
						if ( (*classPtr)(xx,yy) == 2 )
							water = true;
				if (water)
					(*outputPtr)(x,y) = 1;
			}
		}
		
/*outputPtr->setMaximum( 2 );
outputPtr->setMinimum( 0 );
setOutput( outputPtr );
return;*/

// Determine starting points on ventricles
	TPoint2D sl ( dimensionSize[0], dimensionSize[1] );
	TPoint2D sr ( 0, dimensionSize[1] );
	for( ushort y = ( dimensionSize[1] - 2 ); y > ( dimensionSize[1] - 1 ) / 2; --y )
		for ( ushort x = 3; x < ( dimensionSize[0] - 3 ); ++x )
		{
			if ( (*outputPtr)(x,y) == 1 )
			{
				if ( x <= sl[0] && y < sl[1] ) { sl[0] = x; sl[1] = y;} 
				if ( x >= sr[0] && y < sr[1] ) { sr[0] = x; sr[1] = y;} 
			} 
		}
// Delete all points above the determined starting points	
	for( ushort y = ( dimensionSize[1] - 2 ); y > sr[1]; --y )
		for ( ushort x = 0; x <= dimensionSize[0]/2; ++x )
			(*outputPtr)(x,y) = 0;
	for( ushort y = ( dimensionSize[1] - 2 ); y > sl[1]; --y )
		for ( ushort x = dimensionSize[0]/2; x < dimensionSize[0]; ++x )
			(*outputPtr)(x,y) = 0;
	cerr << "Detected starting points at " << sl[0] << ";" << sl[1] << " and " << sr[0] << ";" << sr[1] << endl;
// Delete all points that lie "inside" the ventricles when looking from lateral
	for( ushort y = sl[1]; y > ( dimensionSize[1] - 1 ) / 2; --y )
	{
		bool water = false;
		bool mask = false;
		for ( ushort x = 0; x <= dimensionSize[0]/2; ++x )
		{
			if ( !mask && (*inputPtr)(x,y) == 2 )
				mask = true;
			if ( water ) (*outputPtr)(x,y) = 0;
			if ( mask && !water && (*classPtr)(x,y) == 2 &&  (*classPtr)(x-1,y) == 2 && (*classPtr)(x-2,y) == 2 ) // Liquor found
			{
				cerr << "Liquor on " << x << " " << y << endl;
				water = true;
			}
			
		}
	}
	for( ushort y = sr[1]; y > ( dimensionSize[1] - 1 ) / 2; --y )
	{
		bool water = false;
		bool mask = false;
		for ( ushort x = dimensionSize[0]; x > dimensionSize[0]/2; --x )
		{
			if ( !mask && (*inputPtr)(x,y) == 2 )
				mask = true;
			if ( water ) (*outputPtr)(x,y) = 0;
			if ( mask && !water && (*classPtr)(x,y) == 2 &&  (*classPtr)(x+1,y) == 2 && (*classPtr)(x+2,y) == 2 ) // Liquor found
			{
				cerr << "Liquor on " << x << " " << y << endl;
				water = true;
			}
		}
	}

(*outputPtr)(sl[0],sl[1]) = 2;
(*outputPtr)(sr[0],sr[1]) = 2;
outputPtr->setMaximum( 2 );
outputPtr->setMinimum( 0 );
setOutput( outputPtr );
return;
	
	vector<TPoint2D> pv;
	(*outputPtr) = 0;
	ushort usLeftBorder = sl[0], usRightBorder = sr[0]; 
	ushort usStartY = max(sl[0],sl[1]);
/*	ushort usLeftBorder = (dimensionSize[0] - 1), usRightBorder = 0; 
	ushort usStartY = 0;*/
/*cerr << "Looking for local min" << endl;
	// Find minimum Y - Here should be some white matter of at least 60 pixels
	for( ushort y = ( dimensionSize[1] - 2 ); y > ( dimensionSize[1] - 1 ) / 2; --y )
	{
		ushort usCount = 0;
		for( ushort x = 1; x < ( dimensionSize[0] - 1 ); ++x )
		{
			if ( (*inputPtr)( x, y ) == 2 && (*classPtr)( x, y+1 ) == 2 )
				usStartY = y; 
			break; 
		}
		if ( usStartY != 0 ) break;
	}
cerr << "Min Y is " << usStartY << endl;
	// Find minimal and maximal x values 
	for( ushort y = usStartY; y > usStartY - 50; --y )
	{
		for( ushort x = 1; x < ( dimensionSize[0] - 1 ); ++x )
		{
			if ( (*inputPtr)( x, y ) == 2 && (*classPtr)( x, y ) == 3
				&& (*classPtr)( x - 1, y ) == 2 && x > usRightBorder )
				usRightBorder = x;
			if ( (*inputPtr)( x, y ) == 2 && (*classPtr)( x, y ) == 3
				&& (*classPtr)( x + 1 ,y ) == 2 && x < usLeftBorder )
				usLeftBorder = x;
		}
	}
cerr << " xmin " << usLeftBorder << " xmax " << usRightBorder << endl;*/
	ushort mitte = (sr[0]+sl[0])/2;
	// Mark all GM voxels left of a CSF region and in our mask
	for( ushort y = sl[1]; y > 0; --y )
		for( ushort x = usLeftBorder; x < mitte-2; ++x )
		{
			if ( (*inputPtr)( x, y ) == 2 && (*classPtr)( x, y ) == 3 && (*classPtr)( x + 1, y ) == 2 )
				(*outputPtr)( x, y ) = 1;
		}
	for( ushort y = sr[1]; y > 0; --y )
		for( ushort x = mitte+2; x < usRightBorder + 1; ++x )
		{
			if ( (*inputPtr)( x, y ) == 2 && (*classPtr)( x, y ) == 3 && (*classPtr)( x - 1, y ) == 2 )
				(*outputPtr)( x, y ) = 1;
		}		
/*	for( ushort y = max(sl[1],sr[1]); y > 0; --y )
		for( ushort x = usLeftBorder; x < usRightBorder + 1; ++x )
		{
			if ( (*outputPtr)( x, y ) == 1 && countNeighbours( classPtr, x, y, 1, 3 ) < 5 )
				(*outputPtr)( x, y ) = 0;
		}*/
/*outputPtr->setMaximum( 2 );		
outputPtr->setMinimum( 0 );		
setOutput( outputPtr );
return;		*/
	// Finde Start, Ende und Mittelpunkt für NC
	uint state = 0;
	uint dy = 0;
	for( ushort y = usStartY; y >  3; --y )
	{
		for( ushort x = usLeftBorder; x < mitte; ++x )
		{
			if ( (*outputPtr)( x, y ) == 1 && state == 0 )
			{ 
				(*outputPtr)( x, y ) = 2; 
				state++; 
				dy = y; 
				ncl[0][0] = x; 
				ncl[0][1] = y;
			}
			if ( state == 1 && (*outputPtr)( x, y ) == 1 && ( (*classPtr)( x, y - 3 ) == 4
			 || (*classPtr)( x, y - 2 ) == 4 || (*classPtr)( x, y - 1 ) == 4 )  )
			{
				(*outputPtr)( x, y ) = 2; 
				state++; 
				dy = dy - ( dy - y ) / 2;
				ncl[2][0] = x; 
				ncl[2][1] = y;
			}
		}
	}
cerr << "NCL0 " << ncl[0] << endl;
cerr << "NCL2 " << ncl[2] << endl;
cerr << "dy " << dy << endl;
/*	bool bFinished = false;
	int i = 0;
	while( !bFinished )
	{
		for( ushort x = usLeftBorder; x < mitte; ++x )
		if ( (*outputPtr)( x, dy + i ) == 1 ) 
		{
			(*outputPtr)( x, dy + i ) = 2; 
			ncl[3][0] = x; 
			ncl[3][1] = dy + i; 
			bFinished = true;
		}
		if ( !bFinished )
		{
			if ( i == 0 ) 
				i = 1;
			else if ( i > 0 ) 
				i *= -1;
			else 
			{
				i *= -1; 
				i++; 
			};
		}
	}
	TVector2D a ( ncl[0][0], ncl[0][1] );
	TVector2D b ( ncl[2][0], ncl[2][1] );
	TVector2D c ( ncl[3][0], ncl[3][1] );
	TVector2D g = b - a;
	double lambda = dot( g, ( c - a ) ) / ( dot( g, g ) );
	TVector2D res = a + lambda * g;
	res = 1.5 * ( res - c ) + c;
	ncl[1][0] = static_cast<ushort>( res[0] );
	ncl[1][1] = static_cast<ushort>( res[1] );
cerr << ncl[1] << endl;
cerr << ncl[3] << endl;
	(*outputPtr)( ncl[1][0], ncl[1][1] ) = 2;

	state = 0;
	dy = 0;
	for( ushort y = usStartY; y > 3; --y )
	{
		for( ushort x = mitte; x <= usRightBorder; ++x )
		{
			if ( (*outputPtr)( x, y ) == 1 && state == 0 )
			{ 
				(*outputPtr)( x, y ) = 2; 
				state++; 
				dy = y; 
				ncr[0][0] = x; 
				ncr[0][1] = y; 
			}
			if ( state == 1 && (*outputPtr)( x, y ) == 1 && ( (*classPtr)( x, y - 3 ) == 4
			 || (*classPtr)( x, y - 2 ) == 4 || (*classPtr)( x, y - 1 ) == 4 )  )
			{
				(*outputPtr)( x, y ) = 2; 
				dy = dy - ( dy - y ) / 2;
				ncr[2][0] = x; 
				ncr[2][1] = y;
				state = 2; 
				y = dimensionSize[1] - 1;				
			}
		}
	}	
	bFinished = false;
	i = 0;
	while( !bFinished )
	{
		for( ushort x = mitte; x <= usRightBorder; ++x )
		{
			if ( (*outputPtr)( x, dy + i ) == 1 ) 
			{ 
				(*outputPtr)( x, dy + i ) = 2; 
				bFinished = true; 
				ncr[1][0] = x; 
				ncr[1][1] = dy + i;
			}
		}
		if ( !bFinished )
		{
			if ( i == 0 ) 
				i = 1;
			else if ( i > 0 ) 
				i *= -1;
			else 
			{
				i *= -1; 
				i++; 
			}
		}
	}
	a[0] = ncr[0][0]; a[1] = ncr[0][1];
	b[0] = ncr[2][0]; b[1] = ncr[2][1];
	c[0] = ncr[1][0]; c[1] = ncr[1][1];
	g = b - a;
	lambda = dot( g, ( c - a ) ) / ( dot( g, g ) );
	res = a + lambda * g;
	res = 1.5 * ( res - c ) + c;
	ncr[3][0] = static_cast<ushort>( res[0] );
	ncr[3][1] = static_cast<ushort>( res[1] );
cerr << ncr[1] << endl;
cerr << ncr[3] << endl;
	(*outputPtr)( ncr[3][0], ncr[3][1] ) = 2;
	
	// Find Thalamus start points
	ushort thstarty = ncl[2][1] - 5;
cerr << "Looking for TH at " << thstarty <<  " " << usLeftBorder << endl;
	for( ushort y = thstarty; y > 0; --y )
	{
cerr << y << endl;
		for( ushort x = usLeftBorder; x < ( dimensionSize[0] - 1 ); ++x )
		{
			if( (*outputPtr)( x, y ) == 1 )
			{
				(*outputPtr)( x, y ) = 2; 
				thl[0][0] = x; 
				thl[0][1] = y; 
				y = 1; 
				break;
			}
		}
	}
cerr << "TH0 " << thl[0] << endl;
	thl[2][0] = dimensionSize[0] - 1;
	thl[2][1] = thl[0][1] - 1;
	for( ushort y = thl[0][1] - 15; y > thl[0][1] - 40; --y )
	{
		for( ushort x = usLeftBorder; x < dimensionSize[0] - 1; ++x )
		{			
			if( (*outputPtr)( x, y ) == 1 )
			{
				if ( x <= thl[2][0] - 1 && abs( thl[2][1] - y ) <= 30 )
				{
cerr << "At " << x << " " << y << endl;		
					(*outputPtr)( thl[2][0], thl[2][1] ) = 1;
					(*outputPtr)( x, y ) = 2; 
					thl[2][0] = x; 
					thl[2][1] = y;				
				}
				else 
					break;
			}
		}
	}
	dy = thl[0][1] + ( thl[2][1] - thl[0][1] ) / 2;
cerr << "thdy " << dy << endl;
	bFinished = false;
	i = 0;
	while( !bFinished )
	{
		for( ushort x = usLeftBorder; x < ( dimensionSize[0] - 1 ) / 2; ++x )
		{
			if ( (*outputPtr)( x, dy + i ) == 1 ) 
			{ 
				(*outputPtr)( x, dy + i ) = 2; 
				bFinished = true; 
				thl[3][0] = x; 
				thl[3][1] = dy + i;
			}
		}
		if ( !bFinished )
		{
			if ( i == 0 ) 
				i = 1;
			else if ( i > 0 ) 
				i *= -1;
			else 
			{
				i *= -1; 
				i++; 
			}
		}
	}
	a[0] = thl[0][0]; a[1] = thl[0][1];
	b[0] = thl[2][0]; b[1] = thl[2][1];
	c[0] = thl[3][0]; c[1] = thl[3][1];
	g = b - a;
	lambda = dot( g, ( c - a ) ) / ( dot( g, g ) );
	res = a + lambda * g;
	res = 2.5 * ( res - c ) + c;
	thl[1][0] = static_cast<ushort>( res[0] );
	thl[1][1] = static_cast<ushort>( res[1] );
	(*outputPtr)( thl[1][0], thl[1][1] ) = 2;

cerr << "THL is" << endl;
cerr << thl[0] << endl;
cerr << thl[1] << endl;
cerr << thl[2] << endl;
cerr << thl[3] << endl;
		
	// Find right hemisphere thalamus
cerr << "Looking for thr" << endl;
	thstarty = ncr[2][1] - 5;
	thr[0][0] = 0;
	for( ushort y = thstarty; y > 0; --y )
	{
		for( ushort x = usRightBorder; x >= ( dimensionSize[0] - 1 ) / 2; --x )
		{
			if( (*outputPtr)( x, y ) == 1 )
			{
				(*outputPtr)( x, y ) = 2; 
cerr << "Y" << y << endl;
				thr[0][0] = x; 
				thr[0][1] = y; 
				y = 1; 
				break;
			}
		}
	}

	if ( thr[0][0] == 0 ) 
		return;
	thr[2][0] = 1;
	thr[2][1] = thr[0][1] - 1;
	for( ushort y = thr[0][1]-15; y > thr[0][1] - 40; --y )
	{
		for( ushort x = usRightBorder; x > 0; --x )
		{			
			if( (*outputPtr)( x, y ) == 1 )
			{
				if ( x >= thr[2][0] - 1 )
				{
cerr << "At " << x << " " << y << endl;		
					(*outputPtr)( thr[2][0], thr[2][1] ) = 1;
					(*outputPtr)( x, y ) = 2; 
					thr[2][0] = x; 
					thr[2][1] = y;				
				}
				else if ( x < thr[2][0] - 1 && x >= ( dimensionSize[0] - 1 ) / 2 )
				{
					y = 1; 
					break;
				}
				else 
					break;
			}
		}
	}
cerr << "Here " << thr[0][1] << " " << thr[2][1] << endl;
	
	
	dy = thr[0][1] + ( thr[2][1] - thr[0][1] ) / 2;
	bFinished = false;
	i = 0;
	while( !bFinished )
	{
cerr << dy << endl;
		for( ushort x = usRightBorder; x >= ( dimensionSize[0] - 1 ) / 2; --x )
		{
			if ( (*outputPtr)( x, dy + i ) == 1 ) 
			{ 
				(*outputPtr)( x, dy + i ) = 2; 
				bFinished = true; 
				thr[1][0] = x; 
				thr[1][1] = dy + i;
			}
		}
		if ( !bFinished )
		{
			if ( i == 0 ) 
				i = 1;
			else if ( i > 0 ) 
				i *= -1;
			else 
			{
				i *= -1; 
				i++; 
			}
		}
	}
	a[0] = thr[0][0]; a[1] = thr[0][1];
	b[0] = thr[2][0]; b[1] = thr[2][1];
	c[0] = thr[1][0]; c[1] = thr[1][1];
	g = b - a;
	lambda = dot( g, ( c - a ) ) / ( dot( g, g ) );
	res = a + lambda * g;
	res = 2.5 * ( res - c ) + c;
	thr[3][0] = static_cast<ushort>( res[0] );
	thr[3][1] = static_cast<ushort>( res[1] );
	(*outputPtr)( thr[3][0], thr[3][1] ) = 2;

	// Find left Putamen start points
	bFinished = false;
  for( ushort x = usLeftBorder; x >= 10; x-- )
	{
		for( ushort y = ncl[2][1] - 2; y <= ncl[2][1] + 2; y++ )
		{
			if ( (*classPtr)( x, y ) == 3 )
			{
				(*outputPtr)( x, y ) = 2; 
				(*outputPtr)( x - 5, y + 10 ) = 2; 
				(*outputPtr)( x - 5, y - 20 ) = 2; 
				(*outputPtr)( x - 10, y ) = 2; 
				ptl[0][0] = x; 
				ptl[0][1] = y; 
				bFinished = true; 
				break;
			}
		}
		if ( bFinished ) 
			break;
	}
	ptl[1][0] = ptl[0][0] - 5;
	ptl[1][1] = ptl[0][1] + 10;
	ptl[2][0] = ptl[0][0] - 10;
	ptl[2][1] = ptl[0][1];
	ptl[3][0] = ptl[0][0] - 5;
	ptl[3][1] = ptl[0][1] - 20;
	// Find right Putamen start points
	bFinished = false;
  for( ushort x = usRightBorder; x <= dimensionSize[0] - 10; x++ )
	{
		for( ushort y = ncr[2][1] - 2; y <= ncr[2][1] + 2; y++ )
		{
			if ( (*classPtr)( x, y ) == 3 )
			{
				(*outputPtr)( x, y ) = 2; 
				(*outputPtr)( x + 5, y - 20 ) = 2; 
				(*outputPtr)( x + 5, y + 10 ) = 2; 
				(*outputPtr)( x + 10, y ) = 2; 
				ptr[0][0]=x; 
				ptr[0][1]=y; 
				bFinished=true; 
				break;
			}
		}
		if ( bFinished ) 
			break;
	}
	ptr[1][0] = ptr[0][0] + 5;
	ptr[1][1] = ptr[0][1] + 10;
	ptr[2][0] = ptr[0][0] + 10;
	ptr[2][1] = ptr[0][1];
	ptr[3][0] = ptr[0][0] + 5;
	ptr[3][1] = ptr[0][1] - 20;
	outputPtr->setMinimum( 0 );
	outputPtr->setMaximum( 10 );
  setOutput( outputPtr );*/
	
	// Now return the mesh the user selected
// 	ulong meshtype = parameters.getUnsignedLong( "Extractmesh" );
// 	size_t dims[1] = { 4 };
// 	TField2DPtr outputField ( new TField2D( 1, dims ) );
// 	TVector2D points[4];
// 	switch( meshtype )
// 	{
// 		case 0:
// 			points[0][0] = ncl[0][0]; points[0][1] = ncl[0][1];
// 			points[1][0] = ncl[1][0]; points[1][1] = ncl[1][1];
// 			points[2][0] = ncl[2][0]; points[2][1] = ncl[2][1];
// 			points[3][0] = ncl[3][0]; points[3][1] = ncl[3][1];
// 			break;
// 		case 1:
// 			points[0][0] = ncr[0][0]; points[0][1] = ncr[0][1];
// 			points[1][0] = ncr[1][0]; points[1][1] = ncr[1][1];
// 			points[2][0] = ncr[2][0]; points[2][1] = ncr[2][1];
// 			points[3][0] = ncr[3][0]; points[3][1] = ncr[3][1];
// 			break;
// 		case 2:
// 			points[0][0] = thl[0][0]; points[0][1] = thl[0][1];
// 			points[1][0] = thl[1][0]; points[1][1] = thl[1][1];
// 			points[2][0] = thl[2][0]; points[2][1] = thl[2][1];
// 			points[3][0] = thl[3][0]; points[3][1] = thl[3][1];
// 			break;
// 		case 3:
// 			points[0][0] = thr[0][0]; points[0][1] = thr[0][1];
// 			points[1][0] = thr[1][0]; points[1][1] = thr[1][1];
// 			points[2][0] = thr[2][0]; points[2][1] = thr[2][1];
// 			points[3][0] = thr[3][0]; points[3][1] = thr[3][1];
// 			break;
// 		case 4:
// 			points[0][0] = ptl[0][0]; points[0][1] = ptl[0][1];
// 			points[1][0] = ptl[1][0]; points[1][1] = ptl[1][1];
// 			points[2][0] = ptl[2][0]; points[2][1] = ptl[2][1];
// 			points[3][0] = ptl[3][0]; points[3][1] = ptl[3][1];
// 			break;
// 		case 5:
// 			points[0][0] = ptr[0][0]; points[0][1] = ptr[0][1];
// 			points[1][0] = ptr[3][0]; points[1][1] = ptr[3][1];
// 			points[2][0] = ptr[2][0]; points[2][1] = ptr[2][1];
// 			points[3][0] = ptr[1][0]; points[3][1] = ptr[1][1];
// 			break;
// 	}
// 	(*outputField)( 0 ) = points[3];
// 	(*outputField)( 1 ) = points[2];
// 	(*outputField)( 2 ) = points[1];
// 	(*outputField)( 3 ) = points[0];
	
//	setOutput( outputField, 1 );
BENCHSTOP;
}

CPipelineItem* CGenerateStartContour::newInstance( ulong ulID ) const throw()
{
  return new CGenerateStartContour( ulID );
}

bool CGenerateStartContour::isInNeighbourhood( TImagePtr imagePtr, ushort usX, ushort usY, ushort usIntensity )
{
	for( ushort y = usY - 1; y < usY + 2; ++y )
		for( ushort x = usX - 1; x < usX + 2; ++x )
		{			
			if ( ( x != usX || y != usY ) && (*imagePtr)( x, y ) == usIntensity ) 
				return true;
		}
	return false;			
}

ushort CGenerateStartContour::countNeighbours( TImagePtr imagePtr, ushort usX, ushort usY, 
	ushort usRadius, ushort usIntensity )
{
	ushort usCnt = 0;
	for( ushort y = usY - usRadius; y < usY + usRadius + 1; ++y )
		for( ushort x = usX - 1; x < usX + usRadius + 1; ++x )
		{			
			if ( (x != usX || y != usY ) && (*imagePtr)( x, y ) == usIntensity ) 
				usCnt++;			
		}
	return usCnt;			
}
