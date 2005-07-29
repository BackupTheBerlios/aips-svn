/************************************************************************
 * File: cswapendianess.cpp                                             *
 * Project: AIPS basic converter modules                                *
 * Description: Swaps the endianess of the input data                   *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Created: 2004-05-11                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#include "cswapendianess.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

template<unsigned int index> void CSwapEndianess::call() throw()
{	
	swapDataFunctor<TypeAt<datasetTL, index>::Result, 
		dataTraits<TypeAt<datasetTL, index>::Result>::isNumeric> functor;
	if ( !functor() )
		call<index-1>();
}

template<> void CSwapEndianess::call<0>() throw()
{
	typedef TypeAt<datasetTL, 0>::Result TDataType;
	swapDataFunctor<TDataType, dataTraits<TDataType>::isNumeric> functor;
	if ( !functor() )
		alog << LWARN << "Dataset type not supported by module" << endl;
}
 
/** \param ulID unique module ID */
CSwapEndianess::CSwapEndianess( ulong ulID ) throw()
 : CConverter(ulID, 1,1, "CSwapEndianess", "0.3", "CConverter" )
{
  setModuleID( sLibID );
  setModuleName("Data endianess swapper");
  
  sDocumentation = "Swaps the endianess of the given data set\n"
  								 "** Input ports:\n"
                   " 0: A data set\n"
                   "** Output ports:\n"
                   " 0: A data set";

  inputsVec[0].portType = IOOther;
  outputsVec[0].portType = IOOther;
}

CSwapEndianess::~CSwapEndianess() throw()
{
}

/*****************
 * Other methods *
 *****************/
 
void CSwapEndianess::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
	call<Length<datasetTL>::value - 1>();
BENCHSTOP;	
}

/** \param ulID unique module ID */
CPipelineItem* CSwapEndianess::newInstance( ulong ulID ) const throw()
{
	return new CSwapEndianess( ulID );
}

template<typename SetType> 
bool swapDataFunctor<SetType, true>::operator()() throw()
{	
	if ( checkType<SetType>( getInput() ) )
		return false

	// Define local data type 
	typedef typename dataTraits<SetType>::dataType TVoxel;
	shared_ptr<SetType> inputSPtr = static_pointer_cast<SetType>( getInput() );
	
	bModuleReady = true;
  deleteOldOutput();
	
	shared_ptr<SetType> outputSPtr ( new SetType( imagePtr->getDimension(), imagePtr->getExtents(),
		imagePtr->getDataDimension() ) );
	(*outputSPtr) = (*inputSPtr);
		
	TVoxel maximum = inputSPtr->getMaximum();
	swapEndianess( maximum );
	outputSPtr->setMaximum( maximum );
	TVoxel minimum = inputSPtr->getMinimum();
	swapEndianess( minimum );
	outputSPtr->setMinimum( minimum );
		
	typename T::iterator outputIt = outputSPtr->begin();
	PROG_MAX( outputSPtr->getArraySize() );
	ulong cnt = 0;
	while( outputIt != outputSPtr->end() )
	{
		cnt++;
		if ( cnt % 20000 == 0 ) 
			PROG_VAL( cnt );
		swapEndianess( (*outputIt) );
		++outputIt;
	}
	PROG_RESET();
	setOutput( outputPtr );
	return true;
}

template<typename SetType> 
bool swapDataFunctor<SetType, true>::operator()() throw()
{
	alog << LWARN << "Unable to swap non-nnumerical data" << endl;
	return false;
}
