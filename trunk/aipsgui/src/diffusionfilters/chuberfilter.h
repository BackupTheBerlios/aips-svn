/***********************************************************************
 * File: chuberfilter.h                                                *
 * Project: AIPS - Diffusion filter plugin library                     *
 * Description: Smooting Filter based on the Huber's minmax function   *
 *              with variable number of iterations, diffusion factor   *
 *              and Integration constant (lambda)                      *
 *                                                                     *
 * Author: Joaquin Castellanos                                         *                                                                     *
 * Version: 0.1                                                        *
 * Status : Beta                                                       *
 * Created: 2004-04-26                                                 *
 * Changed: 2004-06-16 Incorporated some corrections and optimizations *
 ***********************************************************************/

#ifndef CHUBERFILTER_H
#define CHUBERFILTER_H

// Standard includes
#include <algorithm>

// AIPS includes
#include <cfilter.h>
#include <diffusiondefs.h>
#include <aipsnumbertraits.h>
#include <cglobalprogress.h>
#ifdef BENCHMARK
#include <boost/timer.hpp>
#endif

// lib includes
#include "libid.h"

using namespace aips;

/**  Anisotropic Diffusion filter */
class CHuberFilter : public CFilter
{
private:
  /// Standard constructor
  CHuberFilter();
  /// Copy constructor
  CHuberFilter( CHuberFilter& );
  /// Assignment operator
  CHuberFilter& operator=( CHuberFilter& );
public:
/* Structors */
  /// Constructor
  CHuberFilter( ulong ulID )
    throw();
  /// Destructor
  virtual ~CHuberFilter()
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
