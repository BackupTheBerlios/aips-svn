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
#include "cfindcenterline.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/
 
/**
 * \param ulID unique module ID
 */
CFindCenterLine::CFindCenterLine( ulong ulID ) throw()
  : CFilter ( ulID, "Centerline", 1, 2, "CFindCenterLine", "0.1", "CFilter" )
{
  setModuleID( sLibID );

  sDocumentation = "None\n"
                   "** Input ports:\n"
                   "0: A scalar 2D or 3D multichannel data set\n"
                   "**Output ports:\n"
                   "0: A scalar 2D or 3D multichannel data set\n";
	inputsVec[0].portType = IO2DInteger;
	outputsVec[0].portType = IO2DInteger;
	outputsVec[1].portType = IOOther;
}

CFindCenterLine::~CFindCenterLine() throw()
{
}

/*****************
 * Other methods *
 *****************/
 
void CFindCenterLine::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
	
  if ( !checkInput<TImage>( inputPtr, 2, 2 ) )
    return;
	
	bModuleReady = true;
  deleteOldOutput();

	TImagePtr outputPtr( new TImage(*inputPtr) );
	(*outputPtr) = 0;
	//uint row1 = 0; uint row2 = 0; uint max = 0;

	// Determine height profile
	vector<uint> profile( inputPtr->getExtent(0) );
	for( uint x = 0; x < inputPtr->getExtent(0); ++x )
	{
		uint noPixs = 0;
		for( uint y = 0; y < inputPtr->getExtent(1); ++y )
			if ( (*inputPtr)(x,y) > 0 ) noPixs++;
		profile[x] = noPixs;
	}	
	// Smooth profile
	// Do a gaussian smoothing on the histogram
	// Compute gaussian mask
	double dSigma = 3.0;
	int iMaskSize = static_cast<uint>( floor( 6.0 * dSigma + 0.5 ) );
	vector<double> dGaussMask;
	for( int i = -( iMaskSize / 2 ); i <= ( iMaskSize / 2 ); ++i )
	{
		double dExponential = (1.0/sqrt(dSigma*dSigma*2.0*M_PI))*exp( static_cast<double>(-i*i) / ( 2.0 * ( dSigma * dSigma ) ) );
		dGaussMask.push_back( dExponential );
	}

	// Shift histogram to attend smoothing
	vector<uint> shiftedHisto;
	for( int i = 0; i < iMaskSize/2; ++i )
		shiftedHisto.push_back(0);
	for( uint i = 1; i < profile.size(); ++i )
	{
		shiftedHisto.push_back( profile[i] );
	}
	for( int i = 0; i <= iMaskSize/2; ++i )
		shiftedHisto.push_back(0);
	vector<uint> smoothedHisto( profile.size() );
	for( int i = iMaskSize/2+1; i < static_cast<long>(shiftedHisto.size() - iMaskSize/2); ++i )
	{
		double smoothed = 0.0;
		for( int j = -iMaskSize/2; j <= iMaskSize/2; ++j )
		{
			smoothed += shiftedHisto[i+j]*dGaussMask[j+(iMaskSize/2)];
		}
		smoothedHisto[i-(iMaskSize/2+1)] = static_cast<uint>(smoothed);
	}

	// Determine number of maxima
	vector<uint> maxima;
	ofstream file( "/home/belitz/heightprofile.data" );
	for( uint i = 0; i < smoothedHisto.size(); ++i )
	{
		file << i << " " << smoothedHisto[i] << endl;
	}
	file.close();
	profile = smoothedHisto;
	uint maxbegin = 0;
	uint maxend = 0;
	uint state = 0;
	uint maximum = 0;
	for( uint i = 0; i < profile.size(); ++i )
		if ( profile[i] > maximum )
			maximum = profile[i];
	for( uint i = 0; i < profile.size(); ++i )
	{
		if ( profile[i] < profile[i+1] )
		{
			maxbegin = 0;
			state = 1;			
		}
		else if ( profile[i] == profile[i+1] )
		{
			if ( maxbegin == 0 )
				maxbegin = i;
		}
		else if ( profile[i] > profile[i+1] )
		{
			if ( profile[i] < profile[i-1] )
			{
				maxbegin = 0;
			}
			else
			{
			if ( maxbegin == 0 )
				maxbegin = i;
			maxend = i;
			if ( state == 1 ){
				if ( profile[(maxbegin + maxend)/ 2] > maximum/2 )
					maxima.push_back( (maxbegin + maxend)/ 2 );
				state = 0;
			}
			maxbegin = 0;
			}
		}
	}
	for ( uint i = 0; i < maxima.size(); ++i )
	{
		cerr << "Maximum i: " << maxima[i] << endl;
	}
	// If odd, take center maaxima, if even, take center between the two center maxima
	uint row = 0;
	if ( maxima.empty() )
	{
		row = inputPtr->getExtent(0)/2;
	}
	else
	{
	
	if ( maxima.size() % 2 == 0 )
		row = 2+(maxima[maxima.size()/2]+maxima[maxima.size()/2-1])/2;
	else
		row = 2+maxima[maxima.size()/2];
	}		
/*	for( uint x = 0; x < inputPtr->getExtent(0); ++x )
	{
		uint noPixs = 0;
		for( uint y = 0; y < inputPtr->getExtent(1); ++y )
			if ( (*inputPtr)(x,y) > 0 ) noPixs++;
		if ( noPixs > max )
		{
			row1 = x;
			max = noPixs;
		}
	}
	max = 0;
	for( uint x = inputPtr->getExtent(0)-1; x > row1; --x )
	{
		uint noPixs = 0;
		for( uint y = 0; y < inputPtr->getExtent(1); ++y )
			if ( (*inputPtr)(x,y) > 0 ) noPixs++;
		if ( noPixs > max )
		{
			row2 = x;
			max = noPixs;
		}
	}	
	if ( row2==0 ) row2=inputPtr->getExtent(0)-1;
	cerr << "Rows are " << row1 << " " << row2 << endl;
	uint row = (row2+row1)/2;
	cerr << "Centerline is " << row << endl;
	// Abstand gefundene Stelle vom Bildmittelpunkt
	short distance = abs( static_cast<short>(row) - static_cast<short>(inputPtr->getExtent(0)/2) );
	if ( distance > static_cast<short>(inputPtr->getExtent(0)/10) )
		distance = inputPtr->getExtent(0)/10;
	uint startlost = 0; uint stoplost = 0; uint state = 0;
	for( uint x = (inputPtr->getExtent(0)/2)-distance;
		x < (inputPtr->getExtent(0)/2)+distance; ++x )
	{
		uint noPixs = 0;
		for( uint y =
			(inputPtr->getExtent(1)/2)-(inputPtr->getExtent(1)/10); y < (inputPtr->getExtent(1)/2)+(inputPtr->getExtent(1)/10); ++y )
		{
			if ( (*inputPtr)(x,y) > 0 ) noPixs++;
		}
		if ( noPixs && state == 0 ) state = 1;
		else if ( noPixs == 0 && state == 1 )
		{
			startlost = x; state = 2;
		}
		else if ( noPixs && state == 2 )
		{
			stoplost = x; state = 3;
		}
	}
	cerr << "start is " << startlost << " and stop is " << stoplost << endl;
	if ( stoplost > 0 && startlost > 0 && stoplost - startlost >= 2 )
		row = ( stoplost - startlost ) / 2 + startlost;*/
	for( uint y = 0; y < inputPtr->getExtent(1); ++y )
		(*outputPtr)(row,y) = 1;
	outputPtr->setMinimum( 0 );
	outputPtr->setMaximum( 1 );
	shared_ptr<TInteger> centerLine( new TInteger( row ) );	
	setOutput( outputPtr );
	setOutput( centerLine, 1 );
BENCHSTOP;
}

CPipelineItem* CFindCenterLine::newInstance( ulong ulID ) const throw()
{
  return new CFindCenterLine( ulID );
}

