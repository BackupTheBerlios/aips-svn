/************************************************************************
 * File: cimagetofield.cpp                                              *
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

#include "cimagetofield.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

/**
 * \param ulID unique module ID
 */
CImageToField::CImageToField( ulong ulID ) throw()
  : CConverter ( ulID, 1, 1, "CImageToField", "0.1", "CConverter" )
{
  setModuleID( sLibID );
  setModuleName("Int to float");
  
  sDocumentation = "Converts an integer point image into a float image\n"
									 "** Input ports:\n"
                   " 0: A float multi-channel data set\n"
                   "** Output ports:\n"
                   " 0: A integer multi-channel data set\n"
                   "** parametersVec:\n"
									 " <MinimumValue>: Minimum intensity of output image\n"
                   " <MaximumValue>: Maximum intensity of output image";

  parameters.initDouble( "MinimumValue", 0.0, numeric_limits<double>::min(), numeric_limits<double>::max() );
	parameters.initDouble( "MaximumValue", 1.0, numeric_limits<double>::min(), numeric_limits<double>::max() );
	inputsVec[0].portType = CPipelineItem::IOInteger;
	outputsVec[0].portType = CPipelineItem::IOOther;
}

CImageToField::~CImageToField() throw()
{
}

/*****************
 * Other methods *
 *****************/

void CImageToField::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
	if ( !getInput() || getInput()->getType() != typeid( dataTraits<TImage>::dataType ) )
		return;
	shared_ptr<TImage> inputPtr = static_pointer_cast<TImage>( getInput() );
  if ( !checkInput( inputPtr ) )
   	return;
	bModuleReady = true;
  deleteOldOutput();
	// Get parameters
	double dMinVal = parameters.getDouble( "MinimumValue" );
	double dMaxVal = parameters.getDouble( "MaximumValue" );
	// Determine minimum and maximum conversion
	double dFactor = ( dMaxVal - dMinVal ) 
		/ static_cast<double>( inputPtr->getMaximum() - inputPtr->getMinimum() ); 
	double dInputOffset = static_cast<double>( inputPtr->getMinimum() ) * -1.0;
	double dOutputOffset = dMinVal;
	// Create output field
	TFieldPtr outputPtr( new TField ( inputPtr->getDimension(), inputPtr->getExtents(), 
	 	inputPtr->getDataDimension() ) );

	// Get maximum and minimum of vector field
 	outputPtr->setMaximum( dMaxVal );
 	outputPtr->setMinimum( dMinVal );
 	// Create new field
	PROG_MAX( inputPtr->getArraySize() );
  ulong cnt = 0;
  TField::iterator outputIterator = outputPtr->begin();
  for ( TImage::iterator inputIt = inputPtr->begin();
	  inputIt != inputPtr->end(); ++inputIt, ++outputIterator )
	{
		cnt++;
		if ( cnt % 20000 == 0 ) 
			PROG_VAL( cnt );
		double dVal = ( ( static_cast<double>(*inputIt) + dInputOffset ) * dFactor ) + dOutputOffset;
   	(*outputIterator) = dVal;
  }	
	PROG_RESET();
  setOutput( outputPtr );
BENCHSTOP;	
}

CPipelineItem* CImageToField::newInstance( ulong ulID ) const throw()
{
  return new CImageToField( ulID );
}
