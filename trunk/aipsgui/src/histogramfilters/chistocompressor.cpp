/************************************************************************
 * File: chistocompressor.cpp                                           *
 * Project: AIPS Histogram filter modules                               *
 * Description: Compresses each channel of the input by erasing unused  *
 *              intensities                                             *
 *                                                                      *
 * Author: Hendrik Belitz <h.belitz@fz-juelich.de>                      *
 *                                                                      *
 * Created: 04-02-20                                                    * 
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#include "chistocompressor.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

/**
 * \param ulID unique module ID
 */
CHistoCompressor::CHistoCompressor( ulong ulID ) throw()
  : CGrayLevelTransformation ( ulID, "Histogram compression", 1, 1, "CHistoCompressor", "0.2", "CHistogram" )
{
  setModuleID( sLibID );
  
  sDocumentation = "Compresses the histogram of each channel in the given "
									 "image or volume ( All intensities with a histogram"
									 "value of zero will be erased )\n"
                   "** Input ports:\n"
                   "0: A scalar multidimensional, multichannel data set\n"
                   "**Output ports:\n"
                   "1: A scalar multidimensional, multichannel data set";  
}

CHistoCompressor::~CHistoCompressor() throw()
{
}

/*****************
 * Other methods *
 *****************/

void CHistoCompressor::apply () throw()
{
BENCHSTART;
	bModuleReady = false;
  TImagePtr inputPtr = static_pointer_cast<TImage>( getInput() );
  if ( inputPtr.get() == NULL )
  {
    alog << LWARN << "Input type is no image!" << endl;
    return;
  }
	bModuleReady = true;
  deleteOldOutput();

  TImagePtr outputPtr ( new TImage( inputPtr->getDimension(),
		inputPtr->getExtents(), inputPtr->getDataDimension() ) );
  outputPtr->setMinimum( inputPtr->getMinimum() );
  outputPtr->setMaximum( 0 );
  calculateHistogram( inputPtr );
  ushort usMaxIntensity = ( inputPtr->getMaximum() );
  usNewGrayValuesVec.resize( inputPtr->getMaximum() + 1 );
  for ( ushort usChannel = 0; usChannel < inputPtr->getDataDimension(); usChannel++ )
  {
		ushort usCnt = 0;
    for ( ulong i = 0; i <= usMaxIntensity; i++ )
    {
			if ( histogramVec[usChannel][i] > numeric_limits<double>::epsilon() )
			{
				usNewGrayValuesVec[i] = usCnt;
				usCnt++;
			}      
			else
			{
				usNewGrayValuesVec[i] = usCnt;
			}
    }
		if ( usCnt > outputPtr->getMaximum() )
			outputPtr->setMaximum( usCnt );
    applyNewGrayValues( inputPtr, outputPtr, usChannel );
  }
  setOutput( outputPtr );
BENCHSTOP;	
}

CPipelineItem* CHistoCompressor::newInstance( ulong ulID ) const throw()
{
  return new CHistoCompressor( ulID );
}
