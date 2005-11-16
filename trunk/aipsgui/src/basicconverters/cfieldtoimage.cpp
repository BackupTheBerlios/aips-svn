/************************************************************************
 * File: cfieldtoimage.cpp                                              *
 * Project: AIPS - basic converters plugin library                      *
 * Description: Converts a float image into an integer image            *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 2005-13-01                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#include "cfieldtoimage.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

/**
 * \param ulID unique module ID
 */
CFieldToImage::CFieldToImage( ulong ulID ) throw()
  : CConverter ( ulID, 1, 1, "CFieldToImage", "0.1", "CConverter" )
{
  setModuleID( sLibID );
  setModuleName("Float to int");
  
  sDocumentation = "Converts a floating point image into an integer image\n"
									 "** Input ports:\n"
                   " 0: A float multi-channel data set\n"
                   "** Output ports:\n"
                   " 0: A integer multi-channel data set\n"
                   "** parametersVec:\n"
									 " <MinimumValue>: Minimum intensity of output image\n"
                   " <MaximumValue>: Maximum intensity of output image";

  parameters.initUnsignedLong( "MinimumValue", 0, 0, 65535 );
	parameters.initUnsignedLong( "MaximumValue", 255, 0, 65535 );
	inputsVec[0].portType = CPipelineItem::IOOther;
	outputsVec[0].portType = CPipelineItem::IOOther;
}

CFieldToImage::~CFieldToImage() throw()
{
}

/*****************
 * Other methods *
 *****************/

void CFieldToImage::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
	shared_ptr<TField> inputPtr = dynamic_pointer_cast<TField>( getInput() );
  if ( !checkInput<TField>( inputPtr ) )
   	return;
	bModuleReady = true;
  deleteOldOutput();
	// Get parameters
	ulong ulMinVal = parameters.getUnsignedLong( "MinimumValue" );
	ulong ulMaxVal = parameters.getUnsignedLong( "MaximumValue" );
	// Determine minimum and maximum conversion
	double dFactor = static_cast<double>( ulMaxVal - ulMinVal ) 
		/ ( inputPtr->getDataRange().getMaximum() - inputPtr->getDataRange().getMinimum() ); 
	double dInputOffset = inputPtr->getDataRange().getMinimum() * -1.0;
	double dOutputOffset = ulMinVal;
	// Create output field
	TImagePtr outputPtr( new TImage ( inputPtr->getDimension(), inputPtr->getExtents(), 
	 	inputPtr->getDataDimension() ) );

	// Get maximum and minimum of vector field
 	outputPtr->setMaximum( ulMaxVal );
 	outputPtr->setMinimum( ulMinVal );

 	// Create new field
	PROG_MAX( inputPtr->getArraySize() );
  ulong cnt = 0;
  TImage::iterator outputIterator = outputPtr->begin();
  for ( TField::iterator inputIt = inputPtr->begin();
	  inputIt != inputPtr->end(); ++inputIt, ++outputIterator )
	{
		cnt++;
		if ( cnt % 20000 == 0 ) 
			PROG_VAL( cnt );
		double dVal = ( ( (*inputIt) + dInputOffset ) * dFactor ) + dOutputOffset;
   	(*outputIterator) = static_cast<ushort>( round( dVal ) );
  }	
	PROG_RESET();
  setOutput( outputPtr );
BENCHSTOP;	
}

CPipelineItem* CFieldToImage::newInstance( ulong ulID ) const throw()
{
  return new CFieldToImage( ulID );
}
