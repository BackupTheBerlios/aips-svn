/***************************************************************************
 *   Copyright (C) 2004 by Hendrik Belitz                                  *
 *   h.belitz@fz-juelich.de                                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "cnoiseestimator.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

CNoiseEstimator::CNoiseEstimator( ulong ulID ) throw()
   : CFilter ( ulID, "Noise estimation", 1, 1, "CNoiseEstimator","0.1","CFilter" )
{
	parameters.initUnsignedLong("Radius",15,0,100);
  setModuleID( sLibID );
}

CNoiseEstimator::~CNoiseEstimator() throw()
{
}

/***************** 
 * Other methods *
 *****************/

void CNoiseEstimator::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
  if ( inputPtr.get() == NULL )
  {
    alog << LWARN << "Input type is no 2D/3D image!" << endl;
    return;
  }
	bModuleReady = true;
  deleteOldOutput();	

  uint radius = parameters.getUnsignedLong("Radius");
  
  TImagePtr outputPtr ( new TImage( *inputPtr ) );

  double mean = 0.0;
  double variance = 0.0;
  vector<short> values;
	for( uint y = 0; y < radius; ++y )
		for( uint x = 0; x < radius; ++x )
		{
			values.push_back( (*inputPtr)(x,y) );
		}

  for ( vector<short>::iterator it = values.begin(); it != values.end(); ++it )
  	mean += static_cast<double>( *it );
  mean /= static_cast<double>( values.size() );
  for ( vector<short>::iterator it = values.begin(); it != values.end(); ++it )
  	variance += (static_cast<double>(*it) - mean)*(static_cast<double>(*it) - mean);
  variance /= static_cast<double>( values.size() );
  variance = sqrt( variance );

  cerr << "Absolute: Mean " << mean << " Variance " << variance*1.7 << endl;
  cerr << "Relative: Mean " << mean/static_cast<double>(inputPtr->getMaximum())
  	<< " Variance " << variance*1.7/static_cast<double>(inputPtr->getMaximum()) << endl;
  setOutput( outputPtr );
BENCHSTOP;
}

CPipelineItem* CNoiseEstimator::newInstance( ulong ulID ) const throw()
{
  return new CNoiseEstimator( ulID );
}



