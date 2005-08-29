/************************************************************************
 * File: creducevector.cpp                                              *
 * Project: AIPS - basic converters plugin library                      *
 * Description:                                                         *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.1                                                         *
 * Created: Die Dez 2 2003                                              *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "creducevector.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

/**
 * \param ulID unique module ID
 */
CReduceVector::CReduceVector( ulong ulID ) throw()
  : CConverter ( ulID, 1, 1, "CReduceVector", "0.3", "CConverter" )
{
  setModuleID( sLibID );
  setModuleName("Vector field to scalar field");
  
  sDocumentation = "Converts a vector field to a scalar field by using the vector norm\n"
									 "** Input ports:\n"
                   " 0: A vector multi-channel data set\n"
                   "** Output ports:\n"
                   " 0: A scalar multi-channel data set";
									 
  inputsVec[0].portType = CPipelineItem::IOVector;
  outputsVec[0].portType = CPipelineItem::IOOther;
}

CReduceVector::~CReduceVector() throw()
{
}

/*****************
 * Other methods *
 *****************/

void CReduceVector::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
	if ( getInput() && getInput()->getType() == typeid( TVector2D ) )
		reduce<TField2D>();
	else if ( getInput() && getInput()->getType() == typeid( TVector3D ) )
		reduce<TField3D>();		
	else
		alog << LWARN << SERROR( "No input or wrong data type" ) << endl;
BENCHSTOP;	
}

CPipelineItem* CReduceVector::newInstance( ulong ulID ) const throw()
{
  return new CReduceVector( ulID );
}

template<typename T> void CReduceVector::reduce() throw()
{
	if ( !getInput() || getInput()->getType() != typeid( typename dataTraits<T>::dataType ) )
		return;
	shared_ptr<T> inputPtr = static_pointer_cast<T>( getInput() );
  if ( !checkInput<T>( inputPtr ) )
   	return;
	
	bModuleReady = true;
  deleteOldOutput();
  
  // Determine value range in input data
  double dMaxNorm = norm( inputPtr->getMaximum() );

  // Create output field
	TFieldPtr outputPtr( new TField ( inputPtr->getDimension(), inputPtr->getExtents(), 
	 	inputPtr->getDataDimension() ) );

	// Get maximum and minimum of vector field
 	outputPtr->setMaximum( dMaxNorm );
 	outputPtr->setMinimum( 0.0 );

 	// Create new field
	PROG_MAX( inputPtr->getArraySize() );
  ulong cnt = 0;
  TField::iterator outputIterator = outputPtr->begin();
  for ( typename T::iterator inputIt = inputPtr->begin();
	  inputIt != inputPtr->end(); ++inputIt, ++outputIterator )
	{
		cnt++;
		if ( cnt % 20000 == 0 ) 
			PROG_VAL( cnt );
   	(*outputIterator) = norm(*inputIt);
  }	
	PROG_RESET();
  setOutput( outputPtr );
}
