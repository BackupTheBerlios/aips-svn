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
#include "cbinarymorphology.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/
 
CBinaryMorphology::CBinaryMorphology( ulong ulID ) throw()
: CFilter ( ulID, "Binary morphology operator", 2, 1, "CBinaryMorphology", "0.1", "CFilter" )
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
	parameters.initUnsignedLong( "What", 1UL, -32767UL, 32767UL );
	parameters.initUnsignedLong( "From", 0UL, -32767UL, 32767UL );

  inputsVec[0].portType = CPipelineItem::IOInteger;
	inputsVec[1].portType = CPipelineItem::IOInteger;
  outputsVec[0].portType = CPipelineItem::IOInteger;
}

CBinaryMorphology::~CBinaryMorphology() throw()
{
}

/*****************
 * Other methods *
 *****************/
 
void CBinaryMorphology::apply() throw()
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

CPipelineItem* CBinaryMorphology::newInstance( ulong ulID ) const throw()
{
  return new CBinaryMorphology( ulID );
}

/*******************
 * Private methods *
 *******************/

void CBinaryMorphology::morph2D( const TImage& input, const TImage& kernel ) throw()
{
	bModuleReady = true;
  deleteOldOutput();

  TImagePtr outputPtr ( new TImage( input ) );
	outputPtr->getDataRange().setMaximum( input.getDataRange().getMaximum() );
	(*outputPtr) = 0;
  TImage work( input );
	
	TImagePtr roiPtr = static_pointer_cast<TImage>( getInput( 1 ) );
	TImage valid ( input );
	bool roiSelf = false;
	if ( roiPtr.get() == NULL )
		roiSelf = true;
  
	bool bSearchFor = parameters.getBool( "Type" );
	ulong ulMaxIterations = parameters.getUnsignedLong( "Iterations" );	
	ulong what = parameters.getUnsignedLong( "What" );	
	ulong from = parameters.getUnsignedLong( "From" );	
  
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
			
   // (*outputPtr) = work;    
		PROG_VAL( ( usIterations + 1 ) );
		APP_PROC();  
	  for ( ushort y = 0; y < outputPtr->getExtent( 1 ); ++y )
      for ( ushort x = 0; x < outputPtr->getExtent( 0 ); ++x )
      {
				if ( ( !bOnlyBG || valid(x,y) ) && ( roiSelf || (*roiPtr)(x,y)>0 ) )
				{
          if ( bSearchFor )
 	        {
						(*outputPtr)( x, y ) = 0;
						bool bValue = false;
 	  	      for ( short t = -1; t < 2; ++t )
  	  	      for ( short s = -1; s < 2; ++s )
        	      if ( static_cast<short>(x) - s >= 0
          	      && static_cast<short>(y) - t >= 0
            	    && static_cast<short>(x) - s < static_cast<short>(work.getExtent(0))
              	  && static_cast<short>(y) - t < static_cast<short>(work.getExtent(1)) )
                	{
	                  if ( work( x - s, y - t ) == 1 ) 
											bValue = true;
            	    }
						if ( bValue )
							(*outputPtr)( x, y ) = 1;
         	}
	        else 
  	      {
						(*outputPtr)( x, y ) = work( x, y );
						bool bValue = false;
    	      for ( short t = -1; t < 2; ++t )
      	      for ( short s = -1; s < 2; ++s )
        	      if ( static_cast<short>(x) + s >= 0
          	      && static_cast<short>(y) + t >= 0
            	    && static_cast<short>(x) + s < static_cast<short>(work.getExtent(0))
              	  && static_cast<short>(y) + t < static_cast<short>(work.getExtent(1)) )
                	{
	                  if ( work( x + s, y + t ) == from && work( x,y ) == what ) 
											bValue = true;
      	          }
						if ( bValue )
							(*outputPtr)( x, y ) = 0;
       	  } // ELSE
				} // Valid
      } // FOR
    work = (*outputPtr);
  }
  setOutput( outputPtr );
	PROG_RESET();
}

void CBinaryMorphology::morph3D( const TImage& input, const TImage& kernel ) throw()
{
	bModuleReady = true;
  deleteOldOutput();

	TImagePtr outputPtr ( new TImage( input.getDimension(), input.getExtents(), 1 ) );
  TImage work( input );
	
	TImagePtr roiPtr = static_pointer_cast<TImage>( getInput( 1 ) );
	TImage valid ( input );
	bool roiSelf = false;
	if ( roiPtr.get() == NULL )
		roiSelf = true;
  DS("A");
	bool bSearchFor = parameters.getBool( "Type" );
	ulong ulMaxIterations = parameters.getUnsignedLong( "Iterations" );	
	PROG_MAX( input.getExtent( 2 ) * ulMaxIterations );
	bool bOnlyBG = parameters.getBool( "Only background" );
	DS("B");
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
				/*		if ( work(x,y,z-1) == 0 ) usNeighbors++;
						if ( work(x,y,z+1) == 0 ) usNeighbors++;
				*/		if ( usNeighbors > 0 )
							valid(x,y,z) = 1;
						else
							valid(x,y,z) = 0;
					}			
		}
		DS("C");
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
					//		for ( short u = -1; u < 2; ++u )
  	  	      for ( short t = -1; t < 2; ++t )
  	  	      for ( short s = -1; s < 2; ++s )
        	      if ( static_cast<short>(x) - s >= 0
          	      && static_cast<short>(y) - t >= 0
						//			&& static_cast<short>(z) - u >= 0
            	    && static_cast<short>(x) - s < static_cast<short>(work.getExtent(0))
              	  && static_cast<short>(y) - t < static_cast<short>(work.getExtent(1))
							//		&& static_cast<short>(z) - u < static_cast<short>(work.getExtent(2))
									 )
                	{
	                  long lValue = work( x - s, y - t, z ) + kernel( s + 1, t + 1, 0 );
										if ( lValue > 32767 ) lValue = 32767;
										outputPtr->adjustDataRange( lValue );
      	            if ( lValue > (*outputPtr)( x, y, z ) )
        		           (*outputPtr)( x, y, z ) = static_cast<ushort>( lValue );
            	    }
          	}
	          else 
  	        {
							//for ( short u = -1; u < 2; ++u )
							bool bValue = false;
    	      	for ( short t = -1; t < 2; ++t )
      	      for ( short s = -1; s < 2; ++s )
        	      if ( static_cast<short>(x) + s >= 0
          	      && static_cast<short>(y) + t >= 0
									//&& static_cast<short>(z) + u >= 0
            	    && static_cast<short>(x) + s < static_cast<short>(work.getExtent(0))
              	  && static_cast<short>(y) + t < static_cast<short>(work.getExtent(1)) 
									//&& static_cast<short>(z) + u < static_cast<short>(work.getExtent(2)) 
									)
                	{										
										if ( work( x + s, y + t, z ) == 0 ) 
											bValue = true;       		          
      	          }
							if (!bValue) (*outputPtr)( x, y, z ) = 1;
							else (*outputPtr)( x, y, z ) = 0;
        	  }
					}
        }
		}
		DS("D");
    work = (*outputPtr);
  }
	DS("E");
  setOutput( outputPtr );
	PROG_RESET();
}
