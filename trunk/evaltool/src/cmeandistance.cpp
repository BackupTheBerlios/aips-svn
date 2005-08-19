/************************************************************************
 * File: cmeandistance.cpp                                              *
 * Project: AIPS                                                        *
 * Description: Computes the mean distance of two areas                 *
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
#include "cmeandistance.h"

using namespace std;
using namespace boost;

/* Structors */

CMeanDistance::CMeanDistance( ulong ulID ) throw()
: CFilter ( ulID, "Mean Distance", 2, 1 )
{
  setModuleID( sLibID );

	sDocumentation = "Computes the mean distance of two shapes. The images must "
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

CMeanDistance::~CMeanDistance() throw()
{
}

/* Other methods */

void CMeanDistance::apply() throw()
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
  if ( !inputPtr2 || inputPtr2->getDimension() < 2 || inputPtr2->getDimension() > 3 
		|| inputPtr2->getDataDimension() > 1 )
  {
    alog << LWARN << "Input type is no single-channel 2D or 3D image!" << endl;
    return;
  }
	deleteOldOutput();
	double dDistanceSum1 = 0.0; double dDistanceSum2 = 0.0; 
	ulong ulAreaImage1 = 0; 
	ulong ulAreaImage2 = 0;	
		for( ushort y = 0; y < inputPtr1->getExtent(1); ++y )
			for( ushort x = 0; x < inputPtr1->getExtent(0); ++x )
			{
				if ( (*inputPtr1)( x, y ) == 0 )
					continue;
				ulAreaImage1++;			
				double dLocalMinimum = 100000.0;
					for( ushort v = 0; v < inputPtr1->getExtent(1); ++v )
						for( ushort u = 0; u < inputPtr1->getExtent(0); ++u )
							if ( (*inputPtr2)( u, v ) > 0 )
								dLocalMinimum = std::min( dLocalMinimum, sqrt( static_cast<double>( ( x - u ) * ( x - u ) ) 
									+ static_cast<double>( ( y - v ) * ( y - v ) )  ) );
				dDistanceSum1 += dLocalMinimum;
			}
		for( ushort y = 0; y < inputPtr1->getExtent(1); ++y )
			for( ushort x = 0; x < inputPtr1->getExtent(0); ++x )
			{
				if ( (*inputPtr2)( x, y ) == 0 )
					continue;
				ulAreaImage2++;
				double dLocalMinimum = 100000.0;
					for( ushort v = 0; v < inputPtr1->getExtent(1); ++v )
						for( ushort u = 0; u < inputPtr1->getExtent(0); ++u )
							if ( (*inputPtr1)( u, v ) > 0 )
								dLocalMinimum = std::min( dLocalMinimum, sqrt( static_cast<double>( ( x - u ) * ( x - u ) ) 
									+ static_cast<double>( ( y - v ) * ( y - v ) ) ) );
				dDistanceSum2 += dLocalMinimum;
			}
	double dMeanDistance = ( dDistanceSum1 + dDistanceSum2 ) /
		static_cast<double>( ulAreaImage1 + ulAreaImage2 );
	parameters.setDouble( "Result", dMeanDistance );
	
DS( "*** Mean distance coefficient is: " << dMeanDistance );
BENCHSTOP;
}
   
CPipelineItem* CMeanDistance::newInstance( ulong ulID ) const throw()
{
  return new CMeanDistance( ulID );
}
