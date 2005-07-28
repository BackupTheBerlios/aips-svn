/***********************************************************************
 * File: cweickertfilter.h                                             *
 * Project: AIPS - Diffusion filter plugin library                     *
 * Description: Smooting Filter based on the Weickert's m=2 function   *
 *              with variable number of iterations, diffusion factor   *
 *              and Integration constant (lambda)                      *
 *                                                                     *
 * Author: Joaquin Castellanos                                         *                                                                     *
 * Version: 0.1                                                        *
 * Status : Beta                                                       *
 * Created: 2004-04-27                                                 *
 * Changed: 2004-06-16 Incorporated some corrections and optimizations *
 ***********************************************************************/

#ifndef CWEICKERTFILTER_H
#define CWEICKERTFILTER_H

// Standard includes
#include <algorithm>

// AIPS includes
#include <cfilter.h>
#include <diffusiondefs.h>
#include <cglobalprogress.h>
#ifdef BENCHMARK
#include <boost/timer.hpp>
#endif

// lib includes
#include "libid.h"

using namespace aips;

/**  Anisotropic Diffusion filter */
class CWeickertFilter : public CFilter
{
private:
  /// Standard constructor
  CWeickertFilter();
  /// Copy constructor
  CWeickertFilter( CWeickertFilter& );
  /// Assignment operator
  CWeickertFilter& operator=( CWeickertFilter& );
public:
/* Structors */
  /// Constructor
  CWeickertFilter( ulong ulID )
    throw();
  /// Destructor
  virtual ~CWeickertFilter()
    throw();
/* Other methods */
  /// Reimplemented from CPipelineItem
  virtual CPipelineItem* newInstance( ulong ulID = 0 ) const
    throw();
  /// Reimplemented from CPipelineItem  
  virtual void apply()
    throw();
private:
	template<typename T> void filter() throw();
};

#endif
