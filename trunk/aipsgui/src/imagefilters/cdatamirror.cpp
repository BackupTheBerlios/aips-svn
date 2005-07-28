/************************************************************************
 * File: cdatamirror.cpp                                                *
 * Project: AIPS image filters plugin                                   *
 * Description: Mirrors the given dataset on the x,y,z axes             *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 2004-05-11                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#include "cdatamirror.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

/** \param uLID unique module ID */
CDataMirror::CDataMirror( ulong ulID ) throw()
: CFilter ( ulID, "Image mirror", 1, 1, "CDataMirror", "0.2", "CFilter" )
{
  setModuleID( sLibID );

  sDocumentation = "Mirrors an image along the x,y and/or z axis.\n"
                   "** Input ports:\n"
                   "0: A scalar 2D or 3D multichannel data set\n"
                   "**Output ports:\n"
                   "1: A scalar 2D or 3D multichannel data set\n"
                   "**Parameters:\n"
                   "Mirror X: Mirror on X axis\n"
									 "Mirror Y: Mirror on Y axis\n"
									 "Mirror Z: Mirror on Z axis";

  parameters.initBool( "Mirror X", false );
	parameters.initBool( "Mirror Y", false );
	parameters.initBool( "Mirror Z", false );

  inputsVec[0].portType = IOOther;
	outputsVec[0].portType = IOOther;
}  

CDataMirror::~CDataMirror() throw()
{
}

/* Other methods */

void CDataMirror::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
	if ( dynamic_cast<TImage*>( getInput().get() ) )
		mirrorImage();
	else if ( dynamic_cast<TField2D*>( getInput().get() ) )
		mirrorField2D();
	else if ( dynamic_cast<TField3D*>( getInput().get() ) )
		mirrorField3D();
	else 
	{
		alog << LWARN << SERROR("No valid input") << endl; return;
	}	
  
	PROG_RESET();	
BENCHSTOP;
}
   
CPipelineItem* CDataMirror::newInstance( ulong ulID ) const throw()
{
  return new CDataMirror( ulID );
}

void CDataMirror::mirrorImage() throw()
{
	TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
  if ( inputPtr.get() == NULL || inputPtr->getDimension() < 2 || inputPtr->getDimension() > 3 )
  {
    alog << LWARN << SERROR("Input type is no 2D or 3D image!") << endl;
    return;
  }

	bModuleReady = true;
  deleteOldOutput();
	
	std::vector<size_t> dimensionSize = inputPtr->getExtents();
	if ( dimensionSize.size() == 2 )
		dimensionSize.push_back( 1 );
  TImagePtr outputPtr ( new TImage( inputPtr->getDimension(),
    inputPtr->getExtents(), inputPtr->getDataDimension() ) );
  outputPtr->setMaximum( inputPtr->getMaximum() );
  outputPtr->setMinimum( inputPtr->getMinimum() );
	
	uint ax, ay, az;
	PROG_MAX( dimensionSize[2]-1 );
	bool bMirrorX = parameters.getBool( "Mirror X" );
	bool bMirrorY = parameters.getBool( "Mirror Y" );
	bool bMirrorZ = parameters.getBool( "Mirror Z" );
	
  for ( uint z = 0; z < dimensionSize[2]; ++z )
	{
		APP_PROC();
		PROG_VAL( z );
    for ( uint y = 0; y < dimensionSize[1]; ++y )
		  for ( uint x = 0; x < dimensionSize[0]; ++x )
	    {
  	    if ( bMirrorX )
    	    ax = dimensionSize[0] - ( x + 1 );
	      else
  	      ax = x;
    	  if ( bMirrorY )
      	  ay = dimensionSize[1] - ( y + 1 );
	      else
  	      ay = y;
    	  if ( bMirrorZ )
      	  az = dimensionSize[2] - ( z + 1 );
	      else
  	      az = z;        
	      for ( uint i = 0; i < inputPtr->getDataDimension(); ++i )
  	     	(*outputPtr)( ax, ay, az, i ) = (*inputPtr)( x, y, z, i );
    	}
	}
  setOutput( outputPtr );
}

void CDataMirror::mirrorField2D() throw()
{
	TField2DPtr inputPtr = static_pointer_cast<TField2D>( getInput() );
  if ( inputPtr.get() == NULL || inputPtr->getDimension() < 2 || inputPtr->getDimension() > 3 )
  {
    alog << LWARN << SERROR("Input type is no 2D field!") << endl;
    return;
  }

	bModuleReady = true;
  deleteOldOutput();
	
	std::vector<size_t> dimensionSize = inputPtr->getExtents();
	if ( dimensionSize.size() == 2 )
		dimensionSize.push_back( 1 );
  TField2DPtr outputPtr ( new TField2D( inputPtr->getDimension(),
    inputPtr->getExtents(), inputPtr->getDataDimension() ) );
  outputPtr->setMaximum( inputPtr->getMaximum() );
  outputPtr->setMinimum( inputPtr->getMinimum() );
	
	uint ax, ay, az;
	PROG_MAX( dimensionSize[2]-1 );
	bool bMirrorX = parameters.getBool( "Mirror X" );
	bool bMirrorY = parameters.getBool( "Mirror Y" );
	bool bMirrorZ = parameters.getBool( "Mirror Z" );
	
  for ( uint z = 0; z < dimensionSize[2]; ++z )
	{
		APP_PROC();
		PROG_VAL( z );
    for ( uint y = 0; y < dimensionSize[1]; ++y )
		  for ( uint x = 0; x < dimensionSize[0]; ++x )
	    {
  	    if ( bMirrorX )
    	    ax = dimensionSize[0] - ( x + 1 );
      	else
        	ax = x;
	      if ( bMirrorY )
  	      ay = dimensionSize[1] - ( y + 1 );
    	  else
      	  ay = y;
	      if ( bMirrorZ )
  	      az = dimensionSize[2] - ( z + 1 );
    	  else
      	  az = z;        
	      for ( uint i = 0; i < inputPtr->getDataDimension(); ++i )
  	     	(*outputPtr)( ax, ay, az, i ) = (*inputPtr)( x, y, z, i );
    	}
	}
  setOutput( outputPtr );
}

void CDataMirror::mirrorField3D() throw()
{
	TField3DPtr inputPtr = static_pointer_cast<TField3D>( getInput() );
  if ( inputPtr.get() == NULL || inputPtr->getDimension() < 2 || inputPtr->getDimension() > 3 )
  {
    alog << LWARN << SERROR("Input type is no 3D field!") << endl;
    return;
  }

	bModuleReady = true;
  deleteOldOutput();
	
	std::vector<size_t> dimensionSize = inputPtr->getExtents();
	if ( dimensionSize.size() == 2 )
		dimensionSize.push_back( 1 );
  TField3DPtr outputPtr ( new TField3D( inputPtr->getDimension(),
    inputPtr->getExtents(), inputPtr->getDataDimension() ) );
  outputPtr->setMaximum( inputPtr->getMaximum() );
  outputPtr->setMinimum( inputPtr->getMinimum() );
	
	uint ax, ay, az;
	PROG_MAX( dimensionSize[2]-1 );
	bool bMirrorX = parameters.getBool( "Mirror X" );
	bool bMirrorY = parameters.getBool( "Mirror Y" );
	bool bMirrorZ = parameters.getBool( "Mirror Z" );
	
  for ( uint z = 0; z < dimensionSize[2]; ++z )
	{
		APP_PROC();
		PROG_VAL( z );
    for ( uint y = 0; y < dimensionSize[1]; ++y )
		  for ( uint x = 0; x < dimensionSize[0]; ++x )
  	  {
    	  if ( bMirrorX )
      	  ax = dimensionSize[0] - ( x + 1 );
	      else
  	      ax = x;
    	  if ( bMirrorY )
      	  ay = dimensionSize[1] - ( y + 1 );
	      else
  	      ay = y;
    	  if ( bMirrorZ )
      	  az = dimensionSize[2] - ( z + 1 );
	      else
  	      az = z;        
    	  for ( uint i = 0; i < inputPtr->getDataDimension(); ++i )
      	 	(*outputPtr)( ax, ay, az, i ) = (*inputPtr)( x, y, z, i );
	    }
	}
  setOutput( outputPtr );
}
