/************************************************************************
 * File: chausdorffdistance.cpp                                         *
 * Project: AIPS                                                        *
 * Description: Computes the hausdorff distance of two areas            *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 2004-06-07                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#include "chausdorffdistance.h"

using namespace std;
using namespace boost;

/* Structors */

CHausdorffDistance::CHausdorffDistance( ulong ulID ) throw()
: CFilter ( ulID, "Hausdorff Distance", 2, 1 )
{
  setModuleID( sLibID );

  sDocumentation = "Computes the hausdorff distance of two shapes. The images must "
									 "be of the same size and dimensionality. They also must have "
									 "only one channel.\n"
                   "** Input ports:\n"
                   "0: A scalar 2D or 3D singlechannel data set\n"
									 "1: A scalar 2D or 3D singlechannel data set\n"
                   "**Output ports:\n"
                   "0: A single scalar floating point value";

	parameters.initDouble("Result",0,0,1000);

  inputsVec[0].portType = IOInteger;
  inputsVec[1].portType = IOInteger;
	outputsVec[0].portType = IOPoint;
}  

CHausdorffDistance::~CHausdorffDistance() throw()
{
}

/* Other methods */

void CHausdorffDistance::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
  TImagePtr inputPtr1 = static_pointer_cast<TImage>( getInput( 0 ) );
  if ( !inputPtr1 || inputPtr1->getDimension() < 2 || inputPtr1->getDimension() > 3 
		|| inputPtr1->getDataDimension() > 1 )
  {
    alog << LWARN << "Input type is no single-channel 2D or 3D image!" << endl;
    return;
  }

  TImagePtr inputPtr2 = static_pointer_cast<TImage>( getInput( 1 ) );
  if ( !inputPtr2  || inputPtr2->getDimension() < 2 || inputPtr2->getDimension() > 3 
		|| inputPtr2->getDataDimension() > 1 )
  {
    alog << LWARN << "Input type is no single-channel 2D or 3D image!" << endl;
    return;
  }

	deleteOldOutput();
	
	double dHausdorffDistance = 0.0;
	CTypedData<double> work ( inputPtr1->getDimension(), inputPtr1->getExtents() );
	ushort usDimZ = 1;
	if ( inputPtr1->getDimension() > 2 )
		usDimZ = inputPtr1->getExtent(2);
	
	for( ushort z = 0; z < usDimZ; ++z )	
		for( ushort y = 0; y < work.getExtent(1); ++y )
			for( ushort x = 0; x < work.getExtent(0); ++x )
			{
				if ( (*inputPtr1)( x, y, z ) == 0 )
				{
					work( x, y, z ) = 0.0;
					continue;
				}
				work( x, y, z ) = 1000000.0;
				for( ushort w = 0; w < usDimZ; ++w )	
					for( ushort v = 0; v < work.getExtent(1); ++v )
						for( ushort u = 0; u < work.getExtent(0); ++u )
							if ( (*inputPtr2)( u, v, w ) > 0 )
								work( x, y, z ) = std::min( work( x, y, z ), sqrt( 
									static_cast<double>( ( x - u ) * ( x - u ) ) 
									+ static_cast<double>( ( y - v ) * ( y - v ) ) 
									+ static_cast<double>( ( z - w ) * ( z - w ) ) ) );
			}
			
	for( ushort z = 0; z < usDimZ; ++z )	
		for( ushort y = 0; y < work.getExtent(1); ++y )
			for( ushort x = 0; x < work.getExtent(0); ++x )
				if ( work( x, y, z ) > dHausdorffDistance )
					dHausdorffDistance = work( x, y, z );
					
	for( ushort z = 0; z < usDimZ; ++z )		
		for( ushort y = 0; y < work.getExtent(1); ++y )
			for( ushort x = 0; x < work.getExtent(0); ++x )
			{
				if ( (*inputPtr2)( x, y, z ) == 0 )
				{
					work( x, y, z )=0.0;
					continue;
				}
				work( x, y, z ) = 1000000.0;
				for( ushort w = 0; w < usDimZ; ++w )	
					for( ushort v = 0; v < work.getExtent(1); ++v )
						for( ushort u = 0; u < work.getExtent(0); ++u )
							if ( (*inputPtr1)( u, v, w ) > 0 )
								work( x, y, z ) = std::min( work( x, y, z ), sqrt( 
									static_cast<double>( ( x - u ) * ( x - u ) ) 
									+ static_cast<double>( ( y - v ) * ( y - v ) ) 
									+ static_cast<double>( ( z - w ) * ( z - w ) ) ) );
			}	
	
	for( ushort z = 0; z < usDimZ; ++z )		
		for( ushort y = 0; y < work.getExtent(1); ++y )
			for( ushort x = 0; x < work.getExtent(0); ++x )
				if ( work( x, y, z ) > dHausdorffDistance )
					dHausdorffDistance = work( x, y, z );
	parameters.setDouble( "Result", dHausdorffDistance );
	
DS( "*** Hausdorff distance coefficient is: " << dHausdorffDistance );
BENCHSTOP;
}
   
CPipelineItem* CHausdorffDistance::newInstance( ulong ulID ) const throw()
{
  return new CHausdorffDistance( ulID );
}
