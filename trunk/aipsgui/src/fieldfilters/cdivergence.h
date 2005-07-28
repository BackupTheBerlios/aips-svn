/************************************************************************
 * File: cdivergence.h                                                  *
 * Project: AIPS                                                        *
 * Description: Generates a scalar divergence field from a vector field *
 *                                                                      *
 * Author: Hendrik Belitz (h.belitz@fz-juelich.de)                      *
 *                                                                      *
 * Version: 0.1                                                         *
 * Status : Alpha                                                       *
 * Created: 03.02.04                                                    *
 * Changed:                                                             *
 ************************************************************************
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 ************************************************************************/

#ifndef CDIVERGENCE_H
#define CDIVERGENCE_H

// AIPS includes
#include <cfilter.h>
#include <aipsnumeric.h>
#include <cglobalprogress.h>
#ifdef BENCHMARK
#include <boost/timer.hpp>
#endif

// library includes
#include "libid.h"

using namespace aips;
/**
 * Generates a scalar divergence field from a vector field
 */
class CDivergence : public CFilter
{
private:
  /// Standard constructor
  CDivergence();
  /// Copy constructor
  CDivergence( CDivergence& );
  /// Assignment operator
  CDivergence& operator=( CDivergence& );
public:
/* Structors */
  /// Constructor
  CDivergence( ulong ulID )
    throw();
  /// Destructor
  ~CDivergence()
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
