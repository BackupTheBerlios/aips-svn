/************************************************************************
 * File: $FILE                                                          *
 * Project: AIPS                                                        *
 * Description:                                                         *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status:  Pre-Alpha                                                   *
 * Created: 2004-08-12                                                  *
 * Changed:                                                             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CRESCALEHISTOGRAM_H
#define CRESCALEHISTOGRAM_H

// AIPS includes
#include <cfilter.h>
#include <cglobalprogress.h>
// library includes
#include "libid.h"

using namespace aips;

/**
 * Adds or subtracts the intensities of the given input images.
 * Both images can be weighted before the operation.
 */
class CRescaleHistogram
 : public CFilter
{
private:
	/// Standard constructor
	CRescaleHistogram();
	/// Copy constructor
	CRescaleHistogram( CRescaleHistogram & );
	/// Assignment operator
	CRescaleHistogram& operator=( CRescaleHistogram & );
public: 
/* Structors */
	/// Constructor
  CRescaleHistogram( ulong ulID = 0 ) 
		throw();
	/// Destructor
  virtual ~CRescaleHistogram() 
		throw();
/* Other methods */
  /// Reimplemented from CPipelineItem
  virtual CPipelineItem* newInstance( ulong ulID = 0 ) const
    throw();
  /// Reimplemented from CPipelineItem
  virtual void apply()
    throw();
};
#endif
