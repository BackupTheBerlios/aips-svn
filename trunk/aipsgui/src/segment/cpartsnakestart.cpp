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
#include "cpartsnakestart.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

double ptl( const TPoint2D& p, const TPoint2D& s0, const TPoint2D& s1 )
{    
//	DS("a");
	TVector2D P;
	P[0] = p[0]; P[1] = p[1];
	TVector2D S0;
	S0[0] = s0[0]; 
	S0[1] = s0[1];
	TVector2D S1;
	S1[0] = s1[0]; 
	S1[1] = s1[1];
//	DS("A");
	TVector2D v = S1 - S0;
  TVector2D w = P - S0;
// 	DS("B");
	double c1 = dot(w,v);
  if ( c1 <= 0 )
    return norm( P - S0 );
// 	DS("C");
  double c2 = dot(v,v);
  if ( c2 <= c1 )
    return norm( P - S1 );
// 	DS("D");
  double b = c1 / c2;
  TVector2D Pb = S0 + b * v;
	return norm( P - Pb );
}
 
/**
 * \param ulID unique module ID
 */
CPartSnakeStart::CPartSnakeStart( ulong ulID ) throw()
  : CFilter ( ulID, "Partsnake polygon", 1, 2, "CPartSnakeStart", "0.1", "CFilter" )
{
  setModuleID( sLibID );

  sDocumentation = "Extracts local maxima from the given image\n"
                   "** Input ports:\n"
                   "0: A scalar 2D or 3D multichannel data set\n"
                   "**Output ports:\n"
                   "0: A scalar 2D or 3D multichannel data set\n"
                   "**Parameters:\n"
                   "Radius: radius of filter kernel";
	//parameters.initUnsignedLong( "Radius", 2, 1, 1000 );
	inputsVec[0].portType = IO2DInteger;
	outputsVec[1].portType = IO1DVector;
}

CPartSnakeStart::~CPartSnakeStart() throw()
{
}

/*****************
 * Other methods *
 *****************/
 
void CPartSnakeStart::apply() throw()
{
	alog << "APPLY begin" << endl;
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
	// Fill outer rim
	(*outputPtr)=(*inputPtr);
	queue<TPoint2D> pointQueue;
	TPoint2D p(0,0);	
	pointQueue.push( p );
	while( !pointQueue.empty() )
	{
		TPoint2D q = pointQueue.front(); pointQueue.pop();
		if ( (*outputPtr)(q[0],q[1]) == 0 )
		{
			(*outputPtr)(q[0],q[1]) = 1;
			TPoint2D r (q);
			if ( q[0] < static_cast<long>(dimensionSize[0]-1) && (*outputPtr)(q[0]+1,q[1]) == 0 ) 
				{ r[0] = q[0]+1; r[1] = q[1]; pointQueue.push( r ); }
			if ( q[0] > 0 && (*outputPtr)(q[0]-1,q[1]) == 0 ) 
				{ r[0] = q[0]-1; r[1] = q[1]; pointQueue.push( r ); }
			if ( q[1] < static_cast<long>(dimensionSize[1]-1) && (*outputPtr)(q[0],q[1]+1) == 0 ) 
				{ r[0] = q[0]; r[1] = q[1]+1; pointQueue.push( r ); }
			if ( q[1] > 0 && (*outputPtr)(q[0],q[1]-1) == 0 ) 
				{ r[0] = q[0]; r[1] = q[1]-1; pointQueue.push( r ); }			
		}
	}
 	for( ushort y = 1; y < dimensionSize[1]-1; ++y )
		for( ushort x = 1; x < dimensionSize[0]-1; ++x )
		{
			if ( (*outputPtr)(x,y) == 0 && 
			( (*outputPtr)(x-1,y) == 1 || (*outputPtr)(x+1,y) == 1 || (*outputPtr)(x,y-1) == 1 || (*outputPtr)(x,y+1) == 1 )
			)
				(*outputPtr)(x,y) = 2;
		}
	outputPtr->setMinimum( 0 );
	outputPtr->setMaximum( 3 );
	
 	for( ushort y = 1; y < dimensionSize[1]-1; ++y )
		for( ushort x = 1; x < dimensionSize[0]-1; ++x )
		{
			if ( (*outputPtr)(x,y) == 2 &&  (*outputPtr)(x-1,y) != 0 && (*outputPtr)(x+1,y) != 0 
			 && (*outputPtr)(x,y-1) != 0 && (*outputPtr)(x,y+1) != 0 )			
				(*outputPtr)(x,y) = 1;
		}
	alog << "Outer rim filled" << endl;
	vector<TPoint2D> dump;
	vector<TPoint2D> pointList;
	map<double,TPoint2D> angleMap;
	
	// Extract points from Image
	for ( ushort y = 0; y < outputPtr->getExtent(1); ++y )
		for ( ushort x = 0; x < outputPtr->getExtent(0); ++x )
		{
			if ( (*outputPtr)(x,y) == 2 )
			{
				TPoint2D p(x,y);
				pointList.push_back(p);
				//DS("Found point " << p );
			}
		}
		
	alog << "POints extracted" << endl;
	// Find point with smallest coordinate: p0 which has index 0 in the above vector
	// Sort points according to angle to p0
	TPoint2D zero ( 1, 0 );
	vector<TPoint2D>::iterator ti = pointList.begin(); ++ti; 
	for( vector<TPoint2D>::iterator it = ti; it != pointList.end(); ++it )
	{
		TPoint2D v1 = *it - *pointList.begin();		
		double angle = atan2( double( v1[1] ),double(  v1[0] ) );
		DS( angle << ": " << v1 );
		if ( angle < 0.0 ) angle += 2*M_PI;
		map<double,TPoint2D>::iterator mit = angleMap.find( angle );
		if ( mit == angleMap.end() )
			angleMap[ angle ] = (*it); 
		else
		{
			if ( norm (mit->second - *pointList.begin()) < norm (*it-*pointList.begin()) )
			{
				dump.push_back( angleMap[ angle ] );
				angleMap[ angle ] = (*it);				
			}
		}			
	}
	for( map<double,TPoint2D>::iterator it = angleMap.begin(); it != angleMap.end(); ++it )
	{
		alog << it->first << " " << it->second << endl;
	}
	vector<TPoint2D> stack;	
	stack.resize( pointList.size() );
	uint idx = 2;
	map<double,TPoint2D>::iterator mapIt = angleMap.begin();	
	stack[0] = ( *pointList.begin() );
	stack[1] = ( mapIt->second );
	++mapIt;
	stack[2] = ( mapIt->second );
	++mapIt;
 alog << "Beg" << endl;	
	while( mapIt != angleMap.end() )
	{
		bool left = false;
/*		DS("Stack trace");*/
/*		for( uint i = 0; i <= idx; ++i )
		DS( stack[i] );*/
		while( !left && idx > 0 )
		{
			TPoint2D v1 = stack[idx] - mapIt->second;
			TPoint2D v2 = stack[idx-1] - mapIt->second;
			double angle = atan2( double( v1[1] ),double(  v1[0] ) );
			angle = atan2( double( v2[1] ),double(  v2[0] ) ) - angle;
			//DS(angle<< " " << idx);
			if ( angle > 0.0 )
			{
				dump.push_back( stack[idx] );
				idx--;
			}
			else
				left = true;
		}		
		//DS(" Getting next item "); 
		if ( mapIt != angleMap.end() )
		{
			idx++;
			stack[idx] = ( mapIt->second );
		}
		++mapIt;
	}
alog << "End" << endl;	
	list<TPoint2D> pl;
	for( uint i = 0; i <= idx; ++i )
	{
		alog << stack[i] << endl;
		pl.push_back( stack [i] );
		(*outputPtr)( stack[i][0],stack[i][1] ) = 3;
	}

	// Find central ridge begin and end y-coordinate
	
	alog << "Looking for ridge" << endl;
	ushort usRidgeMin = 0;
	ushort usRidgeMax = 0;
	ushort usState = 0;
	vector<TPoint2D> intervals;
	for ( short y = outputPtr->getExtent(1) - 1; y >= 0; --y )
	{
		for ( ushort x = 0; x < outputPtr->getExtent(0); ++x )
		{
			if ( usState == 0 && (*outputPtr)(x,y) > 1 )
			{
					usState++; 
				break;
			}
			else if ( usState == 1 && (*outputPtr)(x,y) > 1 )
			{
				break;
			}
			else if ( usState == 1 && x == outputPtr->getExtent(0)-1 )
			{
				usState++; usRidgeMin = y;
				break;
			}
			else if ( usState == 2 && (*outputPtr)(x,y) > 1 )
			{
				usState++; usRidgeMax = y + 1;
			}
		}
		if ( usState == 3 ) 
		{
		 usState = 0; 
		 TPoint2D interv( usRidgeMin, usRidgeMax );
		 intervals.push_back( interv );
		 alog << "Found " << usRidgeMin << " " << usRidgeMax << endl;
		}
	}
	vector<ushort> intDist; // Distances of intervals from image center
	for( vector<TPoint2D>::iterator it = intervals.begin(); it != intervals.end(); ++it )
	{
		ushort tmp = 
			std::min( abs( static_cast<int>((*it)[0])-static_cast<int>(outputPtr->getExtent(1)/2)), 
			abs( static_cast<int>((*it)[1])-static_cast<int>(outputPtr->getExtent(1)/2) ));
		intDist.push_back( tmp );
	}
	ushort usMinDist = outputPtr->getExtent(1);	
	for( ushort i = 0; i < intDist.size(); ++i )
	{
		if ( intDist[i] < usMinDist )
		{
			usRidgeMin = intervals[i][0];
			usRidgeMax = intervals[i][1];
			usMinDist = intDist[i];
		}
	}
	
	alog << "Ridge is located at" << usRidgeMin << " till " << usRidgeMax << endl;
	// Find convex hull points above ridge with minimal and maximal x coordinate
	ushort usMinX = outputPtr->getExtent(0);
	ushort usMaxX = 0;
	for ( ushort y = outputPtr->getExtent(1); y >= usRidgeMin; --y )
	{
		for ( ushort x = 0; x < outputPtr->getExtent(0); ++x )
		{
			if ( (*outputPtr)(x,y) == 3 && x < usMinX ) usMinX = x;
			if ( (*outputPtr)(x,y) == 3 && x > usMaxX ) usMaxX = x;
		}
	}
	
	alog << "Found minmax x's at " << usMinX << " and " << usMaxX << endl;
	
	TPoint2D MinX( usMinX, usRidgeMin + ( usRidgeMax - usRidgeMin )/2 );
	TPoint2D MaxX( usMaxX, usRidgeMin + ( usRidgeMax - usRidgeMin )/2 );
	pl.push_back( MinX );
	pl.push_back( MaxX );
		
	size_t dims[1]; dims[0]  = pl.size();
	TField2D vertexField( 1, dims );
	int i = 0;
	for( list<TPoint2D>::iterator it = pl.begin(); it != pl.end(); ++it, ++i )
	{
		(vertexField)( i )[0] = (*it)[0];
		(vertexField)( i )[1] = (*it)[1];
		
	}
	
	// Produce a two peasants polygon
	TVector2D minVec, maxVec;
	minVec = *( vertexField.begin() );
	maxVec = *( vertexField.begin() );

	// Find the points with the smallest and largest coordinates
	for( TField2D::iterator it = vertexField.begin(); it != vertexField.end(); ++it )
	{
		if ( (*it)[1] < minVec[1] ) 
			minVec = *it;
		else if ( (*it)[1] == minVec[1] && (*it)[0] < minVec[0] ) 
			minVec = *it;
		if ( (*it)[1] > maxVec[1] ) 
			maxVec = *it;
		else if ( (*it)[1] == maxVec[1] && (*it)[0] > maxVec[0] ) 
			maxVec = *it;
	}
	alog << "Found minimum points" << endl;
	list<TVector2D> upperPointList, lowerPointList;
	
	// Put all other points into the lists according to their position above or below the
  // connecting line of the mininum and maximum points
	for( TField2D::iterator it = vertexField.begin(); it != vertexField.end(); ++it )
	{
		if ( ( (*it)[1] != minVec[1] || (*it)[0] != minVec[0] ) 
			&& ( (*it)[1] != maxVec[1] || (*it)[0] != maxVec[0] ) )
		{
			if ( checkUpper( minVec, maxVec, (*it) ) )
			{
				if ( upperPointList.empty() )
					upperPointList.push_back( *it );
				else
				{
					bool bBreak = false;
					for( list<TVector2D>::iterator ut = upperPointList.begin(); ut != upperPointList.end(); ++ut )	
					{
						if ( (*it)[1] < (*ut)[1] )
						{
							upperPointList.insert( ut, *it ); 
							bBreak=true; 
							break;
						}
						else if  ( (*it)[1] == (*ut)[1] && (*it)[0] < (*ut)[0] )
						{
							upperPointList.insert( ut, *it ); 
							bBreak=true; 
							break;
						}
					}		
					// Check if the current point wasn't inserted. In that case, just put it to te list end		
					if ( !bBreak )
						upperPointList.push_back( *it );
				}			
			}
			else
			{
				if ( lowerPointList.empty() )
					lowerPointList.push_back( *it );
				else
				{
					bool bBreak = false;
					for( list<TVector2D>::iterator lt = lowerPointList.begin(); lt != lowerPointList.end(); ++lt )	
					{
						if ( (*it)[1] < (*lt)[1] )
						{
							lowerPointList.insert( lt, *it ); 
							bBreak =true;
							break; 
						}
						else if  ( (*it)[1] == (*lt)[1] && (*it)[0] < (*lt)[0] )
						{
							lowerPointList.insert( lt, *it ); 
							bBreak=true;
							break; 
						}
					}
					if ( !bBreak )
						lowerPointList.push_back( *it );
				}			
			}
		}
	}
	alog << "Generated point lists" << endl;
	// Generate Polygon
	TField2DPtr outputField ( new TField2D( vertexField ) );
	(*outputField)(0) = minVec;
	i = 1;
	if ( !upperPointList.empty() )
	{	
		list<TVector2D>::iterator it = upperPointList.begin();
		while ( it != upperPointList.end() )
		{
			(*outputField)(i) = *it;
			++i;
			++it;
		}
	}
	(*outputField)(i) = maxVec; ++i;
	if ( !lowerPointList.empty() )
	{
		list<TVector2D>::reverse_iterator rit = lowerPointList.rbegin();
		while ( rit != lowerPointList.rend() )
		{
			(*outputField)(i) = *rit;
			++i;
			++rit;
		}	
	}
	alog << "Generated polygon" << endl;
	for( TField2D::iterator it = outputField->begin(); it != outputField->end(); ++it )
	{
		alog << (*it) << endl;
	}
	std::reverse( outputField->begin(), outputField->end() ); 
	
PROG_RESET();
  setOutput( outputPtr );
	setOutput( outputField, 1 );
BENCHSTOP;
}

CPipelineItem* CPartSnakeStart::newInstance( ulong ulID ) const throw()
{
  return new CPartSnakeStart( ulID );
}

bool CPartSnakeStart::checkUpper( const TVector2D& p0, const TVector2D& p1, const TVector2D& s )
const throw()
{
 	double cp = (p1[1] - p0[1]) * (s[0] - p1[0]) - (p1[0] - p0[0]) * (s[1] - p1[1]);
	double cz = (p1[1] - p0[1]) * ( - p1[0]) - (p1[0] - p0[0]) * ( - p1[1]);
	if ( ( cp > 0 && cz < 0 ) || ( cz > 0 && cp < 0 ) )
		return true;
	else
		return false;
}
