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
	if ( !swapData<typename TypeAt<datasetTL, index>::Result>() )
		call<index-1>();
}

template<> void CSwapEndianess::call<0>() throw()
{
	typedef TypeAt<datasetTL, 0>::Result TDataType;
	if ( !swapData<TDataType>() )
		alog << LWARN << "Dataset type not supported by module" << endl;
}
 
/** \param ulID unique module ID */
CSwapEndianess::CSwapEndianess( ulong ulID ) throw()
 : CConverter(ulID, 1,1, "CSwapEndianess", "0.3", "CConverter" )
{
  setModuleID( sLibID );
  setModuleName("Data endianess swapper");
  
  sDocumentation = "** Input ports:\n"
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

template<typename T> bool CSwapEndianess::swapData() throw()
{
	typedef typename dataTraits<T>::dataType TVoxel;
	if ( !getInput() || getInput()->getType() != typeid( TVoxel ) )
		return false;

	shared_ptr<T> imagePtr = static_pointer_cast<T>( getInput() );
	if ( !checkInput( imagePtr ) ) // This is an image
		return false;
	
	bModuleReady = true;
  deleteOldOutput();
	shared_ptr<T> outputPtr ( new T( imagePtr->getDimension(), imagePtr->getExtents(),
		imagePtr->getDataDimension() ) );
	
	
	TVoxel maximum = imagePtr->getMaximum();
	swapEndianess( maximum );
	outputPtr->setMaximum( maximum );
	TVoxel minimum = imagePtr->getMinimum();
	swapEndianess( minimum );
	outputPtr->setMinimum( minimum );
		
	typename T::iterator inputIt = imagePtr->begin();
	typename T::iterator outputIt = outputPtr->begin();
	PROG_MAX( imagePtr->getArraySize() );
	ulong cnt = 0;
	while( inputIt != imagePtr->end() )
	{
		cnt++;
		if ( cnt % 20000 == 0 ) 
			PROG_VAL( cnt );
		TVoxel value = (*inputIt);
		swapEndianess( value );
		(*outputIt) = value;
		++inputIt;
		++outputIt;
	}
	PROG_RESET();
	setOutput( outputPtr );
	return true;
}

template<> bool CSwapEndianess::swapData<TStringField>() throw()
{
	return false;
}
