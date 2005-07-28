/************************************************************************
 * File: ccropintensityrange.h                                          *
 * Project: AIPS                                                        *
 * Description: Module to delete unwanted intensity values from an image*
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status:  Pre-Alpha                                                   *
 * Created: 08-07-2004                                                  *
 * Changed:                                                             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CCROPINTENSITYRANGE_H
#define CCROPINTENSITYRANGE_H

// AIPS includes
#include <cfilter.h>
#include <cglobalprogress.h>
#ifdef BENCHMARK
#include <boost/timer.hpp>
#endif

// Library includes
#include "libid.h"

using namespace aips;

/** Deletes all voxels that are not in the given range */
class CCropIntensityRange : public CFilter
{
private:
  /// Standard constructor
  CCropIntensityRange();
  /// Copy constructor
  CCropIntensityRange( CCropIntensityRange& );
  /// Assignment operator
  CCropIntensityRange& operator=( CCropIntensityRange& );
public:
/* Structors */
	/// Constructor
  CCropIntensityRange( ulong ulID ) throw();
	/// Destructor
  ~CCropIntensityRange() throw();
/* Other methods */    
  /// Reimplemented from CPipelineItem  
  virtual CPipelineItem* newInstance( ulong ulID ) const
    throw();
  /// Reimplemented from CPipelineItem    
  virtual void apply()
    throw();	
};

#endif
