/************************************************************************
 * File: $FILE                                                          *
 * Project: AIPS                                                        *
 * Description:                                                         *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: $DATE                                                       *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#include "cwagenknechthomogenity.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

/**
 * \param ulID unique module ID
 */
CWagenknechtHomogenity::CWagenknechtHomogenity( ulong ulID ) throw()
  : CFilter ( ulID, "WG Homogenity filter", 1, 1, "CWagenknechtHomogenity", "0.4", "CFilter" )
{
  setModuleID( sLibID );

  sDocumentation = "Filters the input image with a median filter\n"
                   "** Input ports:\n"
                   "0: A scalar 2D or 3D multichannel data set\n"
                   "**Output ports:\n"
                   "1: A scalar 2D or 3D multichannel data set\n"
                   "**Parameters:\n"
                   "Radius: radius of filter kernel";
									 
	parameters.initUnsignedLong( "Radius", 3, 1, 100 );
	inputsVec[0].portType = IO2DInteger;
}

CWagenknechtHomogenity::~CWagenknechtHomogenity() throw()
{
}

/*****************
 * Other methods *
 *****************/
 
void CWagenknechtHomogenity::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
  if ( inputPtr.get() == NULL || inputPtr->getDimension() < 2 || inputPtr->getDimension() > 3 )
  {
    alog << LWARN << "Input type is no 2D or 3D image!" << endl;
    return;
  }
	bModuleReady = true;
  deleteOldOutput();

  size_t dimensionSize[3];
  dimensionSize[0] = inputPtr->getExtent(0);
  dimensionSize[1] = inputPtr->getExtent(1);
  dimensionSize[2] = ( inputPtr->getDimension() == 3 ) ? inputPtr->getExtent(2) : 1;
  
  TImagePtr outputPtr ( new TImage( inputPtr->getDimension(),
    inputPtr->getExtents(), inputPtr->getDataDimension() ) );
  (*outputPtr) = 65535;
  outputPtr->setMaximum( 0 );
  outputPtr->setMinimum( 0 );

	ulong ulRadius = parameters.getUnsignedLong( "Radius" );	
	if ( dimensionSize[2] < (2*ulRadius)+1 )
	{
		ushort z = 0;
		if ( dimensionSize[2] < 2 ) 
			z = 0;
		else 
			z = dimensionSize[2]/2;
		for( ushort y = ulRadius; y < inputPtr->getExtent(1)-ulRadius; y++ )
			for( ushort x = ulRadius; x < inputPtr->getExtent(0)-ulRadius; x++ )
			{
				ushort inputVal = (*inputPtr)(x,y,z);			
				long value = 0;
				if ( inputVal == 0 ) value = 65535;
				long xv = 0;
				long yv = 0;
				for( short i = -static_cast<short>( ulRadius ); i < static_cast<short>( ulRadius ); i++ )
				{
					xv+= ((*inputPtr)(x+i,y,z)-inputVal)*((*inputPtr)(x+i,y,z)-inputVal);
					yv+= ((*inputPtr)(x,y+i,z)-inputVal)*((*inputPtr)(x,y+i,z)-inputVal);
				}
				if ( value == 0 ) 
				{ 
					value = ( xv+yv ) / 255;	
					if ( value > outputPtr->getMaximum() )
						outputPtr->setMaximum( static_cast<ushort>( value ) ); 
				}
				(*outputPtr)(x,y,z) = static_cast<ushort>( value );
			}
	}
	else
	{
		ulong factor = (((2*ulRadius)+1)*inputPtr->getMaximum());
		for( ushort z = ulRadius; z < inputPtr->getExtent(2)-ulRadius; z++ )
			for( ushort y = ulRadius; y < inputPtr->getExtent(1)-ulRadius; y++ )
				for( ushort x = ulRadius; x < inputPtr->getExtent(0)-ulRadius; x++ )
		{
			ushort inputVal = (*inputPtr)(x,y,z);			
			long value = 0;
 			if ( inputVal == 0 ) value = 65535;
			long xv = 0;
			long yv = 0;
			long zv = 0;
			for( short i = -static_cast<short>( ulRadius ); i < static_cast<short>( ulRadius ); i++ )
			{
				xv += ((*inputPtr)(x+i,y,z)-inputVal)*((*inputPtr)(x+i,y,z)-inputVal);
				yv += ((*inputPtr)(x,y+i,z)-inputVal)*((*inputPtr)(x,y+i,z)-inputVal);
				zv += ((*inputPtr)(x,y,z+i)-inputVal)*((*inputPtr)(x,y,z+i)-inputVal);
			}
			if ( value == 0 ) 
			{ 
				value = ( xv+yv+zv ) / factor;	
				if ( value > outputPtr->getMaximum() )
				{
					outputPtr->setMaximum( static_cast<ushort>( value ) ); 
					DS("New Max " << value );
				}
			}
			(*outputPtr)(x,y,z) = static_cast<ushort>( value );
		}
	}
 	TImage::iterator oit = outputPtr->begin();
	while( oit != outputPtr->end() )
	{
		if ( (*oit) > outputPtr->getMaximum() )
			*oit = outputPtr->getMaximum();
		++oit;
	}
  setOutput( outputPtr );
BENCHSTOP;
}

CPipelineItem* CWagenknechtHomogenity::newInstance( ulong ulID ) const throw()
{
  return new CWagenknechtHomogenity( ulID );
}
