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
#include "cfillcentralregion.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/
 
CFillCentralRegion::CFillCentralRegion( ulong ulID ) throw()
: CFilter ( ulID, "Extract central region", 1, 1, "CFillCentralRegion", "0.1", "CFilter" )
{
  setModuleID( sLibID );
  sDocumentation = "A simple morphological operator\n"
                   "** Input ports:\n"
                   "0: A singlechannel image\n"
                   "** Output ports:\n"
                   "1: A singlechannel image\n"
                   "** Parameters:\n"
                   "Type: 0 : Dilation, 1 : Erosion\n"
									 "Iterations: Number of filter steps\n"
									 "Kernel height: Strength of structural element\n"
									 "Only Background: Work only on voxels with background neighbourhood\n";

  parameters.initLong( "Type X/Y", 0, 0, 1 );
//   parameters.initUnsignedLong( "Iterations", 1UL, 1UL, 1000UL );
//   parameters.initUnsignedLong( "Kernel height", 1UL, 1UL, 255UL );
// 	parameters.initBool( "Only background", false );

  inputsVec[0].portType = CPipelineItem::IOInteger;
//	inputsVec[1].portType = CPipelineItem::IOInteger;
  outputsVec[0].portType = CPipelineItem::IOInteger;
}

CFillCentralRegion::~CFillCentralRegion() throw()
{
}

/*****************
 * Other methods *
 *****************/
 
void CFillCentralRegion::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
  if ( inputPtr.get() == NULL )
  {
    alog << LWARN << SERROR("Input type is no image!") << endl;
    return;
  }

	bModuleReady = true;
  deleteOldOutput();

  TImagePtr outputPtr ( new TImage( *inputPtr ) );
	outputPtr->setMaximum( inputPtr->getMaximum() + 1 );
	   
	PROG_MAX( 1 );
	bool type = ( parameters.getLong( "Type X/Y" ) == 1 );
	bool state = false; // 0 = Nothing, 1 = Active
	if ( !type )
	{
	for ( ushort y = 0; y < outputPtr->getExtent( 1 ); ++y )
    for ( ushort x = 0; x < outputPtr->getExtent( 0 )-1; ++x )
		{
			if ( x == 0 ) state = false;
			if ( state )
			{
				(*outputPtr)(x,y) = 2;
			}
			if ( (*inputPtr)(x,y) > 0 && (*inputPtr)(x+1,y) == 0 && !state )
			{
				state = true;
			}
			else if ( (*inputPtr)(x,y) == 0 && (*inputPtr)(x+1,y) > 0 && state )
			{
				state = false;
			}
		}
		for ( ushort y = 0; y < outputPtr->getExtent( 1 ); ++y )    	
		{
			if ( (*outputPtr)(outputPtr->getExtent( 0 )-2,y) == 2 ) 
			{
				ushort x = outputPtr->getExtent( 0 )-2;
				while( (*outputPtr)(x,y) != 1 && x > 0 )
				{
					(*outputPtr)(x,y) = 0;
					--x;
				}
			}
		}		
	}
	else
	{
	for ( ushort x = 0; x < outputPtr->getExtent( 0 )-1; ++x )
	for ( ushort y = 0; y < outputPtr->getExtent( 1 )-1; ++y )    
		{
			if ( y == 0 ) state = false;
			if ( state )
			{
				(*outputPtr)(x,y) = 2;
			}
			if ( (*inputPtr)(x,y) > 0 && (*inputPtr)(x,y+1) == 0 && !state )
			{
				state = true;
			}
			else if ( (*inputPtr)(x,y) == 0 && (*inputPtr)(x,y+1) > 0 && state )
			{
				state = false;
			}
		}
		for ( ushort x = 0; x < outputPtr->getExtent( 0 ); ++x )    	
		{
			if ( (*outputPtr)(x,outputPtr->getExtent( 1 )-2) == 2 ) 
			{
				ushort y = outputPtr->getExtent( 1 )-2;
				while( (*outputPtr)(x,y) != 1 && y > 0 )
				{
					(*outputPtr)(x,y) = 0;
					--y;
				}
			}
		}			
	}
  setOutput( outputPtr );
	PROG_RESET();  
BENCHSTOP;	
}

CPipelineItem* CFillCentralRegion::newInstance( ulong ulID ) const throw()
{
  return new CFillCentralRegion( ulID );
}
