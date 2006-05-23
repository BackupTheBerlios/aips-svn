/************************************************************************
 * File: chistocompressor.cpp                                           *
 * Project: AIPS Histogram filter modules                               *
 * Description: Compresses each channel of the input by erasing unused  *
 *              intensities                                             *
 *                                                                      *
 * Author: Hendrik Belitz <h.belitz@fz-juelich.de>                      *
 *                                                                      *
 * Created: 2004-02-20                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#include "chistocompressor.h"

using namespace std;
using namespace boost;
namespace aips {

/*************
 * Structors *
 *************/

/**
 * \param ulID unique module ID
 */
CHistoCompressor::CHistoCompressor( ulong ulID ) throw()
  : CGrayLevelTransformation ( ulID, "Histogram compression", 1, 1,
  	"CHistoCompressor", CHISTOCOMPRESSOR_VERSION, "CHistogram" )
{
  setModuleID( sLibID );
  
  sDocumentation = "Compresses the histogram of each channel in the given "
									 "image or volume ( All intensities with a histogram"
									 "value of zero will be erased )\n"
                   "** Input ports:\n"
                   "1: A scalar multidimensional, multichannel data set\n"
                   "**Output ports:\n"
                   "1: A scalar multidimensional, multichannel data set";
}

CHistoCompressor::~CHistoCompressor() throw()
{
}

/*****************
 * Other methods *
 *****************/

DEFINE_CALL_MACRO( CHistoCompressor::call, CHistoCompressor::compute, intimageTL );

void CHistoCompressor::apply () throw()
{
FBEGIN;
BENCHSTART;
	bModuleReady = false;
  if ( getInput() )
		call<Length<intimageTL>::value - 1>();
	else
  {
    alog << LWARN << SERROR("Input type is no 2D or 3D integer image!") << endl;
    return;
  }	
BENCHSTOP;
FEND;
}

template <typename ImageType>
bool CHistoCompressor::compute() throw()
{
	boost::shared_ptr<ImageType> theInputSPtr = static_pointer_cast<ImageType>( getInput() );
  if ( !checkInput<ImageType>( theInputSPtr, 2, 3 ) )
  {
    return false;
  }
	bModuleReady = true;
  deleteOldOutput();

  boost::shared_ptr<ImageType> outputPtr ( new ImageType( theInputSPtr->getDimension(),
		theInputSPtr->getExtents(), theInputSPtr->getDataDimension() ) );
  outputPtr->setMinimum( theInputSPtr->getDataRange().getMinimum() );
  outputPtr->setMaximum( 0 );

  calculateHistogram( theInputSPtr );
  typename ImageType::TDataType theMaxIntensity = ( theInputSPtr->getDataRange().getMaximum() );
  usNewGrayValuesVec.resize( theMaxIntensity + 1 );
  
  for ( ushort usChannel = 0; usChannel < theInputSPtr->getDataDimension(); usChannel++ )
  {
		ushort usCnt = 0;
    for ( long i = 0; i <= theMaxIntensity; i++ )
    {
			if ( theHistogramVec[usChannel][i] > numeric_limits<double>::epsilon() )
			{
				usNewGrayValuesVec[i] = usCnt;
				usCnt++;
			}      
			else
			{
				usNewGrayValuesVec[i] = usCnt;
			}
    }
    outputPtr->adjustDataRange( usCnt );
    applyNewGrayValues( theInputSPtr, outputPtr, usChannel );
  }
  setOutput( outputPtr );
  return true;
}

}
