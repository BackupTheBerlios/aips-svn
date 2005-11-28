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

double interpolatedVoxelValue2D( int x, int y, const TVector2D& direction, TField2DPtr image )
{
	double xpos = static_cast<double>( x ) + (direction[0]*sqrt(2.0) );
	double ypos = static_cast<double>( y ) + (direction[1]*sqrt(2.0) );
	return norm( (*image)( static_cast<uint>( round( xpos ) ), 
		static_cast<uint>( round( ypos ) ) ) );
}

double interpolatedVoxelValue3D( int x, int y, int z, const TVector3D& direction, TField3DPtr image )
{
	double xpos = static_cast<double>( x ) + ( direction[0] * sqrt( 3.0 ) );
	double ypos = static_cast<double>( y ) + ( direction[1] * sqrt( 3.0 ) );
	double zpos = static_cast<double>( z ) + ( direction[2] * sqrt( 3.0 ) );
	return norm( (*image)( static_cast<uint>( round( xpos ) ), 
		static_cast<uint>( round( ypos ) ), static_cast<uint>( round( zpos ) ) ) );
}

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
  if ( inputPtr && inputPtr->getDimension() == 2 )
  {
		bModuleReady = true;
  	deleteOldOutput();
	  shared_ptr<TField2D> outputPtr ( new TField2D( 2, inputPtr->getExtents() ) );
		(*outputPtr) = VEC_ZERO2D;
		outputPtr->setMaximum( TVector2D( 1.0, 1.0 ) );
		outputPtr->setMinimum( VEC_ZERO2D );
		for( ushort y = 1; y < (outputPtr->getExtent(1)-1); ++y )
			for( ushort x = 1; x < (outputPtr->getExtent(0)-1); ++x )
			{
				if ( norm( (*inputPtr)(x,y) ) > DBL_EPSILON )
				{
					TVector2D dir = (*inputPtr)(x,y) / norm( (*inputPtr)(x,y) );
					double intensity = norm( (*inputPtr)(x,y) );
					double l = interpolatedVoxelValue2D( x, y, dir, inputPtr );
					dir *= -1.0;
					double r = interpolatedVoxelValue2D( x, y, dir, inputPtr );
					if ( intensity >= l && intensity >= r )
						(*outputPtr)(x,y) = (*inputPtr)(x,y);
				}
			}	
 		setOutput( outputPtr );
  }
  else
  {
  	shared_ptr<TField3D> inputPtr = static_pointer_cast<TField3D>( getInput() );
  	if ( inputPtr && inputPtr->getDimension() == 3 )
	  {
			bModuleReady = true;
  		deleteOldOutput();
		  shared_ptr<TField3D> outputPtr ( new TField3D( 3, inputPtr->getExtents() ) );
			(*outputPtr) = VEC_ZERO3D;
			outputPtr->setMaximum( TVector3D( 1.0, 1.0, 1.0 ) );
			outputPtr->setMinimum( VEC_ZERO3D );
			for( ushort z = 1; z < (outputPtr->getExtent(2)-1); ++z )
			for( ushort y = 1; y < (outputPtr->getExtent(1)-1); ++y )
				for( ushort x = 1; x < (outputPtr->getExtent(0)-1); ++x )
				{
					if ( norm( (*inputPtr)(x,y,z ) ) > DBL_EPSILON )
					{
						TVector3D dir = (*inputPtr)(x,y,z ) / norm( (*inputPtr)(x,y,z) );
						double intensity = norm( (*inputPtr)(x,y,z) );
						double l = interpolatedVoxelValue3D( x, y, z,dir, inputPtr );
						dir *= -1.0;
						double r = interpolatedVoxelValue3D( x, y, z,dir, inputPtr );
						if ( intensity >= l && intensity >= r )
							(*outputPtr)(x,y,z) = (*inputPtr)(x,y,z);
					}
				}	
 			setOutput( outputPtr );
 		}
	  else
  	{
    	alog << LWARN << "Input type is no 2D or 3D image!" << endl;
    	return;
  	}
  }
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

