/************************************************************************
 * File: cstructureddataadapter.cpp                                     *
 * Project: AIPS                                                        *
 * Description: Abstract adapter for structured data sets               *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Created: 2004-22-12                                                  *
 * Changed:                                                             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#include "cstructureddataadapter.h"

using namespace std;
using namespace aips;
using boost::shared_ptr;

/*************
 * Structors *
 *************/

/**
 * \param sClassName_ class name
 * \param sClassVersion_ class version
 * \param sDerivedFrom_ parent class
 * \post all members are initialised properly
 */
CStructuredDataAdapter::CStructuredDataAdapter( const string &sClassName_, const string &sClassVersion_, 
	const string &sDerivedFrom_ ) throw() 
	: CDataAdapter( sClassName_, sClassVersion_, sDerivedFrom_ ), bSwapYAxis( false )
{
  internalDataSPtr.reset();
}
   
/**
 * \param internalDataPtr_ shared pointer to internal dataset representation to use
 * \param sClassName_ class name
 * \param sClassVersion_ class version
 * \param sDerivedFrom_ parent class
 * \post all members are initialised properly
 */
CStructuredDataAdapter::CStructuredDataAdapter( TDataSetPtr internalDataSPtr_,
	const string &sClassName_, const string &sClassVersion_, const string &sDerivedFrom_ ) throw() 
  : CDataAdapter( sClassName_, sClassVersion_, sDerivedFrom_ ),
    internalDataSPtr ( internalDataSPtr_ ), bSwapYAxis( false )
{
}

CStructuredDataAdapter::~CStructuredDataAdapter() throw()
{  
}

/*****************
 * Other methods *
 *****************/
 
/**
 * \param internalDataSPtr_ new dataset to use for conversions into external format
 * \post new data set is used
 */
void CStructuredDataAdapter::setInternalData( TDataSetPtr internalDataSPtr_ )
{
	internalDataSPtr = internalDataSPtr_;
}

/**
 * \param bSwapYAxis_ set new value for swapping the y axis 
 * \post new axis swapping 
 */

void CStructuredDataAdapter::setYSwapping( bool bSwapYAxis_ ) 
{ 
	bSwapYAxis = bSwapYAxis_; 
}
