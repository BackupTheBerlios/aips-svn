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
                   "**outputSPtr ports:\n"
                   "1: A single channel image"
                   "** Parameters:\n"
                   "Orientation threshold";

  inputsVec[0].portType = IOVector;
  outputsVec[0].portType = IOVector;
}

/** Destructor */
CEdgeThinner::~CEdgeThinner() throw()
{
}

/** Reimplemented from CPipelineItem */
void CEdgeThinner::apply() throw()
{
FBEGIN;
BENCHSTART;	
	bModuleReady = false;
	if ( getInput() )
		call<Length<vectorfieldTL>::value - 1>();
	else
  {
    alog << LWARN << SERROR("Input type is no 2D or 3D image!") << endl;
    return;
  }	
BENCHSTOP;	
FEND;
}

template<> 
bool CEdgeThinner::suppress<TField2D>() throw()
{
FBEGIN;
  TField2DPtr inputSPtr = static_pointer_cast<TField2D>( getInput() );
  if ( !checkInput<TField2D>( inputSPtr, 2, 2 ) )
  	return false;
	bModuleReady = true;
 	deleteOldOutput();
  TField2DPtr outputSPtr ( new TField2D( 2, inputSPtr->getExtents(), inputSPtr->getDataDimension() ) );
	(*outputSPtr) = VEC_ZERO2D;
	outputSPtr->setMaximum( 1.0 );
	outputSPtr->setMinimum( 0.0 );
	for( ushort d = 0; d < outputSPtr->getDataDimension(); ++d )
		for( ushort y = 1; y < (outputSPtr->getExtent(1)-1); ++y )
			for( ushort x = 1; x < (outputSPtr->getExtent(0)-1); ++x )
			{
				double n = norm( (*inputSPtr)(x,y,d) );
				if ( n > DBL_EPSILON )
				{
					TVector2D dir = (*inputSPtr)(x,y,d) / n;					
					double l = norm( (*inputSPtr)( static_cast<uint>( round( static_cast<double>( x ) + (dir[0]*sqrt(2.0) ) ) ), 
						static_cast<uint>( round( static_cast<double>( y ) + (dir[1]*sqrt(2.0) ) ) ),d ) );
					dir *= -1.0;
					double r = norm( (*inputSPtr)( static_cast<uint>( round( static_cast<double>( x ) + (dir[0]*sqrt(2.0) ) ) ), 
						static_cast<uint>( round( static_cast<double>( y ) + (dir[1]*sqrt(2.0) ) ) ),d ) );
					if ( n >= l && n >= r )
						(*outputSPtr)(x,y,d) = (*inputSPtr)(x,y,d);
				}
			}	
	setOutput( outputSPtr );
BENCHSTOP;
FEND;	
	return true;
}

template<>
bool CEdgeThinner::suppress<TField3D>() throw()
{
FBEGIN;
  TField3DPtr inputSPtr = static_pointer_cast<TField3D>( getInput() );
  if ( !checkInput<TField3D>( inputSPtr, 3, 3 ) )
  	return false;
	bModuleReady = true;
 	deleteOldOutput();
  TField3DPtr outputSPtr ( new TField3D( 3, inputSPtr->getExtents(), inputSPtr->getDataDimension() ) );
	(*outputSPtr) = VEC_ZERO3D;
	outputSPtr->setMaximum( 1.0 );
	outputSPtr->setMinimum( 0.0 );
	for( ushort d = 0; d < outputSPtr->getDataDimension(); ++d )
		for( ushort z = 1; z < (outputSPtr->getExtent(2)-1); ++z )
			for( ushort y = 1; y < (outputSPtr->getExtent(1)-1); ++y )
				for( ushort x = 1; x < (outputSPtr->getExtent(0)-1); ++x )
				{
					double n = norm( (*inputSPtr)(x,y,z,d) );
					if ( n > DBL_EPSILON )
					{
						TVector3D dir = (*inputSPtr)(x,y,z,d) / n;
						double l = norm( (*inputSPtr)( 
							static_cast<uint>( round( static_cast<double>( x ) + ( dir[0] * sqrt( 3.0 ) ) ) ), 
							static_cast<uint>( round( static_cast<double>( y ) + ( dir[1] * sqrt( 3.0 ) ) ) ), 
							static_cast<uint>( round( static_cast<double>( z ) + ( dir[2] * sqrt( 3.0 ) ) ) ), d ) );
						dir *= -1.0;
						double r = norm( (*inputSPtr)( 
							static_cast<uint>( round( static_cast<double>( x ) + ( dir[0] * sqrt( 3.0 ) ) ) ), 
							static_cast<uint>( round( static_cast<double>( y ) + ( dir[1] * sqrt( 3.0 ) ) ) ), 
							static_cast<uint>( round( static_cast<double>( z ) + ( dir[2] * sqrt( 3.0 ) ) ) ), d ) );
						if ( n >= l && n >= r )
							(*outputSPtr)(x,y,z,d) = (*inputSPtr)(x,y,z,d);
					}
				}	
  setOutput( outputSPtr );
BENCHSTOP;
FEND;	
	return true;
}

template<typename FieldType> 
bool CEdgeThinner::suppress() throw()
{
	return false;
}

DEFINE_CALL_MACRO( CEdgeThinner::call, CEdgeThinner::suppress, vectorfieldTL )
