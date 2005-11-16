/************************************************************************
 * File: cmorphology.cpp                                                *
 * Project: AIPS neighbourhood filters plugin library                   * 
 * Description: A class for simple morphological operations             *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 11.02.04                                                    *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "cmorphology.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/
 
CMorphology::CMorphology( ulong ulID ) throw()
: CFilter ( ulID, "Simple morphology operator", 2, 1, "CMorphology", "0.1", "CFilter" )
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

  parameters.initBool( "Type", false );
  parameters.initUnsignedLong( "Iterations", 1UL, 1UL, 1000UL );
  parameters.initUnsignedLong( "Kernel height", 1UL, 1UL, 255UL );
	parameters.initBool( "Only background", false );

  inputsVec[0].portType = CPipelineItem::IOInteger;
	inputsVec[1].portType = CPipelineItem::IOInteger;
  outputsVec[0].portType = CPipelineItem::IOInteger;
}

CMorphology::~CMorphology() throw()
{
}

/*****************
 * Other methods *
 *****************/
 
void CMorphology::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
  if ( inputPtr.get() == NULL )
  {
    alog << LWARN << SERROR("Input type is no image!") << endl;
    return;
  }

  size_t kernelSize[] = { 3, 3, 3 };
  TImage kernel( inputPtr->getDimension(), kernelSize );
	kernel = parameters.getUnsignedLong( "Kernel height" );
	kernel(0,0,0) = 0;
	kernel(2,0,0) = 0;
	kernel(0,2,0) = 0;
	kernel(2,2,0) = 0;	
	kernel(0,0,1) = 0;
	kernel(2,0,1) = 0;
	kernel(0,2,1) = 0;
	kernel(2,2,1) = 0;
	kernel(0,0,2) = 0;
	kernel(2,0,2) = 0;
	kernel(0,2,2) = 0;
	kernel(2,2,2) = 0;
	
	
	if ( inputPtr->getDimension() == 2 )
		morph2D( *inputPtr, kernel );
	else if ( inputPtr->getDimension() == 3 )
		morph3D( *inputPtr, kernel );
	else
  {
    alog << LWARN << SERROR("Input type is no 2D or 3D image!") << endl;
    return;
  }
  
BENCHSTOP;	
}

CPipelineItem* CMorphology::newInstance( ulong ulID ) const throw()
{
  return new CMorphology( ulID );
}

/*******************
 * Private methods *
 *******************/

void CMorphology::morph2D( const TImage& input, const TImage& kernel ) throw()
{
	bModuleReady = true;
  deleteOldOutput();

  TImagePtr outputPtr ( new TImage( input.getDimension(), input.getExtents(),
		input.getDataDimension() ) );
  TImage work( input );
	
	TImagePtr roiPtr = static_pointer_cast<TImage>( getInput( 1 ) );
	TImage valid ( input );
	bool roiSelf = false;
	if ( roiPtr == NULL )
		roiSelf = true;
  
	bool bSearchFor = parameters.getBool( "Type" );
	ulong ulMaxIterations = parameters.getUnsignedLong( "Iterations" );	
  
	PROG_MAX( ulMaxIterations );
	bool bOnlyBG = parameters.getBool( "Only background" );
  for ( ushort usIterations = 0; usIterations < ulMaxIterations;	++usIterations )
  {	
		if( bOnlyBG )
		{
			for ( ushort y = 1; y < work.getExtent( 1 )-1; ++y )
   	    for ( ushort x = 1; x < work.getExtent( 0 )-1; ++x )
					{
						ushort usNeighbors = 0;
						if ( work(x-1,y) == 0 ) usNeighbors++;
						if ( work(x+1,y) == 0 ) usNeighbors++;
						if ( work(x,y-1) == 0 ) usNeighbors++;
						if ( work(x,y+1) == 0 ) usNeighbors++;
						if ( usNeighbors > 0 )
							valid(x,y) = 1;
						else
							valid(x,y) = 0;
					}		
		}
			
    (*outputPtr) = work;    
		PROG_VAL( ( usIterations + 1 ) );
		APP_PROC();  
	  for ( ushort y = 0; y < work.getExtent( 1 ); ++y )
      for ( ushort x = 0; x < work.getExtent( 0 ); ++x )
      {
				if ( ( !bOnlyBG || valid(x,y) ) && ( roiSelf || (*roiPtr)(x,y)>0 ) )
				{
          if ( bSearchFor )
 	        {
 	  	      for ( short t = -1; t < 2; ++t )
  	  	      for ( short s = -1; s < 2; ++s )
        	      if ( static_cast<short>(x) - s >= 0
          	      && static_cast<short>(y) - t >= 0
            	    && static_cast<short>(x) - s < static_cast<short>(work.getExtent(0))
              	  && static_cast<short>(y) - t < static_cast<short>(work.getExtent(1)) )
                	{
	                  long lValue = work( x - s, y - t ) + kernel( s + 1, t + 1 );
										if ( lValue > 65535 ) lValue = 65535;
										outputPtr->adjustDataRange( lValue );
      	            if ( lValue > (*outputPtr)( x, y ) )
        		           (*outputPtr)( x, y ) = static_cast<ushort>( lValue );
            	    }
         	}
	        else 
  	      {
    	      for ( short t = -1; t < 2; ++t )
      	      for ( short s = -1; s < 2; ++s )
        	      if ( static_cast<short>(x) + s >= 0
          	      && static_cast<short>(y) + t >= 0
            	    && static_cast<short>(x) + s < static_cast<short>(work.getExtent(0))
              	  && static_cast<short>(y) + t < static_cast<short>(work.getExtent(1)) )
                	{
                  	long lValue = work( x + s, y + t ) - kernel( s + 1, t + 1 );
										if ( lValue < 0 ) lValue = 0;
										else if ( lValue > 65535 ) lValue = 65535;
										outputPtr->adjustDataRange( lValue );
      	            if ( lValue < (*outputPtr)( x, y ) )
        		           (*outputPtr)( x, y ) = static_cast<ushort>( lValue );
      	          }
       	  } // ELSE
				} // Valid
      } // FOR
    work = (*outputPtr);
  }
  setOutput( outputPtr );
	PROG_RESET();

}

void CMorphology::morph3D( const TImage& input, const TImage& kernel ) throw()
{
	bModuleReady = true;
  deleteOldOutput();

	TImagePtr outputPtr ( new TImage( input.getDimension(), input.getExtents(), 1 ) );
  TImage work( input );
	
	TImagePtr roiPtr = static_pointer_cast<TImage>( getInput( 1 ) );
	TImage valid ( input );
	bool roiSelf = false;
	if ( roiPtr == NULL )
		roiSelf = true;
  
	bool bSearchFor = parameters.getBool( "Type" );
	ulong ulMaxIterations = parameters.getUnsignedLong( "Iterations" );	
	PROG_MAX( input.getExtent( 2 ) * ulMaxIterations );
	bool bOnlyBG = parameters.getBool( "Only background" );
  for ( ushort usIterations = 0; usIterations < ulMaxIterations;	++usIterations )
  {	
		if( bOnlyBG )
		{
			for ( ushort z = 1; z < work.getExtent( 2 )-1; ++z )
				for ( ushort y = 1; y < work.getExtent( 1 )-1; ++y )
   	    	for ( ushort x = 1; x < work.getExtent( 0 )-1; ++x )
					{
						ushort usNeighbors = 0;
						if ( work(x-1,y,z) == 0 ) usNeighbors++;
						if ( work(x+1,y,z) == 0 ) usNeighbors++;
						if ( work(x,y-1,z) == 0 ) usNeighbors++;
						if ( work(x,y+1,z) == 0 ) usNeighbors++;
						if ( work(x,y,z-1) == 0 ) usNeighbors++;
						if ( work(x,y,z+1) == 0 ) usNeighbors++;
						if ( usNeighbors > 0 )
							valid(x,y,z) = 1;
						else
							valid(x,y,z) = 0;
					}			
		}
			
    (*outputPtr) = work;    
    for ( ushort z = 1; z < input.getExtent( 2 )-1; ++z )
		{
			PROG_VAL( ( usIterations + 1 ) * z );
			APP_PROC();
      for ( ushort y = 1; y < work.getExtent( 1 )-1; ++y )
        for ( ushort x = 1; x < work.getExtent( 0 )-1; ++x )
        {
					if ( ( !bOnlyBG || valid(x,y,z) ) && ( roiSelf || (*roiPtr)(x,y,z)>0 ) )
					{
	          if ( bSearchFor )
  	        {
							for ( short u = -1; u < 2; ++u )
  	  	      for ( short t = -1; t < 2; ++t )
  	  	      for ( short s = -1; s < 2; ++s )
        	      if ( static_cast<short>(x) - s >= 0
          	      && static_cast<short>(y) - t >= 0
            	    && static_cast<short>(x) - s < static_cast<short>(work.getExtent(0))
              	  && static_cast<short>(y) - t < static_cast<short>(work.getExtent(1)) )
                	{
	                  long lValue = work( x - s, y - t, z - u ) + kernel( s + 1, t + 1, u + 1 );
										if ( lValue > 65535 ) lValue = 65535;
										outputPtr->adjustDataRange( lValue );
      	            if ( lValue > (*outputPtr)( x, y, z ) )
        		           (*outputPtr)( x, y, z ) = static_cast<ushort>( lValue );
            	    }
          	}
	          else 
  	        {
							for ( short u = -1; u < 2; ++u )
    	      	for ( short t = -1; t < 2; ++t )
      	      for ( short s = -1; s < 2; ++s )
        	      if ( static_cast<short>(x) + s >= 0
          	      && static_cast<short>(y) + t >= 0
            	    && static_cast<short>(x) + s < static_cast<short>(work.getExtent(0))
              	  && static_cast<short>(y) + t < static_cast<short>(work.getExtent(1)) )
                	{
                  	long lValue = work( x + s, y + t, z + u ) - kernel( s + 1, t + 1, u + 1 );
										if ( lValue < 0 ) lValue = 0;
										else if ( lValue > 65535 ) lValue = 65535;
										outputPtr->adjustDataRange( lValue );
      	            if ( lValue > (*outputPtr)( x, y, z ) )
        		           (*outputPtr)( x, y, z ) = static_cast<ushort>( lValue );
      	          }
        	  }
					}
        }
		}
    work = (*outputPtr);
  }
  setOutput( outputPtr );
	PROG_RESET();
}
