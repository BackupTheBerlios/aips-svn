/************************************************************************
 * File: cchannelcombiner.cpp                                           *
 * Project: AIPS - basic converters plugin library                      *
 * Description: Combines one or more images in a new multi-channel      *
 *              image.                                                  *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.2                                                         *
 * Created: 2004-02-04                                                  *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "cchannelcombiner.h"

using namespace std;
using namespace boost;

/*************
 * Structors *
 *************/

template<unsigned int index> void CChannelCombiner::call() throw()
{	
	combine<typename TypeAt<datasetTL, index>::Result, 
		SDataTraits<typename TypeAt<datasetTL, index>::Result>::isComparable> c;
	if ( !c(this) )
		call<index-1>();
}

template<> void CChannelCombiner::call<0>() throw()
{
	typedef TypeAt<datasetTL, 0>::Result TDataType;
	combine<TDataType, SDataTraits<TDataType>::isComparable> c;
	if ( !c(this) )
		alog << LWARN << "Dataset type not supported by module" << endl;
}
 
/** 
 * \param ulID unique module ID
 */
CChannelCombiner::CChannelCombiner( ulong ulID ) throw()
  : CConverter ( ulID, 4, 1, "CChannelCombiner", "0.2", "CConverter" )
{
  sModuleID = getClassName() + "/" + getClassVersion() + "/" + sLibID;
  setModuleName("Channel combiner");

  sDocumentation = "Combines two images into one new image by using seperate channels.\n"
									 "The input images need to have the same dimension and extends!\n"
									 "** Input ports:\n"
                   " 0: A multi-channel 2D or 3D data set\n"
                   " 1: A multi-channel 2D or 3D data set\n"
									 " 2: A multi-channel 2D or 3D data set (optional)\n"
									 " 3: A multi-channel 2D or 3D data set (optional)\n"
                   "** Output ports:\n"
                   " 0: A multi-channel 2D or 3D data set";
	
  inputsVec[0].portType = IOOther;
  inputsVec[1].portType = IOOther;
  inputsVec[2].portType = IOOther;
  inputsVec[3].portType = IOOther;
  outputsVec[0].portType = IOOther;
}

CChannelCombiner::~CChannelCombiner() throw()
{
}

/*****************
 * Other methods *
 *****************/

void CChannelCombiner::apply() throw()
{
BENCHSTART;
	bModuleReady = false;
	call<Length<datasetTL>::value - 1>();
BENCHSTOP;
}

CPipelineItem* CChannelCombiner::newInstance( ulong ulID ) const throw()
{
  return new CChannelCombiner( ulID );
}

template<typename T>
bool CChannelCombiner::combine<T, true>::operator() ( CChannelCombiner* parent ) throw()
{
	if ( !parent->getInput() )
		return false;
	const std::type_info& inputType = parent->getInput( 0 )->getType();
	if ( inputType != typeid( typename T::TDataType ) ||
		parent->getInput( 1 )->getType() != typeid( typename T::TDataType ) )
	{
		alog << LWARN << "Illegal input type" << endl;
		return false;
	}
cerr << "A" << endl;	
	shared_ptr<T> inputPtr1 = static_pointer_cast<T>( parent->getInput( 0 ) );
	if ( !parent->checkInput<T>( inputPtr1, 2, 3 ) )
		return false;
  shared_ptr<T> inputPtr2 = static_pointer_cast<T>( parent->getInput( 1 ) );
	if ( !parent->checkInput<T>( inputPtr2 ) || inputPtr2->getDimension() != inputPtr1->getDimension() )
		return false;
	if ( inputPtr2->getExtents() != inputPtr1->getExtents() )
	{
		alog << LWARN << SERROR("Can only work with images of the same extent") << endl;
		return false;
	}
cerr << "B" << endl;
	parent->bModuleReady = true;
	
	// Check optional inputs
	shared_ptr<T> inputPtr3;
	if( parent->getInput( 2 ) && parent->getInput( 2 )->getType() == inputType )
		inputPtr3 = static_pointer_cast<T>( parent->getInput( 2 ) );
	else	
		inputPtr3.reset();
	if (  inputPtr3.get() == NULL || inputPtr3->getDimension() != inputPtr1->getDimension() 
		|| inputPtr3->getExtents() != inputPtr1->getExtents() )
		inputPtr3.reset();
cerr << "C" << endl;
	shared_ptr<T> inputPtr4;
	if( parent->getInput( 3 ) && parent->getInput( 3 )->getType() == inputType )
		inputPtr4 = static_pointer_cast<T>( parent->getInput( 3 ) );
	else
		inputPtr4.reset();
	if (  inputPtr4.get() == NULL || inputPtr4->getDimension() != inputPtr1->getDimension() 
		|| inputPtr4->getExtents() != inputPtr1->getExtents() )
		inputPtr4.reset();
cerr << "D" << endl;
	parent->deleteOldOutput();
	vector<shared_ptr<T> > srcPtrVec;
	srcPtrVec.push_back( inputPtr1 );
	srcPtrVec.push_back( inputPtr2 );
	ushort usOutputChannels = inputPtr1->getDataDimension() + inputPtr2->getDataDimension();
	if ( inputPtr3.get() != NULL ) 
	{
		srcPtrVec.push_back( inputPtr3 );
		usOutputChannels += inputPtr3->getDataDimension();

		if ( inputPtr4.get() != NULL ) 
		{
			usOutputChannels += inputPtr4->getDataDimension();
			srcPtrVec.push_back( inputPtr4 );
		}
	}
cerr << "E" << endl;
	shared_ptr<T> outputPtr ( new T( inputPtr1->getDimension(), inputPtr1->getExtents(),
		usOutputChannels ) );
	outputPtr->setMinimum( min( inputPtr1->getMinimum(), inputPtr2->getMinimum() ) );
  outputPtr->setMaximum( max( inputPtr1->getMaximum(), inputPtr2->getMaximum() ) );
	if ( inputPtr3.get() )
	{
 		outputPtr->setMinimum( min( outputPtr->getMinimum(), inputPtr3->getMinimum() ) );
  	outputPtr->setMaximum( max( outputPtr->getMaximum(), inputPtr3->getMaximum() ) );
		if ( inputPtr4 )
		{
 			outputPtr->setMinimum( min( outputPtr->getMinimum(), inputPtr4->getMinimum() ) );
	  	outputPtr->setMaximum( max( outputPtr->getMaximum(), inputPtr4->getMaximum() ) );
		}
	}
	
	ushort usInputCounter = 0;
	typename T::iterator inputIt = srcPtrVec[0]->begin();
cerr << "F" << endl;
  for ( typename T::iterator outputIt = outputPtr->begin(); outputIt != outputPtr->end(); 
		++outputIt )
  {
		*outputIt = *inputIt;
		++inputIt;
		if ( inputIt == srcPtrVec[usInputCounter]->end() )
		{
			++usInputCounter;
			if ( usInputCounter == srcPtrVec.size() )
				break;
			else
				inputIt = srcPtrVec[usInputCounter]->begin();
		}
  }
  parent->setOutput( outputPtr );
	return true;
}

template<typename T> bool CChannelCombiner::combine<T,false>::operator() ( CChannelCombiner* parent ) throw()
{
	const std::type_info& inputType = parent->getInput( 0 )->getType();
	if ( inputType != typeid( typename T::TDataType ) ||
		parent->getInput( 1 )->getType() != typeid( typename T::TDataType ) )
	{
		alog << LWARN << "Illegal input type" << endl;
		return false;
	}

	shared_ptr<T> inputPtr1 = static_pointer_cast<T>( parent->getInput( 0 ) );
	if ( !parent->checkInput<T>( inputPtr1, 2, 3 ) )
		return false;
  shared_ptr<T> inputPtr2 = static_pointer_cast<T>( parent->getInput( 1 ) );
	if ( !parent->checkInput<T>( inputPtr2 ) || inputPtr2->getDimension() != inputPtr1->getDimension() )
		return false;
	if ( inputPtr2->getExtents() != inputPtr1->getExtents() )
	{
		alog << LWARN << SERROR("Can only work with images of the same extent") << endl;
		return false;
	}
	
	parent->bModuleReady = true;
	
	// Check optional inputs
	shared_ptr<T> inputPtr3;
	if( parent->getInput( 2 )->getType() == inputType )
		inputPtr3 = static_pointer_cast<T>( parent->getInput( 2 ) );
	else	
		inputPtr3.reset();	
	if (  inputPtr3.get() == NULL || inputPtr3->getDimension() != inputPtr1->getDimension()
		|| inputPtr3->getExtents() != inputPtr1->getExtents() )
		inputPtr3.reset();
	shared_ptr<T> inputPtr4;
	if( parent->getInput( 3 )->getType() == inputType )
		inputPtr4 = static_pointer_cast<T>( parent->getInput( 3 ) );
	else	
		inputPtr4.reset();
	if (  inputPtr4.get() == NULL || inputPtr4->getDimension() != inputPtr1->getDimension()
		|| inputPtr4->getExtents() != inputPtr1->getExtents() )
		inputPtr4.reset();
		
	parent->deleteOldOutput();
	vector<shared_ptr<T> > srcPtrVec;
	srcPtrVec.push_back( inputPtr1 );
	srcPtrVec.push_back( inputPtr2 );
	ushort usOutputChannels = inputPtr1->getDataDimension() + inputPtr2->getDataDimension();
	if ( inputPtr3.get() != NULL ) 
	{
		srcPtrVec.push_back( inputPtr3 );
		usOutputChannels += inputPtr3->getDataDimension();

		if ( inputPtr4.get() != NULL ) 
		{
			usOutputChannels += inputPtr4->getDataDimension();
			srcPtrVec.push_back( inputPtr4 );
		}
	}
		
	shared_ptr<T> outputPtr ( new T( inputPtr1->getDimension(), inputPtr1->getExtents(),
		usOutputChannels ) );
	outputPtr->setDataRange( inputPtr1->getDataRange() );
	ushort usInputCounter = 0;
	typename T::iterator inputIt = srcPtrVec[0]->begin();
  for ( typename T::iterator outputIt = outputPtr->begin(); outputIt != outputPtr->end(); 
		++outputIt )
  {
		*outputIt = *inputIt;
		++inputIt;
		if ( inputIt == srcPtrVec[usInputCounter]->end() )
		{
			++usInputCounter;
			if ( usInputCounter == srcPtrVec.size() )
				break;
			else
				inputIt = srcPtrVec[usInputCounter]->begin();
		}
  }
  parent->setOutput( outputPtr );
	return true;
}

template<> bool CChannelCombiner::combine<TStringField,false>::operator() ( CChannelCombiner* parent ) throw()
{
	return false;
}
