/************************************************************************
 * File: cstructureddataadapter.h                                       *
 * Project: AIPS                                                        *
 * Description: Abstract adapter for structured data sets               *
 *                                                                      *
 * Author: Hendrik Belitz                                               *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status : PreAlpha                                                    *
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
 */
CStructuredDataAdapter::CStructuredDataAdapter( const string &sClassName_, const string &sClassVersion_, 
	const string &sDerivedFrom_ ) throw() : CDataAdapter( sClassName_, sClassVersion_, sDerivedFrom_ )
{
}
   
/**
 * \param internalDataPtr_ shared pointer to internal dataset representation to use
 * \param sClassName_ class name
 * \param sClassVersion_ class version
 * \param sDerivedFrom_ parent class
 */
CStructuredDataAdapter::CStructuredDataAdapter( TDataSetPtr internalDataPtr_,
	const string &sClassName_, const string &sClassVersion_, const string &sDerivedFrom_ )
  throw() : CDataAdapter( sClassName_, sClassVersion_, sDerivedFrom_ ), internalDataPtr ( internalDataPtr_ )
{
}

CStructuredDataAdapter::~CStructuredDataAdapter() throw() {}

/*****************
 * Other methods *
 *****************/
 
/** \param internalDataPtr_ new dataset to use for conversions into external format */
void CStructuredDataAdapter::setInternalData( TDataSetPtr internalDataPtr_ )
{
	internalDataPtr = internalDataPtr_;
}

void CStructuredDataAdapter::setYSwapping( bool bSwapYAxis_ ) 
{ 
	bSwapYAxis = bSwapYAxis_; 
}
