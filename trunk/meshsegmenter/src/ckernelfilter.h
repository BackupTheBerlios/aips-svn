/************************************************************************
 * File: ckernelfilter.cpp                                              *
 * Project: AIPS - Neighbourhood filters plugin library                 *
 * Description: Base class of all filters that use convolution          *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.4                                                         *
 * Status : Beta                                                        *
 * Created: 2003-11-13                                                  *
 * Changed: 2003-12-16 Filter adapted for multi-channel data            *
 *          2004-01-29 Clarified and documented source code             *
 *                     Removed unneccessary tests for data field types  *
 *          2004-05-04 Corrected some errors                            *
 *          2004-05-05 Updated code for standalone filtering. This code *
 *                     looks quite crazy, but is as fast as the blitz++ *
 *                     version                                          *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CKERNELFILTER_H
#define CKERNELFILTER_H

// AIPS includes
#include <cfilter.h>
#include <aipsnumeric.h>
#include <cglobalprogress.h>
#ifdef USE_BLITZ
#include <blitz/array.h>
#endif

// lib includes
#include "libid.h"

using namespace aips;

/** Abstract base class for all convolution filters */
class CKernelFilter : public CFilter
{
private:
  /// Standard constructor
  CKernelFilter();
  /// Copy constructor
  CKernelFilter( CKernelFilter& );
  /// Assignment operator
  CKernelFilter& operator=( CKernelFilter& );
public:
/* Structors */
  /// Constructor
  CKernelFilter( ulong ulID, const std::string &sName = "Generic filter kernel",
		const ushort usNoOfInputs = 1, const ushort usNoOfOutputs = 1,
		const std::string &sClassName_ = "CKernelFilter", const std::string &sClassVersion_ = "0.4", 
		const std::string &sDerivedFrom_ = "CFilter" ) throw();
  /// Destructor
  virtual ~CKernelFilter()
    throw();
protected:    
/* Other methods */
#ifdef USE_BLITZ    
  /// Applies a given filter kernel to a volume
  TImagePtr applyKernel ( TImagePtr dataPtr, blitz::Array<double,3>& kernel )
    throw();
  /// Applies a given filter kernel to an image
  TImagePtr applyKernel ( TImagePtr dataPtr, blitz::Array<double,2>& kernel )
    throw();
#endif
  /// Applies a given filter kernel to a volume
  TImagePtr applyKernel ( TImagePtr dataPtr, CTypedData<double>& kernel )
    throw();
};
#endif
