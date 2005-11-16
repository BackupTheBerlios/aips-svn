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
#include "cfieldtochannel.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

/**
 * \param ulID unique module ID
 */
CFieldToChannel::CFieldToChannel( ulong ulID ) throw()
  : CConverter ( ulID, 1, 3, "CFieldToChannel", "0.1", "CConverter" )
{
  setModuleID( sLibID );
  setModuleName("Field to channel converter");
  
  sDocumentation = "** Input ports:\n"
                   " 0: A vector multi-channel data set\n"
                   "** Output ports:\n"
                   " 0: A scalar multi-channel data set\n"
									 " 1: A scalar multi-channel data set\n"
									 " 2: A scalar multi-channel data set\n"
                   "** parametersVec:\n"
                   " <Maximum value>: Maximum intensity of output image";


  parameters.initUnsignedLong( "MaximumValue", 4095, 0, 65535 );
  inputsVec[0].portType = CPipelineItem::IOVector;
  outputsVec[0].portType = CPipelineItem::IOInteger;
	outputsVec[1].portType = CPipelineItem::IOInteger;
	outputsVec[2].portType = CPipelineItem::IOInteger;
}

CFieldToChannel::~CFieldToChannel() throw()
{
}

/*****************
 * Other methods *
 *****************/

void CFieldToChannel::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
	if ( !getInput() || getInput()->getDimension() < 2
		|| getInput()->getDimension() > 3 )
	{
		alog << LWARN << SERROR("Input field is no vector field") << endl;
		return;
	}	
	if ( getInput()->getType() == typeid( dataTraits<TField2D>::dataType ) && getInput()->getDimension() == 2 )
	{
  	TField2DPtr inputPtr = static_pointer_cast<TField2D>( getInput() );
  	if ( inputPtr.get() == NULL )
  	{
    	alog << LWARN << SERROR("Input field is no 2D double vector field") << endl;
    	return;
  	}
		bModuleReady = true;
  	deleteOldOutput();
  	// Determine value range in input data
  	double dMaxNorm = inputPtr->getDataRange().getMaximum();

  	// Create output fields
	  TImagePtr outputPtr1 ( new TImage ( 2, inputPtr->getExtents(), inputPtr->getDataDimension() ) );
  	// Get maximum and minimum of vector field
  	outputPtr1->setMaximum( parameters.getUnsignedLong("MaximumValue") );
  	outputPtr1->setMinimum( 0 );
	  TImagePtr outputPtr2 ( new TImage ( 2, inputPtr->getExtents(), inputPtr->getDataDimension() ) );
  	// Get maximum and minimum of vector field
  	outputPtr2->setMaximum( parameters.getUnsignedLong("MaximumValue") );
  	outputPtr2->setMinimum( 0 );
		
		ulong ulMaxValue = parameters.getUnsignedLong("MaximumValue");
  	// Determine f(|v|(r)) = s(r) = |v|/m*s
		double dQuot = ulMaxValue;
		if ( dMaxNorm > 1.0 )
  	 dQuot /= dMaxNorm;
  	// Create new field
		PROG_MAX( inputPtr->getArraySize() );
    ulong cnt = 0;
  	TImage::iterator outputIterator1 = outputPtr1->begin();
		TImage::iterator outputIterator2 = outputPtr2->begin();
  	for ( TField2D::iterator inputIt = inputPtr->begin();
	    inputIt != inputPtr->end(); ++inputIt, ++outputIterator1, ++outputIterator2 )
	  {
			cnt++;
			if ( cnt % 2000 == 0 ) PROG_VAL( cnt );
  	  ushort usValue1 = static_cast<ushort>( std::abs(round( (*inputIt)[0] * dQuot ) ) );
			ushort usValue2 = static_cast<ushort>( std::abs(round( (*inputIt)[1] * dQuot ) ) );
    	if ( usValue1 > ulMaxValue )
    	{
      	alog << LWARN << SERROR( "Value is too large " ) << usValue1 << endl;
      	usValue1 = ulMaxValue;
    	}
			if ( usValue2 > ulMaxValue )
    	{
      	alog << LWARN << SERROR( "Value is too large " ) << usValue2 << endl;
      	usValue2 = ulMaxValue;
    	}
    	(*outputIterator1) = usValue1;
			(*outputIterator2) = usValue2;
  	}
		PROG_RESET();
		// Set output
  	setOutput( outputPtr1 );
		setOutput( outputPtr2, 1 );
  }
	else if ( getInput()->getType() == typeid( dataTraits<TField3D>::dataType ) )
	{
  	TField3DPtr inputPtr = static_pointer_cast<TField3D>( getInput() );
  	if ( inputPtr == NULL )
  	{
    	alog << LWARN << SERROR( "Input field is no 3D double vector field" ) << endl;
    	return;
  	}
		bModuleReady = true;
  	deleteOldOutput();
  
  	// Determine value range in input data
  	double dMaxNorm = inputPtr->getDataRange().getMaximum();

  	// Create output field
	  TImagePtr outputPtr1 ( new TImage ( 3, inputPtr->getExtents(), inputPtr->getDataDimension() ) );
		TImagePtr outputPtr2 ( new TImage ( 3, inputPtr->getExtents(), inputPtr->getDataDimension() ) );
		TImagePtr outputPtr3 ( new TImage ( 3, inputPtr->getExtents(), inputPtr->getDataDimension() ) );

		// Get maximum and minimum of vector field
		ulong ulMaxValue = parameters.getUnsignedLong("Maximum value");
  	outputPtr1->setMaximum( ulMaxValue );
  	outputPtr1->setMinimum( 0 );
		outputPtr2->setMaximum( ulMaxValue );
  	outputPtr2->setMinimum( 0 );
		outputPtr3->setMaximum( ulMaxValue );
  	outputPtr3->setMinimum( 0 );

  	// Determine f(|v|(r)) = s(r) = |v|/m*s
  	double dQuot = ulMaxValue / dMaxNorm;

  	// Create new field
		PROG_MAX( inputPtr->getArraySize() );
    ulong cnt = 0;
  	TImage::iterator outputIterator1 = outputPtr1->begin();
		TImage::iterator outputIterator2 = outputPtr2->begin();
		TImage::iterator outputIterator3 = outputPtr3->begin();
  	for ( TField3D::iterator inputIt = inputPtr->begin();
	    inputIt != inputPtr->end(); ++inputIt, ++outputIterator1, ++outputIterator2, ++outputIterator3)
	  {
			cnt++;
			if ( cnt % 20000 == 0 ) PROG_VAL( cnt );
  	  ushort usValue1 = static_cast<ushort>( std::abs( round( (*inputIt)[0] * dQuot ) ) );
			ushort usValue2 = static_cast<ushort>( std::abs( round( (*inputIt)[1] * dQuot ) ) );
			ushort usValue3 = static_cast<ushort>( std::abs( round( (*inputIt)[2] * dQuot ) ) );			
    	if ( usValue1 > ulMaxValue )
    	{
      	alog << LWARN << SERROR("Value is too large") << usValue1 << endl;
      	usValue1 = ulMaxValue;
    	}
    	(*outputIterator1) = usValue1;
			(*outputIterator2) = usValue2;
			(*outputIterator3) = usValue3;
  	}	
		PROG_RESET();
		setOutput( outputPtr1, 0 );
		setOutput( outputPtr2, 1 );
		setOutput( outputPtr3, 2 );
	}
	else
	{
		alog << LWARN << "Illegal input type" << endl;
		return;
	}
BENCHSTOP;	
}

CPipelineItem* CFieldToChannel::newInstance( ulong ulID ) const throw()
{
  return new CFieldToChannel( ulID );
}
