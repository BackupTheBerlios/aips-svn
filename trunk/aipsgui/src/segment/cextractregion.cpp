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
#include "cextractregion.h"

using namespace std;
using namespace boost;

CExtractRegion::CExtractRegion( ulong ulID ) throw()
  : CFilter( ulID, "Extract largest region", 1, 1, "CExtractRegion", "0.2", "CFilter" )
{
  sModuleID = getClassName() + "/" + getClassVersion() + "/" + sLibID;
  
  parameters.initUnsignedLong( "IntensityValue", 4, 0, 65535 );
	parameters.initUnsignedLong( "HowManyRegions", 10, 0, 65535 );
	parameters.initUnsignedLong( "SizeThreshold", 1, 0, 65535 );
  
	inputsVec[0].portType = IOInteger;
  outputsVec[0].portType = IOInteger;
}

CExtractRegion::~CExtractRegion() throw()
{
}

CPipelineItem* CExtractRegion::newInstance( ulong ulID ) const throw()
{
  return new CExtractRegion( ulID );
}

void CExtractRegion::apply() throw()
{
FBEGIN;
	bModuleReady = false;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
  if ( !checkInput<TImage>( inputPtr , 0, 3 )  )
    return;
	
	bModuleReady = true;
  // Initialize fields
  TImage work( inputPtr->getDimension(), inputPtr->getExtents() );
	TImage mask( inputPtr->getDimension(), inputPtr->getExtents() );
	work = 0;
	mask = 0;
	ushort x = 0; ushort y = 0; ushort z = 0;
	ushort zDim = 1;
	if ( inputPtr->getDimension() == 3 ) zDim = work.getExtent(2);
	uint index = 1;
	ushort intensity = parameters.getUnsignedLong( "IntensityValue" );
	ushort mustregs = parameters.getUnsignedLong( "HowManyRegions" );
	DS("Scanning voxels in " << zDim << " slices" );
	uint numregs = 0;
	while ( z < zDim )
	{		
		// Check for a new region starting point
		if ( mask(x,y,z) == 0 && (*inputPtr)(x,y,z) == intensity )
		{
			cerr << ".";
			//DS("# Found new region " << index << " at " << x << ";" << y << ";" << z );
			createRegion( work, *inputPtr, x, y, z, intensity );
			updateMask( mask, work, index );
			ulong siz = computeSize(work);
			if ( siz > parameters.getUnsignedLong( "SizeThreshold" ) )
			{
				regionSize.push_back( siz );
				numregs++;
				//DS("# Region set");
			}
			else
			{
				regionSize.push_back( 0 );
				//DS("# region too small");
			}
			work = 0; index++;
		}		
		++x;
		if ( x == work.getExtent(0) )
		{
			x = 0; y++;
		}
		if ( y == work.getExtent(1) )
		{
			y = 0; z++;
		}
	}
	cerr << endl;
	//if ( regionSize.size() > 0 ) regionSize[0] = 0;
		
	DS("Found " << numregs << " suitable regions, but only needing " << mustregs );
	while( mustregs < numregs )
	{
		uint smallestindex = 0;
		for ( uint i = 0; i < regionSize.size(); ++i )
			if ( regionSize[i] > 0 ) { smallestindex = i; break; }
		for ( uint i = 0; i < regionSize.size(); ++i )
			if (regionSize[i] < regionSize[smallestindex] && regionSize[i] >  0 )
				smallestindex = i;
		regionSize[smallestindex] = 0;
		DS("# Throwing away region " << smallestindex);
		numregs--;		
	}
	for ( ushort z = 0; z < zDim; ++z )
	for ( ushort y = 0; y < mask.getExtent(1); ++y )
			for ( ushort x = 0; x < mask.getExtent(0); ++x )
			{
				if ( mask(x,y,z)>0 && regionSize[mask(x,y,z)-1] == 0 ) mask(x,y,z) = 0;
			}
			
	TImagePtr outputPtr ( new TImage( mask ) );
	outputPtr->setMinimum( 0 );
	outputPtr->setMaximum( index );
	setOutput( outputPtr );
	regionSize.clear();
}

void CExtractRegion::createRegion( TImage& work, const TImage& input,
	ushort xx, ushort yy, ushort zz, ushort intensity )
{
	std::queue<blitz::TinyVector<ushort,3> > regionpts;
	blitz::TinyVector<ushort,3> start( xx,yy,zz );
	regionpts.push( start );
	work(xx,yy,zz) = 1;
	ushort zDim = 1;
	if ( work.getDimension() == 3 ) zDim = work.getExtent(2);
	bool changed = true;
	while ( !regionpts.empty() )
	{
		blitz::TinyVector<ushort,3> vec = regionpts.front();
		regionpts.pop();
		ushort x = vec[0]; ushort y = vec[1]; ushort z = vec[2];			
		if ( x > work.getExtent(0) || y > work.getExtent(1) || z > zDim )
		{
			DS( vec );
			exit(0);
		}
		if ( work(x,y,z) == 1 )
		{
			if ( x > 0 && x < work.getExtent(0) )
			{
				if ( input(x-1,y,z) == intensity && work(x-1,y,z)!=1 ) 
					{ work(x-1,y,z)=1; changed = true; regionpts.push( blitz::TinyVector<ushort,3>( x-1,y,z ) ); }
				if ( input(x+1,y,z) == intensity && work(x+1,y,z)!=1 ) 
					{ work(x+1,y,z)=1; changed = true; regionpts.push( blitz::TinyVector<ushort,3>( x+1,y,z ) ); }
			}
			if ( y > 0 && y < work.getExtent(1) )
			{
				if ( input(x,y-1,z) == intensity && work(x,y-1,z)!=1 ) 
					{ work(x,y-1,z)=1; changed = true; regionpts.push( blitz::TinyVector<ushort,3>( x,y-1,z ) );}
				if ( input(x,y+1,z) == intensity && work(x,y+1,z)!=1 ) 
					{ work(x,y+1,z)=1; changed = true; regionpts.push( blitz::TinyVector<ushort,3>( x,y+1,z ) );}
			}
			if ( z > 0 && z < zDim )
			{
				if ( input(x,y,z-1) == intensity && work(x,y,z-1)!=1 ) 
					{ work(x,y,z-1)=1; changed = true; regionpts.push( blitz::TinyVector<ushort,3>( x,y,z-1 ) );}
				if ( input(x,y,z+1) == intensity && work(x,y,z+1)!=1 ) 
					{ work(x,y,z+1)=1; changed = true; regionpts.push( blitz::TinyVector<ushort,3>( x,y,z+1 ) );}
			}
		}
	}
}

void CExtractRegion::updateMask( TImage& mask, TImage& work, uint index )
{
	TImage::iterator in = work.begin();
	TImage::iterator out = mask.begin();
	while( in != work.end() )
	{
		if ( (*in) > 0 ) (*out) = index;
		++in; ++out;
	}
}

ulong CExtractRegion::computeSize( TImage& region )
{
	ulong size = 0;
	TImage::iterator in = region.begin();
	while( in != region.end() )
	{
		if ( (*in) > 0 ) size++;
		++in;
	}	
	return size;
}
