/************************************************************************
 * File: cedgethinner.cpp                                               *
 * Project: AIPS                                                        *
 * Description: Thins out and homogenieses the given vector field       *
 *              to produce a better edge field                          *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 04-05-18                                                    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#include "cedgethinner.h"
#include <cfloat>

using namespace std;
using namespace boost;

/**
 * Consrtuctor
 * \param ulID unique module ID
 */
CEdgeThinner::CEdgeThinner( ulong ulID ) throw()
  : CFilter ( ulID, "Edge thinner", 1, 1, "CEdgeThinner", "0.2", "CFilter" )
{
  setModuleID( sLibID );
  sDocumentation = "Tries to generate a useful partition of a vector field\n"
                   "** Input ports:\n"
                   "0: A vector field\n"
                   "**outputPtr ports:\n"
                   "1: A single channel image"
                   "** Parameters:\n"
                   "Orientation threshold";

  parameters.initDouble( "Threshold", M_PI/8.0, 0, 2*M_PI );

  inputsVec[0].portType = IO2DVector;
  outputsVec[0].portType = IO2DVector;

}

/** Destructor */
CEdgeThinner::~CEdgeThinner() throw()
{
}

/** Reimplemented from CPipelineItem */
void CEdgeThinner::apply() throw()
{
BENCHSTART;	
	bModuleReady = false;
  shared_ptr<TField2D> inputPtr = static_pointer_cast<TField2D>( getInput() );
  if ( inputPtr.get() == NULL || inputPtr->getDimension() != 2 )
  {
    alog << LWARN << "Input type is no 2D image!" << endl;
    return;
  }
	bModuleReady = true;
  deleteOldOutput();
  shared_ptr<TField2D> outputPtr ( new TField2D( 2, inputPtr->getExtents() ) );
	(*outputPtr) = VEC_ZERO2D;
	outputPtr->setMaximum( TVector2D( 1.0, 1.0 ) );
	outputPtr->setMinimum( VEC_ZERO2D );
	for( ushort y = 1; y < (outputPtr->getExtent(1)-1); ++y )
		for( ushort x = 1; x < (outputPtr->getExtent(0)-1); ++x )
		{
			double intensity = norm((*inputPtr)(x,y));
			double l,r;
			// Get Edge direction
			switch( sector( (*inputPtr)(x,y) ) )
			{
				case 0:
				case 4:
					l = norm((*inputPtr)(x-1,y)); r = norm((*inputPtr)(x+1,y));
					break;
				case 1:
				case 5:
					l = norm((*inputPtr)(x-1,y-1)); r = norm((*inputPtr)(x+1,y+1));
					break;
				case 2:
				case 6:
					l = norm((*inputPtr)(x,y+1)); r = norm((*inputPtr)(x,y-1));
					break;
				case 3:
				case 7:
					l = norm((*inputPtr)(x-1,y+1)); r = norm((*inputPtr)(x+1,y-1));
					break;
				default:
					terminate();
			}
			// Find neighbors in edge direction und supress pixel if needed
			if ( intensity >= l && intensity >= r )
				(*outputPtr)(x,y) = (*inputPtr)(x,y);
		}	
/*	CTypedData<ushort> swapped (2, inputPtr->getExtents() );
	swapped = false;
  (*outputPtr) = (*inputPtr);
	for( ushort y = 1; y < (outputPtr->getExtent(1)-1); ++y )
		for( ushort x = 1; x < (outputPtr->getExtent(0)-1); ++x )
		{
			if ( quadNorm( (*outputPtr)(x,y) ) > DBL_EPSILON )
			{
			TVector2D base = (*outputPtr)(x,y);
			TVector2D xp = (*outputPtr)(x+1,y);
			TVector2D yp = (*outputPtr)(x,y+1);
			TVector2D dp = (*outputPtr)(x+1,y+1);
			TVector2D d2p = (*outputPtr)(x+1,y-1);
			if ( ( sector(base) == 0 || sector(base) == 4 ) && sector(base) == sector(xp) &&
				norm(base)/norm(xp) < 10.0 && norm(base)/norm(xp) >= 1.0 )				
			{
				if ( sector(base) == 0 && !swapped(x+1,y) )
				{
					(*outputPtr)(x+1,y) *= 0.0; swapped(x+1,y)=true;
				}
				else if ( sector(base) == 4 && !swapped(x,y) )
				{
					(*outputPtr)(x,y) *= 0.0; swapped(x,y)=true;
				}
			}			
			else if ( ( sector(base) == 2 || sector(base) == 6 ) && sector(base) == sector(yp) &&
				norm(base)/norm(yp) < 10.0 && norm(base)/norm(yp) >= 1.0 )				
			{
				if ( sector(base) == 2 && !swapped(x,y+1) )
				{
					(*outputPtr)(x,y+1) *= 0.0; swapped(x,y+1)=true;
				}
				else if ( sector(base) == 6  && !swapped(x,y) )
				{
					(*outputPtr)(x,y) *= 0.0; swapped(x,y)=true;			
				}
			}
			else if ( ( sector(base) == 1 || sector(base) == 5 ) && sector(base) == sector(dp) &&
				norm(base)/norm(dp) < 10.0 && norm(base)/norm(dp) >= 1.0 )				
			{
				if ( sector(base) == 1 && !swapped(x+1,y+1) )
				{
					(*outputPtr)(x+1,y+1) *= 0.0; swapped(x+1,y+1)=true;			
				}
				else if ( sector(base) == 5 && !swapped(x,y) )
				{
					(*outputPtr)(x,y) *= 0.0;	swapped(x,y)=true;			
				}
			}
			else if ( ( sector(base) == 7 || sector(base) == 3 ) && sector(base) == sector(d2p) &&
				norm(base)/norm(d2p) < 10.0 && norm(base)/norm(d2p) >= 1.0 )				
			{
				if ( sector(base) == 7  && !swapped(x+1,y-1) )
				{
					(*outputPtr)(x+1,y-1) = 0.0 * (*inputPtr)(x+1,y-1);
					swapped(x+1,y-1)=true;			
				}
				else if ( sector(base) == 3  && !swapped(x,y))
				{
					(*outputPtr)(x,y) = 0.0 * (*inputPtr)(x,y);				
					swapped(x,y)=true;			
				}
			}
			}
		}
 // Und jetzt noch die Überbleibsel glätten
 // Hier sollte eine größere Nachbarschaft betrachtet werden und der Vektor in die 
 // in der NB dominierende Richtung gedreht werden.
 for( ushort y = 1; y < (outputPtr->getExtent(1)-1); ++y )
	for( ushort x = 1; x < (outputPtr->getExtent(0)-1); ++x )
	{
		if ( swapped(x,y) == false )
		{
			ushort sec = sector( (*outputPtr)(x,y) );
			if ( sec == 0 || sec == 1 || sec == 7 )
			{
				ushort su = sector( (*outputPtr)(x,y-1) );
				ushort sd = sector( (*outputPtr)(x,y+1) );
				if ( in( int(su), 3, 5 ) && in ( int(sd), 3, 5 ) )
					(*outputPtr)(x,y) *= 0.0;
			}
			if ( in( int(sec), 3, 5 ) )
			{
				ushort su = sector( (*outputPtr)( x, y - 1 ) );
				ushort sd = sector( (*outputPtr)( x, y + 1 ) );
				if ( ( su == 0 || su == 1 || su == 7 ) && ( sd == 0 || sd == 1 || sd == 7 ) ) 
					(*outputPtr)(x,y) *= 0.0;
			}
			if ( in( int(sec), 1, 3 ) )
			{
				ushort su = sector( (*outputPtr)(x-1,y) );
				ushort sd = sector( (*outputPtr)(x+1,y) );
				if ( in( int(su), 5, 7 ) && in ( int(sd), 5, 7 ) )
					(*outputPtr)(x,y) *= 0.0;				
			}
			if ( in( int(sec), 5, 7 ) )
			{
				ushort su = sector( (*outputPtr)(x-1,y) );
				ushort sd = sector( (*outputPtr)(x+1,y) );
				if ( in( int(su), 1, 3 ) && in ( int(sd), 1, 3 ) )
					(*outputPtr)(x,y) *= 0.0;				
			}
		}
 	}
	for( ushort y = 1; y < (outputPtr->getExtent(1)-1); ++y )
		for( ushort x = 1; x < (outputPtr->getExtent(0)-1); ++x )
	{
		ushort nb = 0;
		if( quadNorm( (*outputPtr)(x,y) ) > DBL_EPSILON )
		{
			if ( quadNorm( (*outputPtr)(x-1,y) ) <= DBL_EPSILON )
				nb++;
			if ( quadNorm( (*outputPtr)(x+1,y) ) <= DBL_EPSILON )
				nb++;
			if ( quadNorm( (*outputPtr)(x,y-1) ) <= DBL_EPSILON )
				nb++;
			if ( quadNorm( (*outputPtr)(x,y+1) ) <= DBL_EPSILON )
				nb++;
		if ( nb > 2 )
			(*outputPtr)(x,y) *= 0.0;
		}
	}*/
  setOutput( outputPtr );
BENCHSTOP;
}

ushort CEdgeThinner::sector( TVector2D aVector ) throw()
{
	double hw = M_PI/8;
	double fw = M_PI/4;
	double angle = atan2( aVector[0], aVector[1] );
	if ( angle < 0.0 )
		angle = 2*M_PI + angle;
	if ( angle >= hw + 7*fw || angle < hw )
		return 2;
	if ( angle >= hw && angle < hw + fw )
		return 1;
	if ( angle >= hw + fw && angle < hw + 2 * fw )
		return 0;
	if ( angle >= hw + 2 * fw && angle < hw + 3 * fw )
		return 7;
	if ( angle >= hw + 3 * fw && angle < hw + 4 * fw )
		return 6;
	if ( angle >= hw + 4 * fw && angle < hw + 5 * fw )
		return 5;
	if ( angle >= hw + 5 * fw && angle < hw + 6 * fw )
		return 4;
	if ( angle >= hw + 6 * fw && angle < hw + 7 * fw )
		return 3;		
	terminate();
}

/** Reimplemented from CPipelineItem */
CPipelineItem* CEdgeThinner::newInstance( ulong ulID ) const throw()
{
  return new CEdgeThinner( ulID );
}

