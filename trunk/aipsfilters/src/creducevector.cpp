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

DEFINE_CALL_MACRO( CReduceVector::call, CReduceVector::reduce, vectorfieldTL )

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
FBEGIN;
BENCHSTART;
	bModuleReady = false;
	if ( getInput() )
		call<Length<vectorfieldTL>::value - 1>();
	else
  {
    alog << LWARN << SERROR("Input type is no 2D or 3D vector field!") << endl;
    return;
  }
BENCHSTOP;	
FEND;
}

template<typename T> 
bool CReduceVector::reduce() throw()
{
FBEGIN;
  shared_ptr<T> inputSPtr = static_pointer_cast<T>( getInput() );
  if ( !checkInput<T>( inputSPtr ) )
    return false;
   	
	bModuleReady = true;
  deleteOldOutput();
  // Determine value range in input data
  double dMaxNorm = inputSPtr->getDataRange().getMaximum();

  // Create output field
	TFieldPtr outputSPtr( new TField ( inputSPtr->getDimension(), inputSPtr->getExtents(), 
	 	inputSPtr->getDataDimension() ) );

	// Get maximum and minimum of vector field
 	outputSPtr->setMaximum( dMaxNorm );
 	outputSPtr->setMinimum( 0.0 );

 	// Create new field
  TField::iterator outputIterator = outputSPtr->begin();
  for ( typename T::iterator inputIt = inputSPtr->begin();
	  inputIt != inputSPtr->end(); ++inputIt, ++outputIterator )
	{
   	(*outputIterator) = norm(*inputIt);
  }	
  setOutput( outputSPtr );
FEND;
	return true;  
}
