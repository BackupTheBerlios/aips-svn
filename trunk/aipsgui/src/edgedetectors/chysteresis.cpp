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
#include "chysteresis.h"

using namespace std;
using namespace boost;

/* Structors */

/**
 * \param ulID unique module ID
 */
CHysteresis::CHysteresis( ulong ulID ) throw()
: CFilter( ulID, "Hysteresis thresholding", 2, 1, "CHysteresis", "0.1", "CFilter" ) 
{
  setModuleID( sLibID );
  
  sDocumentation = "Creates a gradient field from the input image via a sobel filter\n"
                   "** Input ports:\n"
                   "0: A scalar single channel 2D or 3D data set (image data, required)\n"
									 "(region of interest, optional)\n"
                   "** Output ports:\n"
                   "0: A scalar single channel 2D or 3D data set";

	parameters.initUnsignedLong( "Low", 40, 0, 65535 );
	parameters.initUnsignedLong( "High", 120, 0, 65535 );
  inputsVec[0].portType = IOInteger;
  outputsVec[0].portType = IOInteger;
}

CHysteresis::~CHysteresis() throw()
{
}

/* Other methods */

void CHysteresis::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
	if ( getInput()->getType() != typeid( TImage::TDataType ) )
		return;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
  if ( checkInput<TImage>( inputPtr, 2, 3 ) )
  {
    alog << LWARN << "Input type is no 2D or 3D image!" << endl;
    //return;
  }
	bModuleReady = true;	
  deleteOldOutput();	
	TImagePtr outputPtr ( 
		new TImage( inputPtr->getDimension(), inputPtr->getExtents(), inputPtr->getDataDimension() ) );
	outputPtr->setDataRange( inputPtr->getDataRange() );
	(*outputPtr) = 0;
	long lot = parameters.getUnsignedLong( "Low" );
	long hit = parameters.getUnsignedLong( "High" );
  if ( inputPtr->getDimension() == 2 )
  {
		for( ushort y = 1; y < outputPtr->getExtent(1); ++y )
			for( ushort x = 1; x < outputPtr->getExtent(0); ++x )
			{
				if ( (*inputPtr)(x,y) > hit )
					(*outputPtr)(x,y) = (*inputPtr)(x,y);
				else if ( (*inputPtr)(x,y) >= lot )
				{
					ushort nb = 0;
					if ( (*inputPtr)(x+1,y) > hit ) nb++;
					if ( (*inputPtr)(x-1,y) > hit ) nb++;
					if ( (*inputPtr)(x,y+1) > hit ) nb++;
					if ( (*inputPtr)(x,y-1) > hit ) nb++;
					if ( (*inputPtr)(x+1,y+1) > hit ) nb++;
					if ( (*inputPtr)(x+1,y-1) > hit ) nb++;
					if ( (*inputPtr)(x-1,y+1) > hit ) nb++;
					if ( (*inputPtr)(x-1,y-1) > hit ) nb++;
					if ( nb )
						(*outputPtr)(x,y) = (*inputPtr)(x,y);
				}
			}
		APP_PROC();	
	}
	else
  {
  	for( ushort z = 1; z < outputPtr->getExtent(2)-1; ++z )
			for( ushort y = 1; y < outputPtr->getExtent(1)-1; ++y )
				for( ushort x = 1; x < outputPtr->getExtent(0)-1; ++x )
				{
					if ( (*inputPtr)(x,y,z) > hit )
						(*outputPtr)(x,y,z) = (*inputPtr)(x,y,z);
					else if ( (*inputPtr)(x,y,z) >= lot )
					{
						ushort nb = 0;
						if ( (*inputPtr)(x+1,y,z) > hit ) nb++;
						if ( (*inputPtr)(x-1,y,z) > hit ) nb++;
						if ( (*inputPtr)(x,y+1,z) > hit ) nb++;
						if ( (*inputPtr)(x,y-1,z) > hit ) nb++;
						if ( (*inputPtr)(x+1,y+1,z) > hit ) nb++;
						if ( (*inputPtr)(x+1,y-1,z) > hit ) nb++;
						if ( (*inputPtr)(x-1,y+1,z) > hit ) nb++;
						if ( (*inputPtr)(x-1,y-1,z) > hit ) nb++;
						
						if ( (*inputPtr)(x,y,z-1) > hit ) nb++;
						if ( (*inputPtr)(x+1,y,z-1) > hit ) nb++;
						if ( (*inputPtr)(x-1,y,z-1) > hit ) nb++;
						if ( (*inputPtr)(x,y+1,z-1) > hit ) nb++;
						if ( (*inputPtr)(x,y-1,z-1) > hit ) nb++;
						if ( (*inputPtr)(x+1,y+1,z-1) > hit ) nb++;
						if ( (*inputPtr)(x+1,y-1,z-1) > hit ) nb++;
						if ( (*inputPtr)(x-1,y+1,z-1) > hit ) nb++;
						if ( (*inputPtr)(x-1,y-1,z-1) > hit ) nb++;
						
						if ( (*inputPtr)(x,y,z+1) > hit ) nb++;
						if ( (*inputPtr)(x+1,y,z+1) > hit ) nb++;
						if ( (*inputPtr)(x-1,y,z+1) > hit ) nb++;
						if ( (*inputPtr)(x,y+1,z+1) > hit ) nb++;
						if ( (*inputPtr)(x,y-1,z+1) > hit ) nb++;
						if ( (*inputPtr)(x+1,y+1,z+1) > hit ) nb++;
						if ( (*inputPtr)(x+1,y-1,z+1) > hit ) nb++;
						if ( (*inputPtr)(x-1,y+1,z+1) > hit ) nb++;
						if ( (*inputPtr)(x-1,y-1,z+1) > hit ) nb++;
						
						if ( nb )
							(*outputPtr)(x,y,z) = (*inputPtr)(x,y,z);
				}
			}
		APP_PROC();	
	}
	PROG_RESET();
  setOutput( outputPtr );	
BENCHSTOP;		
}

CPipelineItem* CHysteresis::newInstance( ulong ulID ) const throw()
{
  return new CHysteresis( ulID );
}


