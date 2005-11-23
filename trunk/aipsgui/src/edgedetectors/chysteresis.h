/************************************************************************
 * File: $FILE                                                          *
 * Project: AIPS                                                        *
 * Description:                                                         *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status:  Pre-Alpha                                                   *
 * Created: $DATE                                                       *
 * Changed:                                                             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/
#ifndef CHYSTERESIS_H
#define CHYSTERESIS_H

// AIPS includes
#include <aipsnumbertraits.h>
#include <cfilter.h>
#include <cglobalprogress.h>
#ifdef BENCHMARK
#include <boost/timer.hpp>
#endif
// library includes
#include "libid.h"

using namespace aips;
  
/** This one generates a vector field from an image using the sobel gradient operator. */
class CHysteresis : public CFilter
{
private:
  /// Standard constructor
  CHysteresis();
  /// Copy constructor
  CHysteresis( CHysteresis& );
  /// Assignment operator
  CHysteresis& operator=( CHysteresis& );
public:
/* Structors */
  /// Constructor
  CHysteresis( ulong ulID ) 
		throw();
  /// Destructor
  virtual ~CHysteresis() 
		throw();
/* Other methods */
  /// Reimplemented from CPipelineItem  
  virtual void apply() 
		throw();
  /// Reimplemented from CPipelineItem  
  virtual CPipelineItem* newInstance( ulong ulID = 0 ) const 
		throw(); 
};

#endif
