/************************************************************************
 * File: cswapendianess.h                                               *
 * Project: AIPS basic converter modules                                *
 * Description: Swaps the endianess of the input data                   *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status:  Pre-Alpha                                                   *
 * Created: 2004-05-11                                                  *
 * Changed: 2004-06-16 Updated output of apply()                        *
 *          2005-01-12 Templatized filter functionality                 *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CSWAPENDIANESS_H
#define CSWAPENDIANESS_H

// AIPS includes
#include <cconverter.h>
#include <aipsdatatraits.h>
#include <cglobalprogress.h>

// lib includes
#include "libid.h"

using namespace aips;

/**
 * Swaps the endianess of the given input data
 */
class CSwapEndianess : public CConverter
{
private:
	/// Standard constructor
	CSwapEndianess();
	/// Copy constructor
	CSwapEndianess( CSwapEndianess& );
	/// Assignment operator
	CSwapEndianess& operator=( CSwapEndianess& );
public:
/* Structors */
	/// Constructor
	CSwapEndianess( ulong ulID ) 
		throw();
	/// Destructor
  ~CSwapEndianess() 
		throw();
/* Other member functions */
  /// Reimplemented from CPipelineItem
  virtual void apply()
    throw();
  /// Reimplemented from CPipelineItem
  virtual CPipelineItem* newInstance( ulong ulID = 0 ) const
    throw();	
private:
	/// Internal method template for the actual filter process
	template<typename T> bool swapData() throw();
	template<unsigned int index> void call() throw();
};

#endif
