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

DEFINE_CALL_MACRO( CFieldToImage::call, CFieldToImage::reduce, imageTL )

/**
 * \param ulID unique module ID
 */
CFieldToImage::CFieldToImage( ulong ulID ) throw()
  : CConverter ( ulID, 1, 2, "CFieldToImage", CFIELDTOIMAGE_VERSION, "CConverter" )
{
  setModuleID( sLibID );
  setModuleName("Float to int");
  
  sDocumentation = "Converts a scalar image into an integer image\n"
									 "** Input ports:\n"
                   " 0: A scalar multi-channel data set\n"
                   "** Output ports:\n"
                   " 0: A 16 bit signed integer multi-channel data set\n"
                   " 1: A 8 bit unsigned integer multi-channel data set\n"
                   "** parametersVec:\n"
									 " <MinimumValue>: Minimum intensity of output image\n"
                   " <MaximumValue>: Maximum intensity of output image";

  parameters.initLong( "MinimumValue", 0, -32767, 32767 );
	parameters.initLong( "MaximumValue", 255, -32767, 32767 );
	inputsVec[0].portType = CPipelineItem::IOOther;
	outputsVec[0].portType = CPipelineItem::IOInteger;
	outputsVec[1].portType = CPipelineItem::IOInteger;
}

CFieldToImage::~CFieldToImage() throw()
{
}

/*****************
 * Other methods *
 *****************/

void CFieldToImage::apply() throw()
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
BENCHSTOP;	
FEND;
}

template<typename ImageType>
bool CFieldToImage::reduce() throw()
{
FBEGIN;
	shared_ptr<ImageType> inputSPtr = static_pointer_cast<ImageType>( getInput() );
  if ( !checkInput<ImageType>( inputSPtr ) )
  	return false;
		
	bModuleReady = true;
  deleteOldOutput();
	// Get parameters
	long lMinVal = parameters.getLong( "MinimumValue" );
	long lMaxVal = parameters.getLong( "MaximumValue" );
	if ( lMaxVal < lMinVal )
	{
		alog << LWARN << "Illegal parameters. MinimumValue needs to be smaller or equal to MaximumValue." << endl;
		return false;
	}
	// Determine minimum and maximum conversion
	double dFactor = static_cast<double>( lMaxVal - lMinVal ) 
		/ ( inputSPtr->getDataRange().getMaximum() - inputSPtr->getDataRange().getMinimum() ); 
	double dInputOffset = inputSPtr->getDataRange().getMinimum() * -1.0;
	double dOutputOffset = lMinVal;
	// Create output field one
	TImagePtr output1SPtr( new TImage ( inputSPtr->getDimension(), inputSPtr->getExtents(), 
 		inputSPtr->getDataDimension() ) );
	// Get maximum and minimum of input field
	output1SPtr->setMaximum( lMaxVal );
 	output1SPtr->setMinimum( lMinVal );
	// Create output field two
	TSmallImagePtr output2SPtr;
	if ( in( lMinVal, static_cast<long>( numeric_limits<TSmallImage::TDataType>::min() ),	
		static_cast<long>( numeric_limits<TSmallImage::TDataType>::max() ) ) && 
			in( lMaxVal, static_cast<long>( numeric_limits<TSmallImage::TDataType>::min() ),
		static_cast<long>( numeric_limits<TSmallImage::TDataType>::max() ) ) )
	{
		output2SPtr.reset( new TSmallImage( inputSPtr->getDimension(), inputSPtr->getExtents(), 
 		inputSPtr->getDataDimension() ) );
		// Get maximum and minimum of input field
		output2SPtr->setMaximum( lMaxVal );
 		output2SPtr->setMinimum( lMinVal );
 	}
 	
 	// Create new field
	TImage::iterator outputIterator = output1SPtr->begin();
  for ( typename ImageType::iterator inputIt = inputSPtr->begin();
	 	inputIt != inputSPtr->end(); ++inputIt, ++outputIterator )
	{
		double dVal = ( ( static_cast<double>(*inputIt) + dInputOffset ) * dFactor ) + dOutputOffset;
   	(*outputIterator) = static_cast<typename TImage::TDataType>( round( dVal ) );
  }	
  if ( output2SPtr )
  {
		TSmallImage::iterator ot = output2SPtr->begin();
	  for ( TImage::iterator it = output1SPtr->begin();
		 	it != output1SPtr->end(); ++it, ++ot )
		{
			double dVal = ( ( (*it) + dInputOffset ) * dFactor ) + dOutputOffset;
  	 	(*ot) = static_cast<typename TSmallImage::TDataType>( round( dVal ) );
	  }	
  }
  setOutput( output1SPtr, 0 );
  setOutput( output2SPtr, 1 );
FEND;  
  return true;  
}
