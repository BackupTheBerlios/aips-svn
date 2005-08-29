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
#include "cregiongrowing.h"

using namespace std;
using namespace boost;

CRegionGrowing::CRegionGrowing( ulong ulID ) throw()
  : CFilter( ulID, "Region growing", 2, 1, "CRegionGrowing", "0.1", "CFilter" )
{
  sModuleID = getClassName() + "/" + getClassVersion() + "/" + sLibID;
  
  parameters.initUnsignedLong( "RegionThreshold", 0, 0, 65535 );

  inputsVec[0].portType = IOInteger;
  inputsVec[1].portType = IO1DVector;
  outputsVec[0].portType = IOInteger;
}

CRegionGrowing::~CRegionGrowing() throw()
{
}

CPipelineItem* CRegionGrowing::newInstance( ulong ulID ) const throw()
{
  return new CRegionGrowing( ulID );
}

void CRegionGrowing::apply() throw()
{
FBEGIN;
cerr << "Begin CRegionGrowing" << endl;
	bModuleReady = false;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
  if ( !checkInput<TImage>(inputPtr, 2, 3, 1, 1 ) )
	{
    return;
	}
	size_t siz[1] = {1};	
  TField2DPtr seedPointsPtr; 
  TField3DPtr seedPointsPtr3D;
  if ( getInput(1).get() )
  {
  	cerr << getInput(1)->getType().name() << endl;
  if ( getInput(1)->getType() == typeid( TVector2D ) )
  	seedPointsPtr = static_pointer_cast<TField2D>( getInput(1) );
  else if ( inputPtr->getDimension() == 3 )
  {
  	seedPointsPtr3D = static_pointer_cast<TField3D>( getInput(1) );
  	if ( seedPointsPtr3D.get() == NULL )
		{
			alog << LWARN << "No input point list. Region growing is done from image center" << endl;
			seedPointsPtr3D.reset( new TField3D( 1, siz ) );
			(*seedPointsPtr3D)(0)[0] = inputPtr->getExtent(0)/2;
			(*seedPointsPtr3D)(0)[1] = inputPtr->getExtent(1)/2;
			(*seedPointsPtr3D)(0)[2] = inputPtr->getExtent(2)/2;
		}
	}
	}
	else
	{
		alog << LWARN << "No input point list. Region growing is done from image center" << endl;
		seedPointsPtr.reset( new TField2D( 1, siz ) );
		(*seedPointsPtr)(0)[0] = 0;//inputPtr->getExtent(0)/2;
		(*seedPointsPtr)(0)[1] = 0;//inputPtr->getExtent(1)/2;
	}
	
	bModuleReady = true;
  // Initialize fields
  vector<size_t> dims;
  for( uint i = 0; i < inputPtr->getDimension(); ++i )
  	dims.push_back( inputPtr->getExtent(i) );
  	
  TImagePtr outputPtr ( new TImage( dims.size(), dims ) );
  (*outputPtr) = 0;
cerr << " Setting seed points" << endl;
  // Set the seedPointsPtr points
  if ( seedPointsPtr.get() )
	  for ( TField2D::iterator it = seedPointsPtr->begin(); it != seedPointsPtr->end(); ++it )
		{
			TPoint3D p( static_cast<short>((*it)[0]), static_cast<short>((*it)[1]), 0 );
			(*outputPtr)( p[0], p[1] ) = 1;
			work.push(p);
		}
	else
	  for ( TField3D::iterator it = seedPointsPtr3D->begin(); it != seedPointsPtr3D->end(); ++it )
		{
			TPoint3D p( static_cast<short>((*it)[0]), inputPtr->getExtent(1)-1-static_cast<short>((*it)[1]), static_cast<short>((*it)[2]) );
			(*outputPtr)( p[0], inputPtr->getExtent(1)-1-p[1], p[2] ) = 1;
			work.push(p);
		}

	ulRegionThreshold = parameters.getUnsignedLong( "RegionThreshold" );
//  ulong ulTimestep = 0; 
cerr << " Computing area" << endl;
  // Computation loop  
  while( !work.empty() )
  {
    TPoint3D p = work.front(); work.pop();
		if ( inputPtr->getDimension() == 2 )
		{
			if ( ( p[0] > 0 )
			  && ( (*outputPtr)( p[0]-1, p[1] ) == 0 )
				&& ( static_cast<ulong>(abs((*inputPtr)( p[0]-1, p[1])-(*inputPtr)( p[0], p[1])))<=ulRegionThreshold ) )
				{
					work.push( TPoint3D( p[0]-1, p[1], 0 ) ); (*outputPtr)( p[0]-1, p[1] ) = 1;
				}
			if ( ( p[0] < static_cast<long>(inputPtr->getExtent(0)-1) )
			  && ( (*outputPtr)( p[0]+1, p[1] ) == 0 )
				&& ( static_cast<ulong>(abs((*inputPtr)( p[0]+1, p[1])-(*inputPtr)( p[0], p[1])))<=ulRegionThreshold ) )
				{
					work.push( TPoint3D( p[0]+1, p[1], 0 ) ); (*outputPtr)( p[0]+1, p[1] ) = 1;
				}
			if ( ( p[1] > 0 )
		  	&& ( (*outputPtr)( p[0], p[1]-1 ) == 0 )
				&& ( static_cast<ulong>(abs((*inputPtr)( p[0], p[1]-1)-(*inputPtr)( p[0], p[1])))<=ulRegionThreshold ) )
				{
					work.push( TPoint3D( p[0], p[1]-1, 0 ) ); (*outputPtr)( p[0], p[1]-1 ) = 1;
				}
			if ( ( p[1] < static_cast<long>(inputPtr->getExtent(1)-1) )
			  && ( (*outputPtr)( p[0], p[1]+1 ) == 0 )
				&& ( static_cast<ulong>(abs((*inputPtr)( p[0], p[1]+1)-(*inputPtr)( p[0], p[1])))<=ulRegionThreshold ) )
				{
					work.push( TPoint3D( p[0], p[1]+1, 0 ) ); (*outputPtr)( p[0], p[1]+1 ) = 1;
				}
		}
		else
		{
			if ( ( p[0] > 0 )
			  && ( (*outputPtr)( p[0]-1, p[1], p[2] ) == 0 )
				&& ( static_cast<ulong>( abs( (*inputPtr)( p[0]-1, p[1], p[2])
					- (*inputPtr)( p[0], p[1], p[2]) ) ) <= ulRegionThreshold ) )
				{
					work.push( TPoint3D( p[0]-1, p[1], p[2] ) ); (*outputPtr)( p[0]-1, p[1], p[2] ) = 1;
				}
			if ( ( p[0] < static_cast<long>(inputPtr->getExtent(0)-1) )
			  && ( (*outputPtr)( p[0]+1, p[1], p[2] ) == 0 )
				&& ( static_cast<ulong>( abs((*inputPtr)( p[0]+1, p[1], p[2])-(*inputPtr)( p[0], p[1], p[2])))<=ulRegionThreshold ) )
				{
					work.push( TPoint3D( p[0]+1, p[1], p[2] ) ); (*outputPtr)( p[0]+1, p[1], p[2] ) = 1;
				}
			if ( ( p[1] > 0 )
			  && ( (*outputPtr)( p[0], p[1]-1, p[2] ) == 0 )
				&& ( static_cast<ulong>(abs((*inputPtr)( p[0], p[1]-1, p[2])-(*inputPtr)( p[0], p[1], p[2])))<=ulRegionThreshold ) )
				{
					work.push( TPoint3D( p[0], p[1]-1, p[2] ) ); (*outputPtr)( p[0], p[1]-1, p[2] ) = 1;
				}
			if ( ( p[1] < static_cast<long>(inputPtr->getExtent(1)-1) )
			  && ( (*outputPtr)( p[0], p[1]+1, p[2] ) == 0 )
				&& ( static_cast<ulong>(abs((*inputPtr)( p[0], p[1]+1, p[2])-(*inputPtr)( p[0], p[1], p[2])))<=ulRegionThreshold ) )
				{
					work.push( TPoint3D( p[0], p[1]+1, p[2] ) ); (*outputPtr)( p[0], p[1]+1, p[2] ) = 1;
				}
			if ( ( p[2] > 0 )
			  && ( (*outputPtr)( p[0], p[1], p[2]-1 ) == 0 )
				&& ( static_cast<ulong>(abs((*inputPtr)( p[0], p[1], p[2]-1 ) - (*inputPtr)( p[0], p[1], p[2])))<=ulRegionThreshold ) )
				{
					work.push( TPoint3D( p[0], p[1], p[2]-1 ) ); (*outputPtr)( p[0], p[1], p[2]-1 ) = 1;
				}
			if ( ( p[2] < static_cast<long>(inputPtr->getExtent(2)-1) )
			  && ( (*outputPtr)( p[0], p[1], p[2]+1 ) == 0 )
				&& ( static_cast<ulong>(abs((*inputPtr)( p[0], p[1], p[2]+1 ) - (*inputPtr)( p[0], p[1], p[2])))<=ulRegionThreshold ) )
				{
					work.push( TPoint3D( p[0], p[1], p[2]+1 ) ); (*outputPtr)( p[0], p[1], p[2]+1 ) = 1;
				}
		}
 	}
	outputPtr->setMinimum( 0 );
  outputPtr->setMaximum( 1 );
  setOutput( outputPtr );
cerr << "End CRegionGrowing" << endl;	
FEND;
}


