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
using namespace boost::lambda;

namespace aips {

DEFINE_CALL_MACRO( CNoiseEstimator::call, CNoiseEstimator::compute, imageTL );

/*************
 * Structors *
 *************/

CNoiseEstimator::CNoiseEstimator( ulong ulID ) throw()
   : CFilter ( ulID, "Noise estimation", 1, 1, "CNoiseEstimator", CNOISEESTIMATOR_VERSION,
   "CFilter" )
{
  setModuleID( sLibID );
  sDocumentation = "Estimate noise in an image. The noise is assumed to be Gaussian and\n"
  								 "homogeneous, and it is assumed that the +X+Y-Z corner of the image\n"
  								 "should be homogeneous.\n"
                   "** Input ports:\n"
                   "* 1. A 2D or 3D image.\n"
                   "** Parameters:\n"
                   "* 1. Boxsize : Size of image area to estimate noise from.\n"
                   "* 2. Mean : Estimated mean for the tested image area. (Output)\n"
 									 "* 3. Variance : Estimated mean for the tested image area. (Output)\n";

  inputsVec[0].portType = IOInteger;
  outputsVec[0].portType = IOInteger;
	parameters.initUnsignedLong( "Boxsize",15,0,100 );
	parameters.initDouble( "Mean",0.0,-32000.0,32000.0 );
	parameters.initDouble( "Variance",0.0,-32000.0,32000.0 );
}

CNoiseEstimator::~CNoiseEstimator() throw()
{
}

/***************** 
 * Other methods *
 *****************/

void CNoiseEstimator::apply() throw()
{
FBEGIN;
BENCHSTART;
	bModuleReady = false;
	if ( getInput() )
		call<Length<imageTL>::value - 1>();
	else
  {
    alog << LWARN << SERROR("Input type is no 2D or 3D image!") << endl;
    return;
  }		
FEND;
BENCHSTOP;
}

template<typename ImageType>
bool CNoiseEstimator::compute() throw()
{
  shared_ptr<ImageType> theInputSPtr = static_pointer_cast<ImageType>( getInput() );
  if ( !checkInput<ImageType>( theInputSPtr, 2, 3 ) )
  {
    return false;
  }
	bModuleReady = true;
  deleteOldOutput();	

  uint uiBoxSize = parameters.getUnsignedLong( "Boxsize" );
  double dMean = 0.0;
  double dVariance = 0.0;
  vector<typename ImageType::TDataType> valueVec;
cerr << theInputSPtr->getExtent(0) << endl;
cerr << theInputSPtr->getExtent(1) << endl;
cerr << theInputSPtr->getExtent(2) << endl;
  // Suche Startpunkt. Gehe von Superior abw�ts,
  // bis eine Schicht gefunden wird, in der keine Nullen stehen.
  // Selbiges ist danach fr y und x zu tun
  uint uiStartZ = theInputSPtr->getExtent(2)-1;
  for( uint z = theInputSPtr->getExtent( 2 )-1; z > theInputSPtr->getExtent( 2 ) / 2; --z )
		for( uint y = 0; y < theInputSPtr->getExtent( 1 ) / 2; ++y )
			for( uint x = 0; x < theInputSPtr->getExtent( 0 ) / 2; ++x )
				if ( (*theInputSPtr)( x, y, z ) != 0 )
				{
					uiStartZ = z;
					z = 1;
				}
	uint uiStartY = 0; uint uiStartX = 0;
	for( uint y = 0; y < theInputSPtr->getExtent( 1 ) / 2; ++y )
		for( uint x = 0; x < theInputSPtr->getExtent( 0 ) / 2; ++x )
				if ( (*theInputSPtr)( x, y, uiStartZ ) != 0 )
				{
					uiStartY = y;
					uiStartX = x;
					x = theInputSPtr->getExtent(0)-1;
					y = theInputSPtr->getExtent(1)-1;
				}
  for( uint z = uiStartZ; z > uiStartZ - uiBoxSize; --z )
		for( uint y = uiStartY; y < uiStartY + uiBoxSize; ++y )
			for( uint x = uiStartX; x < uiStartX + uiBoxSize; ++x )
		{
			valueVec.push_back( (*theInputSPtr)( x, y, z ) );
		}
	for_each( valueVec.begin(), valueVec.end(),	dMean +=  _1 );
	dMean /= static_cast<double>( valueVec.size() );
	for_each( valueVec.begin(), valueVec.end(),	dVariance += ( _1 - dMean ) * ( _1 - dMean ) );
  dVariance /= static_cast<double>( valueVec.size() );
  dVariance = sqrt( dVariance );

	parameters.setDouble("Mean", dMean);
	parameters.setDouble("Variance", dVariance);

DBG3(  "Absolute: Mean " << dMean << " Variance " << dVariance << endl
<< "Relative: Mean " << dMean/static_cast<double>(theInputSPtr->getDataRange().getMaximum())
<< " Variance " << dVariance/static_cast<double>(theInputSPtr->getDataRange().getMaximum()) );
  
  return true;
}

}
